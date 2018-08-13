#pragma once
#include "Header.h"

struct SKYSPHEREVERTEX
{
	FLOAT x,y,z;	//��������
	FLOAT nx,ny,nz;	//���㷨��
	FLOAT u,v;		//������������
};

#define D3DFVF_SKYSPHEREVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)	//�����ʽ

class CSkySphere
{
private:
	LPDIRECT3DDEVICE9		m_pD3DDevice;
	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;
	LPDIRECT3DTEXTURE9		m_pTexture;
	LPDIRECT3DINDEXBUFFER9  m_pIndexBuffer;


	//Ч����ر���
	LPDIRECT3DTEXTURE9		m_pSkyBoxTex; 
	LPDIRECT3DTEXTURE9		m_pCloudTex;
	LPD3DXEFFECT			m_pSkyEffect;
	D3DXVECTOR2				m_vCloudOffset;		//�Ƶ�ƫ����



	int                     m_nRings;				//��¼������Ŀ(����-γ��)
	int                     m_nSegments;			//��¼Ƭ�ε���Ŀ(����-����)
	DWORD                   m_dwNumOfVertices;		//������
	DWORD                   m_dwNumOfIndices;		//������
	DWORD                   m_dwNumOfPolygons;		//�����θ���
	float					m_fRadius;			    //�뾶


public:
	bool     UpdateVertices();
	bool     SetSkyDomeTexture(const char* szTextureFilePath);
	bool     Render();


	bool     SetEffectValue();//����Ч������



	CSkySphere(LPDIRECT3DDEVICE9 pD3DDevice, int nRings = 20, int nSegments = 20);
	~CSkySphere(void);
};
