
#ifndef _ArtificialIntelligence_H_
#define _ArtificialIntelligence_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include "Player.h"

OSG_USING_NAMESPACE

enum AIState {
	AI_STATE_IDLE = 0,
	AI_STATE_ATTACK,
	AI_STATE_DEFEND,
	SIZE_OF_AI_STATE_ENUM
};
static const char* AIStateNames[] = { "idle", "attacking", "defending" };
static_assert(sizeof(AIStateNames)/sizeof(char*) == SIZE_OF_AI_STATE_ENUM, "sizes dont match");

extern Vec3f getPositionForAIInBounds(Vec3f input);

class AIStateHandler {
public:
	Vec3f getHeadPosition();
	Vec3f getDiskArmPosition();
	Vec3f getShieldArmPosition();
	Quaternion getHeadRotation();
	Quaternion getDiskArmRotation();
	Quaternion getShieldArmRotation();
	virtual AIState update() = 0;
	AIStateHandler(Player* me);
protected:
	Player* me;
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