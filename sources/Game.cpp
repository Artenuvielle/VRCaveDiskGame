#include "Game.h"
#include <fstream>

#include "Animations.h"

#ifdef _logFrames_
std::ofstream logFile;
#endif

PositionPacketType createPosition(Vec3f pos) {
    PositionPacketType ret;
    ret.set_x(pos.x());
    ret.set_y(pos.y());
    ret.set_z(pos.z());
    return ret;
}

OrientationPacketType createOrientation(Quaternion rot) {
    OrientationPacketType ret;
    ret.set_x(rot.x());
    ret.set_y(rot.y());
    ret.set_z(rot.z());
    ret.set_w(rot.w());
    return ret;
}

Vec3f createVector(PositionPacketType pos) {
	return Vec3f(pos.x(), pos.y(), pos.z());
}

Quaternion createQuaternion(OrientationPacketType rot) {
	return Quaternion(rot.x(), rot.y(), rot.z(), rot.w());
}

GameManager::GameManager(Client* client, Input* input) {
	_packets = std::vector<PacketInformation>();
	_client = client;
	_input = input;
	_user = new Player(userFaction, false);
	_enemy = new Player(enemyFaction, true);
	_user->setEnemy(_enemy);
	_enemy->setEnemy(_user);
	_ai = new AI(_enemy);

	_user->getDisk()->attach(this);
	_enemy->getDisk()->attach(this);
	
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
	_user->getDisk()->detach(this);
	_enemy->getDisk()->detach(this);
	delete _ai;
	delete _user;
	delete _enemy;
}

void GameManager::requestGameStart() {
	if (_userId < 0) {
		startGame();
	} else {
		GameInformation* gi = new GameInformation();
		gi->set_is_running(true);
		ProtobufMessagePacket* packet = new ProtobufMessagePacket();
		packet->set_header(ProtobufMessagePacket_Header_CTOS_PACKET_TYPE_START_GAME_REQUEST);
		packet->set_allocated_game_information(gi);
		_client->sendPacket(packet, true);
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
	PlayerPosition* pp = new PlayerPosition();
	pp->set_player_id(_userId);
	pp->set_faction_id(userFaction);
	PositionPacketType head_pos = createPosition(_user->getHeadPosition());
	PositionPacketType main_hand_pos = createPosition(_user->getDiskArmPosition());
	PositionPacketType off_hand_pos = createPosition(_user->getShieldArmPosition());
	OrientationPacketType head_rot = createOrientation(_user->getHeadRotation());
	OrientationPacketType main_hand_rot = createOrientation(_user->getDiskArmRotation());
	OrientationPacketType off_hand_rot = createOrientation(_user->getShieldArmRotation());
	pp->set_allocated_head_pos(&head_pos);
	pp->set_allocated_head_rot(&head_rot);
	pp->set_allocated_main_hand_pos(&main_hand_pos);
	pp->set_allocated_main_hand_rot(&main_hand_rot);
	pp->set_allocated_off_hand_pos(&off_hand_pos);
	pp->set_allocated_off_hand_rot(&off_hand_rot);
	ProtobufMessagePacket* packet = new ProtobufMessagePacket();
	packet->set_header(ProtobufMessagePacket_Header_CTOS_PACKET_TYPE_PLAYER_POSITION_INFORMATION);
	packet->set_allocated_player_position(pp);
	_client->sendPacket(packet);
}

void GameManager::handleGameTick() {
	processReceivedPackages();
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

void GameManager::handleGameStateBroadcast(GameInformation information) {
	if (information.is_running()) {
		std::cout << "game started by server" << std::endl;
		startGame();
	} else {
		if (information.has_winning_player_id()) {

		}
		// ensure game ending
	}
}

void GameManager::handlePlayerIdentification(PlayerInformation information) {
	std::cout << "got player id '" << information.player_id() << "' from server" << std::endl;
	_userId = information.player_id();
}

void GameManager::handlePlayerPositionBroadcast(PlayerPosition information) {
	if (information.player_id() != _userId) {
		_enemy->setHeadPosition(createVector(information.head_pos()));
		_enemy->setDiskArmPosition(createVector(information.main_hand_pos()));
		_enemy->setShieldArmPosition(createVector(information.off_hand_pos()));
		_enemy->setHeadRotation(createQuaternion(information.head_rot()));
		_enemy->setDiskArmRotation(createQuaternion(information.main_hand_rot()));
		_enemy->setShieldArmRotation(createQuaternion(information.off_hand_rot()));
	}
}

void GameManager::handlePlayerChangeLifeBroadcast(PlayerCounterInformation information) {
	std::cout << "player '" << information.player_id() << "' life: " << information.counter() << std::endl;
	if (information.player_id() == _userId) {
		_user->getLifeCounter()->setLifeCount(information.counter());
	} else {
		_enemy->getLifeCounter()->setLifeCount(information.counter());
	}
}

void GameManager::handlePlayerChangeShieldChargeBroadcast(PlayerCounterInformation information) {
	std::cout << "player '" << information.player_id() << "' shield: " << information.counter() << std::endl;
	if (information.player_id() == _userId) {
		_user->getShield()->setCharges(information.counter());
	} else {
		_enemy->getShield()->setCharges(information.counter());
	}
}

void GameManager::handleDiskStatusBroadcast(DiskStatusInformation information) {
	// TODO: sync local calculation
}

void GameManager::handleDiskThrowBroadcast(DiskThrowInformation information) {
	if (information.player_id() != _userId) {
		_enemy->getDisk()->forceThrow(createVector(information.disk_pos()), createVector(information.disk_momentum()));
	}
}

void GameManager::handleDiskPositionBroadcast(DiskPosition information) {
	if (information.player_id() == _userId) {
		_user->getDisk()->setPosition(createVector(information.disk_pos()));
		_user->getDisk()->setRotation(createQuaternion(information.disk_rot()));
	} else {
		_enemy->getDisk()->setPosition(createVector(information.disk_pos()));
		_enemy->getDisk()->setRotation(createQuaternion(information.disk_rot()));
	}
	// TODO: sync local calculation
}

void GameManager::handleSToCPacket(unsigned short peerId, ProtobufMessagePacket* packet) {
	_packetVectorMutex.lock();
	PacketInformation pi = {peerId, packet};
	_packets.push_back(pi);
	_packetVectorMutex.unlock();
}

void GameManager::processReceivedPackages() {
	std::vector<PacketInformation> packetsToHandle = std::vector<PacketInformation>();
	_packetVectorMutex.lock();
	_packets.swap(packetsToHandle);
	_packetVectorMutex.unlock();
	for (std::vector<PacketInformation>::iterator it = packetsToHandle.begin() ; it != packetsToHandle.end(); ++it) {
		processSToCPacket(it->peerId, it->packet);
	}
}

void GameManager::processSToCPacket(unsigned short peerId, ProtobufMessagePacket* packet) {
	switch (packet->header()) {
	case ProtobufMessagePacket_Header_STOC_PACKET_TYPE_GAME_STATE_BROADCAST:
		handleGameStateBroadcast(packet->game_information());
		break;
	case ProtobufMessagePacket_Header_STOC_PACKET_TYPE_PLAYER_IDENTIFICATION:
		handlePlayerIdentification(packet->player_information());
		break;
	case ProtobufMessagePacket_Header_STOC_PACKET_TYPE_PLAYER_POSITION_BROADCAST:
		handlePlayerPositionBroadcast(packet->player_position());
		break;
	case ProtobufMessagePacket_Header_STOC_PACKET_TYPE_PLAYER_CHANGED_LIFE_BROADCAST:
		handlePlayerChangeLifeBroadcast(packet->player_counter_information());
		break;
	case ProtobufMessagePacket_Header_STOC_PACKET_TYPE_PLAYER_CHANGED_SHIELD_CHARGE_BROADCAST:
		handlePlayerChangeShieldChargeBroadcast(packet->player_counter_information());
		break;
	case ProtobufMessagePacket_Header_STOC_PACKET_TYPE_DISK_STATUS_BROADCAST:
		handleDiskStatusBroadcast(packet->disk_status_information());
		break;
	case ProtobufMessagePacket_Header_STOC_PACKET_TYPE_DISK_THROW_BROADCAST:
		handleDiskThrowBroadcast(packet->disk_throw_information());
		break;
	case ProtobufMessagePacket_Header_STOC_PACKET_TYPE_DISK_POSITION_BROADCAST:
		handleDiskPositionBroadcast(packet->disk_position());
		break;
	}
}

bool GameManager::observableUpdate(GameNotifications notification, Observable<GameNotifications>* src) {
	switch (notification) {
	case GAME_NOTIFICATION_DISK_THROWN:
		if (src == _user->getDisk()) {
			DiskThrowInformation* dti = new DiskThrowInformation();
			dti->set_player_id(_userId);
			dti->set_faction_id(userFaction);
			PositionPacketType disk_pos = createPosition(_user->getDisk()->getPosition());
			PositionPacketType disk_momentum = createPosition(_user->getDisk()->getMomentum());
			dti->set_allocated_disk_pos(&disk_pos);
			dti->set_allocated_disk_momentum(&disk_momentum);
			ProtobufMessagePacket* packet = new ProtobufMessagePacket();
			packet->set_header(ProtobufMessagePacket_Header_CTOS_PACKET_TYPE_PLAYER_THROW_INFORMATION);
			packet->set_allocated_disk_throw_information(dti);
			_client->sendPacket(packet, true);
		}
		break;
	}
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
