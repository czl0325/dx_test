#pragma once
#include "BaseObject.h"
#include "StaticObject.h"

class CHouse : public CBaseObject
{
private:
	LPDIRECT3DDEVICE9	m_pD3DDevice;
	CStaticObject*		m_pStaticObject;
	D3DXMATRIX			m_matWorld;
	LPD3DXMESH			m_pMeshInfo;

	D3DXVECTOR3         m_vObject;

private:
	//���㾲̬������������
	void CalWorldMatrix(D3DXVECTOR3* Pos,D3DXVECTOR3* RotAngle,D3DXVECTOR3* Zoom);
	DWORD	F2DW( FLOAT f ){ return *( (DWORD*)&f );}

public:
	CHouse(void);
	~CHouse(void);
	//��ʼ����̬����
	bool InitStaticObject(LPDIRECT3DDEVICE9 pdevice,CStaticObject* pStaticObject,D3DXVECTOR3 Pos,D3DXVECTOR3 Rot,D3DXVECTOR3 Zoom);
	//��Ⱦ��̬����
	void RenderStaticObject();
	void UpdateStaticObject();

	//�ж����ߺ������Ƿ��з�������
	BOOL CheckIntersect(D3DXVECTOR3 vRayOrigin,D3DXVECTOR3 vRayDir,D3DXVECTOR3& vHitPoint,float& fDistance,D3DXVECTOR3* vNormal);

	BoundingSphere GetBoundingSphere(){return m_pStaticObject->GetBoundingSphere();}
	D3DXVECTOR3 GetObjPosition(){return m_vObject;}
};
