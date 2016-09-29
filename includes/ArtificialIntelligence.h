
#ifndef _ArtificialIntelligence_H_
#define _ArtificialIntelligence_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include "Player.h"

OSG_USING_NAMESPACE

enum AIState {
	AI_STATE_IDLE = 0
};

class AIStateHandler {
public:
	Vec3f getHeadPosition();
	Vec3f getDiskArmPosition();
	Vec3f getShieldArmPosition();
	Quaternion getHeadRotation();
	Quaternion getDiskArmRotation();
	Quaternion getShieldArmRotation();
	virtual AIState update(Player* me) = 0;
protected:
	Vec3f headPosition;
	Vec3f diskArmPosition;
	Vec3f shieldArmPosition;
	Quaternion headRotation;
	Quaternion diskArmRotation;
	Quaternion shieldArmRotation;
};

class AI {
public:
	void update();
	AI(Player* aiTarget);
	~AI();
private:
	Player* me;
	AIState state;
	AIStateHandler* stateHandler;
};

#endif