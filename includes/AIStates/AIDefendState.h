
#ifndef _AIDefendState_H_
#define _AIDefendState_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include "ArtificialIntelligence.h"

OSG_USING_NAMESPACE

class AIDefendState : public AIStateHandler {
public:
	AIState update();
	AIDefendState(Player* me);
private:
	Real32 startTime;
	Vec3f calculateNewTargetPosition();
	Vec3f targetShieldArmPosition;
};

#endif