#include "AIStates/AIDefendState.h"

#include "Common.h"

OSG_USING_NAMESPACE

const Real32 aiDefendArmTorsoDistance = 35.f;
	
AIDefendState::AIDefendState(Player* me) : AIStateHandler(me) {
	startShieldArmPosition = me->getShieldArmPosition();
	startTime = glutGet(GLUT_ELAPSED_TIME);
	calculateNewTargetPosition();
}

Vec3f AIDefendState::calculateNewTargetPosition() {
	Vec3f p1 = me->getHeadPosition();//(me->getTorsoPosition() + me->getHeadPosition()) * 0.5f;
	Vec3f p2 = me->getEnemy()->getDisk()->getPosition();
	Vec3f targetDirection = p2 - p1;//Vec3f(p2.x(), 0, p2.z()) - Vec3f(p1.x(), 0, p1.z());
	targetDirection.normalize();
	targetDirection *= aiDefendArmTorsoDistance;
	return /*me->getTorsoPosition() +*/ targetDirection + p1;//Vec3f(0, (p1.y() + p2.y()) / 2, 0);
}

AIState AIDefendState::update() {
	Real32 time = glutGet(GLUT_ELAPSED_TIME);
	
	Vec3f rotatedShoulderOffset(25,0,0);
	headRotation.multVec(rotatedShoulderOffset, rotatedShoulderOffset);
	
	Real32 interpolationPercentage = time - startTime / 200;
	if (interpolationPercentage > 1) {
		targetShieldArmPosition = calculateNewTargetPosition();
		startTime = glutGet(GLUT_ELAPSED_TIME);
		interpolationPercentage -= 1.f;
	}
	shieldArmPosition = targetShieldArmPosition;//splineInterpolation(interpolationPercentage, startShieldArmPosition, targetShieldArmPosition, Vec3f(0,0,1), Vec3f(-1,0,0));
	Vec3f shieldArmDirection = me->getShieldArmPosition() - me->getTorsoPosition();
	shieldArmRotation = Quaternion(Vec3f(0,1,0), Vec3f(shieldArmDirection.x(), 0, shieldArmDirection.z()));// * Quaternion(Vec3f(1,0,0), osgDegree2Rad(-90));

	headPosition = aiDefaultHeadPosition + Vec3f(osgCos(time / 1000.f), osgSin(time / 1000.f) * osgCos(time / 1000.f)) * 10;
	headRotation = Quaternion(Vec3f(0,0,1), shieldArmDirection);
	headRotation.scaleAngle(0.5);
	diskArmPosition = me->getHeadPosition() + Vec3f(0,-60,0) - rotatedShoulderOffset;
	diskArmRotation = Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(90));
	if (me->getEnemy()->getDisk()->getState() == DISK_STATE_FREE_FLY) {
		return AI_STATE_DEFEND;
	} else {
		return AI_STATE_IDLE;
	}
}