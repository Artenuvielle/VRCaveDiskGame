#include "Disk.h"

#include <OpenSG/OSGSceneFileHandler.h>

#include "BuildScene.h"
#include "Animations.h"
#include "Common.h"

OSG_USING_NAMESPACE

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

void Disk::setTargetReturningPosition(Vec3f newPosition) {
	targetReturningPosition = newPosition;
}

Vec3f Disk::getTargetReturningPosition() {
	return targetReturningPosition;
}

bool Disk::setRotation(Quaternion newRotation) {
	if(state == DISK_STATE_READY || state == DISK_STATE_DRAWN) {
		transform->setRotation(newRotation);
		return true;
	}
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

bool Disk::endDraw(Vec3f position) {
	if(state == DISK_STATE_DRAWN) {
		state = DISK_STATE_FREE_FLY;
		momentum.normalize();
		std::cout << "finished drawing a disk... LET IF FLYYYYYY" << '\n';
		return true;
	}
	return false;
}

Vec3f Disk::calculateMovement(Real32 deltaTime) {
	return deltaTime * diskSpeed * momentum;
}

void Disk::updatePosition() {
	Real32 time = glutGet(GLUT_ELAPSED_TIME);
	if(state == DISK_STATE_FREE_FLY) {
		moveDiskAtLeastUntilCollision(time - lastPositionUpdateTime);
		//transform->setTranslation(transform->getTranslation() + (time - lastPositionUpdateTime) * diskSpeed * momentum);
	} else if (state == DISK_STATE_RETURNING) {
		interpolateReturningMomentum(time - lastPositionUpdateTime);
		moveDiskAtLeastUntilCollision(time - lastPositionUpdateTime);
		//transform->setTranslation(transform->getTranslation() + (time - lastPositionUpdateTime) * diskSpeed * momentum);
		if (diskType == mainUserFaction && transform->getTranslation().z() > targetReturningPosition.z()) {
			state = DISK_STATE_READY;
		} else if(diskType != mainUserFaction && transform->getTranslation().z() < targetReturningPosition.z()) {
			state = DISK_STATE_READY;
		}
	}
	lastPositionUpdateTime = time;
}



void Disk::moveDiskAtLeastUntilCollision(Real32 deltaTime) {
	Real32 x,y,z;
	Real32 returnPercentage = 1.f;
	Vec3f nextMomentum(momentum);
	transform->getTranslation().getSeparateValues(x,y,z);
	Vec3f axis;
	transform->getRotation().multVec(Vec3f(0,1,0), axis);
	Vec3f nearestXOffset = axis.cross(Vec3f(1,0,0)).cross(axis);
	Vec3f nearestYOffset = axis.cross(Vec3f(0,1,0)).cross(axis);
	Vec3f nearestZOffset = axis.cross(Vec3f(0,0,1)).cross(axis);
	nearestXOffset *= diskRadius / nearestXOffset.length();
	nearestYOffset *= diskRadius / nearestYOffset.length();
	nearestZOffset *= diskRadius / nearestZOffset.length();
	Vec3f moveVector = calculateMovement(deltaTime);
	// TODO: optimize to select only x values and then add them up
	if ((transform->getTranslation() + nearestXOffset + moveVector * returnPercentage).x() > WALL_X_MAX) {
		nextMomentum = Vec3f(-nextMomentum.x(), nextMomentum.y(), nextMomentum.z());
		returnPercentage = (WALL_X_MAX - x - nearestXOffset.x()) / moveVector.x();
		createAnimationAtCollisionPoint(Vec3f(WALL_X_MAX,y,z), COLLISION_WALL_NORMAL_X);
	} else if((transform->getTranslation() - nearestXOffset + moveVector * returnPercentage).x() < WALL_X_MIN) {
		nextMomentum = Vec3f(-nextMomentum.x(), nextMomentum.y(), nextMomentum.z());
		returnPercentage = (WALL_X_MIN - x + nearestXOffset.x()) / moveVector.x();
		createAnimationAtCollisionPoint(Vec3f(WALL_X_MIN,y,z), COLLISION_WALL_NORMAL_X);
	}

	if ((transform->getTranslation() + nearestYOffset + moveVector * returnPercentage).y() > WALL_Y_MAX) {
		nextMomentum = Vec3f(nextMomentum.x(), -nextMomentum.y(), nextMomentum.z());
		returnPercentage = (WALL_Y_MAX - y - nearestYOffset.y()) / moveVector.y();
		createAnimationAtCollisionPoint(Vec3f(x,WALL_Y_MAX,z), COLLISION_WALL_NORMAL_Y);
	} else if((transform->getTranslation() - nearestYOffset + moveVector * returnPercentage).y() < WALL_Y_MIN) {
		nextMomentum = Vec3f(nextMomentum.x(), -nextMomentum.y(), nextMomentum.z());
		returnPercentage = (WALL_Y_MIN - y - nearestYOffset.y()) / moveVector.y();
		createAnimationAtCollisionPoint(Vec3f(x,WALL_Y_MIN,z), COLLISION_WALL_NORMAL_Y);
	}
	
	if ((transform->getTranslation() + nearestZOffset + moveVector * returnPercentage).z() > WALL_Z_MAX) {
		nextMomentum = Vec3f(nextMomentum.x(), nextMomentum.y(), -nextMomentum.z());
		returnPercentage = (WALL_Z_MAX - z - nearestZOffset.z()) / moveVector.z();
		createAnimationAtCollisionPoint(Vec3f(x,y,WALL_Z_MAX), COLLISION_WALL_NORMAL_Z);
		if (diskType != mainUserFaction) {
			state = DISK_STATE_RETURNING;
			std::cout << "enemy disk returning" << '\n';
		}
	} else if((transform->getTranslation() - nearestZOffset + moveVector * returnPercentage).z() < WALL_Z_MIN) {
		nextMomentum = Vec3f(nextMomentum.x(), nextMomentum.y(), -nextMomentum.z());
		returnPercentage = (WALL_Z_MIN - z + nearestZOffset.z()) / moveVector.z();
		createAnimationAtCollisionPoint(Vec3f(x,y,WALL_Z_MIN), COLLISION_WALL_NORMAL_Z);
		if (diskType == mainUserFaction) {
			state = DISK_STATE_RETURNING;
			std::cout << "player disk returning" << '\n';
		}
	}
	transform->setTranslation(transform->getTranslation() + moveVector * returnPercentage);
	momentum = nextMomentum;
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

void Disk::interpolateReturningMomentum(Real32 deltaTime) {
	Vec3f directionToInterpolateTo = targetReturningPosition - transform->getTranslation();
	Vec3f rotationAxis = momentum.cross(directionToInterpolateTo);
	Real32 angleToRotate = osgACos(momentum.dot(directionToInterpolateTo) / (momentum.length() * directionToInterpolateTo.length()));
	// multiplies the quaternion with momentum and saves the result in momentum
	Quaternion(rotationAxis, angleToRotate * deltaTime / 1000).multVec(momentum, momentum);
}