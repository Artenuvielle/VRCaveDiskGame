
#ifndef _AIIdleState_H_
#define _AIIdleState_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include "ArtificialIntelligence.h"

OSG_USING_NAMESPACE

class AIIdleState : public AIStateHandler {
public:
	AIState update(Player* me);
};

#endif