#pragma once
#include "Header.h"
#include "Config.h"
#include "Terrain.h"

struct GRASS_VERTEX
{
	D3DXVECTOR3 Pos;//位置
	D3DXVECTOR2 Tex;//纹理
	D3DXVECTOR2 Dir;//在XZ平面的摆动方向
};

#define D3DFVF_GRASS (D3DFVF_XYZ | D3DFVF_TEX2)

class CGrass
{
private:
	LPDIRECT3DDEVICE9		m_pD3DDevice;
	LPDIRECT3DTEXTURE9		m_pGrassTex;	//草的大纹理
	LPD3DXEFFECT			m_GrassEffect;	//效果
	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;//顶点
	LPDIRECT3DINDEXBUFFER9  m_pIndexBuffer;	//索引

	int						m_NumGrass;		//草的总数量
	CTerrain*				m_pTerrain;
	D3DXVECTOR3				m_vGrassSize;	//草的大小
	float					m_fWagSpeed;	//草的摆动速度
	float					m_XOffset;		//草的X轴摆动量
	float					m_ZOffset;		//草的Y轴摆动量

private:
	bool	CreateGrassVertexBuffer();		//创建草的顶点
	bool	CreateGrassIndexBuffer();		//创建草的索引
	float	GetRandFloat(float fMin,float fMax);//在两个值之间取随即值
	bool	TimeMode(float &StartTime, float TimeDelta, float TimeELapce);
	bool	CreateGrassEffectFromFile();

public:
	CGrass(void);
	~CGrass(void);
	bool	InitGrass(LPDIRECT3DDEVICE9 pD3DDevice, CTerrain* pTerrain);
	void	UpdateGrass(float TimeDelta);
	void	RenderGrass();
};
