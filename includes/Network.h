
#ifndef _Network_H_
#define _Network_H_

#include "NetworkPackets.h"
#include <enet/enet.h>

class SToCPacketHandler {
public:
	virtual void handleConnect(unsigned short peerId) = 0;
	virtual void handleDisconnect(unsigned short peerId) = 0;
	virtual void handleSToCPacket(unsigned short peerId, SToCPacketType* header, void* data, int size) = 0;
};

class Client {
public:
	Client();
	~Client();

	void setPacketHandler(SToCPacketHandler* handler);
	SToCPacketHandler* getPacketHandler();

	bool connect(const char* hostAdress, short port);
	void disconnect();

	bool isConnected();
	void networkLoop();
	void sendPacket(CToSPacketType header, void* data, int size, bool reliable = false);
private:
	SToCPacketHandler* _packetHandler;

	ENetHost* _enetHost;
	ENetPeer* _peer;

	bool keepConnection;
};

void networkLoopOnClient(void* client);

#endif