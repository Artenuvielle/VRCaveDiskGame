
#ifndef _Simulation_H_
#define _Simulation_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OSGCSM/OSGCAVESceneManager.h>

OSG_USING_NAMESPACE

struct SimStep {
	Real32 timestamp;
	Vec3f head_position;
	Quaternion head_orientation;
	Vec3f wand_position;
	Quaternion wand_orientation;
	SimStep(Real32 t, Vec3f hp, Quaternion ho, Vec3f wp, Quaternion wo):timestamp(t), head_position(hp), head_orientation(ho), wand_position(wp), wand_orientation(wo){};
};

struct InputStep {
	Real32 timestamp;
	bool buttonPushed;
	InputStep(Real32 t, bool b):timestamp(t), buttonPushed(b){};
};

extern SimStep getSimulationStep(Real32 currentSimulationTimestamp);
extern InputStep getInputStep(Real32 currentSimulationTimestamp);
extern void initSimulation();

#endif