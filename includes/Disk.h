
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

enum DiskType {
	DISK_TYPE_PLAYER = 0,
	DISK_TYPE_ENEMY
};

class Disk {
public:
	bool setPosition(Vec3f newPosition);
	Vec3f getPosition();
	void setTargetReturningPosition(Vec3f newPosition);
	Vec3f getTargetReturningPosition();
	bool setRotation(Quaternion newRotation);
	Quaternion getRotation();
	DiskState getState();

	bool startDraw(Vec3f pos);
	bool endDraw(Vec3f pos);
	bool forceReturn();
	void updatePosition();
	Disk(DiskType type);
private:
	void checkWallCollision();
	void interpolateReturningMomentum(Real32 deltaTime);
	Vec3f momentum;
	Vec3f targetReturningPosition;
	DiskType diskType;
	Vec3f lastPositionWhileDrawn;
	ComponentTransformRecPtr transform;
	Real32 lastPositionUpdateTime;
	DiskState state;
};

#endif