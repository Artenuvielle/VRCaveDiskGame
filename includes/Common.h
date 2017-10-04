
#ifndef _Common_H_
#define _Common_H_

//#define _logFrames_
#define _simulate_
//#define _highrezTextures_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>

#include <OSGCSM/OSGCAVESceneManager.h>

OSG_USING_NAMESPACE

enum PlayerFaction {
	PLAYER_FACTION_BLUE = 0,
	PLAYER_FACTION_ORANGE
};

const PlayerFaction userFaction = PLAYER_FACTION_BLUE;
const PlayerFaction enemyFaction = (userFaction == PLAYER_FACTION_BLUE ? PLAYER_FACTION_ORANGE : PLAYER_FACTION_BLUE);

const Color3f colorBlue(0, 172.f / 255.f, 198.f / 255.f);
const Color3f colorOrange(198.f / 255.f, 123.f / 255.f, 5.f / 255.f);

const Real32 WALL_X_MAX = 135;
const Real32 WALL_X_MIN = -135;
const Real32 WALL_Y_MAX = 270;
const Real32 WALL_Y_MIN = 0;
const Real32 WALL_Z_MAX = 135;
const Real32 WALL_Z_MIN = -945;

const Real32 WALL_X_MID = (WALL_X_MAX + WALL_X_MIN) / 2;
const Real32 WALL_Y_MID = (WALL_Y_MAX + WALL_Y_MIN) / 2;
const Real32 WALL_Z_MID = (WALL_Z_MAX + WALL_Z_MIN) / 2;
const Real32 WALL_Z_DIFF = WALL_Z_MAX - WALL_Z_MIN;

const Real32 PLAYER_HEAD_SIZE = 15;
const Real32 PLAYER_TORSO_HEAD_OFFSET = 10;
const Real32 PLAYER_GEOMETRY_SCALE = 2;

const Int32 lifeCounterMaxLife = 4;
const Real32 lifeCounterMaxTransparency = 0.3;
const Real32 lifeCounterTransparencyChangeTime = 0.2; // s

const Real32 diskRadius = 15; // cm
const Real32 diskHeight = 3; // cm
const Real32 diskSpeed = 0.5f; // cm/ms
const Real32 diskAxisRotationFactor = 100;
const Real32 diskRotationTimeAfterCollision = 0.3f; // s
const Real32 diskEnemyMomentumAttractionFactor = 1.8f; // deg/sec
const Real32 diskOwnerMomentumAttractionFactor = 2.0f; // deg/sec
const Real32 diskMinimalAxialRotationAfterCollision = 0.0004f; // deg/millisec

const Real32 shieldMinimumRadius = 2.5; // cm
const Real32 shieldMaximumRadius = 20; // cm
const Real32 shieldGrowStartDistance = 400; // cm
const Real32 shieldGrowEndDistance = 250; // cm
const Int32 shieldMaximumCharges = 3;

const Vec3f aiDefaultHeadPosition = Vec3f(0,165,-810);
const Real32 aiHeadMaxSpeed = 25.f; // cm/s
const Real32 aiHeadMaxRotation = 0.8f; // rad/s
const Real32 aiArmMaxSpeed = 50.f; // cm/s
const Real32 aiArmMaxRotation = 3.5f; // rad/s
const Real32 aiMinTimeUntilAttack = 2.f; // s
const Real32 aiDefendArmTorsoDistance = 35.f; // cm
const Real32 aiCatchArmTorsoDistance = 50.f; // cm

const Real32 collisionAnimationSize = 150; // in cm
const Real32 scoreAnimationSize = 1200; // in cm

const Int32 lightTrailMaxPoints = 70;
const Real32 lightTrailPointsPerSecond = 30.f;
const Real32 lightTrailSizeGrow = 100.f;
const Real32 lightTrailMaxSize = 1.5f;
const Real32 lightTrailInputPointMinDistance = 20.f;

extern NodeRecPtr root;

extern bool gameRunning;
extern bool isConnected;
extern int gameResultAnimationId;

enum GameNotifications {
	GAME_NOTIFICATION_PLAYER_CHANGED_LIFE = 0,
	GAME_NOTIFICATION_PLAYER_CHANGED_SHIELD_CHARGE,
	GAME_NOTIFICATION_DISK_STATE_CHANGED,
	GAME_NOTIFICATION_DISK_THROWN
};

#endif
