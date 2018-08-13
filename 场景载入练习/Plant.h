#pragma once
#include "BaseObject.h"
#include "StaticObject.h"
#include "Terrain.h"
#include "Config.h"


class CPlant : public CBaseObject
{
public:
	CStaticObject*      m_pStaticObject;
	CTerrain*           m_pTerrain;
	float	            m_fScale;	       //缩放
	bool	            m_bIgnoreCollision;//是否进行碰撞检测
	bool	            m_bAlphaBlend;     //是否进行透明绘制

	vector<CPlant*> m_vPlant;//静态对象列表
	vector<CStaticObject*>	m_vecObjectMesh;//对象网格列表

	D3DXMATRIX matWorld;

public:
	bool Init(LPDIRECT3DDEVICE9 pDevice,CStaticObject* pStaticObject,float fScale,
		bool bIgnoreCollision,bool bAlphaBlend,D3DXVECTOR3 vPosition,CTerrain* pTerrain);
	void Render(bool bBoundingVolume);
	void CleanUp();

	bool PickMesh(D3DXVECTOR3* PickOrig,D3DXVECTOR3* PickDirect,
		D3DXVECTOR3& OutPickPosition,FLOAT& Dis,D3DXVECTOR3* Normal=NULL);




	CPlant(void);
	~CPlant(void);
};
