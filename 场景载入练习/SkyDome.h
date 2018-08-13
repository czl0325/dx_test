#pragma once
#include "Header.h"
#include "Config.h"

struct SKYDOMEVERTEX
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex;
};

#define D3DFVF_SKYDOMEVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)	//�����ʽ

class CSkyDome
{
private:
	LPDIRECT3DDEVICE9		m_pD3DDevice;
	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9  m_pIndexBuffer;
	LPDIRECT3DTEXTURE9		m_pSkyBoxTex; 
	LPDIRECT3DTEXTURE9		m_pCloudTex;
	D3DXVECTOR2				m_vCloudOffset;		//�Ƶ�ƫ����

	int                     m_nRings;			//��¼������Ŀ(����-γ��)
	int                     m_nSegments;		//��¼Ƭ�ε���Ŀ(����-����)
	DWORD                   m_dwNumOfVertices;	//������
	DWORD                   m_dwNumOfIndices;	//������
	DWORD                   m_dwNumOfPolygons;	//�����θ���
	float					m_fRadius;			//�뾶

	//Ч����ر���
	LPD3DXEFFECT			m_pSkyEffect;

private:
	bool	CreateVertices();	//����������Ķ���
	bool	SetEffectValue();	//��shader(Ч��)��ֵ

public:
	CSkyDome(LPDIRECT3DDEVICE9 pD3DDevice);
	~CSkyDome(void);
	bool	InitSkyDome();
	void	UpdateSkyDome(float fTimeDelta);
	void	Render();
};
