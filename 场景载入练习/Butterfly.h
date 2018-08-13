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
	D3DXVECTOR3 Pos;		//������λ��
	FLOAT		fWingSpeed;	//�����ȶ��ٶ�
	FLOAT		fFlyHeight;	//���и߶�
	FLOAT		fWingAngle;	//���ĽǶ�
	int			nAngle;		//�����ĳ���Ƕ�
	FLOAT		fSize;		//�����Ĵ�С
	D3DXMATRIX	matZoomRot;
	D3DXMATRIX	matWorld;
	BOOL		bVisible;	//�����Ƿ�ɼ�
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
	LPDIRECT3DTEXTURE9		m_ButterflyTexture;	//�����Ĵ�����
	LPD3DXEFFECT			m_ButterflyEffect;

	int						m_NumButterfly;		//������������
	Butterfly*				m_ButterflyColony;	//��¼����������

private:
	//������ֵ֮��ȡ�漴ֵ
	float	GetRandFloat(float fMin,float fMax);
	//����̫���ĺ�����Ϊ���ɼ�----��box
	bool	CheckPosInBoudingBox(ViewBox Box,D3DXVECTOR3 Pos);
	//����̫Զ��̫���ĺ�����Ϊ���ɼ�----��sphere
	bool	CheckPosInInSphere(D3DXVECTOR3 View,D3DXVECTOR3 Pos);

public:
	CButterfly(LPDIRECT3DDEVICE9 pD3DDevice);
	~CButterfly(void);
	bool	InitBufferfly();
	void	UpdateBufferfly(FLOAT TimeDelta,CTerrain* pTerrain);
	void	RenderBufferfly();
};
