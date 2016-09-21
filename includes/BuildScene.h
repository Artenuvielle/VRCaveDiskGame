
#ifndef _BuildScene_H_
#define _BuildScene_H_

#include <vector>

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGImage.h>

OSG_USING_NAMESPACE

extern NodeRecPtr diskModelBlue;
extern NodeRecPtr diskModelOrange;
extern std::vector<ImageRecPtr> collisionImagesBlue;
extern std::vector<ImageRecPtr> collisionImagesOrange;

extern NodeTransitPtr buildScene();

#endif