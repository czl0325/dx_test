#pragma once
#include "Header.h"
#include "BoundingVolume.h"

struct MESHVERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	float       u, v;

	static const DWORD FVF;
};

class CStaticObject
{
public:
	LPDIRECT3DDEVICE9            m_pDevice;
	LPD3DXMESH                   m_pMesh;          //�����ȡ��X�ļ�������
	CBoundingVolume              m_BoundingVolume; //��Χ��
	vector<D3DMATERIAL9>         m_vMtrls;         //��������
	vector<LPDIRECT3DTEXTURE9>   m_vTextures;      //��������
public:
	bool Init(LPDIRECT3DDEVICE9 Device,char* pFileName);
	void Render();
	void UpdateBounding(D3DXMATRIX matWord);

	LPD3DXMESH GetMeshInformation(){return m_pMesh;}
	BoundingSphere GetBoundingSphere(){return m_BoundingVolume.Getboundingspehere();}

	CStaticObject(void);
	~CStaticObject(void);
};
