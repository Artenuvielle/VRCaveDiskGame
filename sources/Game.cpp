#include "Game.h"
#include <fstream>

#include "Animations.h"

#ifdef _logFrames_
std::ofstream logFile;
#endif

GameManager::GameManager(Client* client, Input* input) {
	_client = client;
	_input = input;
	_user = new Player(userFaction, false);
	_enemy = new Player(enemyFaction, true);
	_user->setEnemy(_enemy);
	_enemy->setEnemy(_user);
	_ai = new AI(_enemy);

	//_user->attach(this);
	//_enemy->attach(this);
	
	_userId = -2;
	_enemyId = -2;
		
#ifdef _logFrames_
	_isButtonPushed = false;
	logFile.open ("example.txt");
#endif
}

GameManager::~GameManager() {
#ifdef _logFrames_
	logFile.close();
#endif
	delete _ai;
	delete _user;
	delete _enemy;
}

void GameManager::requestGameStart() {
	if (_userId < 0) {
		startGame();
	} else {
		_client->sendPacket(CTOS_PACKET_TYPE_START_GAME_REQUEST, new GameInformation(), sizeof(GameInformation), true);
	}
}

void GameManager::startGame() {
	if (!gameRunning) {
		if (gameResultAnimationId >= 0) {
			endAnimation(gameResultAnimationId);
			gameResultAnimationId = -1;
		}
		_user->getLifeCounter()->setLifeCount(lifeCounterMaxLife);
		_user->getShield()->refillCharges();
		_enemy->getLifeCounter()->setLifeCount(lifeCounterMaxLife);
		_enemy->getShield()->refillCharges();
		_ai->resetState();
		std::cout << "Game starting, have fun :)" << '\n';
		gameRunning = true;
	} else {
		std::cout << "Game allready running, wait for it to finish..." << '\n';
	}
}

void GameManager::sendUserPosition() {
	PlayerPosition pp;
	pp.playerId = _userId;
	pp.factionId = userFaction;
	_user->getHeadPosition().getSeparateValues(pp.headPosX, pp.headPosY, pp.headPosZ);
	_user->getHeadRotation().getValueAsQuat(pp.headRotX, pp.headRotY, pp.headRotZ, pp.headRotW);
	_user->getDiskArmPosition().getSeparateValues(pp.rightPosX, pp.rightPosY, pp.rightPosZ);
	_user->getDiskArmRotation().getValueAsQuat(pp.rightRotX, pp.rightRotY, pp.rightRotZ, pp.rightRotW);
	_user->getShieldArmPosition().getSeparateValues(pp.leftPosX, pp.leftPosY, pp.leftPosZ);
	_user->getShieldArmRotation().getValueAsQuat(pp.leftRotX, pp.leftRotY, pp.leftRotZ, pp.leftRotW);
	_client->sendPacket(CTOS_PACKET_TYPE_PLAYER_POSITION_INFORMATION, &pp, sizeof(PlayerPosition));
}

void GameManager::handleGameTick() {
	_user->setHeadRotation(_input->getHeadOrientation());
	_user->setHeadPosition(_input->getHeadPosition());
	_user->setDiskArmRotation(_input->getMainHandOrientation());
	_user->setDiskArmPosition(_input->getMainHandPosition());
	_user->setShieldArmRotation(_input->getOffHandOrientation());
	_user->setShieldArmPosition(_input->getOffHandPosition());
	_user->update();

	if (_userId < 0) {
		_ai->update();
	}
	_enemy->update();

	if (_userId >= 0) {
		sendUserPosition();
	}

#ifdef _logFrames_
	Vec3f head_position = _input->getHeadPosition();
	Vec3f wand_position = _input->getMainHandPosition();
	Vec3f shield_position = _input->getOffHandPosition();
	Quaternion head_orientation = _input->getHeadOrientation();
	Quaternion wand_orientation = _input->getMainHandOrientation();
	Quaternion shield_orientation = _input->getOffHandOrientation();
	logFile << "		simSteps.push(SimStep(" << time << ", Vec3f(" << head_position << "), Quaternion(" << head_orientation.x() << ", " << head_orientation.y() << ", " << head_orientation.z() << ", " << head_orientation.w();
	logFile << "), Vec3f(" << wand_position << "), Quaternion(" << wand_orientation.x() << ", " << wand_orientation.y() << ", " << wand_orientation.z() << ", " << wand_orientation.w();
	logFile << ", Vec3f(" << shield_position << "), Quaternion(" << shield_orientation.x() << ", " << shield_orientation.y() << ", " << shield_orientation.z() << ", " << shield_orientation.w() << ")));\n";
#endif
}


void GameManager::handleConnect() {}

void GameManager::handleDisconnect() {
	_userId = -2;
	_enemyId = -2;
}

void GameManager::handleGameStateBroadcast(GameInformation* information) {
	if (information->isRunning) {
		std::cout << "game started by server" << std::endl;
		startGame();
	} else {
		// ensure game ending
	}
}

void GameManager::handlePlayerIdentification(PlayerInformation* information) {
	std::cout << "got player id '" << information->playerId << "' from server" << std::endl;
	_userId = information->playerId;
}

void GameManager::handlePlayerPositionBroadcast(PlayerPosition* information) {
	if (information->playerId != _userId) {
		_enemy->setHeadPosition(Vec3f(information->headPosX, information->headPosY, information->headPosZ));
		_enemy->setHeadRotation(Quaternion(information->headRotX, information->headRotY, information->headRotZ, information->headRotW));
		_enemy->setDiskArmPosition(Vec3f(information->rightPosX, information->rightPosY, information->rightPosZ));
		_enemy->setDiskArmRotation(Quaternion(information->rightRotX, information->rightRotY, information->rightRotZ, information->rightRotW));
		_enemy->setShieldArmPosition(Vec3f(information->leftPosX, information->leftPosY, information->leftPosZ));
		_enemy->setShieldArmRotation(Quaternion(information->leftRotX, information->leftRotY, information->leftRotZ, information->leftRotW));
	}
}

void GameManager::handlePlayerChangeLifeBroadcast(PlayerCounterInformation* information) {
	std::cout << "player '" << information->playerId << "' life: " << information->counter << std::endl;
	if (information->playerId == _userId) {
		_user->getLifeCounter()->setLifeCount(information->counter);
	} else {
		_user->getLifeCounter()->setLifeCount(information->counter);
	}
}

void GameManager::handlePlayerChangeShieldChargeBroadcast(PlayerCounterInformation* information) {
	std::cout << "player '" << information->playerId << "' shield: " << information->counter << std::endl;
	if (information->playerId == _userId) {
		_user->getShield()->setCharges(information->counter);
	} else {
		_user->getShield()->setCharges(information->counter);
	}
}

void GameManager::handleDiskStatusBroadcast(DiskStatusInformation* information) {
	// TODO: sync local calculation
}

void GameManager::handleDiskThrowBroadcast(DiskThrowInformation* information) {
	std::cout << "player '" << information->playerId << "' throw: Vec3f("
		<< information->diskPosX << ", "
		<< information->diskPosY << ", "
		<< information->diskPosZ << ") Vec3f("
		<< information->diskMomentumX << ", "
		<< information->diskMomentumY << ", "
		<< information->diskMomentumZ << ")"<< std::endl;
	if (information->playerId != _userId) {
		_enemy->getDisk()->forceThrow(Vec3f(information->diskPosX, information->diskPosY, information->diskPosZ), Vec3f(information->diskMomentumX, information->diskMomentumY, information->diskMomentumZ));
	}
}

void GameManager::handleDiskPositionBroadcast(DiskPosition* information) {
	// TODO: sync local calculation
}

void GameManager::handleSToCPacket(unsigned short peerId, SToCPacketType* header, void* data, int size) {
	switch (*header) {
	case STOC_PACKET_TYPE_GAME_STATE_BROADCAST:
		handleGameStateBroadcast(reinterpret_cast<GameInformation*>(data));
		break;
	case STOC_PACKET_TYPE_PLAYER_IDENTIFICATION:
		handlePlayerIdentification(reinterpret_cast<PlayerInformation*>(data));
		break;
	case STOC_PACKET_TYPE_PLAYER_POSITION_BROADCAST:
		handlePlayerPositionBroadcast(reinterpret_cast<PlayerPosition*>(data));
		break;
	case STOC_PACKET_TYPE_PLAYER_CHANGED_LIFE_BROADCAST:
		handlePlayerChangeLifeBroadcast(reinterpret_cast<PlayerCounterInformation*>(data));
		break;
	case STOC_PACKET_TYPE_PLAYER_CHANGED_SHIELD_CHARGE_BROADCAST:
		handlePlayerChangeShieldChargeBroadcast(reinterpret_cast<PlayerCounterInformation*>(data));
		break;
	case STOC_PACKET_TYPE_DISK_STATUS_BROADCAST:
		handleDiskStatusBroadcast(reinterpret_cast<DiskStatusInformation*>(data));
		break;
	case STOC_PACKET_TYPE_DISK_THROW_BROADCAST:
		handleDiskThrowBroadcast(reinterpret_cast<DiskThrowInformation*>(data));
		break;
	case STOC_PACKET_TYPE_DISK_POSITION_BROADCAST:
		handleDiskPositionBroadcast(reinterpret_cast<DiskPosition*>(data));
		break;
	}
}

bool GameManager::observableUpdate(GameNotifications notification, Observable<GameNotifications>* src) {
	
	return true;
}

void GameManager::observableRevoke(GameNotifications notification, Observable<GameNotifications>* src) {
	
}

void GameManager::handleHeadTracking(Vec3f position, Quaternion orientation) {}
void GameManager::handleMainHandTracking(Vec3f position, Quaternion orientation) {}
void GameManager::handleOffHandTracking(Vec3f position, Quaternion orientation) {}

void GameManager::handleButtonUpdate(int buttonId, bool isPressed) {
	if (buttonId == 0) {
		if (isPressed) {
#ifdef _logFrames_
			if (!_isButtonPushed) {
				logFile << "		inputSteps.push(InputStep(" << glutGet(GLUT_ELAPSED_TIME) << ", true));";
				_isButtonPushed = true;
			}
#endif
			if(_user->getDisk()->getState() == DISK_STATE_READY) {
				_user->getDisk()->startDraw(_input->getMainHandPosition());
			}
		} else {
#ifdef _logFrames_
			if (_isButtonPushed) {
				logFile << "		inputSteps.push(InputStep(" << glutGet(GLUT_ELAPSED_TIME) << ", false));";
				_isButtonPushed = false;
			}
#endif
			if(_user->getDisk()->getState() == DISK_STATE_DRAWN) {
				_user->getDisk()->endDraw(_input->getMainHandPosition());
			}
		}
	} else if (!gameRunning && buttonId == 1) {
		if (isPressed) {
			requestGameStart();
		}
	}
}
