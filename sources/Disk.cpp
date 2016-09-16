#include "Disk.h"

#include <OpenSG/OSGSceneFileHandler.h>

#include "BuildScene.h"
#include "Common.h"

OSG_USING_NAMESPACE

Disk::Disk(DiskType type) {
	diskType = type;
	NodeRecPtr model;
	if (diskType == DISK_TYPE_PLAYER) {
		model = deepCloneTree(diskModelPlayer);
	} else {
		model = deepCloneTree(diskModelEnemy);
	}
	transform = ComponentTransform::create();
	transform->setTranslation(Vec3f(0,135,0));
	transform->setScale(Vec3f(10.f,10.f,10.f));

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
		std::cout << "finished drawing a disk... LET IF FLYYYYYY" << '\n';
		return true;
	}
	return false;
}

void Disk::updatePosition() {
	Real32 time = glutGet(GLUT_ELAPSED_TIME);
	if(state == DISK_STATE_FREE_FLY) {
		checkWallCollision();
		transform->setTranslation(transform->getTranslation() + (time - lastPositionUpdateTime) / 100 * momentum);
	} else if (state == DISK_STATE_RETURNING) {
		checkWallCollision();
		interpolateReturningMomentum(time - lastPositionUpdateTime);
		transform->setTranslation(transform->getTranslation() + (time - lastPositionUpdateTime) / 100 * momentum);
	}
	lastPositionUpdateTime = time;
}

void Disk::checkWallCollision() {
	Real32 x,y,z;
	transform->getTranslation().getSeparateValues(x,y,z);
	if (x > 135 || x < -135) {
		momentum = Vec3f(-momentum.x(), momentum.y(), momentum.z());
	}
	if (y > 270 || y < 0) {
		momentum = Vec3f(momentum.x(), -momentum.y(), momentum.z());
	}
	if (z > 135 || z < -945) {
		momentum = Vec3f(momentum.x(), momentum.y(), -momentum.z());
		if (z < -945 && diskType == DISK_TYPE_PLAYER) {
			state = DISK_STATE_RETURNING;
			std::cout << "player disk returning" << '\n';
		} else if (z > 135 && diskType == DISK_TYPE_ENEMY) {
			state = DISK_STATE_RETURNING;
			std::cout << "enemy disk returning" << '\n';
		}
	}
}

void Disk::interpolateReturningMomentum(Real32 deltaTime) {
	Vec3f directionToInterpolateTo = targetReturningPosition - momentum;
	Vec3f rotationAxis = momentum.cross(directionToInterpolateTo);
	Real32 angleToRotate = osgACos(momentum.dot(directionToInterpolateTo) / (momentum.length() * directionToInterpolateTo.length()));
	// multiplies the quaternion with momentum and saves the result in momentum
	Quaternion(rotationAxis, angleToRotate * deltaTime / 1000).multVec(momentum, momentum);
}