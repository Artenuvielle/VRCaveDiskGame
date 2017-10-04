
#ifndef _Player_H_
#define _Player_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include "Common.h"
#include "Disk.h"
#include "Shield.h"
#include "LifeCounter.h"

OSG_USING_NAMESPACE

class Player : public DiskEventHandler {
public:
	Player* getEnemy();
	void setEnemy(Player* newEnemy);
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
	PlayerFaction getFaction();
	Disk* getDisk();
	Shield* getShield();
	LifeCounter* getLifeCounter();
	void handleDiskCatch();
	Player(PlayerFaction faction, bool drawModel);
	~Player();
	void update();
	void loseLife();
private:
	void recalculatePositions();
	bool modelIncluded;
	PlayerFaction faction;
	Player* enemy;
	Disk* disk;
	Shield* shield;
	LifeCounter* lifeCounter;
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