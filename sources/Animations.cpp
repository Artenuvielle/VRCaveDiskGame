#include "Animations.h"

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

	newAnimation.textureToAnimate = SimpleTexturedMaterial::create();
	newAnimation.imageSet = &(faction == PLAYER_FACTION_BLUE ? collisionImagesBlue : collisionImagesOrange);
	newAnimation.fps = 25;
	newAnimation.duration = 1000;
	//newAnimation.animationImage = Image::create();
	//newAnimation.animationImage->setData(newAnimation.imageSet->at(10)->getData());
	//newAnimation.textureToAnimate->setImage(newAnimation.animationImage);
	newAnimation.textureToAnimate->setImage(newAnimation.imageSet->at(0));

	GeometryRecPtr planeGeo = dynamic_cast<Geometry*>(animationPlane->getCore());
	planeGeo->setMaterial(newAnimation.textureToAnimate);

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
			Int32 timeInAnimation = (time - iterator->startTime) % (iterator->imageSet->size() * timePerFrame);
			//iterator->animationImage->setData(iterator->imageSet->at(osgAbs(timeInAnimation / timePerFrame))->getData());
			//iterator->textureToAnimate->imageChanged();
			iterator->textureToAnimate->setImage(iterator->imageSet->at(osgAbs(timeInAnimation / timePerFrame)));
		}
	}

	if (changedSceneGraph) {
		commitChanges();
		changedSceneGraph = false;
	}
}