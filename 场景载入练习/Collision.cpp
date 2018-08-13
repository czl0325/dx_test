#include "Collision.h"

CCollision::CCollision(void)
{
}

CCollision::~CCollision(void)
{
}

bool CCollision::InterectTwoSphere( BoundingSphere& sphere1,BoundingSphere& sphere2 )
{
	float f = D3DXVec3Length(&(sphere1.vCenter - sphere2.vCenter));
	return f <= (sphere1.fRadius + sphere2.fRadius) ? true : false;
}