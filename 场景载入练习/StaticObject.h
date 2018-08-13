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
	LPD3DXMESH                   m_pMesh;          //保存读取的X文件到网格
	CBoundingVolume              m_BoundingVolume; //包围盒
	vector<D3DMATERIAL9>         m_vMtrls;         //材质容器
	vector<LPDIRECT3DTEXTURE9>   m_vTextures;      //纹理容器
public:
	bool Init(LPDIRECT3DDEVICE9 Device,char* pFileName);
	void Render();
	void UpdateBounding(D3DXMATRIX matWord);

	LPD3DXMESH GetMeshInformation(){return m_pMesh;}
	BoundingSphere GetBoundingSphere(){return m_BoundingVolume.Getboundingspehere();}

	CStaticObject(void);
	~CStaticObject(void);
};
