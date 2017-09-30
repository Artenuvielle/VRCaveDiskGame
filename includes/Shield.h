
#ifndef _Shield_H_
#define _Shield_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include "Common.h"

OSG_USING_NAMESPACE

class Shield {
public:
	void setPosition(Vec3f newPosition);
	Vec3f getPosition();
	void setRotation(Quaternion newRotation);
	Quaternion getRotation();
	void setRadius(Real32 newRadius);
	Real32 getRadius();
	void setCharges(Int32 newCharges);
	Int32 getCharges();
	
	void reduceCharges();
	void refillCharges();
	bool hasCharges();
	void update(Vec3f enemyDiskPosition);
	Shield(PlayerFaction type);
private:
	Int32 charges;
	ComponentTransformRecPtr transform;
};

#endif