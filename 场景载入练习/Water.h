#pragma once
#include "Header.h"
#include "Role.h"
#include "Terrain.h"
#include "SkyDome.h"
#include "Config.h"

struct	CUSTOMVERTEX
{
	FLOAT	x,y,z;
	FLOAT	u,v;
	FLOAT	u2,v2;
	static const DWORD FVF=D3DFVF_XYZ|D3DFVF_TEX2;
};

class CWater
{
private:
	LPDIRECT3DDEVICE9		m_pD3DDevice;
	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;
	LPDIRECT3DTEXTURE9		m_pTexture;
	LPDIRECT3DTEXTURE9		m_pTexture2;

	LPDIRECT3DVERTEXBUFFER9	m_pMirrorVB;//���ӵĻ�����



	CRole*					m_pRole;
	CTerrain*				m_pTerrain;
	CSkyDome*				m_pSkyDome;//�����ָ��	
	D3DXMATRIX				m_matWorld;
	LPD3DXEFFECT			m_pEffect;//Ч���ļ�

	int		m_nSize,m_nNumVertex,m_nNumIndex;
	int		m_nNumTriangle;
	int		m_nScellpace;//���ӵĴ�С
	int		m_nRow,m_nCol;//�к���
	int		m_nWidth,m_nDepth;//���Ϳ�

private:
	BOOL	CreateVertex();//��������
	BOOL	CreateIndex();//��������

public:
	CWater(void);
	~CWater(void);
	BOOL	InitWater(LPDIRECT3DDEVICE9	pDevice,CRole*	pRole,CTerrain*	pTerrain);//��ʼ������
	BOOL	Render(float fTime);
	BOOL	Update(float fTime);
};
