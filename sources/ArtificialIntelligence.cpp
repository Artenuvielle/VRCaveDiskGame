#include "ArtificialIntelligence.h"

OSG_USING_NAMESPACE

const Vec3f aiDefaultHeadPosition = Vec3f(0,165,-810);

AI::AI(Player* aiTarget) {
	me = aiTarget;
	state = AI_STATE_IDLE;
	me->setHeadPosition(aiDefaultHeadPosition);
	me->setHeadRotation(Quaternion());
	me->setDiskArmPosition(aiDefaultHeadPosition + Vec3f(-25,-60,0));
	me->setDiskArmRotation(Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(90)));
	me->setShieldArmPosition(aiDefaultHeadPosition + Vec3f(25,-60,0));
	me->setShieldArmRotation(Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(-90)));
	targetHeadPosition = me->getHeadPosition();
	targetHeadRotation = me->getHeadRotation();
	targetDiskArmPosition = me->getDiskArmPosition();
	targetDiskArmRotation = me->getDiskArmRotation();
	targetShieldArmPosition = me->getShieldArmPosition();
	targetShieldArmRotation = me->getShieldArmRotation();
}

void AI::update() {
	Real32 time = glutGet(GLUT_ELAPSED_TIME);
	switch (state)
	{
	case AI_STATE_IDLE:
		targetHeadPosition = aiDefaultHeadPosition + Vec3f(osgCos(time / 1000.f), osgSin(time / 1000.f) * osgCos(time / 1000.f)) * 10;
		targetHeadRotation = Quaternion();
		targetDiskArmPosition = targetHeadPosition + Vec3f(-25,-60,0);
		targetDiskArmRotation = Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(90));
		targetShieldArmPosition = targetHeadPosition + Vec3f(25,-60,0);
		targetShieldArmRotation = Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(-90));
		break;
	default:
		break;
	}
	updateActualPositions();
}

void AI::updateActualPositions() {
	me->setHeadPosition(targetHeadPosition);
	me->setHeadRotation(targetHeadRotation);
	me->setDiskArmPosition(targetDiskArmPosition);
	me->setDiskArmRotation(targetDiskArmRotation);
	me->setShieldArmPosition(targetShieldArmPosition);
	me->setShieldArmRotation(targetShieldArmRotation);
}
