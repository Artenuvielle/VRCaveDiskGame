
#ifndef _Common_H_
#define _Common_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>

#include <OSGCSM/OSGCAVESceneManager.h>

OSG_USING_NAMESPACE

extern NodeRecPtr root;
extern NodeRecPtr boundingBoxModel;
extern ComponentTransformRecPtr boundingBoxModelCT;
extern Real32 startTime;

#endif