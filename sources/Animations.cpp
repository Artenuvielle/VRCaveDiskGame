#include "Animations.h"

#include "OpenSG/OSGTextureEnvChunk.h"

#include <list>

#include "BuildScene.h"

OSG_USING_NAMESPACE

struct AnimationParams {
	int id;
	Vec3f position;
	Real32 xsize;
	Real32 ysize;
	Vec3f normal;
	ImageRecPtr image;
	Int32 fps;
	Real32 duration;
	bool loop;
};

std::list<AnimationData> allAnimations = std::list<AnimationData>();
std::list<AnimationParams> newAnimations = std::list<AnimationParams>();
int animationIdCounter = 0;
bool changedSceneGraph = false;

int createWallCollisionAnimation(Vec3f position, Real32 xsize, Real32 ysize, Vec3f normal, PlayerFaction faction) {
	ImageRecPtr image = (faction == PLAYER_FACTION_BLUE ? collisionImageBlue : collisionImageOrange);
	return createAnimation(position, xsize, ysize, normal, image, 25, 1000, true);
}

void processNewlyQueuedAnimations() {
	while (!newAnimations.empty()) {
		AnimationParams params = newAnimations.front();
		AnimationData newAnimation;
		newAnimation.id = params.id;
		newAnimation.startTime = glutGet(GLUT_ELAPSED_TIME);
		NodeRecPtr animationPlane = makePlane(params.xsize, params.ysize, 1, 1);
		ComponentTransformRecPtr animationTransform = ComponentTransform::create();
		animationTransform->setTranslation(params.position);
		animationTransform->setRotation(Quaternion(Vec3f(0,0,1), params.normal));
		newAnimation.animationTransformNode = makeNodeFor(animationTransform);
		newAnimation.animationTransformNode->addChild(animationPlane);
		root->addChild(newAnimation.animationTransformNode);
	
		SimpleMaterialRecPtr chunkMaterial = SimpleMaterial::create();
		BlendChunkRecPtr blendChunk = BlendChunk::create();
		chunkMaterial->addChunk(blendChunk);
		blendChunk->setSrcFactor(GL_SRC_ALPHA);
		blendChunk->setDestFactor(GL_ONE_MINUS_SRC_ALPHA);
	
		newAnimation.textureToAnimate = TextureObjChunk::create();
		ImageRecPtr imageSet(params.image);
		newAnimation.imageSet = imageSet;
		newAnimation.fps = params.fps;
		newAnimation.duration = params.duration;
		newAnimation.loop = params.loop;
		newAnimation.textureToAnimate->setImage(newAnimation.imageSet);
		newAnimation.textureToAnimate->setFrame(0);

		chunkMaterial->addChunk(newAnimation.textureToAnimate);

		TexGenChunkRecPtr texGenChunk = TexGenChunk::create();
		chunkMaterial->addChunk(texGenChunk);

		TextureEnvChunkRecPtr envChunk = TextureEnvChunk::create();
		chunkMaterial->addChunk(envChunk);

		GeometryRecPtr planeGeo = dynamic_cast<Geometry*>(animationPlane->getCore());
		planeGeo->setMaterial(chunkMaterial);
	
		allAnimations.push_front(newAnimation);
		newAnimations.pop_front();
		changedSceneGraph = true;
	}
}

int createAnimation(Vec3f position, Real32 xsize, Real32 ysize, Vec3f normal, ImageRecPtr image, Int32 fps, Real32 duration, bool loop) {
	AnimationParams newAnimationParams;
	newAnimationParams.id = animationIdCounter;
	animationIdCounter++;
	newAnimationParams.position = position;
	newAnimationParams.xsize = xsize;
	newAnimationParams.ysize = ysize;
	newAnimationParams.normal = normal;
	newAnimationParams.image = image;
	newAnimationParams.fps = fps;
	newAnimationParams.duration = duration;
	newAnimationParams.loop = loop;
	
	newAnimations.push_front(newAnimationParams);

	return newAnimationParams.id;
}

void updateAnimations() {
	Int32 time = glutGet(GLUT_ELAPSED_TIME);
	processNewlyQueuedAnimations();
	for(std::list<AnimationData>::iterator iterator = allAnimations.begin(); iterator != allAnimations.end(); ++iterator) {
		if (time > iterator->startTime + iterator->duration) {
			root->subChild(iterator->animationTransformNode);
			changedSceneGraph = true;
			//std::cout << "Animation " << iterator->id << " ended" << '\n';
			iterator = allAnimations.erase(iterator);
			if (iterator != allAnimations.begin()) iterator--;
		} else {
			Int32 timePerFrame = osgAbs(1000 / iterator->fps);
			Int32 timeInAnimation;
			if (!iterator->loop) {
				timeInAnimation = osgMin(time - iterator->startTime, iterator->imageSet->getFrameCount() * timePerFrame - 1);
			} else {
				timeInAnimation = (time - iterator->startTime) % (iterator->imageSet->getFrameCount() * timePerFrame);
			}
			iterator->textureToAnimate->setFrame(osgAbs(timeInAnimation / timePerFrame));
		}
	}

	if (changedSceneGraph) {
		commitChanges();
		changedSceneGraph = false;
	}
}

void endAnimation(int id) {
	for(std::list<AnimationData>::iterator iterator = allAnimations.begin(); iterator != allAnimations.end(); ++iterator) {
		if (iterator->id == id) {
			root->subChild(iterator->animationTransformNode);
			changedSceneGraph = true;
			allAnimations.erase(iterator);
			return;
		}
	}
}

void createAnimationAtCollisionPoint(Vec3f position, Real32 size, CollisionWallNormal direction, PlayerFaction faction) {
	Real32 distXTop = osgAbs(WALL_X_MAX - position.x());
	Real32 distXBot = osgAbs(WALL_X_MIN - position.x());
	Real32 distYTop = osgAbs(WALL_Y_MAX - position.y());
	Real32 distYBot = osgAbs(WALL_Y_MIN - position.y());
	Real32 distZTop = osgAbs(WALL_Z_MAX - position.z());
	Real32 distZBot = osgAbs(WALL_Z_MIN - position.z());
	createWallAnimationsAtPositionFacingDirection(position, size, direction, faction);
	switch (direction)
	{
	case COLLISION_WALL_NORMAL_X:
		if (distYTop < size / 2) {
			Vec3f newPosition(position.x() + (position.x() > WALL_X_MID ? 1 : -1) * distYTop, WALL_Y_MAX, position.z());
			createWallAnimationsAtPositionFacingDirection(newPosition, size, COLLISION_WALL_NORMAL_Y, faction);
		}
		if (distYBot < size / 2) {
			Vec3f newPosition(position.x() + (position.x() > WALL_X_MID ? 1 : -1) * distYBot, WALL_Y_MIN, position.z());
			createWallAnimationsAtPositionFacingDirection(newPosition, size, COLLISION_WALL_NORMAL_Y, faction);
		}
		if (distZTop < size / 2) {
			Vec3f newPosition(position.x() + (position.x() > WALL_X_MID ? 1 : -1) * distZTop, position.y(), WALL_Z_MAX);
			createWallAnimationsAtPositionFacingDirection(newPosition, size, COLLISION_WALL_NORMAL_Z, faction);
		}
		if (distZBot < size / 2) {
			Vec3f newPosition(position.x() + (position.x() > WALL_X_MID ? 1 : -1) * distZBot, position.y(), WALL_Z_MIN);
			createWallAnimationsAtPositionFacingDirection(newPosition, size, COLLISION_WALL_NORMAL_Z, faction);
		}
		break;
	case COLLISION_WALL_NORMAL_Y:
		if (distXTop < size / 2) {
			Vec3f newPosition(WALL_X_MAX, position.y() + (position.y() > WALL_Y_MID ? 1 : -1) * distXTop, position.z());
			createWallAnimationsAtPositionFacingDirection(newPosition, size, COLLISION_WALL_NORMAL_X, faction);
		}
		if (distXBot < size / 2) {
			Vec3f newPosition(WALL_X_MIN, position.y() + (position.y() > WALL_Y_MID ? 1 : -1) * distXBot, position.z());
			createWallAnimationsAtPositionFacingDirection(newPosition, size, COLLISION_WALL_NORMAL_X, faction);
		}
		if (distZTop < size / 2) {
			Vec3f newPosition(position.x(), position.y() + (position.y() > WALL_Y_MID ? 1 : -1) * distZTop, WALL_Z_MAX);
			createWallAnimationsAtPositionFacingDirection(newPosition, size, COLLISION_WALL_NORMAL_Z, faction);
		}
		if (distZBot < size / 2) {
			Vec3f newPosition(position.x(), position.y() + (position.y() > WALL_Y_MID ? 1 : -1) * distZBot, WALL_Z_MIN);
			createWallAnimationsAtPositionFacingDirection(newPosition, size, COLLISION_WALL_NORMAL_Z, faction);
		}
		break;
	case COLLISION_WALL_NORMAL_Z:
		if (distXTop < size / 2) {
			Vec3f newPosition(WALL_X_MAX, position.y(), position.z() + (position.z() > WALL_Z_MID ? 1 : -1) * distXTop);
			createWallAnimationsAtPositionFacingDirection(newPosition, size, COLLISION_WALL_NORMAL_X, faction);
		}
		if (distXBot < size / 2) {
			Vec3f newPosition(WALL_X_MIN, position.y(), position.z() + (position.z() > WALL_Z_MID ? 1 : -1) * distXBot);
			createWallAnimationsAtPositionFacingDirection(newPosition, size, COLLISION_WALL_NORMAL_X, faction);
		}
		if (distYTop < size / 2) {
			Vec3f newPosition(position.x(), WALL_Y_MAX, position.z() + (position.z() > WALL_Z_MID ? 1 : -1) * distYTop);
			createWallAnimationsAtPositionFacingDirection(newPosition, size, COLLISION_WALL_NORMAL_Y, faction);
		}
		if (distYBot < size / 2) {
			Vec3f newPosition(position.x(), WALL_Y_MIN, position.z() + (position.z() > WALL_Z_MID ? 1 : -1) * distYBot);
			createWallAnimationsAtPositionFacingDirection(newPosition, size, COLLISION_WALL_NORMAL_Y, faction);
		}
		break;
	}
}

Vec3f getWallNormal(Vec3f pos, CollisionWallNormal wall) {
	switch (wall)
	{
	case COLLISION_WALL_NORMAL_X:
		return Vec3f((pos.x() > WALL_X_MID ? -1 : 1), 0, 0);
		break;
	case COLLISION_WALL_NORMAL_Y:
		return Vec3f(0, (pos.y() > WALL_Y_MID ? -1 : 1), 0);
		break;
	case COLLISION_WALL_NORMAL_Z:
		return Vec3f(0, 0, (pos.z() > WALL_Z_MID ? -1 : 1));
		break;
	}
}

void createWallAnimationsAtPositionFacingDirection(Vec3f position, Real32 size, CollisionWallNormal wall, PlayerFaction faction) {
	Vec3f wallNormal = getWallNormal(position, wall);
	// correction with wall normal for not intersecting with the box
	createWallCollisionAnimation(position + wallNormal * (1.f - osgRand() * 0.3), size, size, wallNormal, faction);
}

void createWallAnimationsForScores(PlayerFaction faction) {
	Vec3f position = Vec3f(WALL_X_MID, WALL_Y_MID, faction == userFaction ? WALL_Z_MAX : WALL_Z_MIN);
	CollisionWallNormal direction = COLLISION_WALL_NORMAL_Z;
	createAnimationAtCollisionPoint(position, scoreAnimationSize, direction, faction);
}
