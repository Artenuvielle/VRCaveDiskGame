
#ifndef _NetworkPackets_H_
#define _NetworkPackets_H_

enum SToCPacketType {
	GAME_STATE_BROADCAST = 0,
	PLAYER_IDENTIFICATION,
	PLAYER_POSITION_BROADCAST,
	PLAYER_LOSE_LIFE_BROADCAST,
	PLAYER_LOSE_SHIELD_CHARGE_BROADCAST,
	DISK_STATUS_BROADCAST,
	DISK_THROW_BROADCAST,
	DISK_POSITION_BROADCAST
};

enum CToSPacketType {
	START_GAME_REQUEST = 0,
	PLAYER_POSITION_INFORMATION,
	PLAYER_THROW_INFORMATION
};

struct PlayerInformation {
	int playerId;
	int factionId;
};

struct GameInformation {
	bool isRunning;
};

struct PlayerCounterInformation {
	int playerId;
	int factionId;
	int counter;
};

struct PlayerPosition {
	int playerId;
	int factionId;
	float headPosX, headPosY, headPosZ;
	float headRotX, headRotY, headRotZ, headRotW;
	float rightPosX, rightPosY, rightPosZ;
	float rightRotX, rightRotY, rightRotZ, rightRotW;
	float leftPosX, leftPosY, leftPosZ;
	float leftRotX, leftRotY, leftRotZ, leftRotW;
};

struct DiskStatusInformation {
	int playerId;
	int factionId;
	int diskStatusId;
};

struct DiskThrowInformation {
	int playerId;
	int factionId;
	float diskPosX, diskPosY, diskPosZ;
	float diskMomentumX, diskMomentumY, diskMomentumZ;
};

struct DiskPosition {
	int playerId;
	int factionId;
	float diskPosX, diskPosY, diskPosZ;
	float diskMomentumX, diskMomentumY, diskMomentumZ;
};

#endif