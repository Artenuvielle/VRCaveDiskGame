
#ifndef _AIAttackState_H_
#define _AIAttackState_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include "ArtificialIntelligence.h"

OSG_USING_NAMESPACE

class AIAttackState : public AIStateHandler {
public:
	AIState update(Player* me);
};

#endif