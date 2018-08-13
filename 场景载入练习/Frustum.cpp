#include "Frustum.h"

CFrustum::CFrustum(void)
{
}

CFrustum::~CFrustum(void)
{
}

void CFrustum::Update( const D3DXMATRIX& matViewProj )
{
	//右平面
	m_plane[0].a=matViewProj._14-matViewProj._11;
	m_plane[0].b=matViewProj._24-matViewProj._21;
	m_plane[0].c=matViewProj._34-matViewProj._31;
	m_plane[0].d=matViewProj._44-matViewProj._41;

	//左平面
	m_plane[1].a=matViewProj._14+matViewProj._11;
	m_plane[1].b=matViewProj._24+matViewProj._21;
	m_plane[1].c=matViewProj._34+matViewProj._31;
	m_plane[1].d=matViewProj._44+matViewProj._41;

	//上平面
	m_plane[2].a=matViewProj._14-matViewProj._12;
	m_plane[2].b=matViewProj._24-matViewProj._22;
	m_plane[2].c=matViewProj._34-matViewProj._32;
	m_plane[2].d=matViewProj._44-matViewProj._42;

	//下平面
	m_plane[3].a=matViewProj._14+matViewProj._12;
	m_plane[3].b=matViewProj._24+matViewProj._22;
	m_plane[3].c=matViewProj._34+matViewProj._32;
	m_plane[3].d=matViewProj._44+matViewProj._42;

	//远平面
	m_plane[4].a = matViewProj._14 - matViewProj._13;
	m_plane[4].b = matViewProj._24 - matViewProj._23;
	m_plane[4].c = matViewProj._34 - matViewProj._33;
	m_plane[4].d = matViewProj._44 - matViewProj._43;

	//近平面
	m_plane[5].a = matViewProj._14 + matViewProj._13;
	m_plane[5].b = matViewProj._24 + matViewProj._23;
	m_plane[5].c = matViewProj._34 + matViewProj._33;
	m_plane[5].d = matViewProj._44 + matViewProj._43;

	D3DXPlaneNormalize(&m_plane[0],&m_plane[0]);
	D3DXPlaneNormalize(&m_plane[1],&m_plane[1]);
	D3DXPlaneNormalize(&m_plane[2],&m_plane[2]);
	D3DXPlaneNormalize(&m_plane[3],&m_plane[3]);
	D3DXPlaneNormalize(&m_plane[4],&m_plane[4]);
	D3DXPlaneNormalize(&m_plane[5],&m_plane[5]);
}

bool CFrustum::IsBoxIn( const D3DXVECTOR3& vMin,const D3DXVECTOR3& vMax )
{
	for (int i=0;i<6;i++)
	{
		if (!(D3DXPlaneDotCoord(&m_plane[i],&vMax) >= 0.0f
			|| D3DXPlaneDotCoord(&m_plane[i],&D3DXVECTOR3(vMax.x,vMax.y,vMin.z)) >= 0.0f
			|| D3DXPlaneDotCoord(&m_plane[i],&D3DXVECTOR3(vMin.x,vMax.y,vMin.z)) >= 0.0f
			|| D3DXPlaneDotCoord(&m_plane[i],&D3DXVECTOR3(vMin.x,vMax.y,vMax.z)) >= 0.0f
			|| D3DXPlaneDotCoord(&m_plane[i],&vMin) >= 0.0f
			|| D3DXPlaneDotCoord(&m_plane[i],&D3DXVECTOR3(vMin.x,vMin.y,vMax.z)) >= 0.0f
			|| D3DXPlaneDotCoord(&m_plane[i],&D3DXVECTOR3(vMax.x,vMin.y,vMax.z)) >= 0.0f
			|| D3DXPlaneDotCoord(&m_plane[i],&D3DXVECTOR3(vMax.x,vMin.y,vMin.z)) >= 0.0f))
		{
			return false;
		}
	}
	return true;
}

bool CFrustum::IsBoxIn(const LPBOUNDINGBOX pBoundingBox)
{
	return IsBoxIn(pBoundingBox->vMin,pBoundingBox->vMax);
}

bool CFrustum::IsSphereIn( const D3DXVECTOR3& vCenter,const float fRadius )
{
	for (int i=0;i<6;i++)
	{
		if(!(D3DXPlaneDotCoord(&m_plane[i],&vCenter) < -fRadius))
			return false;
	}
	return true;
}

bool CFrustum::IsSphereIn( const LPBOUNDINGSPHERE pBoundingSphere )
{
	return IsSphereIn(pBoundingSphere->vCenter,pBoundingSphere->fRadius);
}

bool CFrustum::PointInFrustum( D3DXVECTOR3* vPoint )
{
	return true;
}