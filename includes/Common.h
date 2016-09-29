
#ifndef _Common_H_
#define _Common_H_

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

const Real32 PLAYER_HEAD_SIZE = 15;
const Real32 PLAYER_TORSO_HEAD_OFFSET = 10;
const Real32 PLAYER_GEOMETRY_SCALE = 2;

const Real32 diskRadius = 15; // cm
const Real32 diskHeight = 3; // cm
const Real32 diskSpeed = 0.4; // cm/ms
const Real32 diskAxisRotationFactor = 100;
const Real32 diskRotationTimeAfterCollision = 0.3; // s

const Real32 shieldMinimumRadius = 2.5; // cm
const Real32 shieldMaximumRadius = 20; // cm
const Real32 shieldGrowStartDistance = 400; // cm
const Real32 shieldGrowEndDistance = 250; // cm

const Real32 diskEnemyMomentumAttractionFactor = 1.2f; // deg/sec
const Real32 diskOwnerMomentumAttractionFactor = 1.5f; // deg/sec
const Real32 diskMinimalAxialRotationAfterCollision = 0.0004f; // deg/millisec

const Real32 collisionAnimationSize = 150; // in cm

const Vec3f aiDefaultHeadPosition = Vec3f(0,165,-810);

extern NodeRecPtr root;
extern NodeRecPtr boundingBoxModel;
extern ComponentTransformRecPtr boundingBoxModelCT;

#endif