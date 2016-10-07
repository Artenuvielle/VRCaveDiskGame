
#ifndef _AICatchState_H_
#define _AICatchState_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include "ArtificialIntelligence.h"

OSG_USING_NAMESPACE

class AICatchState : public AIStateHandler {
public:
	AIState update();
	AICatchState(Player* me);
private:
	Real32 startTime;
	Vec3f calculateNewTargetPosition();
	Vec3f targetDiskArmPosition;
};

#endif