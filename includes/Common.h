
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

const PlayerFaction mainUserFaction = PLAYER_FACTION_BLUE;
const PlayerFaction enemyFaction = (mainUserFaction == PLAYER_FACTION_BLUE ? PLAYER_FACTION_ORANGE : PLAYER_FACTION_BLUE);

const Real32 WALL_X_MAX = 135;
const Real32 WALL_X_MIN = -135;
const Real32 WALL_Y_MAX = 270;
const Real32 WALL_Y_MIN = 0;
const Real32 WALL_Z_MAX = 135;
const Real32 WALL_Z_MIN = -945;

const Real32 diskRadius = 15; // cm
const Real32 diskHeight = 3; // cm
const Real32 diskSpeed = 0.3; // in cm/ms
const Real32 diskAxisRotationFactor = 100;
const Real32 diskRotationTimeAfterCollision = 0.3; // s

const Real32 collisionAnimationSize = 150; // in cm

extern NodeRecPtr root;
extern NodeRecPtr boundingBoxModel;
extern ComponentTransformRecPtr boundingBoxModelCT;
extern ComponentTransformRecPtr enemyPoint;

#endif