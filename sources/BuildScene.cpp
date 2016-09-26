#include "BuildScene.h"

#include <OpenSG/OSGSceneFileHandler.h>

#include <OpenSG/OSGGeoBuilder.h>
#include <OpenSG/OSGMultiPassMaterial.h>
#include <OpenSG/OSGPolygonChunk.h>
#include <OpenSG/OSGShaderProgram.h>
#include <OpenSG/OSGShaderProgramChunk.h>
#include <OpenSG/OSGShaderProgramVariableChunk.h>

#include "Common.h"

OSG_USING_NAMESPACE
	
NodeRecPtr diskModelBlue;
NodeRecPtr diskModelOrange;
std::vector<ImageRecPtr> collisionImagesBlue;
std::vector<ImageRecPtr> collisionImagesOrange;
NodeRecPtr playerModelTorso;
NodeRecPtr playerModelHeadBlue;
NodeRecPtr playerModelHeadOrange;
NodeRecPtr playerModelArmBlue;
NodeRecPtr playerModelArmOrange;


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
	playerModelTorso = SceneFileHandler::the()->read("models/robot_torso.OBJ");
	playerModelHeadBlue = SceneFileHandler::the()->read("models/robot_head_blue.OBJ");
	playerModelHeadOrange = SceneFileHandler::the()->read("models/robot_head_orange.OBJ");
	playerModelArmBlue = SceneFileHandler::the()->read("models/robot_arm_blue.OBJ");
	playerModelArmOrange = SceneFileHandler::the()->read("models/robot_arm_orange.OBJ");


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

	enemyPoint = ComponentTransform::create();
	enemyPoint->setTranslation(Vec3f(0,167,-900));
	NodeRecPtr enemyTrans = makeNodeFor(enemyPoint);
	enemyTrans->addChild(makeSphere(1,10));
	root->addChild(enemyTrans);

	GeoBuilder builder;
	builder.begin(GL_QUAD_STRIP);
	Real32 r = 10;
	int n = 4;
	Real32 b = osgDegree2Rad(360 / n * -1);
	for (int i = 0; i <= n; i++) {
		Real32 a = osgDegree2Rad(360 / n * i);
		builder.fullVertex(Pnt3f(r * osgSin(a), 10, r * -osgCos(a)), Vec3f(osgSin(a), 0, -osgCos(a)), Color3f(1,1,1));
		builder.fullVertex(Pnt3f(r * osgSin(a), -10, r * -osgCos(a)), Vec3f(osgSin(a), 0, -osgCos(a)), Color3f(1,1,1));
		//builder.fullVertex(Pnt3f(r * osgCos(a), 40, r * osgSin(a)), Vec3f(osgCos(a), 0, osgSin(a)), Color3f(1,0,0));
		std::cout << Pnt3f(r * osgCos(a), 40, r * osgSin(a)) << "   " << Vec3f(osgCos(a), 0, osgSin(a)) << '\n';
		b = a;
	}
	/*builder.fullVertex(Pnt3f(10, 10, 10), Vec3f(0, 1, 0), Color3f(0,0,1));
	builder.fullVertex(Pnt3f(-10, 10, 10), Vec3f(0, 1, 0), Color3f(1,0,1));
	builder.fullVertex(Pnt3f(10, 0, 10), Vec3f(0, 0, 1), Color3f(0,1,1));
	builder.fullVertex(Pnt3f(-10, 0, 10), Vec3f(0, 0, 1), Color3f(0,1,1));
	builder.fullVertex(Pnt3f(10, -10, 10), Vec3f(0, -1, 0), Color3f(0,1,1));
	builder.fullVertex(Pnt3f(-10, -10, 10), Vec3f(0, -1, 0), Color3f(0,1,1));
	*/builder.end();
	NodeRecPtr testModel = makeNodeFor(builder.getGeometry());//makeCylinder(80,10,20,true,true,true);//makeBox(10,10,30,1,1,1);
	ComponentTransformRecPtr testTrans = ComponentTransform::create();
	testTrans->setTranslation(Vec3f(0,135,15));
	//testTrans->setRotation(Quaternion(Vec3f(1,0,0), osgDegree2Rad(-90)) * Quaternion(Vec3f(1,0,0), osgDegree2Rad(-90)));
	NodeRecPtr testTransNode = makeNodeFor(testTrans);
	testTransNode->addChild(testModel);
	//root->addChild(testTransNode);
	
	ShaderProgramRefPtr vpPPL = ShaderProgram::createVertexShader();
	ShaderProgramRefPtr fpPPL = ShaderProgram::createFragmentShader();
	ShaderProgramRefPtr gpPPL = ShaderProgram::createGeometryShader();
	vpPPL->readProgram("shaders/pixellight.vp.glsl");
	fpPPL->readProgram("shaders/pixellight.fp.glsl");
	gpPPL->readProgram("shaders/pixellight.gp.glsl");
	
	ShaderProgramChunkRefPtr shaderChunk = ShaderProgramChunk::create();
	shaderChunk->addShader(vpPPL);
	shaderChunk->addShader(fpPPL);
	//shaderChunk->addShader(gpPPL);
	
	//ShaderProgramVariableChunkRecPtr varChunk = ShaderProgramVariableChunk::create();
	
	ChunkMaterialRecPtr chunkMat = ChunkMaterial::create();
	MaterialChunkRecPtr matChunk = MaterialChunk::create();
	matChunk->setDiffuse(Color4f(0.8,0.2,0.2,1));
	matChunk->setAmbient(Color4f(0.8,0.2,0.2,1));
	chunkMat->addChunk(matChunk);
	chunkMat->addChunk(shaderChunk);
	//mat->addChunk(varChunk);

	PolygonChunkRecPtr polyChunk = PolygonChunk::create();
	polyChunk->setFrontMode(GL_LINE);
	polyChunk->setBackMode(GL_LINE);
	ChunkMaterialRecPtr chunk2Mat = ChunkMaterial::create();
	chunk2Mat->addChunk(polyChunk);

	MultiPassMaterialRecPtr passMat = MultiPassMaterial::create();
	passMat->addMaterial(chunkMat);
	passMat->addMaterial(chunk2Mat);
	

	GeometryRecPtr testGeo = dynamic_cast<Geometry*>(testModel->getCore());
	testGeo->setMaterial(passMat);
	
	// you will see a donut at the floor, slightly skewed, depending on head_position
	return NodeTransitPtr(root);
}
