#pragma once
#include "UDPServerCore.h"
#include "ActionProtocol.h"

struct GameClientConfig : public BaseServerConfig {
	ThreadNotifyCallback<PlayerID>* idRecievedCallback; //caller retains ownership
	ThreadNotifyCallback<ActionStore*>* actionsRecievedCallback; //caller retains ownership. ActionStores sendt become recievers responsibility.
};

class SimpleGameClient : public ServerBase {
	void errorFeedback(LoadReader& reader);
	ThreadNotifyCallback<PlayerID>* idRecievedCallback;
	ThreadNotifyCallback<ActionStore*>* actionsRecievedCallback;
	unsigned long long lastSecret;
	unsigned long long lastPassword;
	unsigned long long currentKey;
	int sequenceNumber;
	PlayerID myID;

	void setServer(LoadReader& reader, SourceID::Type source);
	void connectLogin(LoadReader& reader, SourceID::Type source);
	void loginAccepted(LoadReader& reader, SourceID::Type source);
	void sendAction(LoadReader& reader, CharBlob& data, SourceID::Type source);
	void confirmedActions(LoadReader& reader, SourceID::Type source);
	void unknownMessage(SourceID::Type source);

public:
	SimpleGameClient(GameClientConfig& config);
	virtual bool handleMessage(CharBlob &data, SourceID::Type type);
};