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


	bool m_bSnow;     //是否下雪
	bool m_bFog;      //是否雾化
	bool m_bSkySphere;//是否为天空球
	bool m_bDrawBoundingVolume;//是否绘制包围球

public:
	CD3DInit(void);
	~CD3DInit(void);
	bool GameInit(HWND hWnd,HINSTANCE hInstance);
	HRESULT InitD3D(HWND hWnd);
	void Render();
	void MatricesTrans();
	void ReadInput();
	void KeyDown(WPARAM wParam);
	void KeyUp(WPARAM wParam);//键盘弹起

	void ShowFontInfo();

public:
	bool LoadObjectMeshList(LPDIRECT3DDEVICE9 pDevice,CTerrain* pTerrain);//载入静态模型
};
