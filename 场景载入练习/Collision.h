#pragma once
#include "BoundingVolume.h"

class CCollision
{
public:
	//�ж����������ཻ
	static bool InterectTwoSphere(BoundingSphere& sphere1,BoundingSphere& sphere2);
	CCollision(void);
	~CCollision(void);
};
