
#ifndef _Player_H_
#define _Player_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include "Common.h"

OSG_USING_NAMESPACE

const Real32 PLAYER_HEAD_SIZE = 15;
const Real32 PLAYER_TORSO_HEAD_OFFSET = 10;
const Real32 PLAYER_GEOMETRY_SCALE = 2;

class Player {
public:
	Vec3f getTorsoPosition();
	Vec3f getHeadDirection();
	void setHeadDirection(Vec3f newDirection);
	void setHeadDirection(Quaternion rotation);
	Vec3f getHeadPosition();
	void setHeadPosition(Vec3f newPosition);
	Vec3f getLeftArmDirection();
	void setLeftArmDirection(Vec3f newDirection);
	void setLeftArmDirection(Quaternion rotation);
	Vec3f getLeftArmPosition();
	void setLeftArmPosition(Vec3f newPosition);
	Vec3f getRightArmDirection();
	void setRightArmDirection(Vec3f newDirection);
	void setRightArmDirection(Quaternion rotation);
	Vec3f getRightArmPosition();
	void setRightArmPosition(Vec3f newPosition);
	Player(PlayerFaction faction, bool drawModel);
	void update();
private:
	void recalculatePositions();
	bool modelIncluded;
	Quaternion facingRotation;
	ComponentTransformRecPtr torsoTransform;
	ComponentTransformRecPtr headTransform;
	ComponentTransformRecPtr leftArmTransform;
	ComponentTransformRecPtr rightArmTransform;
	Vec3f torsoPosition;
	Vec3f headDirection;
	Vec3f headPosition;
	Vec3f leftArmDirection;
	Vec3f leftArmPosition;
	Vec3f rightArmDirection;
	Vec3f rightArmPosition;
};

#endif