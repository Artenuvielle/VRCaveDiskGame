#include "ArtificialIntelligence.h"

#include "AIStates/AIIdleState.h"
#include "AIStates/AIAttackState.h"
#include "AIStates/AIDefendState.h"
#include "AIStates/AICatchState.h"

OSG_USING_NAMESPACE

Real32 h1(Real32 x) { return 2*x*x*x-3*x*x+1; }
Real32 h2(Real32 x) { return -2*x*x*x+3*x*x; }
Real32 h3(Real32 x) { return x*x*x-2*x*x+x; }
Real32 h4(Real32 x) { return x*x*x-x*x; }

Vec3f splineInterpolation(Real32 t, Vec3f p0, Vec3f p1, Vec3f d0, Vec3f d1) {
	return p0*h1(t) + p1*h2(t) + d0*h3(t) + d1*h4(t);
}

Real32 splineLengthApproximation(Int32 steps, Vec3f p0, Vec3f p1, Vec3f d0, Vec3f d1) {
	Real32 res = 0;
	Vec3f lastPos = p0;
	for(Int32 i = 0; i < steps + 1; i++) {
		Vec3f newPos = splineInterpolation((i + 1.f) / (steps + 1.f), p0, p1, d0, d1);
		res += (newPos - lastPos).length();
		lastPos = newPos;
	}
	return res;
}

Vec3f getPositionForAIInBounds(Vec3f input) {
	return Vec3f(osgMax(WALL_X_MIN + 30,  osgMin(WALL_X_MAX - 30, input.x())),
	             osgMax(WALL_Y_MIN + 30,  osgMin(WALL_Y_MAX - 30, input.y())),
	             osgMax(WALL_Z_MIN + 30,  osgMin(WALL_Z_MIN + 270 - 30, input.z())));
}

AI::AI(Player* aiTarget) {
	me = aiTarget;
	state = AI_STATE_IDLE;
	stateHandler = new AIIdleState(me);
	me->setHeadPosition(aiDefaultHeadPosition);
	me->setHeadRotation(Quaternion());
	me->setDiskArmPosition(aiDefaultHeadPosition + Vec3f(-25,-60,0));
	me->setDiskArmRotation(Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(90)));
	me->setShieldArmPosition(aiDefaultHeadPosition + Vec3f(25,-60,0));
	me->setShieldArmRotation(Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(-90)));
}

Vec3f capMovement(Vec3f start, Vec3f end, Real32 maximalMovement) {
	Real32 distance = (end - start).length();
	if (distance >= maximalMovement) {
		return start + (end - start) * maximalMovement / distance;
	}
	return end;
}

Quaternion capRotation(Quaternion start, Quaternion end, Real32 maximalRotation) {
	Real32 distance = (end - start).length();
	if (distance >= maximalRotation) {
		return start + (end - start) * maximalRotation / distance;
	}
	return end;
}

void AI::resetState() {
	setState(state);
}

void AI::update() {
	AIState newState = stateHandler->update();
	Real32 time = glutGet(GLUT_ELAPSED_TIME);
	Real32 elapsedSeconds = (time - lastUpdateTime) / 1000.f;
	me->setHeadPosition(capMovement(me->getHeadPosition(), stateHandler->getHeadPosition(), aiHeadMaxSpeed * elapsedSeconds));
	me->setHeadRotation(capRotation(me->getHeadRotation(), stateHandler->getHeadRotation(), aiHeadMaxRotation * elapsedSeconds));
	me->setDiskArmPosition(capMovement(me->getDiskArmPosition(), stateHandler->getDiskArmPosition(), aiArmMaxSpeed * elapsedSeconds));
	me->setDiskArmRotation(capRotation(me->getDiskArmRotation(), stateHandler->getDiskArmRotation(), aiArmMaxRotation * elapsedSeconds));
	me->setShieldArmPosition(capMovement(me->getShieldArmPosition(), stateHandler->getShieldArmPosition(), aiArmMaxSpeed * elapsedSeconds));
	me->setShieldArmRotation(capRotation(me->getShieldArmRotation(), stateHandler->getShieldArmRotation(), aiArmMaxRotation * elapsedSeconds));
	if (!gameRunning) newState = AI_STATE_IDLE;
	if (newState != state) {
		setState(newState);
	}
	lastUpdateTime = time;
}

void AI::setState(AIState newState) {
	delete stateHandler;
	switch (newState)
	{
	case AI_STATE_ATTACK:
		stateHandler = new AIAttackState(me);
		break;
	case AI_STATE_DEFEND:
		stateHandler = new AIDefendState(me);
		break;
	case AI_STATE_CATCH:
		stateHandler = new AICatchState(me);
		break;
	case AI_STATE_IDLE:
	default:
		stateHandler = new AIIdleState(me);
		break;
	}
	state = newState;
	std::cout << "AI changed state to " << AIStateNames[state] << '\n';
}

AI::~AI() {
	delete stateHandler;
}

AIStateHandler::AIStateHandler(Player* me) : me(me) {
	headPosition = me->getHeadPosition();
	headRotation = me->getHeadRotation();
	diskArmPosition = me->getDiskArmPosition();
	diskArmRotation = me->getDiskArmRotation();
	shieldArmPosition = me->getShieldArmPosition();
	shieldArmRotation = me->getShieldArmRotation();
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
