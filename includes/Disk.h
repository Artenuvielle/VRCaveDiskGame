
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
	void setTargetOwnerPosition(Vec3f newPosition);
	Vec3f getTargetOwnerPosition();
	void setTargetEnemyPosition(Vec3f newPosition);
	Vec3f getTargetEnemyPosition();
	bool setRotation(Quaternion newRotation);
	Quaternion getRotation();
	DiskState getState();

	bool startDraw(Vec3f pos);
	bool endDraw(Vec3f pos);
	bool forceReturn();
	void update();
	Disk(PlayerFaction type);
private:
	void moveDiskAtLeastUntilCollision(Real32 deltaTime);
	Vec3f calculateMovement(Real32 deltaTime);
	void createAnimationAtCollisionPoint(Vec3f position, CollisionWallNormal direction);
	void createWallAnimationsAtPositionFacingDirection(Vec3f position, CollisionWallNormal direction);
	Vec3f momentum;
	Vec3f targetOwnerPosition;
	Vec3f targetEnemyPosition;
	Vec3f currentAxis;
	Real32 lastCollisionAngle;
	Real32 currentAngle;
	Real32 targetAngle;
	Real32 lastCollisionTime;
	Real32 axialRotationPerMillisecond;
	Real32 rotationAroundAxis;
	PlayerFaction diskType;
	Vec3f lastPositionWhileDrawn;
	Vec3f lastFowardVectorWhileDrawn;
	ComponentTransformRecPtr transform;
	Real32 lastPositionUpdateTime;
	Real32 lastRotationUpdateTime;
	DiskState state;
};

#endif