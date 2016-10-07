#include "LifeCounter.h"

#include <OpenSG/OSGSceneFileHandler.h>
#include <OpenSG/OSGGeoBuilder.h>


#include "BuildScene.h"

OSG_USING_NAMESPACE

LifeDisplayRect::LifeDisplayRect(PlayerFaction faction, Real32 width, Real32 height, Vec3f pos) {
	Color3f factionColor;
	if (faction == PLAYER_FACTION_BLUE) {
		factionColor = colorBlue;
	} else {
		factionColor = colorOrange;
	}
	currentTransparency = 1.f;

	GeometryRecPtr rect = makePlaneGeo(width, height, 1, 1);
	ComponentTransformRecPtr trans = ComponentTransform::create();
	trans->setTranslation(pos);
	trans->setRotation(Quaternion(Vec3f(1,0,0), osgDegree2Rad(-90)));
	NodeRecPtr transNode = makeNodeFor(trans);
	NodeRecPtr rectGroup = makeNodeFor(Group::create());
	transNode->addChild(rectGroup);
	rectGroup->addChild(makeNodeFor(rect));
	root->addChild(transNode);
	transparentMaterial = SimpleMaterial::create();
	transparentMaterial->setAmbient(factionColor);
	transparentMaterial->setDiffuse(factionColor);
	transparentMaterial->setTransparency(currentTransparency);
	rect->setMaterial(transparentMaterial);

	Real32 strokeWidth = 1.5f;
	GeoBuilder rectBuilder;
	rectBuilder.begin(GL_QUAD_STRIP);
	rectBuilder.fullVertex(Pnt3f(-width / 2, height / 2, 0), Vec3f(0,0,-1), factionColor);
	rectBuilder.fullVertex(Pnt3f(-width / 2 - strokeWidth, height / 2 + strokeWidth, 0), Vec3f(0,0,-1), factionColor);
	rectBuilder.fullVertex(Pnt3f(width / 2, height / 2, 0), Vec3f(0,0,-1), factionColor);
	rectBuilder.fullVertex(Pnt3f(width / 2 + strokeWidth, height / 2 + strokeWidth, 0), Vec3f(0,0,-1), factionColor);
	rectBuilder.fullVertex(Pnt3f(width / 2, -height / 2, 0), Vec3f(0,0,-1), factionColor);
	rectBuilder.fullVertex(Pnt3f(width / 2 + strokeWidth, -height / 2 - strokeWidth, 0), Vec3f(0,0,-1), factionColor);
	rectBuilder.fullVertex(Pnt3f(-width / 2, -height / 2, 0), Vec3f(0,0,-1), factionColor);
	rectBuilder.fullVertex(Pnt3f(-width / 2 - strokeWidth, -height / 2 - strokeWidth, 0), Vec3f(0,0,-1), factionColor);
	rectBuilder.fullVertex(Pnt3f(-width / 2, height / 2, 0), Vec3f(0,0,-1), factionColor);
	rectBuilder.fullVertex(Pnt3f(-width / 2 - strokeWidth, height / 2 + strokeWidth, 0), Vec3f(0,0,-1), factionColor);
	rectBuilder.end();
	GeometryRecPtr strokeGeo = rectBuilder.getGeometry();
	rectGroup->addChild(makeNodeFor(strokeGeo));
	SimpleMaterialRecPtr strokeMaterial = SimpleMaterial::create();
	strokeMaterial->setAmbient(factionColor);
	strokeMaterial->setDiffuse(factionColor);
	strokeGeo->setMaterial(strokeMaterial);
}

void LifeDisplayRect::setFilled(bool shouldBeFilled) {
	transparencyChangePerSecond = ((shouldBeFilled ? lifeCounterMaxTransparency : 1) - currentTransparency) / lifeCounterTransparencyChangeTime;
	isFilled = shouldBeFilled;
}

bool LifeDisplayRect::getFilled() {
	return isFilled;
}

void LifeDisplayRect::update() {
	Real32 time = glutGet(GLUT_ELAPSED_TIME);
	currentTransparency = osgMin(1.f, osgMax(lifeCounterMaxTransparency, currentTransparency + transparencyChangePerSecond * (time - lastUpdateTime) / 1000.f));
	transparentMaterial->setTransparency(currentTransparency);
	lastUpdateTime = time;
}

LifeCounter::LifeCounter(PlayerFaction faction) {
	rects = new std::vector<LifeDisplayRect>();

	Int32 tileNum = lifeCounterMaxLife * (lifeCounterMaxLife + 1) / 2;
	Real32 tileSize = 240.f / tileNum;
	Int32 usedTiles = 0;
	for (Int32 i = 0; i < lifeCounterMaxLife; i++) {
		Real32 factionZPos;
		Real32 currentTiles = lifeCounterMaxLife - i;
		Real32 currentTileSize = currentTiles * tileSize;
		Real32 offset = usedTiles * tileSize + currentTileSize / 2;
		if (faction == userFaction) {
			factionZPos = -135 - offset;
		} else {
			factionZPos = -660 + offset;
		}
		rects->push_back(LifeDisplayRect(faction, 50, currentTileSize - 8, Vec3f(0, 2, factionZPos)));
		usedTiles += currentTiles;
	}

	setLifeCount(0);

	commitChanges();
}

LifeCounter::~LifeCounter() {
	delete rects;
}

void LifeCounter::update() {
	for (Int32 i = 0; i < rects->size(); i++) {
		if (rects->at(i).getFilled() != (i < lifeCount)) {
			rects->at(i).setFilled(i < lifeCount);
		}
		rects->at(i).update();
	}
}

void LifeCounter::setLifeCount(Int32 newCount) {
	lifeCount = osgMax(0, osgMin(lifeCounterMaxLife, newCount));
}

Int32 LifeCounter::getLifeCount() {
	return lifeCount;
}