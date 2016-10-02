
#ifndef _Animations_H_
#define _Animations_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include "OpenSG/OSGSimpleTexturedMaterial.h"
#include <OpenSG/OSGTextureObjChunk.h>
#include <OpenSG/OSGImage.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include "Common.h"

OSG_USING_NAMESPACE

struct AnimationData {
	int id;
	Int32 startTime;
	Int32 fps;
	Real32 duration;
	NodeRecPtr animationTransformNode;
	TextureObjChunkRecPtr textureToAnimate;
	ImageRecPtr imageSet;
};

extern AnimationData createWallCollisionAnimation(Vec3f position, Real32 xsixe, Real32 ysize, Vec3f normal, PlayerFaction faction);
extern void updateAnimations();

#endif