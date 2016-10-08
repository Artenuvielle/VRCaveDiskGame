
#ifndef _BuildScene_H_
#define _BuildScene_H_

#include <vector>

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGImage.h>
#include <OSGCSM/OSGCAVESceneManager.h>
#include <OpenSG/OSGDirectionalLight.h>
#include <OpenSG/OSGSimpleTexturedMaterial.h>

OSG_USING_NAMESPACE

extern NodeRecPtr diskModelBlue;
extern NodeRecPtr diskModelOrange;

extern ImageRecPtr collisionImageBlue;
extern ImageRecPtr collisionImageOrange;

extern NodeRecPtr playerModelTorso;
extern NodeRecPtr playerModelHeadBlue;
extern NodeRecPtr playerModelHeadOrange;
extern NodeRecPtr playerModelArmBlue;
extern NodeRecPtr playerModelArmOrange;

extern SimpleMaterialRecPtr shieldTorusMaterialBlue;
extern SimpleMaterialRecPtr shieldTorusMaterialOrange;
extern SimpleMaterialRecPtr shieldRingMaterialBlue;
extern SimpleMaterialRecPtr shieldRingMaterialOrange;

extern SimpleTexturedMaterialRecPtr lightTrailMaterialBlue;
extern SimpleTexturedMaterialRecPtr lightTrailMaterialOrange;

extern DirectionalLightRecPtr headLight;

extern ComponentTransformRecPtr testTrans;

extern NodeTransitPtr buildScene();

#endif