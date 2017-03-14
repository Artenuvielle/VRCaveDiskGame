
#ifndef _NetworkPackets_H_
#define _NetworkPackets_H_

enum SToCPacketType {
	STOC_PACKET_TYPE_GAME_STATE_BROADCAST = 0,
	STOC_PACKET_TYPE_PLAYER_IDENTIFICATION,
	STOC_PACKET_TYPE_PLAYER_POSITION_BROADCAST,
	STOC_PACKET_TYPE_PLAYER_CHANGED_LIFE_BROADCAST,
	STOC_PACKET_TYPE_PLAYER_CHANGED_SHIELD_CHARGE_BROADCAST,
	STOC_PACKET_TYPE_DISK_STATUS_BROADCAST,
	STOC_PACKET_TYPE_DISK_THROW_BROADCAST,
	STOC_PACKET_TYPE_DISK_POSITION_BROADCAST
};

enum CToSPacketType {
	CTOS_PACKET_TYPE_START_GAME_REQUEST = 0,
	CTOS_PACKET_TYPE_PLAYER_POSITION_INFORMATION,
	CTOS_PACKET_TYPE_PLAYER_THROW_INFORMATION
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