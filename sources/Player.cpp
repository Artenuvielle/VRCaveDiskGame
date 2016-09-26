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
	headDirection = Vec3f(0,1,0);
	headDirection.normalize();
	headPosition = Vec3f(0,170,30);
	leftArmPosition = Vec3f(0,130,-800);
	rightArmPosition = Vec3f(0,130,0);
	if (faction == mainUserFaction) {
		facingRotation = Quaternion(Vec3f(0,1,0), osgDegree2Rad(180));
	} else {
		facingRotation = Quaternion();
	}
	if (drawModel) {
		torsoTransform = cloneModelWithTranform(playerModelTorso);
		if (faction == PLAYER_FACTION_BLUE) {
			headTransform = cloneModelWithTranform(playerModelHeadBlue);
			leftArmTransform = cloneModelWithTranform(playerModelArmBlue);
			rightArmTransform = cloneModelWithTranform(playerModelArmBlue);
		} else {
			headTransform = cloneModelWithTranform(playerModelHeadOrange);
			leftArmTransform = cloneModelWithTranform(playerModelArmOrange);
			rightArmTransform = cloneModelWithTranform(playerModelArmOrange);
		}

		torsoTransform->setScale(Vec3f(PLAYER_GEOMETRY_SCALE, PLAYER_GEOMETRY_SCALE, PLAYER_GEOMETRY_SCALE) * 1.3);
		headTransform->setScale(Vec3f(PLAYER_GEOMETRY_SCALE, PLAYER_GEOMETRY_SCALE, PLAYER_GEOMETRY_SCALE));
		leftArmTransform->setScale(Vec3f(PLAYER_GEOMETRY_SCALE, PLAYER_GEOMETRY_SCALE, PLAYER_GEOMETRY_SCALE));
		rightArmTransform->setScale(Vec3f(PLAYER_GEOMETRY_SCALE, PLAYER_GEOMETRY_SCALE, PLAYER_GEOMETRY_SCALE));

		recalculatePositions();

		commitChanges();
	}
}

void Player::update() {
	recalculatePositions();
};

void Player::recalculatePositions() {
	torsoPosition = headPosition - headDirection * PLAYER_HEAD_SIZE - Vec3f(0,PLAYER_TORSO_HEAD_OFFSET,0);
	if (modelIncluded) {
		torsoTransform->setTranslation(torsoPosition);
		torsoTransform->setRotation(facingRotation);
		headTransform->setTranslation(headPosition);
		headTransform->setRotation(facingRotation * Quaternion(Vec3f(0,1,0),headDirection));
		rightArmTransform->setTranslation(rightArmPosition);
		rightArmTransform->setRotation(Quaternion(Vec3f(0,1,0),rightArmDirection));
	}
}

Vec3f Player::getTorsoPosition() {
	return torsoPosition;
}

Vec3f Player::getHeadDirection() {
	return headDirection;
}

void Player::setHeadDirection(Vec3f newDirection) {
	headDirection = newDirection;
}

void Player::setHeadDirection(Quaternion rotation) {
	rotation.multVec(Vec3f(0,1,0), headDirection);
}

Vec3f Player::getHeadPosition() {
	return headPosition;
}

void Player::setHeadPosition(Vec3f newPosition) {
	headPosition = newPosition;
}

Vec3f Player::getLeftArmDirection() {
	return leftArmDirection;
}

void Player::setLeftArmDirection(Quaternion rotation) {
	rotation.multVec(Vec3f(0,1,0), leftArmDirection);
}

void Player::setLeftArmDirection(Vec3f newDirection) {
	leftArmDirection = newDirection;
}

Vec3f Player::getLeftArmPosition() {
	return leftArmPosition;
}

void Player::setLeftArmPosition(Vec3f newPosition) {
	leftArmPosition = newPosition;
}

Vec3f Player::getRightArmDirection() {
	return rightArmDirection;
}

void Player::setRightArmDirection(Quaternion rotation) {
	rotation.multVec(Vec3f(0,1,0), rightArmDirection);
}

void Player::setRightArmDirection(Vec3f newDirection) {
	rightArmDirection = newDirection;
}

Vec3f Player::getRightArmPosition() {
	return rightArmPosition;
}

void Player::setRightArmPosition(Vec3f newPosition) {
	rightArmPosition = newPosition;
}
