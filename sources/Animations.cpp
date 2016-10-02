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
	blendChunk->setAlphaFunc(GL_NONE);
	blendChunk->setAlphaValue(0);   
	
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