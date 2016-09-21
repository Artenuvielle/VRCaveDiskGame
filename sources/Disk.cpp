#include "Disk.h"

#include <OpenSG/OSGSceneFileHandler.h>

#include "BuildScene.h"
#include "Animations.h"
#include "Common.h"

OSG_USING_NAMESPACE

const Real32 diskEnemyMomentumAttractionFactor = 0.8f; // deg/sec
const Real32 diskOwnerMomentumAttractionFactor = 1.0f; // deg/sec

Quaternion interpolateVector(Vec3f vec1, Vec3f vec2, Real32 factor);

Disk::Disk(PlayerFaction type) {
	diskType = type;
	NodeRecPtr model;
	if (diskType == PLAYER_FACTION_BLUE) {
		model = deepCloneTree(diskModelBlue);
	} else {
		model = deepCloneTree(diskModelOrange);
	}
	transform = ComponentTransform::create();
	transform->setTranslation(Vec3f(0,135,-540));
	transform->setScale(Vec3f(diskRadius, diskHeight * 10 / 2, diskRadius));

	ComponentTransformRecPtr innerTransform = ComponentTransform::create();
	innerTransform->setRotation(Quaternion(Vec3f(1,0,0),osgDegree2Rad(90)) * Quaternion(Vec3f(0,1,0),osgDegree2Rad(180)));

	NodeRecPtr transformNode = makeNodeFor(transform);
	NodeRecPtr innerTransformNode = makeNodeFor(innerTransform);
	innerTransformNode->addChild(model);
	transformNode->addChild(innerTransformNode);

	root->addChild(transformNode);

	state = DISK_STATE_READY;

	commitChanges();

	rotationAroundAxis = 0;
}

bool Disk::setPosition(Vec3f newPosition) {
	if(state == DISK_STATE_READY || state == DISK_STATE_DRAWN) {
		if (state == DISK_STATE_DRAWN) {
			momentum = 0.9f * momentum + (newPosition - lastPositionWhileDrawn);
			lastPositionWhileDrawn = newPosition;
		}
		transform->setTranslation(newPosition);
		return true;
	}
	return false;
}

Vec3f Disk::getPosition() {
	return transform->getTranslation();
}

void Disk::setTargetOwnerPosition(Vec3f newPosition) {
	targetOwnerPosition = newPosition;
}

Vec3f Disk::getTargetOwnerPosition() {
	return targetOwnerPosition;
}

void Disk::setTargetEnemyPosition(Vec3f newPosition) {
	targetEnemyPosition = newPosition;
}

Vec3f Disk::getTargetEnemyPosition() {
	return targetEnemyPosition;
}

bool Disk::setRotation(Quaternion newRotation) {
	Real32 time = glutGet(GLUT_ELAPSED_TIME);
	if(state == DISK_STATE_READY || state == DISK_STATE_DRAWN) {
		Vec3f newForward;
		newRotation.multVec(Vec3f(0,0,1), newForward);
		if (state == DISK_STATE_DRAWN) {
			rotationAroundAxis += osgRad2Degree(newForward.enclosedAngle(lastFowardVectorWhileDrawn)) / (time - lastRotationUpdateTime) * 1000;
		}
		lastFowardVectorWhileDrawn = newForward;
		transform->setRotation(newRotation);
		return true;
	}
	lastRotationUpdateTime = time;
	return false;
}

Quaternion Disk::getRotation() {
	return transform->getRotation();
}

DiskState Disk::getState() {
	return state;
}

bool Disk::startDraw(Vec3f position) {
	if(state == DISK_STATE_READY) {
		lastPositionWhileDrawn = position;
		momentum = Vec3f(0,0,0);
		state = DISK_STATE_DRAWN;
		std::cout << "started drawing a disk" << '\n';
		return true;
	}
	return false;
}

Vec3f getRectangularInterpolation(Vec3f from, Vec3f to) {
	Quaternion q(from,to);
	q.scaleAngle(90 / osgRad2Degree(from.enclosedAngle(to)));
	Vec3f result;
	q.multVec(from, result);
	return result;
	//return from.cross(to).cross(from);
}

bool Disk::endDraw(Vec3f position) {
	if(state == DISK_STATE_DRAWN) {
		state = DISK_STATE_FREE_FLY;
		momentum.normalize();
		lastCollisionAxis = currentAxis;
		targetAxis = currentAxis;//getRectangularInterpolation(momentum, currentAxis);
		std::cout << "finished drawing a disk... LET IF FLYYYYYY" << '\n';
		return true;
	}
	return false;
}

Vec3f Disk::calculateMovement(Real32 deltaTime) {
	return deltaTime * diskSpeed * momentum;
}

const Real32 WALL_Z_DIFF = WALL_Z_MAX - WALL_Z_MIN;

void Disk::updatePosition() {
	Real32 time = glutGet(GLUT_ELAPSED_TIME);
	transform->getRotation().multVec(Vec3f(0,1,0), currentAxis);
	if (state == DISK_STATE_FREE_FLY || state == DISK_STATE_RETURNING) {
		Vec3f vectorToTarget;
		Real32 diskMomentumAttractionFactor;
		if (state == DISK_STATE_FREE_FLY) {
			vectorToTarget = targetEnemyPosition - transform->getTranslation();
			diskMomentumAttractionFactor = diskEnemyMomentumAttractionFactor;
		} else {
			vectorToTarget = targetOwnerPosition - transform->getTranslation();
			diskMomentumAttractionFactor = diskOwnerMomentumAttractionFactor;
		}

		Real32 targetProximity = 1 - (vectorToTarget.length() / WALL_Z_DIFF);

		Quaternion axisRotation;
		if (time - lastCollisionTime < diskRotationTimeAfterCollision * 1000) {
			axisRotation = interpolateVector(lastCollisionAxis, targetAxis, (time - lastPositionUpdateTime) / (diskRotationTimeAfterCollision * 1000));
		} else {
			Quaternion rotation = interpolateVector(momentum, vectorToTarget, (time - lastPositionUpdateTime) / 1000 * diskMomentumAttractionFactor * targetProximity * targetProximity);
			rotation.multVec(momentum, momentum);
			rotation.multVec(targetAxis, targetAxis);
			targetAxis = getRectangularInterpolation(momentum, currentAxis);
			axisRotation = Quaternion(currentAxis, targetAxis);
		}
		Quaternion aroundAxisRotation(Vec3f(0,-1,0), osgDegree2Rad((time - lastPositionUpdateTime) * rotationAroundAxis / 1000 * diskAxisRotationFactor));
		transform->setRotation(transform->getRotation() * axisRotation /** aroundAxisRotation*/);
		//std::cout << "axis: " << currentAxis << "    targetAxis: " << targetAxis<< '\n';

		//interpolateVector(momentum, vectorToTarget, (time - lastPositionUpdateTime) / 1000 * diskMomentumAttractionFactor * targetProximity * targetProximity).multVec(momentum, momentum);
		moveDiskAtLeastUntilCollision(time - lastPositionUpdateTime);
		if (state == DISK_STATE_RETURNING) {
			if (diskType == mainUserFaction && transform->getTranslation().z() > targetOwnerPosition.z()) {
				rotationAroundAxis = 0;
				state = DISK_STATE_READY;
			} else if(diskType != mainUserFaction && transform->getTranslation().z() < targetOwnerPosition.z()) {
				rotationAroundAxis = 0;
				state = DISK_STATE_READY;
			}
		}
	}
	lastPositionUpdateTime = time;
}

void Disk::moveDiskAtLeastUntilCollision(Real32 deltaTime) {
	Real32 x,y,z;
	Real32 stepLengthPercentage = 1.f;
	Vec3f nextMomentum(momentum);
	Vec3f nextTargetAxis(currentAxis);
	bool collided = false;
	transform->getTranslation().getSeparateValues(x,y,z);
	transform->getRotation().multVec(Vec3f(0,1,0), currentAxis);
	Vec3f nearestXOffset = currentAxis.cross(Vec3f(1,0,0)).cross(currentAxis);
	Vec3f nearestYOffset = currentAxis.cross(Vec3f(0,1,0)).cross(currentAxis);
	Vec3f nearestZOffset = currentAxis.cross(Vec3f(0,0,1)).cross(currentAxis);
	nearestXOffset *= diskRadius / nearestXOffset.length();
	nearestYOffset *= diskRadius / nearestYOffset.length();
	nearestZOffset *= diskRadius / nearestZOffset.length();
	Vec3f moveVector = calculateMovement(deltaTime);
	// TODO: optimize to select only x values and then add them up
	if ((transform->getTranslation() + nearestXOffset + moveVector * stepLengthPercentage).x() > WALL_X_MAX) {
		nextMomentum = Vec3f(-nextMomentum.x(), nextMomentum.y(), nextMomentum.z());
		nextTargetAxis = Vec3f(nextMomentum.x(), 0, nextMomentum.z()).cross(nextMomentum);
		// Vec3f(nextTargetAxis.x(), -nextTargetAxis.y(), -nextTargetAxis.z());
		collided = true;
		stepLengthPercentage = (WALL_X_MAX - x - nearestXOffset.x()) / moveVector.x();
		createAnimationAtCollisionPoint(Vec3f(WALL_X_MAX,y,z), COLLISION_WALL_NORMAL_X);
	} else if((transform->getTranslation() - nearestXOffset + moveVector * stepLengthPercentage).x() < WALL_X_MIN) {
		nextMomentum = Vec3f(-nextMomentum.x(), nextMomentum.y(), nextMomentum.z());
		nextTargetAxis = Vec3f(nextMomentum.x(), 0, nextMomentum.z()).cross(nextMomentum);
		// Vec3f(nextTargetAxis.x(), -nextTargetAxis.y(), -nextTargetAxis.z());
		collided = true;
		stepLengthPercentage = (WALL_X_MIN - x + nearestXOffset.x()) / moveVector.x();
		createAnimationAtCollisionPoint(Vec3f(WALL_X_MIN,y,z), COLLISION_WALL_NORMAL_X);
	}

	if ((transform->getTranslation() + nearestYOffset + moveVector * stepLengthPercentage).y() > WALL_Y_MAX) {
		nextMomentum = Vec3f(nextMomentum.x(), -nextMomentum.y(), nextMomentum.z());
		nextTargetAxis = Vec3f(0, nextMomentum.y(), nextMomentum.z()).cross(nextMomentum);
		// Vec3f(-nextTargetAxis.x(), nextTargetAxis.y(), -nextTargetAxis.z());
		collided = true;
		stepLengthPercentage = (WALL_Y_MAX - y - nearestYOffset.y()) / moveVector.y();
		createAnimationAtCollisionPoint(Vec3f(x,WALL_Y_MAX,z), COLLISION_WALL_NORMAL_Y);
	} else if((transform->getTranslation() - nearestYOffset + moveVector * stepLengthPercentage).y() < WALL_Y_MIN) {
		nextMomentum = Vec3f(nextMomentum.x(), -nextMomentum.y(), nextMomentum.z());
		nextTargetAxis = Vec3f(0, nextMomentum.y(), nextMomentum.z()).cross(nextMomentum);
		// Vec3f(-nextTargetAxis.x(), nextTargetAxis.y(), -nextTargetAxis.z());
		collided = true;
		stepLengthPercentage = (WALL_Y_MIN - y + nearestYOffset.y()) / moveVector.y();
		createAnimationAtCollisionPoint(Vec3f(x,WALL_Y_MIN,z), COLLISION_WALL_NORMAL_Y);
	}
	
	if ((transform->getTranslation() + nearestZOffset + moveVector * stepLengthPercentage).z() > WALL_Z_MAX) {
		nextMomentum = Vec3f(nextMomentum.x(), nextMomentum.y(), -nextMomentum.z());
		nextTargetAxis = Vec3f(nextMomentum.x(), 0, nextMomentum.z()).cross(nextMomentum);
		// Vec3f(-nextTargetAxis.x(), -nextTargetAxis.y(), nextTargetAxis.z());
		collided = true;
		stepLengthPercentage = (WALL_Z_MAX - z - nearestZOffset.z()) / moveVector.z();
		createAnimationAtCollisionPoint(Vec3f(x,y,WALL_Z_MAX), COLLISION_WALL_NORMAL_Z);
		if (diskType != mainUserFaction) {
			state = DISK_STATE_RETURNING;
			std::cout << "enemy disk returning" << '\n';
		}
	} else if((transform->getTranslation() - nearestZOffset + moveVector * stepLengthPercentage).z() < WALL_Z_MIN) {
		nextMomentum = Vec3f(nextMomentum.x(), nextMomentum.y(), -nextMomentum.z());
		nextTargetAxis = Vec3f(nextMomentum.x(), 0, nextMomentum.z()).cross(nextMomentum);
		// Vec3f(-nextTargetAxis.x(), -nextTargetAxis.y(), nextTargetAxis.z());
		collided = true;
		stepLengthPercentage = (WALL_Z_MIN - z + nearestZOffset.z()) / moveVector.z();
		createAnimationAtCollisionPoint(Vec3f(x,y,WALL_Z_MIN), COLLISION_WALL_NORMAL_Z);
		if (diskType == mainUserFaction) {
			state = DISK_STATE_RETURNING;
			std::cout << "player disk returning" << '\n';
		}
	}
	// making sure disk won't get into walls at all...
	Vec3f newPosition = transform->getTranslation() + moveVector * stepLengthPercentage;
	newPosition = Vec3f(
		osgMin(osgMax(newPosition.x(), WALL_X_MIN + nearestXOffset.x()), WALL_X_MAX - nearestXOffset.x()),
		osgMin(osgMax(newPosition.y(), WALL_Y_MIN + nearestXOffset.y()), WALL_Y_MAX - nearestXOffset.y()),
		osgMin(osgMax(newPosition.z(), WALL_Z_MIN + nearestXOffset.z()), WALL_Z_MAX - nearestXOffset.z())
		);
	transform->setTranslation(newPosition);
	if (collided) {
		if (osgRad2Degree(currentAxis.enclosedAngle(nextTargetAxis)) > 180) {
			nextTargetAxis.negate();
		}
		nextTargetAxis.normalize();
		targetAxis = nextTargetAxis;
		lastCollisionAxis = currentAxis;
		momentum = nextMomentum;
		lastCollisionTime = glutGet(GLUT_ELAPSED_TIME);
		std::cout << "axis: " << currentAxis << "    targetAxis: " << targetAxis << '\n';
	}
}

const Real32 WALL_X_MID = (WALL_X_MAX + WALL_X_MIN) / 2;
const Real32 WALL_Y_MID = (WALL_Y_MAX + WALL_Y_MIN) / 2;
const Real32 WALL_Z_MID = (WALL_Z_MAX + WALL_Z_MIN) / 2;

Vec3f getWallNormal(Vec3f pos, CollisionWallNormal wall) {
	switch (wall)
	{
	case COLLISION_WALL_NORMAL_X:
		return Vec3f((pos.x() > WALL_X_MID ? -1 : 1), 0, 0);
		break;
	case COLLISION_WALL_NORMAL_Y:
		return Vec3f(0, (pos.y() > WALL_Y_MID ? -1 : 1), 0);
		break;
	case COLLISION_WALL_NORMAL_Z:
		return Vec3f(0, 0, (pos.z() > WALL_Z_MID ? -1 : 1));
		break;
	}
}

void Disk::createAnimationAtCollisionPoint(Vec3f position, CollisionWallNormal direction) {
	Real32 distXTop = osgAbs(WALL_X_MAX - position.x());
	Real32 distXBot = osgAbs(WALL_X_MIN - position.x());
	Real32 distYTop = osgAbs(WALL_Y_MAX - position.y());
	Real32 distYBot = osgAbs(WALL_Y_MIN - position.y());
	Real32 distZTop = osgAbs(WALL_Z_MAX - position.z());
	Real32 distZBot = osgAbs(WALL_Z_MIN - position.z());
	createWallAnimationsAtPositionFacingDirection(position, direction);
	switch (direction)
	{
	case COLLISION_WALL_NORMAL_X:
		if (distYTop < collisionAnimationSize / 2) {
			Vec3f newPosition(position.x() + (position.x() > WALL_X_MID ? 1 : -1) * distYTop, WALL_Y_MAX, position.z());
			createWallAnimationsAtPositionFacingDirection(newPosition, COLLISION_WALL_NORMAL_Y);
		}
		if (distYBot < collisionAnimationSize / 2) {
			Vec3f newPosition(position.x() + (position.x() > WALL_X_MID ? 1 : -1) * distYBot, WALL_Y_MIN, position.z());
			createWallAnimationsAtPositionFacingDirection(newPosition, COLLISION_WALL_NORMAL_Y);
		}
		if (distZTop < collisionAnimationSize / 2) {
			Vec3f newPosition(position.x() + (position.x() > WALL_X_MID ? 1 : -1) * distZTop, position.y(), WALL_Z_MAX);
			createWallAnimationsAtPositionFacingDirection(newPosition, COLLISION_WALL_NORMAL_Z);
		}
		if (distZBot < collisionAnimationSize / 2) {
			Vec3f newPosition(position.x() + (position.x() > WALL_X_MID ? 1 : -1) * distZBot, position.y(), WALL_Z_MIN);
			createWallAnimationsAtPositionFacingDirection(newPosition, COLLISION_WALL_NORMAL_Z);
		}
		break;
	case COLLISION_WALL_NORMAL_Y:
		if (distXTop < collisionAnimationSize / 2) {
			Vec3f newPosition(WALL_X_MAX, position.y() + (position.y() > WALL_Y_MID ? 1 : -1) * distXTop, position.z());
			createWallAnimationsAtPositionFacingDirection(newPosition, COLLISION_WALL_NORMAL_X);
		}
		if (distXBot < collisionAnimationSize / 2) {
			Vec3f newPosition(WALL_X_MIN, position.y() + (position.y() > WALL_Y_MID ? 1 : -1) * distXBot, position.z());
			createWallAnimationsAtPositionFacingDirection(newPosition, COLLISION_WALL_NORMAL_X);
		}
		if (distZTop < collisionAnimationSize / 2) {
			Vec3f newPosition(position.x(), position.y() + (position.y() > WALL_Y_MID ? 1 : -1) * distZTop, WALL_Z_MAX);
			createWallAnimationsAtPositionFacingDirection(newPosition, COLLISION_WALL_NORMAL_Z);
		}
		if (distZBot < collisionAnimationSize / 2) {
			Vec3f newPosition(position.x(), position.y() + (position.y() > WALL_Y_MID ? 1 : -1) * distZBot, WALL_Z_MIN);
			createWallAnimationsAtPositionFacingDirection(newPosition, COLLISION_WALL_NORMAL_Z);
		}
		break;
	case COLLISION_WALL_NORMAL_Z:
		if (distXTop < collisionAnimationSize / 2) {
			Vec3f newPosition(WALL_X_MAX, position.y(), position.z() + (position.z() > WALL_Z_MID ? 1 : -1) * distXTop);
			createWallAnimationsAtPositionFacingDirection(newPosition, COLLISION_WALL_NORMAL_X);
		}
		if (distXBot < collisionAnimationSize / 2) {
			Vec3f newPosition(WALL_X_MIN, position.y(), position.z() + (position.z() > WALL_Z_MID ? 1 : -1) * distXBot);
			createWallAnimationsAtPositionFacingDirection(newPosition, COLLISION_WALL_NORMAL_X);
		}
		if (distYTop < collisionAnimationSize / 2) {
			Vec3f newPosition(position.x(), WALL_Y_MAX, position.z() + (position.z() > WALL_Z_MID ? 1 : -1) * distYTop);
			createWallAnimationsAtPositionFacingDirection(newPosition, COLLISION_WALL_NORMAL_Y);
		}
		if (distYBot < collisionAnimationSize / 2) {
			Vec3f newPosition(position.x(), WALL_Y_MIN, position.z() + (position.z() > WALL_Z_MID ? 1 : -1) * distYBot);
			createWallAnimationsAtPositionFacingDirection(newPosition, COLLISION_WALL_NORMAL_Y);
		}
		break;
	}
}

void Disk::createWallAnimationsAtPositionFacingDirection(Vec3f position, CollisionWallNormal wall) {
	Vec3f wallNormal = getWallNormal(position, wall);
	// correction with wall normal for not intersecting with the box
	createWallCollisionAnimation(position + wallNormal * 1, collisionAnimationSize, collisionAnimationSize, wallNormal, diskType);
}

Quaternion interpolateVector(Vec3f vec1, Vec3f vec2, Real32 factor) {
	Vec3f rotationAxis = vec1.cross(vec2);
	Real32 angleToRotate = osgRad2Degree(vec2.enclosedAngle(vec1));
	//Real32 angleToRotate = osgACos(momentum.dot(directionToInterpolateTo) / (momentum.length() * directionToInterpolateTo.length()));
	// multiplies the quaternion with momentum and saves the result in momentum
	return Quaternion(rotationAxis, osgDegree2Rad(angleToRotate * factor));
}