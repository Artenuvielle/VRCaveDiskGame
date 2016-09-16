
#ifndef _Common_H_
#define _Common_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>

#include <OSGCSM/OSGCAVESceneManager.h>
#include <OSGCSM/OSGCAVEConfig.h>
#include <OSGCSM/appctrl.h>

OSG_USING_NAMESPACE

extern NodeRecPtr boundingBoxModel;
extern ComponentTransformRecPtr boundingBoxModelCT;
extern ComponentTransformRecPtr movableTransform;
extern Vec3f diskDirection;
extern Real32 startTime;
extern int xangle;

#endif