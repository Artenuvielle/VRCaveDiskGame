#include "ArtificialIntelligence.h"

OSG_USING_NAMESPACE

AI::AI(Player* aiTarget) {
	me = aiTarget;
	state = AI_STATE_IDLE;
	me->setHeadPosition(Vec3f(0,180,-110));
	me->setHeadRotation(Quaternion());
	me->setDiskArmPosition(Vec3f(-25,140,-110));
	me->setDiskArmRotation(Quaternion(Vec3f(0,1,0), osgDegree2Rad(90)) * Quaternion(Vec3f(1,0,0), osgDegree2Rad(-90)));
	me->setShieldArmPosition(Vec3f(25,140,-110));
	me->setShieldArmRotation(Quaternion(Vec3f(1,0,0), osgDegree2Rad(-90)) * Quaternion(Vec3f(0,1,0), osgDegree2Rad(-90)));
}

void AI::update() {

}
