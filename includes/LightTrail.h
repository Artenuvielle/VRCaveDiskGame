
#ifndef _LightTrail_H_
#define _LightTrail_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include "Common.h"

OSG_USING_NAMESPACE

class LightTrail {
public:
	static LightTrail* create(PlayerFaction faction, Vec3f initialPosition);
	static void finish(LightTrail* trail);
	static void updateTrails(Vec3f headPos);
	static void deleteAllTrailsInstantly();
	void addPoint(Vec3f pos);
private:
	static Vec3f headPosition;
	void addPointWithoutTest(Vec3f pos);
	void update();
	LightTrail(PlayerFaction faction, Vec3f initialPosition);
	~LightTrail();
	NodeRecPtr geoNode;
	GeometryRecPtr geo;
	GeoPnt3fPropertyRecPtr pos;
	GeoVec3fPropertyRecPtr norms;
	Vec3f lastPointAdded, secondLastPointAdded;
	Real32 timeOfLastPointAdded;
	std::vector<Vec3f> points;
};

#endif