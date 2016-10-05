#include "AIStates/AIAttackState.h"

#include "Common.h"

OSG_USING_NAMESPACE

const Real32 aiHandNormalVelocity = 1.88f; // m/s
const Real32 aiHandDrawVelocity = 1.0f; // m/s

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

Real32 quadRand(Real32 scale) {
	Real32 r = osgRand();
	return (r*scale*r*scale);
}

AIAttackState::AIAttackState(Player* me) : AIStateHandler(me) {
	startPosition = diskArmPosition;
	startDirection = Vec3f(0, 0, 0);
	
	Vec3f rotatedShoulderOffset(25,0,0);
	headRotation.multVec(rotatedShoulderOffset, rotatedShoulderOffset);

	Vec3f randomDrawStartDirection(osgRand() - 0.3f, (osgRand() - 0.7f) * 0.7f, (osgRand() - 0.8f) * 0.2f);
	randomDrawStartDirection.normalize();
	startDrawingPosition = me->getTorsoPosition() - rotatedShoulderOffset + randomDrawStartDirection * osgPow(osgRand(), 2) * 40;
	startDrawingPosition = getPositionForAIInBounds(startDrawingPosition);
	
	startDrawingDirection = Vec3f(0.f, 0.f, -1.f);
	
	Vec3f randomDrawEndDirection((osgRand() - 0.5f) * 0.5f, (osgRand() - 0.5f) * 0.5f, osgRand());
	randomDrawEndDirection.normalize();
	endDrawingPosition = startDrawingPosition + randomDrawEndDirection * (osgPow(osgRand(), 2) * 20 + 50);
	endDrawingPosition = getPositionForAIInBounds(endDrawingPosition);
	
	endDrawingDirection = endDrawingPosition - startDrawingPosition;
	//endDrawingDirection.normalize();
	
	distanceToDrawStart = splineLengthApproximation(4, startPosition, startDrawingPosition, startDirection, startDrawingDirection);
	distanceToDrawEnd = splineLengthApproximation(4, startDrawingPosition, endDrawingPosition, startDrawingDirection, endDrawingDirection);
	startTime = glutGet(GLUT_ELAPSED_TIME);
	isDrawing = false;
}

AIState AIAttackState::update() {
	Real32 time = glutGet(GLUT_ELAPSED_TIME);
	AIState nextState = AI_STATE_ATTACK;
	Real32 splinePercent = (time - startTime) / 1000;
	if (isDrawing) {
		if (splinePercent >= 1) {
			nextState = AI_STATE_IDLE;
			diskArmPosition = endDrawingPosition;
			me->getDisk()->endDraw(diskArmPosition);
		} else {
			diskArmPosition = splineInterpolation(splinePercent, startDrawingPosition, endDrawingPosition, startDrawingDirection, endDrawingDirection);
		}
	} else {
		if (splinePercent >= 1) {
			startTime = time;
			isDrawing = true;
			diskArmPosition = startDrawingPosition;
			me->getDisk()->startDraw(diskArmPosition);
		} else {
			diskArmPosition = splineInterpolation(splinePercent, startPosition, startDrawingPosition, startDirection, startDrawingDirection);
		}
	}
	Vec3f rotatedShoulderOffset(25,0,0);
	headRotation.multVec(rotatedShoulderOffset, rotatedShoulderOffset);
	Vec3f shoulderPosition = me->getTorsoPosition() - rotatedShoulderOffset;
	Vec3f armDirection = diskArmPosition - me->getTorsoPosition();
	armDirection.normalize();
	//std::cout << armDirection << "\n";
	if (isDrawing) {
		diskArmRotation = Quaternion(Vec3f(0,1,0), Vec3f(0,0,1).enclosedAngle(Vec3f(armDirection.x(), 0, armDirection.z())));
	} else {
		diskArmRotation = Quaternion(Vec3f(0,1,0), Vec3f(0,0,1).enclosedAngle(Vec3f(armDirection.x(), 0, armDirection.z()))) * Quaternion(Vec3f(1,0,0), osgDegree2Rad(90 * (1 - splinePercent)));
	}

	Vec3f ellbowDirection;
	diskArmRotation.multVec(Vec3f(0,0,-30), ellbowDirection);
	Vec3f ellbowPosition = diskArmPosition + ellbowDirection;
	Vec3f shoulderDirection = me->getTorsoPosition() - ellbowPosition;
	shoulderDirection.normalize();
	shoulderDirection *= 30;
	Vec3f nextShoulderPosition = ellbowPosition + shoulderDirection;
	Vec3f shoulderOffset = nextShoulderPosition - shoulderPosition;
	
	headPosition = me->getHeadPosition() + shoulderOffset;
	Vec3f armDistance = me->getDiskArmPosition() - me->getShieldArmPosition();
	headRotation = Quaternion(Vec3f(-1, 0, 0), Vec3f(armDistance.x(), 0, armDistance.z()));
	shieldArmPosition = headPosition + Vec3f(0,-60,0) + rotatedShoulderOffset;
	shieldArmRotation = Quaternion(Vec3f(1,0,0), osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1), osgDegree2Rad(-90));
	return nextState;
}