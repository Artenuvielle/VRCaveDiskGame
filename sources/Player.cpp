#include "Player.h"

//#include <OpenSG/OSGSceneFileHandler.h>

#include "BuildScene.h"
#include "Common.h"

OSG_USING_NAMESPACE

ComponentTransformTransitPtr cloneModelWithTranform(NodeRecPtr modelToClone) {
	NodeRecPtr modelCopy = deepCloneTree(modelToClone);
	ComponentTransformRecPtr transform = ComponentTransform::create();
	NodeRecPtr transformNode = makeNodeFor(transform);
	transformNode->addChild(modelCopy);
	root->addChild(transformNode);
	return ComponentTransformTransitPtr(transform);
}

Player::Player(PlayerFaction faction, bool drawModel) : modelIncluded(drawModel) {
	headRotation = Quaternion();
	headPosition = Vec3f(0,170,30);
	diskArmPosition = Vec3f(20,130,50);
	shieldArmPosition = Vec3f(-20,130,50);

	disk = new Disk(faction);
	shield = new Shield(faction);

	if (drawModel) {
		torsoTransform = cloneModelWithTranform(playerModelTorso);
		if (faction == PLAYER_FACTION_BLUE) {
			headTransform = cloneModelWithTranform(playerModelHeadBlue);
			diskArmTransform = cloneModelWithTranform(playerModelArmBlue);
			shieldArmTransform = cloneModelWithTranform(playerModelArmBlue);
		} else {
			headTransform = cloneModelWithTranform(playerModelHeadOrange);
			diskArmTransform = cloneModelWithTranform(playerModelArmOrange);
			shieldArmTransform = cloneModelWithTranform(playerModelArmOrange);
		}

		torsoTransform->setScale(Vec3f(PLAYER_GEOMETRY_SCALE, PLAYER_GEOMETRY_SCALE, PLAYER_GEOMETRY_SCALE) * 1.3);
		headTransform->setScale(Vec3f(PLAYER_GEOMETRY_SCALE, PLAYER_GEOMETRY_SCALE, PLAYER_GEOMETRY_SCALE));
		diskArmTransform->setScale(Vec3f(PLAYER_GEOMETRY_SCALE, PLAYER_GEOMETRY_SCALE, PLAYER_GEOMETRY_SCALE));
		shieldArmTransform->setScale(Vec3f(PLAYER_GEOMETRY_SCALE, PLAYER_GEOMETRY_SCALE, PLAYER_GEOMETRY_SCALE));

		recalculatePositions();

		commitChanges();
	}
}

Player::~Player() {
	delete disk;
	delete shield;
}

void Player::update() {
	recalculatePositions();

	disk->setPosition(diskArmPosition);
	disk->setRotation(diskArmRotation);
	disk->setTargetOwnerPosition(headPosition);
	disk->setTargetEnemyPosition(enemyPoint->getTranslation());
	disk->update();

	shield->setPosition(shieldArmPosition);
	shield->setRotation(shieldArmRotation);
	shield->update();
};

void Player::recalculatePositions() {
	Vec3f headYAxisDirection;
	headRotation.multVec(Vec3f(0,1,0), headYAxisDirection);
	torsoPosition = headPosition - headYAxisDirection * PLAYER_HEAD_SIZE - Vec3f(0,PLAYER_TORSO_HEAD_OFFSET,0);
	if (modelIncluded) {
		torsoTransform->setTranslation(torsoPosition);
		Vec3f headEulerAxisRotation;
		headRotation.getEulerAngleRad(headEulerAxisRotation);
		torsoTransform->setRotation(Quaternion(Vec3f(0,1,0), headEulerAxisRotation.y()));
		headTransform->setTranslation(headPosition);
		headTransform->setRotation(headRotation);
		diskArmTransform->setTranslation(diskArmPosition);
		diskArmTransform->setRotation(diskArmRotation);
		shieldArmTransform->setTranslation(shieldArmPosition);
		shieldArmTransform->setRotation(shieldArmRotation);
	}
}

Vec3f Player::getTorsoPosition() {
	return torsoPosition;
}

Quaternion Player::getHeadRotation() {
	return headRotation;
}

void Player::setHeadRotation(Quaternion rotation) {
	headRotation = rotation;
}

Vec3f Player::getHeadPosition() {
	return headPosition;
}

void Player::setHeadPosition(Vec3f newPosition) {
	headPosition = newPosition;
}

Quaternion Player::getDiskArmRotation() {
	return diskArmRotation;
}

void Player::setDiskArmRotation(Quaternion rotation) {
	diskArmRotation = rotation;
}

Vec3f Player::getDiskArmPosition() {
	return diskArmPosition;
}

void Player::setDiskArmPosition(Vec3f newPosition) {
	diskArmPosition = newPosition;
}

Quaternion Player::getShieldArmRotation() {
	return shieldArmRotation;
}

void Player::setShieldArmRotation(Quaternion rotation) {
	shieldArmRotation = rotation;
}

Vec3f Player::getShieldArmPosition() {
	return shieldArmPosition;
}

void Player::setShieldArmPosition(Vec3f newPosition) {
	shieldArmPosition = newPosition;
}

Disk* Player::getDisk() {
	return disk;
}

Shield* Player::getShield() {
	return shield;
}
