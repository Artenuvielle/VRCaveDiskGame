#include "Disk.h"

#include <OpenSG/OSGSceneFileHandler.h>

#include "BuildScene.h"
#include "Common.h"

OSG_USING_NAMESPACE

Disk::Disk() {
	NodeRecPtr model = deepCloneTree(diskModel);
	transform = ComponentTransform::create();
	transform->setTranslation(Vec3f(0,135,0));
	transform->setScale(Vec3f(10.f,10.f,10.f));

	ComponentTransformRecPtr innerTransform = ComponentTransform::create();
	innerTransform->setRotation(Quaternion(Vec3f(1,0,0),osgDegree2Rad(90)));

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
		transform->setTranslation(newPosition);
		return true;
	}
	return false;
}

Vec3f Disk::getPosition() {
	return transform->getTranslation();
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

bool Disk::startDraw(Vec3f pos) {
	if(state == DISK_STATE_READY) {
		return true;
	}
	return false;
}

bool Disk::endDraw(Vec3f pos) {
	if(state == DISK_STATE_DRAWN) {
		return true;
	}
	return false;
}