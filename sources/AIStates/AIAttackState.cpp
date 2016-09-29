#include "AIStates/AIAttackState.h"

#include "Common.h"

OSG_USING_NAMESPACE

AIAttackState::AIAttackState(Player* me) : AIStateHandler(me) {
	startPosition = headPosition + Vec3f(-25,-60,0);
	startDirection = Vec3f(0, -1, 0);
	startDrawingPosition = headPosition + Vec3f(25,-60,10);
	startDrawingDirection = Vec3f(0.f, 0.f, -1.f);
	endDrawingPosition = headPosition + Vec3f(10,-60,40);
	endDrawingDirection= Vec3f(0.2f, 0.2f, -1.f);
	startTime = glutGet(GLUT_ELAPSED_TIME);
	isDrawing = false;
}

Real32 h1(Real32 x) { return 2*x*x*x-3*x*x+1; }
Real32 h2(Real32 x) { return -2*x*x*x+3*x*x; }
Real32 h3(Real32 x) { return x*x*x-2*x*x+x; }
Real32 h4(Real32 x) { return x*x*x-x*x; }

AIState AIAttackState::update() {
	Real32 time = glutGet(GLUT_ELAPSED_TIME);
	AIState nextState = AI_STATE_ATTACK;
	Real32 curTimePercent = (time - startTime) / 1000;
	if (isDrawing) {
		if (curTimePercent >= 1) {
			nextState = AI_STATE_IDLE;
			diskArmPosition = endDrawingPosition;
			me->getDisk()->endDraw(diskArmPosition);
		} else {
			diskArmPosition = startDrawingPosition * h1(curTimePercent) + endDrawingPosition * h2(curTimePercent) + startDrawingDirection * h3(curTimePercent) + endDrawingDirection * h4(curTimePercent);
		}
	} else {
		if (curTimePercent >= 1) {
			startTime = time;
			isDrawing = true;
			diskArmPosition = startDrawingPosition;
			me->getDisk()->startDraw(diskArmPosition);
		} else {
			diskArmPosition = startPosition * h1(curTimePercent) + startDrawingPosition * h2(curTimePercent) + startDirection * h3(curTimePercent) + startDrawingDirection * h4(curTimePercent);
		}
	}
	diskArmRotation = Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(90));
	
	headPosition = aiDefaultHeadPosition + Vec3f(osgCos(time / 1000.f), osgSin(time / 1000.f) * osgCos(time / 1000.f)) * 10;
	headRotation = Quaternion();
	shieldArmPosition = headPosition + Vec3f(25,-60,0);
	shieldArmRotation = Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(-90));
	return nextState;
}