#include "GameClient.h"
#include "UDPServerProtocol.h"
#include "GameServerProtocol.h"
#include "GameClientProtocol.h"
#include "ActionProtocol.h"

void SimpleGameClient::errorFeedback(LoadReader & reader)
{
	UDPMsg::ErrorCode error;
	reader.read(&error);
	handleError(error + UDPMsg::ERROR_CODES::FEEDBACK_OFFSET);
}

void SimpleGameClient::setServer(LoadReader & reader, SourceID::Type source)
{
	if (source != SourceID::Master) return;
	NetworkAdressSpecifier address;
	address.deserialize(reader);
	
}

void SimpleGameClient::connectLogin(LoadReader & reader, SourceID::Type source)
{
	if (source != SourceID::Master) {
		handleError(UDPMsg::ERROR_CODES::AUTHENTICATION_ERROR);
		return;
	}

	UDPMsg::CommandConnectLogin command;
	command.deserialize(reader);
	
	if (!core->registerAddress(SourceID::Server, command.address)) {
		handleError(UDPMsg::ERROR_CODES::UNKNOWN_ERROR);
		return;
	}

	lastPassword = command.passwordHash;

	long long base = CryptationHelper::getRandom();
	lastSecret = CryptationHelper::getRandom();
	long long result = CryptationHelper::power(base, lastSecret);

	UDPMsg::ConnectLogin loginData;
	loginData.username = command.username;
	loginData.cryptBase = base;
	loginData.cryptRes = result;

	UDPMsg::NetworkMessageWriter writer(UDPMsg::MessageType::CONNECT_LOGIN);
	writer.write(loginData);
	core->sendTo(SourceID::Server, writer.getData());
}

void SimpleGameClient::loginAccepted(LoadReader & reader, SourceID::Type source)
{
	UDPMsg::AcceptLogin acceptance;
	acceptance.deserialize(reader);
	unsigned long long key = CryptationHelper::power(acceptance.cryptRes, lastSecret);
	key = CryptationHelper::power(key, lastPassword);
	currentKey = key;
	sequenceNumber = 0;
	myID = acceptance.playerID;
	idRecievedCallback->call(myID);
}

void SimpleGameClient::sendAction(LoadReader & reader, CharBlob& data, SourceID::Type source)
{
	CryptationHelper::CryptationData cryptationIn(currentKey, ServerDataStdImpl<CharBlob>(data), sizeof(UDPMsg::MessageType::Type), 0);
	CryptationHelper::CryptationData cryptation = CryptationHelper::symiCrypt(cryptationIn, true);

	UDPMsg::ActionRequestHeader header;
	header.checkSum = cryptation.checksum;
	header.cryptNum = sequenceNumber;
	header.source = myID;

	vector<char> sendData;
	sendData.reserve(data.size() + sizeof(UDPMsg::ActionRequestHeader));
	VectorOutStream stream(sendData);
	SaveWriter writer(stream);
	writer.write(UDPMsg::MessageType::REQUEST_DO_ACTION);
	writer.write(header);
	sendData.insert(sendData.end(), &data[sizeof(UDPMsg::MessageType::Type)], &data[data.size()-1] );
	core->sendTo(SourceID::Server, sendData);

	sequenceNumber += data.size() - sizeof(UDPMsg::MessageType::Type);
	currentKey = cryptation.key;
}

void SimpleGameClient::confirmedActions(LoadReader & reader, SourceID::Type source)
{
	if (source != SourceID::Server) {
		handleError(UDPMsg::ERROR_CODES::PERMISSION_ERROR);
	}
	else {
		ActionStore* store = new ActionStore();
		store->deserialize(reader);
		actionsRecievedCallback->call(store);
	}
}

void SimpleGameClient::unknownMessage(SourceID::Type source)
{
	handleError(UDPMsg::ERROR_CODES::MESSAGE_TYPE_UNKNOWN);
}

SimpleGameClient::SimpleGameClient(GameClientConfig & config) : 
	ServerBase(config), idRecievedCallback(config.idRecievedCallback)
{
}

bool SimpleGameClient::handleMessage(CharBlob & data, SourceID::Type source)
{
	LoadReader reader = LoadReader(BlobInStream(data));
	UDPMsg::MessageType::Type type;
	reader.read(&type);
	switch (type) {
	case (UDPMsg::MessageType::SHUT_DOWN):
		return shutDownRequest(source);
	case(UDPMsg::MessageType::MESSAGE_ERROR):
		errorFeedback(reader);
		break;
	case(UDPMsg::MessageType::COMMAND_CONNECT_LOGIN):
		connectLogin(reader, source);
		break;
	case(UDPMsg::MessageType::COMMAND_REQUEST_DO_ACTION):
		sendAction(reader, data, source);
	case(UDPMsg::MessageType::ACCEPT_LOGIN):
		loginAccepted(reader, source);
		break;
	case (UDPMsg::MessageType::CONFIRMED_ACTIONS):
		confirmedActions(reader, source);
		break;
	default:
		unknownMessage(source);
	}
	return true;
}

