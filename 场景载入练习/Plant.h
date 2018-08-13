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
	float	            m_fScale;	       //����
	bool	            m_bIgnoreCollision;//�Ƿ������ײ���
	bool	            m_bAlphaBlend;     //�Ƿ����͸������

	vector<CPlant*> m_vPlant;//��̬�����б�
	vector<CStaticObject*>	m_vecObjectMesh;//���������б�

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
