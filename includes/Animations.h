
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
	bool loop;
	NodeRecPtr animationTransformNode;
	TextureObjChunkRecPtr textureToAnimate;
	ImageRecPtr imageSet;
};

enum CollisionWallNormal {
	COLLISION_WALL_NORMAL_X = 0,
	COLLISION_WALL_NORMAL_Y,
	COLLISION_WALL_NORMAL_Z
};

extern int createWallCollisionAnimation(Vec3f position, Real32 xsize, Real32 ysize, Vec3f normal, PlayerFaction faction);
extern int createAnimation(Vec3f position, Real32 xsize, Real32 ysize, Vec3f direction, ImageRecPtr image, Int32 fps, Real32 duration, bool loop);
extern void updateAnimations();
extern void endAnimation(int id);

extern void createAnimationAtCollisionPoint(Vec3f position, Real32 size, CollisionWallNormal direction, PlayerFaction faction);
extern void createWallAnimationsAtPositionFacingDirection(Vec3f position, Real32 size, CollisionWallNormal wall, PlayerFaction faction);
extern void createWallAnimationsForScores(PlayerFaction faction);

#endif