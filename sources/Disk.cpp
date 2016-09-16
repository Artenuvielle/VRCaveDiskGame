#include "Disk.h"

#include <OpenSG/OSGSceneFileHandler.h>

#include "BuildScene.h"
#include "Common.h"

OSG_USING_NAMESPACE

Disk::Disk() {
	NodeRecPtr model;
	transform = ComponentTransform::create();
	transform->setTranslation(Vec3f(0,135,0));
	transform->setRotation(Quaternion(Vec3f(1,0,0),osgDegree2Rad(90)) * Quaternion(Vec3f(0,1,0),osgDegree2Rad(180)) * Quaternion(Vec3f(0,0,1),osgDegree2Rad(180)));
	transform->setScale(Vec3f(10.f,10.f,10.f));

	NodeRecPtr transformNode = makeNodeFor(transform);
	transformNode->addChild(model);

	root->addChild(transformNode);

	state = DISK_STATE_READY;

	commitChanges();
}

bool Disk::setPosition(Vec3f pos) {
	if(state == DISK_STATE_READY || state == DISK_STATE_DRAWN) {
		transform->setTranslation(pos);
		return true;
	}
	return false;
}

Vec3f Disk::getPosition() {
	return transform->getTranslation();
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