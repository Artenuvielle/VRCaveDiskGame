#include "BuildScene.h"

#include <OpenSG/OSGSceneFileHandler.h>

#include "Common.h"

OSG_USING_NAMESPACE
	
NodeRecPtr diskModelPlayer;
NodeRecPtr diskModelEnemy;

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
	
	
	diskModelPlayer = SceneFileHandler::the()->read("models/disk.3DS");
	diskModelEnemy = SceneFileHandler::the()->read("models/disk2.3DS");
	
	// you will see a donut at the floor, slightly skewed, depending on head_position
	return NodeTransitPtr(root);
}
