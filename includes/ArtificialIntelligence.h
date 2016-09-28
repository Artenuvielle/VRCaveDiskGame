
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

class AI {
public:
	void update();
	AI(Player* aiTarget);
private:
	Player* me;
	AIState state;
};

#endif