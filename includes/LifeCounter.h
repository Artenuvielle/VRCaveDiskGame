
#ifndef _LifeCounter_H_
#define _LifeCounter_H_

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>

#include <OSGCSM/OSGCAVESceneManager.h>

#include "Common.h"

OSG_USING_NAMESPACE

class LifeDisplayRect {
public:
	void setFilled(bool shouldBeFilled);
	bool getFilled();
	void update();
	LifeDisplayRect(PlayerFaction type, Real32 width, Real32 height, Vec3f pos);
private:
	bool isFilled;
	Real32 currentTransparency;
	Real32 transparencyChangePerSecond;
	Real32 lastUpdateTime;
	SimpleMaterialRecPtr transparentMaterial;
};

class LifeCounter {
public:
	void setLifeCount(Int32 newCount);
	Int32 getLifeCount();
	
	void update();

	LifeCounter(PlayerFaction type);
	~LifeCounter();
private:
	Int32 lifeCount;
	std::vector<LifeDisplayRect>* rects;
};

#endif