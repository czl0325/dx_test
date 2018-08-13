#pragma once
#include "Header.h"

struct SKYSPHEREVERTEX
{
	FLOAT x,y,z;	//顶点坐标
	FLOAT nx,ny,nz;	//顶点法线
	FLOAT u,v;		//顶点纹理坐标
};

#define D3DFVF_SKYSPHEREVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)	//顶点格式

class CSkySphere
{
private:
	LPDIRECT3DDEVICE9		m_pD3DDevice;
	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;
	LPDIRECT3DTEXTURE9		m_pTexture;
	LPDIRECT3DINDEXBUFFER9  m_pIndexBuffer;


	//效果相关变量
	LPDIRECT3DTEXTURE9		m_pSkyBoxTex; 
	LPDIRECT3DTEXTURE9		m_pCloudTex;
	LPD3DXEFFECT			m_pSkyEffect;
	D3DXVECTOR2				m_vCloudOffset;		//云的偏移量



	int                     m_nRings;				//记录环的数目(横向-纬度)
	int                     m_nSegments;			//记录片段的数目(纵向-经度)
	DWORD                   m_dwNumOfVertices;		//顶点数
	DWORD                   m_dwNumOfIndices;		//索引数
	DWORD                   m_dwNumOfPolygons;		//三角形个数
	float					m_fRadius;			    //半径


public:
	bool     UpdateVertices();
	bool     SetSkyDomeTexture(const char* szTextureFilePath);
	bool     Render();


	bool     SetEffectValue();//设置效果参数



	CSkySphere(LPDIRECT3DDEVICE9 pD3DDevice, int nRings = 20, int nSegments = 20);
	~CSkySphere(void);
};
