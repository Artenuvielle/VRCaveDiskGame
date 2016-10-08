#include "Animations.h"

#include "OpenSG/OSGTextureEnvChunk.h"

#include <list>

#include "BuildScene.h"

OSG_USING_NAMESPACE

std::list<AnimationData> allAnimations = std::list<AnimationData>();
int animationIdCounter = 0;
bool changedSceneGraph = false;

AnimationData createWallCollisionAnimation(Vec3f position, Real32 xsixe, Real32 ysize, Vec3f normal, PlayerFaction faction) {
	AnimationData newAnimation;
	newAnimation.id = animationIdCounter;
	newAnimation.startTime = glutGet(GLUT_ELAPSED_TIME);

	NodeRecPtr animationPlane = makePlane(xsixe, ysize, 1, 1);
	ComponentTransformRecPtr animationTransform = ComponentTransform::create();
	animationTransform->setTranslation(position);
	//Quaternion(Vec3f(0,1,0), normal);
	animationTransform->setRotation(Quaternion(Vec3f(0,0,1), normal));
	newAnimation.animationTransformNode = makeNodeFor(animationTransform);
	newAnimation.animationTransformNode->addChild(animationPlane);
	root->addChild(newAnimation.animationTransformNode);
	
	SimpleMaterialRecPtr chunkMaterial = SimpleMaterial::create();
	//std::cout << chunkMaterial->getChunk(0) << "\n";
	BlendChunkRecPtr blendChunk = BlendChunk::create();
	chunkMaterial->addChunk(blendChunk);
	blendChunk->setSrcFactor(GL_SRC_ALPHA);
	blendChunk->setDestFactor(GL_ONE_MINUS_SRC_ALPHA);
	//blendChunk->setAlphaFunc(GL_NONE);
	//blendChunk->setAlphaValue(0);
	
	newAnimation.textureToAnimate = TextureObjChunk::create();
	newAnimation.imageSet = (faction == PLAYER_FACTION_BLUE ? collisionImageBlue : collisionImageOrange);
	newAnimation.fps = 25;
	newAnimation.duration = 1000;
	newAnimation.textureToAnimate->setImage(newAnimation.imageSet);

	chunkMaterial->addChunk(newAnimation.textureToAnimate);

	TexGenChunkRecPtr texGenChunk = TexGenChunk::create();
	chunkMaterial->addChunk(texGenChunk);

	TextureEnvChunkRecPtr envChunk = TextureEnvChunk::create();
	//envChunk->setEnvMode( GL_DECAL );
	chunkMaterial->addChunk(envChunk);

	GeometryRecPtr planeGeo = dynamic_cast<Geometry*>(animationPlane->getCore());
	planeGeo->setMaterial(chunkMaterial);
	//planeGeo->setMaterial(newAnimation.textureToAnimate);

	allAnimations.push_front(newAnimation);

	//std::cout << "Created animation " << animationIdCounter << '\n';	
	animationIdCounter++;
	changedSceneGraph = true;
	return newAnimation;
}

void updateAnimations() {
	Int32 time = glutGet(GLUT_ELAPSED_TIME);
	for(std::list<AnimationData>::iterator iterator = allAnimations.begin(); iterator != allAnimations.end(); ++iterator) {
		if (time > iterator->startTime + iterator->duration) {
			root->subChild(iterator->animationTransformNode);
			changedSceneGraph = true;
			//std::cout << "Animation " << iterator->id << " ended" << '\n';
			iterator = allAnimations.erase(iterator);
			if (iterator != allAnimations.begin()) iterator--;
		} else {
			Int32 timePerFrame = osgAbs(1000 / iterator->fps);
			//Int32 timePerFrame = osgAbs(iterator->imageSet->getFrameDelay() * 1000);
			//Int32 timeInAnimation = (time - iterator->startTime) % (iterator->imageSet->size() * timePerFrame);
			Int32 timeInAnimation = (time - iterator->startTime) % (iterator->imageSet->getFrameCount() * timePerFrame);

			//iterator->imageSet->setData(iterator->images->at(osgAbs(timeInAnimation / timePerFrame))->getData());
			//std::cout << iterator->textureToAnimate->getImage()->isAlphaBinary() << '\n';
			iterator->textureToAnimate->setFrame(osgAbs(timeInAnimation / timePerFrame));
		}
	}

	if (changedSceneGraph) {
		commitChanges();
		changedSceneGraph = false;
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
