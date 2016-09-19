#include "BuildScene.h"

#include <OpenSG/OSGSceneFileHandler.h>

#include "Common.h"

OSG_USING_NAMESPACE
	
NodeRecPtr diskModelBlue;
NodeRecPtr diskModelOrange;
std::vector<ImageRecPtr> collisionImagesBlue;
std::vector<ImageRecPtr> collisionImagesOrange;


NodeTransitPtr buildScene()
{
	root = Node::create();
	root->setCore(Group::create());
	
	boundingBoxModel = SceneFileHandler::the()->read("models/bbox.3DS");
	
	/*ComponentTransformRecPtr */boundingBoxModelCT = ComponentTransform::create();
	boundingBoxModelCT->setTranslation(Vec3f(0,135,-405));
	boundingBoxModelCT->setRotation(Quaternion(Vec3f(1,0,0),osgDegree2Rad(90)) * Quaternion(Vec3f(0,0,1),osgDegree2Rad(180)));
	//boundingBoxModelCT->setRotation(Quaternion(Vec3f(1,0,0),osgDegree2Rad(90)) * Quaternion(Vec3f(0,1,0),osgDegree2Rad(180)) * Quaternion(Vec3f(0,0,1),osgDegree2Rad(180)));
	boundingBoxModelCT->setScale(Vec3f(270.f,270.f,270.f));

	NodeRecPtr boundingBoxModelTrans = makeNodeFor(boundingBoxModelCT);
	boundingBoxModelTrans->addChild(boundingBoxModel);

	root->addChild(boundingBoxModelTrans);
	
	
	diskModelBlue = SceneFileHandler::the()->read("models/disk_blue.3DS");
	diskModelOrange = SceneFileHandler::the()->read("models/disk_orange.3DS");

	for (int i = 0; i < 25; i++) {
		ImageRecPtr image = Image::create();
		std::stringstream s;
		s << "models/wall_collision_blue/wall_collision_blue_000" << (i < 10 ? "0" : "") << i << ".png";
		image->read(s.str().c_str());
		collisionImagesBlue.push_back(image);
	}

	for (int i = 0; i < 25; i++) {
		ImageRecPtr image = Image::create();
		std::stringstream s;
		s << "models/wall_collision_orange/wall_collision_orange_000" << (i < 10 ? "0" : "") << i << ".png";
		image->read(s.str().c_str());
		collisionImagesOrange.push_back(image);
	}
	
	// you will see a donut at the floor, slightly skewed, depending on head_position
	return NodeTransitPtr(root);
}
