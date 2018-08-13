#pragma once
#include "Header.h"
#include "Config.h"
#include "Frustum.h"
#include "Terrain.h"
#include "Camera.h"

struct BUTTERFLY_VERTEX
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR2 Tex;
};

#define D3DFVF_BUTTERFLY (D3DFVF_XYZ | D3DFVF_TEX1 )

struct Butterfly 
{
	D3DXVECTOR3 Pos;		//蝴蝶的位置
	FLOAT		fWingSpeed;	//翅膀的扇动速度
	FLOAT		fFlyHeight;	//飞行高度
	FLOAT		fWingAngle;	//翅膀的角度
	int			nAngle;		//蝴蝶的朝向角度
	FLOAT		fSize;		//蝴蝶的大小
	D3DXMATRIX	matZoomRot;
	D3DXMATRIX	matWorld;
	BOOL		bVisible;	//蝴蝶是否可见
};

struct ViewBox
{
	D3DXVECTOR3 s_Max;
	D3DXVECTOR3 s_Min;
};

class CButterfly
{
private:
	LPDIRECT3DDEVICE9		m_pD3DDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	LPDIRECT3DTEXTURE9		m_ButterflyTexture;	//蝴蝶的大纹理
	LPD3DXEFFECT			m_ButterflyEffect;

	int						m_NumButterfly;		//蝴蝶的总数量
	Butterfly*				m_ButterflyColony;	//记录蝴蝶的属性

private:
	//在两个值之间取随即值
	float	GetRandFloat(float fMin,float fMax);
	//控制太近的蝴蝶都为不可见----用box
	bool	CheckPosInBoudingBox(ViewBox Box,D3DXVECTOR3 Pos);
	//控制太远和太近的蝴蝶都为不可见----用sphere
	bool	CheckPosInInSphere(D3DXVECTOR3 View,D3DXVECTOR3 Pos);

public:
	CButterfly(LPDIRECT3DDEVICE9 pD3DDevice);
	~CButterfly(void);
	bool	InitBufferfly();
	void	UpdateBufferfly(FLOAT TimeDelta,CTerrain* pTerrain);
	void	RenderBufferfly();
};
