#include "BoundingVolume.h"

CBoundingVolume::CBoundingVolume(void)
{
	m_boundingBox.vMax = D3DXVECTOR3(-100000.0f,-100000.0f,-100000.0f);
	m_boundingBox.vMin = D3DXVECTOR3(100000.0f,100000.0f,100000.0f);
	m_boundingSphere.vCenter = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_boundingSphere.fRadius = 0.0f;
}

CBoundingVolume::~CBoundingVolume(void)
{
}

void CBoundingVolume::Create( LPD3DXMESH pMesh )
{
	BYTE* pVertices;
	pMesh->LockVertexBuffer(0,(void**)&pVertices);

	D3DXComputeBoundingBox((D3DXVECTOR3*)pVertices,pMesh->GetNumVertices(),
		D3DXGetFVFVertexSize(pMesh->GetFVF()),&m_boundingBox.vMin,&m_boundingBox.vMax);
	D3DXComputeBoundingSphere((D3DXVECTOR3*)pVertices,pMesh->GetNumVertices(),
		D3DXGetFVFVertexSize(pMesh->GetFVF()),&m_boundingSphere.vCenter,&m_boundingSphere.fRadius);

	pMesh->UnlockVertexBuffer();
}

void CBoundingVolume::Create( CBoundingVolume& volume1,CBoundingVolume& volume2,CBoundingVolume& volume3,CBoundingVolume& volume4 )
{
	m_boundingBox.vMin.x = min(volume1.m_boundingBox.vMin.x,
		min(volume2.m_boundingBox.vMin.x,min(volume3.m_boundingBox.vMin.x,volume4.m_boundingBox.vMin.x)));
	m_boundingBox.vMin.y = min(volume1.m_boundingBox.vMin.y,
		min(volume2.m_boundingBox.vMin.y,min(volume3.m_boundingBox.vMin.y,volume4.m_boundingBox.vMin.y)));
	m_boundingBox.vMin.z = min(volume1.m_boundingBox.vMin.z,
		min(volume2.m_boundingBox.vMin.z,min(volume3.m_boundingBox.vMin.z,volume4.m_boundingBox.vMin.z)));
	m_boundingBox.vMax.x = max(volume1.m_boundingBox.vMax.x,
		max(volume2.m_boundingBox.vMax.x,max(volume3.m_boundingBox.vMax.x,volume4.m_boundingBox.vMax.x)));
	m_boundingBox.vMax.y = max(volume1.m_boundingBox.vMax.y,
		max(volume2.m_boundingBox.vMax.y,max(volume3.m_boundingBox.vMax.y,volume4.m_boundingBox.vMax.y)));
	m_boundingBox.vMax.z = max(volume1.m_boundingBox.vMax.z,
		max(volume2.m_boundingBox.vMax.z,max(volume3.m_boundingBox.vMax.z,volume4.m_boundingBox.vMax.z)));

	m_boundingSphere.vCenter = (m_boundingBox.vMax + m_boundingBox.vMin) / 2.0f;
	m_boundingSphere.fRadius = D3DXVec3Length(&(m_boundingBox.vMax - m_boundingBox.vMin)) / 2.0f;
}

void CBoundingVolume::Create( const D3DXVECTOR3* pVertex1,const D3DXVECTOR3* pVertex2,const D3DXVECTOR3* pVertex3,const D3DXVECTOR3* pVertex4 )
{
	m_boundingBox.vMin.x = min(pVertex1->x,min(pVertex2->x,min(pVertex3->x,pVertex4->x)));
	m_boundingBox.vMin.y = min(pVertex1->y,min(pVertex2->y,min(pVertex3->y,pVertex4->y)));
	m_boundingBox.vMin.z = min(pVertex1->z,min(pVertex2->z,min(pVertex3->z,pVertex4->z)));
	m_boundingBox.vMax.x = max(pVertex1->x,max(pVertex2->x,max(pVertex3->x,pVertex4->x)));
	m_boundingBox.vMax.y = max(pVertex1->y,max(pVertex2->y,max(pVertex3->y,pVertex4->y)));
	m_boundingBox.vMax.z = max(pVertex1->z,max(pVertex2->z,max(pVertex3->z,pVertex4->z)));

	m_boundingSphere.vCenter = (m_boundingBox.vMax + m_boundingBox.vMin) / 2.0f;
	m_boundingSphere.fRadius = D3DXVec3Length(&(m_boundingBox.vMax - m_boundingBox.vMin)) / 2.0f;
}

void CBoundingVolume::Init( LPDIRECT3DDEVICE9 pDevice )
{
	m_pDevice = pDevice;
	memset(&m_boundingBox,0,sizeof(BoundingBox));
	memset(&m_boundingSphere,0,sizeof(BoundingSphere));
}

void CBoundingVolume::CalSphereByBox( D3DXMATRIX matWord )
{
	D3DXVec3TransformCoord(&m_boundingBox.vMax,&m_boundingBox.vMax,&matWord);
	D3DXVec3TransformCoord(&m_boundingBox.vMin,&m_boundingBox.vMin,&matWord);

	//通过包围盒求包围球
	m_boundingSphere.vCenter.x = (m_boundingBox.vMax.x + m_boundingBox.vMin.x) / 2.0f;
	m_boundingSphere.vCenter.y = (m_boundingBox.vMax.y + m_boundingBox.vMin.y) / 2.0f;
	m_boundingSphere.vCenter.z = (m_boundingBox.vMax.z + m_boundingBox.vMin.z) / 2.0f;
	m_boundingSphere.fRadius = D3DXVec3Length(&(m_boundingBox.vMax - m_boundingBox.vMin)) / 2.0f;
}