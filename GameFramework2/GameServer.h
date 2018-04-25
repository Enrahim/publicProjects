#pragma once

#include "UDPServerCore.h"
#include "GameServerProtocol.h"
#include "Cryptography.h"
#include "PlayerStore.h"
#include "ActionProtocol.h"
#include "ThreadNotificationSystem.h"


struct GameServerConfig : public BaseServerConfig {
	ThreadNotifyCallback<NetworkAction* >* actionCallback; //caller retains ownership; sendt newed NetworkAction commes along with ownership.
	PlayerStore* players; // caller retains ownership. Read only.
};

class SimpleGameServer : public ServerBase {
	ThreadNotifyCallback<NetworkAction* >* actionCallback;
	PlayerStore* players;
protected:
	void handleActionMessage(NetworkAction* action);
public:
	SimpleGameServer(GameServerConfig& config);
	virtual bool handleMessage(CharBlob& data, SourceID::Type source);
private:
	void connectLogin(LoadReader& reader);
	void requestDoAction(LoadReader& reader, CharBlob& data);
	void confirmedActions(CharBlob& data, SourceID::Type source);

};