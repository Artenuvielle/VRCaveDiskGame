
#ifndef _Game_H_
#define _Game_H_

#include "Observer.h"
#include "Network.h"
#include "Input.h"
#include "Common.h"
#include "Player.h"
#include "ArtificialIntelligence.h"

class GameManager : public SToCPacketHandler, public Observer<GameNotifications>, public InputHandler {
public:
	GameManager(Client* client, Input* input);
	~GameManager();

	void requestGameStart();
	void handleGameTick();
	
	void handleConnect(unsigned short peerId) override;
	void handleDisconnect(unsigned short peerId) override;
	void handleSToCPacket(unsigned short peerId, SToCPacketType* header, void* data, int size) override;

	bool observableUpdate(GameNotifications notification, Observable<GameNotifications>* src) override;
	void observableRevoke(GameNotifications notification, Observable<GameNotifications>* src) override;

	void handleHeadTracking(Vec3f position, Quaternion orientation);
	void handleMainHandTracking(Vec3f position, Quaternion orientation);
	void handleOffHandTracking(Vec3f position, Quaternion orientation);
	void handleButtonUpdate(int buttonId, bool isPressed);

private:
	void startGame();
	void sendUserPosition();
	Client *_client;
	Input *_input;
	Player *_user, *_enemy;
	AI *_ai;
	int _peerId;
	int _userId, _enemyId;
	
#ifdef _logFrames_
	bool _isButtonPushed;
#endif
};

#endif