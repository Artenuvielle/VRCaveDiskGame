
#ifndef _Game_H_
#define _Game_H_

#include "Observer.h"
#include "Network.h"
#include "Input.h"
#include "Common.h"
#include "Player.h"
#include "ArtificialIntelligence.h"
#include <mutex>

#define MAX_PACKETS_PER_TICK 20

struct PacketInformation {
	unsigned short peerId;
	ProtobufMessagePacket* packet;
};

class GameManager : public SToCPacketHandler, public Observer<GameNotifications>, public InputHandler {
public:
	GameManager(Client* client, Input* input);
	~GameManager();

	void requestGameStart();
	void handleGameTick();
	
	void handleConnect() override;
	void handleDisconnect() override;
	void handleSToCPacket(unsigned short peerId, ProtobufMessagePacket* packet) override;
	
	bool observableUpdate(GameNotifications notification, Observable<GameNotifications>* src) override;
	void observableRevoke(GameNotifications notification, Observable<GameNotifications>* src) override;

	void handleHeadTracking(Vec3f position, Quaternion orientation);
	void handleMainHandTracking(Vec3f position, Quaternion orientation);
	void handleOffHandTracking(Vec3f position, Quaternion orientation);
	void handleButtonUpdate(int buttonId, bool isPressed);

private:
	void processReceivedPackages();
	void processSToCPacket(unsigned short peerId, ProtobufMessagePacket* packet);
	void startGame();
	void sendUserPosition();

	void handleGameStateBroadcast(GameInformation information);
	void handlePlayerIdentification(PlayerInformation information);
	void handlePlayerPositionBroadcast(PlayerPosition information);
	void handlePlayerChangeLifeBroadcast(PlayerCounterInformation information);
	void handlePlayerChangeShieldChargeBroadcast(PlayerCounterInformation information);
	void handleDiskStatusBroadcast(DiskStatusInformation information);
	void handleDiskThrowBroadcast(DiskThrowInformation information);
	void handleDiskPositionBroadcast(DiskPosition information);
	void handleWallCollisonInformation(WallCollisonInformation information);

	std::vector<PacketInformation> _packets;
	std::mutex _packetVectorMutex;
	Client *_client;
	Input *_input;
	Player *_user, *_enemy;
	AI *_ai;
	int _userId, _enemyId;
	
#ifdef _logFrames_
	bool _isButtonPushed;
#endif
};

#endif