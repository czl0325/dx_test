#pragma once
#include "BoundingVolume.h"

class CCollision
{
public:
	//判断两个球体相交
	static bool InterectTwoSphere(BoundingSphere& sphere1,BoundingSphere& sphere2);
	CCollision(void);
	~CCollision(void);
};
