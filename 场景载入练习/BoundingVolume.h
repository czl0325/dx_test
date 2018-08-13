#pragma once
#include "Header.h"

typedef struct BoundingBox
{
	D3DXVECTOR3	vMax;
	D3DXVECTOR3	vMin;
}BOUNDINGBOX,*LPBOUNDINGBOX;

typedef struct BoundingSphere
{
	D3DXVECTOR3	vCenter;
	float	fRadius;
}BOUNDINGSPHERE,*LPBOUNDINGSPHERE;

class CBoundingVolume
{
private:
	LPDIRECT3DDEVICE9 m_pDevice;
	BOUNDINGBOX	m_boundingBox;		// 外接盒
	BOUNDINGSPHERE	m_boundingSphere;	// 外接球

public:
	CBoundingVolume(void);
	~CBoundingVolume(void);
	//创建外接体(四个方法)
	void	Create(const D3DXVECTOR3* pVertices,int nVertices,DWORD dwStride);
	void	Create(LPD3DXMESH pMesh);//创建包围盒
	void	Create(const D3DXVECTOR3* pVertex1,const D3DXVECTOR3* pVertex2,const D3DXVECTOR3* pVertex3,const D3DXVECTOR3* pVertex4);
	void	Create(CBoundingVolume& volume1,CBoundingVolume& volume2,CBoundingVolume& volume3,CBoundingVolume& volume4);


	void    Init(LPDIRECT3DDEVICE9 pDevice);

	void	CalSphereByBox(D3DXMATRIX matWord);

	void CalculateBoxFromSphere()// 由包围球计算包围盒
	{
		float fHalf=m_boundingSphere.fRadius / sqrtf(3.0f);
		m_boundingBox.vMax=m_boundingSphere.vCenter+D3DXVECTOR3(fHalf,fHalf,fHalf);
		m_boundingBox.vMin=m_boundingSphere.vCenter-D3DXVECTOR3(fHalf,fHalf,fHalf);
	}
	void CalculateSphereFromBox()// 由包围盒计算包围球
	{
		m_boundingSphere.vCenter=(m_boundingBox.vMax+m_boundingBox.vMin)/2.0f;
		m_boundingSphere.fRadius=D3DXVec3Length(&((m_boundingBox.vMax-m_boundingBox.vMin)/2.0f));
	}
	void Zoom(float fScaling)// 缩放包围盒
	{
		m_boundingBox.vMax *= fScaling;
		m_boundingBox.vMin *= fScaling;
		m_boundingSphere.fRadius *= fScaling;
	}
	void Move(IN D3DXVECTOR3 &vOffset)
	{
		m_boundingBox.vMax += vOffset;
		m_boundingBox.vMin += vOffset;
		m_boundingSphere.vCenter += vOffset;
	}



	LPBOUNDINGBOX GetBoundingBox(){return &m_boundingBox;}
	LPBOUNDINGSPHERE GetBoundingSphere(){return &m_boundingSphere;}
	BOUNDINGSPHERE Getboundingspehere(){return m_boundingSphere;}
	D3DXVECTOR3 GetSphereCenter(){return m_boundingSphere.vCenter;}
	float GetSphereRadius(){return m_boundingSphere.fRadius;}
	D3DXVECTOR3 GetBoxMax(){return m_boundingBox.vMax;}
	D3DXVECTOR3 GetBoxMin(){return m_boundingBox.vMin;}
	void SetBoundingBox(BoundingBox &box){m_boundingBox=box;}
	void SetSphereCenter(D3DXVECTOR3 &vCenter){m_boundingSphere.vCenter=vCenter;}
	void SetSphereRadius(float fRadius){m_boundingSphere.fRadius=fRadius;}
	void SetBoxMax(D3DXVECTOR3 &vMax){m_boundingBox.vMax=vMax;}
	void SetBoxMin(D3DXVECTOR3 &vMin){m_boundingBox.vMin=vMin;}
};
