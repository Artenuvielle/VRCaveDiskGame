
#ifndef _Player_H_
#define _Player_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include "Common.h"
#include "Disk.h"
#include "Shield.h"

OSG_USING_NAMESPACE

const Real32 PLAYER_HEAD_SIZE = 15;
const Real32 PLAYER_TORSO_HEAD_OFFSET = 10;
const Real32 PLAYER_GEOMETRY_SCALE = 2;

class Player {
public:
	Vec3f getTorsoPosition();
	Quaternion getHeadRotation();
	void setHeadRotation(Quaternion rotation);
	Vec3f getHeadPosition();
	void setHeadPosition(Vec3f newPosition);
	Quaternion getDiskArmRotation();
	void setDiskArmRotation(Quaternion rotation);
	Vec3f getDiskArmPosition();
	void setDiskArmPosition(Vec3f newPosition);
	Quaternion getShieldArmRotation();
	void setShieldArmRotation(Quaternion rotation);
	Vec3f getShieldArmPosition();
	void setShieldArmPosition(Vec3f newPosition);
	Disk* getDisk();
	Shield* getShield();
	Player(PlayerFaction faction, bool drawModel);
	~Player();
	void update();
private:
	void recalculatePositions();
	bool modelIncluded;
	Disk* disk;
	Shield* shield;
	ComponentTransformRecPtr torsoTransform;
	ComponentTransformRecPtr headTransform;
	ComponentTransformRecPtr diskArmTransform;
	ComponentTransformRecPtr shieldArmTransform;
	Vec3f torsoPosition;
	Quaternion headRotation;
	Vec3f headPosition;
	Quaternion diskArmRotation;
	Vec3f diskArmPosition;
	Quaternion shieldArmRotation;
	Vec3f shieldArmPosition;
};

#endif