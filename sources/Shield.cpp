#include "Shield.h"

#include <OpenSG/OSGSceneFileHandler.h>

#include "BuildScene.h"

OSG_USING_NAMESPACE

Quaternion interpolateVector(Vec3f vec1, Vec3f vec2, Real32 factor);

Shield::Shield(PlayerFaction faction) {
	SimpleMaterialRecPtr torusMat, ringMat;
	if (faction == PLAYER_FACTION_BLUE) {
		torusMat = shieldTorusMaterialBlue;
		ringMat = shieldRingMaterialBlue;
	} else {
		torusMat = shieldTorusMaterialOrange;
		ringMat = shieldRingMaterialOrange;
	}

	GeometryRecPtr torusGeo = makeTorusGeo(0.5, 20, 10, 20);
	GeometryRecPtr ringGeo = makeCylinderGeo(0.1, 20, 20, false, true, true);

	torusGeo->setMaterial(torusMat);
	ringGeo->setMaterial(ringMat);

	transform = ComponentTransform::create();
	NodeRecPtr transformNode = makeNodeFor(transform);
	NodeRecPtr groupNode = makeNodeFor(Group::create());
	
	ComponentTransformRecPtr torusTransform = ComponentTransform::create();
	torusTransform->setRotation(Quaternion(Vec3f(1,0,0),osgDegree2Rad(90)));
	NodeRecPtr torusTransformNode = makeNodeFor(torusTransform);
	
	torusTransformNode->addChild(makeNodeFor(torusGeo));
	groupNode->addChild(makeNodeFor(ringGeo));
	groupNode->addChild(torusTransformNode);
	transformNode->addChild(groupNode);
	root->addChild(transformNode);
	
	commitChanges();
}

void Shield::update() {

}

void Shield::setPosition(Vec3f newPosition) {
	transform->setTranslation(newPosition);
}

Vec3f Shield::getPosition() {
	return transform->getTranslation();
}

void Shield::setRotation(Quaternion newRotation) {
	transform->setRotation(newRotation);
}

Quaternion Shield::getRotation() {
	return transform->getRotation();
}

void Shield::setScale(Real32 newScale) {
	transform->setScale(newScale);
}

Real32 Shield::getScale() {
	return transform->getScale().x();
}
