#pragma once
#include "Header.h"
#include "Config.h"
#include "Terrain.h"

struct GRASS_VERTEX
{
	D3DXVECTOR3 Pos;//λ��
	D3DXVECTOR2 Tex;//����
	D3DXVECTOR2 Dir;//��XZƽ��İڶ�����
};

#define D3DFVF_GRASS (D3DFVF_XYZ | D3DFVF_TEX2)

class CGrass
{
private:
	LPDIRECT3DDEVICE9		m_pD3DDevice;
	LPDIRECT3DTEXTURE9		m_pGrassTex;	//�ݵĴ�����
	LPD3DXEFFECT			m_GrassEffect;	//Ч��
	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;//����
	LPDIRECT3DINDEXBUFFER9  m_pIndexBuffer;	//����

	int						m_NumGrass;		//�ݵ�������
	CTerrain*				m_pTerrain;
	D3DXVECTOR3				m_vGrassSize;	//�ݵĴ�С
	float					m_fWagSpeed;	//�ݵİڶ��ٶ�
	float					m_XOffset;		//�ݵ�X��ڶ���
	float					m_ZOffset;		//�ݵ�Y��ڶ���

private:
	bool	CreateGrassVertexBuffer();		//�����ݵĶ���
	bool	CreateGrassIndexBuffer();		//�����ݵ�����
	float	GetRandFloat(float fMin,float fMax);//������ֵ֮��ȡ�漴ֵ
	bool	TimeMode(float &StartTime, float TimeDelta, float TimeELapce);
	bool	CreateGrassEffectFromFile();

public:
	CGrass(void);
	~CGrass(void);
	bool	InitGrass(LPDIRECT3DDEVICE9 pD3DDevice, CTerrain* pTerrain);
	void	UpdateGrass(float TimeDelta);
	void	RenderGrass();
};
