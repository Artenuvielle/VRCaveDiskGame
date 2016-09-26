
#ifndef _BuildScene_H_
#define _BuildScene_H_

#include <vector>

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGImage.h>
#include <OSGCSM/OSGCAVESceneManager.h>

OSG_USING_NAMESPACE

extern NodeRecPtr diskModelBlue;
extern NodeRecPtr diskModelOrange;
extern std::vector<ImageRecPtr> collisionImagesBlue;
extern std::vector<ImageRecPtr> collisionImagesOrange;
extern NodeRecPtr playerModelTorso;
extern NodeRecPtr playerModelHeadBlue;
extern NodeRecPtr playerModelHeadOrange;
extern NodeRecPtr playerModelArmBlue;
extern NodeRecPtr playerModelArmOrange;

extern NodeTransitPtr buildScene();

#endif