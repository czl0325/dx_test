#pragma once
#include "Header.h"
#include "BoundingVolume.h"
//ƽ��ͷ����
class CFrustum
{
private:
	D3DXPLANE m_plane[6];
public:
	CFrustum(void);
	~CFrustum(void);
	void Update(const D3DXMATRIX& matViewProj);// �����������view��*ͶӰ���󣬵õ�6��ƽ��.
	bool IsBoxIn(const D3DXVECTOR3& vMin,const D3DXVECTOR3& vMax);
	bool IsBoxIn(const LPBOUNDINGBOX pBoundingBox);
	bool IsSphereIn(const D3DXVECTOR3& vCenter,const float fRadius);
	bool IsSphereIn(const LPBOUNDINGSPHERE pBoundingSphere);

	// ����ƽ��ͷ��Ŀ��ӷ�Χ��
	bool PointInFrustum(D3DXVECTOR3* vPoint);
};
