#include "AIStates/AIIdleState.h"

#include "Common.h"

OSG_USING_NAMESPACE

AIState AIIdleState::update(Player* me) {
	Real32 time = glutGet(GLUT_ELAPSED_TIME);
	headPosition = aiDefaultHeadPosition + Vec3f(osgCos(time / 1000.f), osgSin(time / 1000.f) * osgCos(time / 1000.f)) * 10;
	headRotation = Quaternion();
	diskArmPosition = headPosition + Vec3f(-25,-60,0);
	diskArmRotation = Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(90));
	shieldArmPosition = headPosition + Vec3f(25,-60,0);
	shieldArmRotation = Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(-90));
	return AI_STATE_IDLE;
}