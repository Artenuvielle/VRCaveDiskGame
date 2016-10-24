#include "LightTrail.h"

#include <OpenSG/OSGSceneFileHandler.h>

#include "BuildScene.h"

OSG_USING_NAMESPACE

struct LightTrailListElement {
	LightTrail* trail;
	bool shouldEnd;
	LightTrailListElement(LightTrail* t) : trail(t), shouldEnd(false) {}
};

std::list<LightTrailListElement>* trailList = new std::list<LightTrailListElement>();

LightTrail* LightTrail::create(PlayerFaction faction, Vec3f initialPosition) {
	LightTrailListElement element(new LightTrail(faction, initialPosition));
	trailList->push_back(element);
	return element.trail;
}

void LightTrail::finish(LightTrail* trail) {
	for(std::list<LightTrailListElement>::iterator it = trailList->begin(); it != trailList->end(); it++) {
		if (it->trail == trail) {
			it->shouldEnd = true;
			return;
		}
	}
}

Vec3f LightTrail::headPosition = Vec3f(0.f, 170.f, 130.f);

void LightTrail::updateTrails(Vec3f headPos) {
	headPosition = headPos;
	for(std::list<LightTrailListElement>::iterator it = trailList->begin(); it != trailList->end(); ++it) {
		if (it->shouldEnd) {
			it->trail->addPointWithoutTest(it->trail->lastPointAdded);
			if (it->trail->points.begin()->dist2(*it->trail->points.end()) < 1.f) {
				delete it->trail;
				it = trailList->erase(it);
				if (it != trailList->begin()) it--;
			} else {
				it->trail->update();
			}
		} else {
			it->trail->update();
		}
	}
}

void LightTrail::deleteAllTrailsInstantly() {
	for(std::list<LightTrailListElement>::iterator it = trailList->begin(); it != trailList->end(); it++) {
		delete it->trail;
		it = trailList->erase(it);
		if (it != trailList->begin()) it--;
	}
}

LightTrail::LightTrail(PlayerFaction faction, Vec3f initialPosition) {
	for (int i = 0; i < lightTrailMaxPoints; i++) {
		points.push_back(initialPosition);
	}
	lastPointAdded = initialPosition;
	secondLastPointAdded = initialPosition;
	timeOfLastPointAdded = 0;
	GeoUInt8PropertyRecPtr types = GeoUInt8Property::create();
	types->addValue(GL_TRIANGLES);
	GeoUInt32PropertyRecPtr lengths = GeoUInt32Property::create();
	lengths->addValue(3 * 4 * (lightTrailMaxPoints - 1));
	pos = GeoPnt3fProperty::create();
	norms = GeoVec3fProperty::create();
	GeoColor3fPropertyRecPtr colors = GeoColor3fProperty::create();
	GeoPnt2fPropertyRecPtr texCoords = GeoPnt2fProperty::create();
	for (int i = 0; i < lightTrailMaxPoints; i++) {
		Vec3f direction;
		if (i + 1 < lightTrailMaxPoints) {
			direction = points.at(i + 1) - points.at(i);
		} else {
			direction = points.at(i) - points.at(i - 1);
		}
		Vec3f offset = direction.cross(headPosition - points.at(i));
		offset *= lightTrailMaxSize * lightTrailMaxSize / offset.squareLength();
		pos->addValue(Pnt3f(points.at(i)));
		pos->addValue(Pnt3f(points.at(i) + offset));
		pos->addValue(Pnt3f(points.at(i) - offset));

		Vec3f n = direction.cross(offset);
		n.normalize();
		norms->addValue(n);
		norms->addValue(n);
		norms->addValue(n);
		
		colors->addValue(colorBlue);
		colors->addValue(colorBlue);
		colors->addValue(colorBlue);

		texCoords->addValue(Pnt2f(0.5f,0.5f));
		texCoords->addValue(Pnt2f(0.f,0.f));
		texCoords->addValue(Pnt2f(1.f,1.f));
	}
	
	GeoUInt32PropertyRecPtr indices = GeoUInt32Property::create();
	for (int i = 1; i < lightTrailMaxPoints; i++) {
		indices->addValue((i - 1) * 3);
		indices->addValue((i - 1) * 3 + 1);
		indices->addValue(i * 3 + 1);

		indices->addValue((i - 1) * 3);
		indices->addValue(i * 3 + 1);
		indices->addValue(i * 3);

		indices->addValue((i - 1) * 3);
		indices->addValue(i * 3 + 2);
		indices->addValue((i - 1) * 3 + 2);

		indices->addValue((i - 1) * 3);
		indices->addValue(i * 3);
		indices->addValue(i * 3 + 2);
	}
	geo = Geometry::create();

	geo->setTypes(types);
	geo->setLengths(lengths);
	geo->setIndices(indices);
	geo->setPositions(pos);
	geo->setNormals(norms);
	if (faction == PLAYER_FACTION_BLUE) {
		geo->setMaterial(lightTrailMaterialBlue);
	} else {
		geo->setMaterial(lightTrailMaterialOrange);
	}
	geo->setColors(colors);
	geo->setTexCoords(texCoords);
	geo->setDlistCache(false);

	geoNode = makeNodeFor(geo);
	root->addChild(geoNode);
	commitChanges();
}

LightTrail::~LightTrail() {
	root->subChild(geoNode);
	commitChanges();
}

void LightTrail::addPoint(Vec3f pos) {
	Real32 time = glutGet(GLUT_ELAPSED_TIME);
	Vec3f direction = pos - lastPointAdded;
	points.erase(points.end() - 1);
	if (timeOfLastPointAdded - time < 1000.f / lightTrailPointsPerSecond && direction.squareLength() > lightTrailInputPointMinDistance * lightTrailInputPointMinDistance) {
		Vec3f secondLastDirection = lastPointAdded - secondLastPointAdded;
		Vec3f Q1 = secondLastPointAdded + secondLastDirection * 0.75;
		Vec3f R0 = lastPointAdded + direction * 0.25;
		Vec3f Q0 = lastPointAdded + direction * 0.75;
		points.push_back(Q1 + (R0 - Q1) * 0.25);
		points.push_back(Q1 + (R0 - Q1) * 0.75);
		points.push_back(R0 + (Q0 - R0) * 0.25);
		points.push_back(R0 + (Q0 - R0) * 0.75);
		timeOfLastPointAdded = time;
		secondLastPointAdded = lastPointAdded;
		lastPointAdded = pos;
		while (points.size() > lightTrailMaxPoints - 1) {
			points.erase(points.begin());
		}
	}
	points.push_back(pos);
}

void LightTrail::addPointWithoutTest(Vec3f pos) {
	points.push_back(pos);
	secondLastPointAdded = lastPointAdded;
	lastPointAdded = pos;
	while (points.size() > lightTrailMaxPoints) {
		points.erase(points.begin());
	}
}

void LightTrail::update() {
	GeoPnt3fProperty::StoredFieldType *posField = pos->editFieldPtr();
	GeoVec3fProperty::StoredFieldType *normsField = norms->editFieldPtr();

	GeoPnt3fProperty::StoredFieldType::iterator posIt = posField->begin();
	GeoVec3fProperty::StoredFieldType::iterator normsIt = normsField->begin();

	for (int i = 0; i < lightTrailMaxPoints; i++) {
		Vec3f direction;
		if (i + 1 < lightTrailMaxPoints) {
			direction = points.at(i + 1) - points.at(i);
		} else {
			direction = points.at(i) - points.at(i - 1);
		}
		Vec3f offset = direction.cross(headPosition - points.at(i));
		Real32 width = lightTrailMaxSize;
		if (i < lightTrailSizeGrow) {
			width *= ((float) i) / lightTrailSizeGrow;
		}
		offset *= width / offset.length();
		(*posIt) = Pnt3f(points.at(i));
		++posIt;
		(*posIt) = Pnt3f(points.at(i) + offset);
		++posIt;
		(*posIt) = Pnt3f(points.at(i) - offset);
		++posIt;

		Vec3f n = direction.cross(offset);
		n.normalize();
		(*normsIt) = n;
		++normsIt;
		(*normsIt) = n;
		++normsIt;
		(*normsIt) = n;
		++normsIt;
	}
}

