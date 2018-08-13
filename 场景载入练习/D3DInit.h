#pragma once
#include "Header.h"
#include "SkyBox.h"
#include "SkySphere.h"
#include "Terrain.h"
#include "Sun.h"
#include "Sun2.h"
#include "Role.h"
#include "Snow.h"
#include "Snow1.h"
#include "Plant.h"
#include "Mouse.h"
#include "Monster.h"
#include "Collision.h"
#include "SkyDome.h"
#include "Butterfly.h"
#include "Grass.h"
#include "Water.h"
#include "SceneObject.h"

class CD3DInit
{
private:
	LPDIRECT3D9         g_pD3D;
	LPDIRECT3DDEVICE9   g_pd3dDevice;
	CSkyBox*            g_SkyBox;
	CTerrain*           g_Terrain;
	CSun*               g_Sun;
	CRole*              g_Role;
	CSnow*              g_Snow;
	CSnow1*             g_Snow1;
	CSkySphere*         g_SkySphere;
	CPlant*             g_Plant;
	CMouse*             g_Mouse;
	CMonster*           g_Monster;
	CSkyDome*           g_SkyDome;
	CSun2*              g_Sun2;
	CButterfly*         g_Butterfly;
	CGrass*             g_Grass;
	CWater*             g_Water;
	CSceneObject*       g_SceneObject;


	bool m_bSnow;     //�Ƿ���ѩ
	bool m_bFog;      //�Ƿ���
	bool m_bSkySphere;//�Ƿ�Ϊ�����
	bool m_bDrawBoundingVolume;//�Ƿ���ư�Χ��

public:
	CD3DInit(void);
	~CD3DInit(void);
	bool GameInit(HWND hWnd,HINSTANCE hInstance);
	HRESULT InitD3D(HWND hWnd);
	void Render();
	void MatricesTrans();
	void ReadInput();
	void KeyDown(WPARAM wParam);
	void KeyUp(WPARAM wParam);//���̵���

	void ShowFontInfo();

public:
	bool LoadObjectMeshList(LPDIRECT3DDEVICE9 pDevice,CTerrain* pTerrain);//���뾲̬ģ��
};
