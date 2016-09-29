
#ifndef _AIAttackState_H_
#define _AIAttackState_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include "ArtificialIntelligence.h"

OSG_USING_NAMESPACE

class AIAttackState : public AIStateHandler {
public:
	AIState update();
	AIAttackState(Player* me);
private:
	Vec3f startPosition, startDirection, startDrawingPosition, startDrawingDirection, endDrawingPosition, endDrawingDirection;
	Real32 distanceToDrawStart, distanceToDrawEnd;
	Real32 startTime;
	bool isDrawing;
};

#endif