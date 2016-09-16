#include "Common.h"

OSG_USING_NAMESPACE

NodeRecPtr boundingBoxModel;
ComponentTransformRecPtr boundingBoxModelCT;
ComponentTransformRecPtr movableTransform;
Vec3f diskDirection = Vec3f(0.f,0.f,0.f);
Real32 startTime;
int xangle = 90;
