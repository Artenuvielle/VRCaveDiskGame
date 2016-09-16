
#ifndef _Disk_H_
#define _Disk_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>

#include <OSGCSM/OSGCAVESceneManager.h>

OSG_USING_NAMESPACE

enum DiskState {
	DISK_STATE_READY = 0,
	DISK_STATE_DRAWN,
	DISK_STATE_FREE_FLY,
	DISK_STATE_RETURNING
};

class Disk {
public:
	bool setPosition(Vec3f newPosition);
	Vec3f getPosition();
	bool setRotation(Quaternion newRotation);
	Quaternion getRotation();
	DiskState getState();

	bool startDraw(Vec3f pos);
	bool endDraw(Vec3f pos);
	void updatePosition();
	Disk();
private:
	void checkWallCollision();
	Vec3f momentum;
	Vec3f lastPositionWhileDrawn;
	ComponentTransformRecPtr transform;
	Real32 lastPositionUpdateTime;
	DiskState state;
};

#endif