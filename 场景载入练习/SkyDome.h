#pragma once
#include "Header.h"
#include "Config.h"

struct SKYDOMEVERTEX
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex;
};

#define D3DFVF_SKYDOMEVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)	//顶点格式

class CSkyDome
{
private:
	LPDIRECT3DDEVICE9		m_pD3DDevice;
	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9  m_pIndexBuffer;
	LPDIRECT3DTEXTURE9		m_pSkyBoxTex; 
	LPDIRECT3DTEXTURE9		m_pCloudTex;
	D3DXVECTOR2				m_vCloudOffset;		//云的偏移量

	int                     m_nRings;			//记录环的数目(横向-纬度)
	int                     m_nSegments;		//记录片段的数目(纵向-经度)
	DWORD                   m_dwNumOfVertices;	//顶点数
	DWORD                   m_dwNumOfIndices;	//索引数
	DWORD                   m_dwNumOfPolygons;	//三角形个数
	float					m_fRadius;			//半径

	//效果相关变量
	LPD3DXEFFECT			m_pSkyEffect;

private:
	bool	CreateVertices();	//计算出天空球的顶点
	bool	SetEffectValue();	//给shader(效果)赋值

public:
	CSkyDome(LPDIRECT3DDEVICE9 pD3DDevice);
	~CSkyDome(void);
	bool	InitSkyDome();
	void	UpdateSkyDome(float fTimeDelta);
	void	Render();
};
