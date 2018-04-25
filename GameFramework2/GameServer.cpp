#include "GameServer.h"
#include "Serialization.h"
#include <ctime>

void SimpleGameServer::handleActionMessage(NetworkAction * action)
{
	actionCallback->call(action);
}

SimpleGameServer::SimpleGameServer(GameServerConfig & config) : ServerBase(config), players(config.players)
{
}

bool SimpleGameServer::handleMessage(CharBlob& data, SourceID::Type source)
{
	LoadReader reader = LoadReader(BlobInStream(data));
	UDPMsg::MessageType::Type type;
	reader.read(&type);
	switch (type) {
	case UDPMsg::MessageType::CONNECT_LOGIN:
		connectLogin(reader);
		break;
	case UDPMsg::MessageType::REQUEST_DO_ACTION:
		requestDoAction(reader, data);
		break;
	case UDPMsg::MessageType::SHUT_DOWN:
		return shutDownRequest(source);
	case UDPMsg::MessageType::CONFIRMED_ACTIONS:
		confirmedActions(data, source);
	default:
		handleError(UDPMsg::ERROR_CODES::MESSAGE_TYPE_UNKNOWN);
	}
	return true;
}

void SimpleGameServer::connectLogin(LoadReader & reader)
{
	UDPMsg::ConnectLogin msg;
	reader.read(&msg);
	PlayerID pId = players->getPlayerId(msg.username);
	if (pId == 0) {
		handleError(UDPMsg::ERROR_CODES::AUTHENTICATION_ERROR);
	}
	
	long long base = msg.cryptBase;
	long long res = msg.cryptRes;

	long long password = players->getPasswordHash(pId);

	CryptationHelper::CryptSequence seq(clock());
	seq.skip(2);
	string stringSalt("12345678");
	for (int i = 0; i < 8; i++) {
		stringSalt[i] = seq.nextChar();
	}
	long long salt = CryptationHelper::stringToUll(stringSalt);
	long long sendRes = CryptationHelper::power(base, salt);
	long long key = CryptationHelper::power(res, salt);
	players->storeKeySuggetion(pId, key);
	key = CryptationHelper::power(key, password);

	std::vector<char> retMessage;
	SaveWriter writer = SaveWriter(VectorOutStream(retMessage));
	UDPMsg::MessageType::Type type = UDPMsg::MessageType::ACCEPT_LOGIN;
	writer.write(type);
	UDPMsg::AcceptLogin retData;
	retData.cryptRes = sendRes;
	retData.playerID = pId;
	writer.write(retData);

	core->reply(retMessage);
}

void SimpleGameServer::requestDoAction(LoadReader & reader, CharBlob& data)
{
	UDPMsg::ActionRequestHeader header;
	header.deserialize(reader);
	DecryptingState* crypt = players->getDecrypter(header.source);
	if (!crypt->tryDecrypt(ServerDataStdImpl<CharBlob>(data), sizeof(UDPMsg::MessageType::Type) + sizeof(header), header.cryptNum, header.checkSum) ) {
		handleError(UDPMsg::ERROR_CODES::AUTHENTICATION_ERROR);
		return;
	}
	NetworkAction* action = NetworkAction::fetchNewWithoutPlayer(reader, header.source);
	handleActionMessage(action);
}

void SimpleGameServer::confirmedActions(CharBlob & data, SourceID::Type source)
{
	if (source != SourceID::Master) {
		handleError(UDPMsg::ERROR_CODES::PERMISSION_ERROR);
	}
	else {
		core->sendTo(SourceID::MultiCast, vector<char>(&data[0], &data[data.size()]));
	}
}



