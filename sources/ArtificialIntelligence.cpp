#include "ArtificialIntelligence.h"

#include "AIStates/AIIdleState.h"

OSG_USING_NAMESPACE

AI::AI(Player* aiTarget) {
	me = aiTarget;
	state = AI_STATE_IDLE;
	stateHandler = new AIIdleState();
	me->setHeadPosition(aiDefaultHeadPosition);
	me->setHeadRotation(Quaternion());
	me->setDiskArmPosition(aiDefaultHeadPosition + Vec3f(-25,-60,0));
	me->setDiskArmRotation(Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(90)));
	me->setShieldArmPosition(aiDefaultHeadPosition + Vec3f(25,-60,0));
	me->setShieldArmRotation(Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(-90)));
}

void AI::update() {
	AIState newState = stateHandler->update(me);
	me->setHeadPosition(stateHandler->getHeadPosition());
	me->setHeadRotation(stateHandler->getHeadRotation());
	me->setDiskArmPosition(stateHandler->getDiskArmPosition());
	me->setDiskArmRotation(stateHandler->getDiskArmRotation());
	me->setShieldArmPosition(stateHandler->getShieldArmPosition());
	me->setShieldArmRotation(stateHandler->getShieldArmRotation());
	if (newState != state) {
		delete stateHandler;
		switch (newState)
		{
		case AI_STATE_IDLE:
		default:
			stateHandler = new AIIdleState();
			break;
		}
		state = newState;
	}
}

AI::~AI() {
	delete stateHandler;
}

Vec3f AIStateHandler::getHeadPosition() {
	return headPosition;
}

Vec3f AIStateHandler::getDiskArmPosition() {
	return diskArmPosition;
}

Vec3f AIStateHandler::getShieldArmPosition() {
	return shieldArmPosition;
}

Quaternion AIStateHandler::getHeadRotation() {
	return headRotation;
}

Quaternion AIStateHandler::getDiskArmRotation() {
	return diskArmRotation;
}

Quaternion AIStateHandler::getShieldArmRotation() {
	return shieldArmRotation;
}
