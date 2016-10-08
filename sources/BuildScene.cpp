#include "BuildScene.h"

#include <sys/stat.h> // stat
#include <errno.h>    // errno, ENOENT, EEXIST
#if defined(_WIN32)
#include <direct.h>   // _mkdir
#endif

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

ImageRecPtr collisionImageBlue;
ImageRecPtr collisionImageOrange;

NodeRecPtr playerModelTorso;
NodeRecPtr playerModelHeadBlue;
NodeRecPtr playerModelHeadOrange;
NodeRecPtr playerModelArmBlue;
NodeRecPtr playerModelArmOrange;

SimpleMaterialRecPtr shieldTorusMaterialBlue;
SimpleMaterialRecPtr shieldTorusMaterialOrange;
SimpleMaterialRecPtr shieldRingMaterialBlue;
SimpleMaterialRecPtr shieldRingMaterialOrange;

SimpleTexturedMaterialRecPtr lightTrailMaterialBlue;
SimpleTexturedMaterialRecPtr lightTrailMaterialOrange;

DirectionalLightRecPtr headLight;

ComponentTransformRecPtr testTrans;

const Char8 *cachePrefix = "cache/";

bool isFileExist(const Char8 *fileName) {
    std::ifstream infile(fileName);
    return infile.good();
}

bool isDirExist(const std::string& path)
{
#if defined(_WIN32)
    struct _stat info;
    if (_stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & _S_IFDIR) != 0;
#else 
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
#endif
}

bool makePath(const std::string& path)
{
#if defined(_WIN32)
    int ret = _mkdir(path.c_str());
#else
    mode_t mode = 0755;
    int ret = mkdir(path.c_str(), mode);
#endif
    if (ret == 0)
        return true;

    switch (errno)
    {
    case ENOENT:
        // parent didn't exist, try to create it
        {
            int pos = path.find_last_of('/');
            if (pos == std::string::npos)
#if defined(_WIN32)
                pos = path.find_last_of('\\');
            if (pos == std::string::npos)
#endif
                return false;
            if (!makePath( path.substr(0, pos) ))
                return false;
        }
        // now, try to create again
#if defined(_WIN32)
        return 0 == _mkdir(path.c_str());
#else 
        return 0 == mkdir(path.c_str(), mode);
#endif

    case EEXIST:
        return isDirExist(path);

    default:
        return false;
    }
}

NodeTransitPtr loadModelFromCache(const Char8 *filename, const Char8 *fileExtension) {
	std::stringstream cacheFileName;
	cacheFileName << cachePrefix << filename << ".OSB";
	if (isFileExist(cacheFileName.str().c_str())) {
        std::cout << "File '" << cacheFileName.str() << "' loaded" << std::endl;
		return SceneFileHandler::the()->read(cacheFileName.str().c_str());
	} else {
		std::stringstream fullPath;
		fullPath << filename << fileExtension;
		NodeRecPtr temp = SceneFileHandler::the()->read(fullPath.str().c_str());
		makePath(cacheFileName.str().substr(0, cacheFileName.str().find_last_of('/')));
		SceneFileHandler::the()->write(temp, cacheFileName.str().c_str());
        std::cout << "File '" << cacheFileName.str() << "' written" << std::endl;
		return NodeTransitPtr(temp);
	}
}

ImageTransitPtr loadImageFromSequence(const Char8 *filename_pre, const Char8 *filename_sur, int numFrames, Real32 fps) {
	std::vector<ImageRecPtr> images;
	for (int i = 0; i < numFrames; i++) {
		ImageRecPtr image = Image::create();
		std::stringstream s;
		s << filename_pre << (i < 10 ? "0" : "") << i << filename_sur;
		image->read(s.str().c_str());
		images.push_back(image);
	}

	ImageRecPtr finalImage = Image::create();
	ImageRecPtr firstImage = images.at(0);
	finalImage->set(
		firstImage->getPixelFormat(),
		firstImage->getWidth(),
		firstImage->getHeight(),
		firstImage->getDepth(),
		firstImage->getMipMapCount(),
		images.size(),
		1.f / fps
		);
	
	UInt8 *destData;
	for(int i = 0; i < images.size(); i++) {
		destData = finalImage->editData(0, i);
		memcpy(destData, images.at(i)->getData(), images.at(i)->getFrameSize());
	}

	return ImageTransitPtr(finalImage);
}

NodeTransitPtr buildScene()
{
	headLight = DirectionalLight::create();
	headLight->setDiffuse(Color4f(1,1,1,1));
	headLight->setAmbient(Color4f(1,1,1,1));
	headLight->setSpecular(Color4f(1,1,1,1));
	headLight->setDirection(Vec3f(0,0,-1));
	root = makeNodeFor(headLight);
	
	NodeRecPtr boundingBoxModel = loadModelFromCache("models/bbox", ".3DS");
	
	ComponentTransformRecPtr boundingBoxModelCT = ComponentTransform::create();
	boundingBoxModelCT->setTranslation(Vec3f(0,135,-405));
	if (userFaction == PLAYER_FACTION_BLUE) {
		boundingBoxModelCT->setRotation(Quaternion(Vec3f(1,0,0),osgDegree2Rad(-90)));
	} else {
		boundingBoxModelCT->setRotation(Quaternion(Vec3f(1,0,0),osgDegree2Rad(-90)) * Quaternion(Vec3f(0,0,1), Pi));
	}
	boundingBoxModelCT->setScale(Vec3f(270.f,270.f,270.f));

	NodeRecPtr boundingBoxModelTrans = makeNodeFor(boundingBoxModelCT);
	boundingBoxModelTrans->addChild(boundingBoxModel);

	root->addChild(boundingBoxModelTrans);
	
	diskModelBlue = loadModelFromCache("models/disk_blue", ".3DS");
	diskModelOrange = loadModelFromCache("models/disk_orange", ".3DS");
	playerModelTorso = loadModelFromCache("models/robot_torso", ".OBJ");
	playerModelHeadBlue = loadModelFromCache("models/robot_head_blue", ".OBJ");
	playerModelHeadOrange = loadModelFromCache("models/robot_head_orange", ".OBJ");
	playerModelArmBlue = loadModelFromCache("models/robot_arm_blue", ".OBJ");
	playerModelArmOrange = loadModelFromCache("models/robot_arm_orange", ".OBJ");

	collisionImageBlue = loadImageFromSequence("models/wall_collision_blue/wall_collision_blue_", ".png", 25, 25);
	collisionImageOrange = loadImageFromSequence("models/wall_collision_orange/wall_collision_orange_", ".png", 25, 25);

	shieldTorusMaterialBlue = SimpleMaterial::create();
	shieldTorusMaterialBlue->setDiffuse(colorBlue);
	shieldTorusMaterialBlue->setAmbient(colorBlue);
	shieldTorusMaterialBlue->setLit(false);

	shieldRingMaterialBlue = SimpleMaterial::create();
	shieldRingMaterialBlue->setDiffuse(colorBlue);
	shieldRingMaterialBlue->setAmbient(colorBlue);
	shieldRingMaterialBlue->setTransparency(0.5);
	shieldRingMaterialBlue->setLit(false);

	shieldTorusMaterialOrange = SimpleMaterial::create();
	shieldTorusMaterialOrange->setDiffuse(colorOrange);
	shieldTorusMaterialOrange->setAmbient(colorOrange);
	shieldTorusMaterialOrange->setLit(false);

	shieldRingMaterialOrange = SimpleMaterial::create();
	shieldRingMaterialOrange->setDiffuse(colorOrange);
	shieldRingMaterialOrange->setAmbient(colorOrange);
	shieldRingMaterialOrange->setTransparency(0.5);
	shieldRingMaterialOrange->setLit(false);

	lightTrailMaterialBlue = SimpleTexturedMaterial::create();
	ImageRecPtr lightTrailImageBlue = Image::create();
	lightTrailImageBlue->read("models/light_trail_blue.PNG");
	lightTrailMaterialBlue->setImage(lightTrailImageBlue);

	lightTrailMaterialOrange = SimpleTexturedMaterial::create();
	ImageRecPtr lightTrailImageOrange = Image::create();
	lightTrailImageOrange->read("models/light_trail_orange.PNG");
	lightTrailMaterialOrange->setImage(lightTrailImageOrange);

	return NodeTransitPtr(root);
}
