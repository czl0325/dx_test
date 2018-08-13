#pragma once
#include "Header.h"
#include "BoundingVolume.h"
#include "Input.h"
#include "Terrain.h"

struct NORMALVERTEX  //������  ���з���
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	FLOAT u,v;
};

#define D3DXFVF_NORMALVERTEX (D3DFVF_XYZ | D3DFVF_TEX1 |D3DFVF_NORMAL)

class CBaseObject
{
public:
	LPDIRECT3DDEVICE9    m_pDevice;  //D3D����
	CBoundingVolume*     m_pBoundingVolume;//�����
	LPD3DXMESH           m_pSphere;  //��ײ��
	D3DXVECTOR3          m_vPosition;//λ��

	D3DXVECTOR3			 m_vDirection;//��ɫ�ĳ���

	D3DXMATRIX			 m_matBVWorld;//�������������

public:
	bool	CreateBoundingVolumeMesh();//���������ģ��
	bool	DrawBoundingVolumeMesh();  //���������ģ��
	bool	UpdateBoundingVolumeMesh();//���������ģ��

	CBoundingVolume*	GetBoundingVolume(){return m_pBoundingVolume;}

	//virtual void Render(bool bBoundingVolume) = 0 ;
	//virtual void CleanUp() ;

	bool IsRoleInRange(D3DXVECTOR3 vPos1,D3DXVECTOR3 vPos2,float fDistance);//��ɫ�Ƿ��ڷ�Χ��

	CBaseObject(void);
	~CBaseObject(void);
};
