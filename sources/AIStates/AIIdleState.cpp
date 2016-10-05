#include "AIStates/AIIdleState.h"

#include "Common.h"

OSG_USING_NAMESPACE

AIIdleState::AIIdleState(Player* me) : AIStateHandler(me) {
	minTimeForAttack = glutGet(GLUT_ELAPSED_TIME) + (aiMinTimeUntilAttack + osgRand() * 1.5f) * 1000;
}

AIState AIIdleState::update() {
	Real32 time = glutGet(GLUT_ELAPSED_TIME);
	headPosition = aiDefaultHeadPosition + Vec3f(osgCos(time / 1000.f), osgSin(time / 1000.f) * osgCos(time / 1000.f)) * 10;
	Vec3f armDistance = me->getDiskArmPosition() - me->getShieldArmPosition();
	headRotation = Quaternion(Vec3f(0, 0, 1), me->getEnemy()->getHeadPosition() - me->getHeadPosition());
	Vec3f rotatedShoulderOffset(25,0,0);
	headRotation.multVec(rotatedShoulderOffset, rotatedShoulderOffset);
	diskArmPosition = headPosition + Vec3f(0,-60,0) - rotatedShoulderOffset;
	diskArmRotation = Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(90));
	shieldArmPosition = headPosition + Vec3f(0,-60,0) + rotatedShoulderOffset;
	shieldArmRotation = Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(-90));
	if (time > minTimeForAttack && me->getDisk()->getState() == DISK_STATE_READY) {
		return AI_STATE_ATTACK;
	}
	return AI_STATE_IDLE;
}