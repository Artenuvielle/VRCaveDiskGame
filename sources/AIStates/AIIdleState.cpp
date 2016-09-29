#include "AIStates/AIIdleState.h"

#include "Common.h"

OSG_USING_NAMESPACE

AIIdleState::AIIdleState(Player* me) : AIStateHandler(me) {

}

AIState AIIdleState::update() {
	Real32 time = glutGet(GLUT_ELAPSED_TIME);
	headPosition = aiDefaultHeadPosition + Vec3f(osgCos(time / 1000.f), osgSin(time / 1000.f) * osgCos(time / 1000.f)) * 10;
	headRotation = Quaternion();
	diskArmPosition = headPosition + Vec3f(-25,-60,0);
	diskArmRotation = Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(90));
	shieldArmPosition = headPosition + Vec3f(25,-60,0);
	shieldArmRotation = Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(-90));
	if (me->getDisk()->getState() == DISK_STATE_READY) {
		return AI_STATE_ATTACK;
	}
	return AI_STATE_IDLE;
}