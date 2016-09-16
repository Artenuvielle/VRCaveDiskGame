#include "BuildScene.h"

#include <OpenSG\OSGSceneFileHandler.h>

#include "Common.h"

OSG_USING_NAMESPACE

NodeTransitPtr buildScene()
{
	NodeRecPtr root = Node::create();
	root->setCore(Group::create());
	
	boundingBoxModel = SceneFileHandler::the()->read("models/bbox.3ds");
	
	/*ComponentTransformRecPtr */boundingBoxModelCT = ComponentTransform::create();
	boundingBoxModelCT->setTranslation(Vec3f(0,135,-405));
	boundingBoxModelCT->setRotation(Quaternion(Vec3f(1,0,0),osgDegree2Rad(xangle)) * Quaternion(Vec3f(0,0,1),osgDegree2Rad(180)));
	//boundingBoxModelCT->setRotation(Quaternion(Vec3f(1,0,0),osgDegree2Rad(90)) * Quaternion(Vec3f(0,1,0),osgDegree2Rad(180)) * Quaternion(Vec3f(0,0,1),osgDegree2Rad(180)));
	boundingBoxModelCT->setScale(Vec3f(270.f,270.f,270.f));

	NodeRecPtr boundingBoxModelTrans = makeNodeFor(boundingBoxModelCT);
	boundingBoxModelTrans->addChild(boundingBoxModel);

	root->addChild(boundingBoxModelTrans);
	
	
	NodeRecPtr diskModel = SceneFileHandler::the()->read("models/disk.3ds");	
	movableTransform = ComponentTransform::create();
	movableTransform->setTranslation(Vec3f(0,135,0));
	movableTransform->setRotation(Quaternion(Vec3f(1,0,0),osgDegree2Rad(90)) * Quaternion(Vec3f(0,1,0),osgDegree2Rad(180)) * Quaternion(Vec3f(0,0,1),osgDegree2Rad(180)));
	movableTransform->setScale(Vec3f(10.f,10.f,10.f));

	NodeRecPtr diskModelTrans = makeNodeFor(movableTransform);
	diskModelTrans->addChild(diskModel);

	root->addChild(diskModelTrans);

	
	// you will see a donut at the floor, slightly skewed, depending on head_position
	return NodeTransitPtr(root);
}
