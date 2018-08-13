#pragma once
#include "Header.h"
#include "BoundingVolume.h"
//平截头体类
class CFrustum
{
private:
	D3DXPLANE m_plane[6];
public:
	CFrustum(void);
	~CFrustum(void);
	void Update(const D3DXMATRIX& matViewProj);// 输入摄像机（view）*投影矩阵，得到6个平面.
	bool IsBoxIn(const D3DXVECTOR3& vMin,const D3DXVECTOR3& vMax);
	bool IsBoxIn(const LPBOUNDINGBOX pBoundingBox);
	bool IsSphereIn(const D3DXVECTOR3& vCenter,const float fRadius);
	bool IsSphereIn(const LPBOUNDINGSPHERE pBoundingSphere);

	// 点在平截头体的可视范围内
	bool PointInFrustum(D3DXVECTOR3* vPoint);
};
