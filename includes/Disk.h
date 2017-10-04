
#ifndef _Disk_H_
#define _Disk_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include "Common.h"
#include "Observer.h"
#include "NetworkPackets.h"
#include "Shield.h"
#include "LightTrail.h"

OSG_USING_NAMESPACE

class DiskEventHandler {
public:
	virtual void handleDiskCatch() = 0;
};

class Disk : public Observable<GameNotifications> {
public:
	bool setPosition(Vec3f newPosition, bool force = false);
	Vec3f getPosition();
	Vec3f getMomentum();
	void setTargetOwnerPosition(Vec3f newPosition);
	Vec3f getTargetOwnerPosition();
	void setTargetEnemyPosition(Vec3f newPosition);
	Vec3f getTargetEnemyPosition();
	bool setRotation(Quaternion newRotation, bool force = false);
	void setEnemyShield(Shield* enemyShield);
	Quaternion getRotation();
	DiskState getState();

	bool startDraw(Vec3f pos);
	bool endDraw(Vec3f pos);
	bool forceReturn();
	bool forceThrow(Vec3f pos, Vec3f momentum);
	void catchDisk();
	void update();
	Disk(PlayerFaction type, DiskEventHandler* handler);
private:
	void updateLightTrails();
	void moveDiskAtLeastUntilWallCollision(Real32 deltaTime);
	Vec3f calculateMovement(Real32 deltaTime);
	bool collidesWithEnemyShield();
	DiskEventHandler* handler;
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
	LightTrail *lightTrailLeft, *lightTrailRight;
	Shield* enemyShield;
	DiskState state;
};

#endif