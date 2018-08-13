#pragma once
#include "BaseObject.h"
#include "AnimMesh.h"
#include "Terrain.h"
#include "Config.h"
#include "Role.h"
#include "Camera.h"


class CMonster : public CBaseObject
{
private:
	LPDIRECT3DDEVICE9   m_pDevice;
	CTerrain*           m_pTerrain;
	CAnimMesh*          m_pAnimMesh;
	CRole*              m_pRole;

	bool			    m_bMove;	//怪物是否移动
	float			    m_fInterval;	//移动间隔
	float				m_fMoveElapse;	//已移动时间
	bool				m_bMoveByWalk;	// 当前行动状态为走路
	float				m_fTimeForMove;// 记录移动AI的时间

	float               m_fWalkSpeed;
	float               m_fRunSpeed;

	D3DXMATRIXA16       m_matWorld;
	D3DXVECTOR3         m_vMonster;	 //怪物的位置
	D3DXVECTOR3			m_vOldPos;   //怪物上一桢的位置
	D3DXVECTOR3         m_vLook;	 //怪物的朝向


public:
	CMonster(void);
	~CMonster(void);
	bool InitMonster(LPDIRECT3DDEVICE9 pdevice,CTerrain* pTerrain,CRole* pRole);
	void Update(float fElapse,D3DXVECTOR3 vPos);
	void Render(bool bBoundingVolume);

	void SetWorldMatrix();
	void CleanUp();

private:
	void	MoveAI();	// 移动AI
	void    Move();
};
