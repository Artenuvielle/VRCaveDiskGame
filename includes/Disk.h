
#ifndef _Disk_H_
#define _Disk_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include "Common.h"

OSG_USING_NAMESPACE

enum DiskState {
	DISK_STATE_READY = 0,
	DISK_STATE_DRAWN,
	DISK_STATE_FREE_FLY,
	DISK_STATE_RETURNING
};

enum CollisionWallNormal {
	COLLISION_WALL_NORMAL_X = 0,
	COLLISION_WALL_NORMAL_Y,
	COLLISION_WALL_NORMAL_Z
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
	Disk(PlayerFaction type);
	void createAnimationAtCollisionPoint(Vec3f position, CollisionWallNormal direction);
private:
	void moveDiskAtLeastUntilCollision(Real32 deltaTime);
	void interpolateReturningMomentum(Real32 deltaTime);
	Vec3f calculateMovement(Real32 deltaTime);
	//void createAnimationAtCollisionPoint(Vec3f position, CollisionWallNormal direction);
	void createWallAnimationsAtPositionFacingDirection(Vec3f position, CollisionWallNormal direction);
	Vec3f momentum;
	Vec3f targetReturningPosition;
	PlayerFaction diskType;
	Vec3f lastPositionWhileDrawn;
	ComponentTransformRecPtr transform;
	Real32 lastPositionUpdateTime;
	DiskState state;
};

#endif