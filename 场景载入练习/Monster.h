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

	bool			    m_bMove;	//�����Ƿ��ƶ�
	float			    m_fInterval;	//�ƶ����
	float				m_fMoveElapse;	//���ƶ�ʱ��
	bool				m_bMoveByWalk;	// ��ǰ�ж�״̬Ϊ��·
	float				m_fTimeForMove;// ��¼�ƶ�AI��ʱ��

	float               m_fWalkSpeed;
	float               m_fRunSpeed;

	D3DXMATRIXA16       m_matWorld;
	D3DXVECTOR3         m_vMonster;	 //�����λ��
	D3DXVECTOR3			m_vOldPos;   //������һ���λ��
	D3DXVECTOR3         m_vLook;	 //����ĳ���


public:
	CMonster(void);
	~CMonster(void);
	bool InitMonster(LPDIRECT3DDEVICE9 pdevice,CTerrain* pTerrain,CRole* pRole);
	void Update(float fElapse,D3DXVECTOR3 vPos);
	void Render(bool bBoundingVolume);

	void SetWorldMatrix();
	void CleanUp();

private:
	void	MoveAI();	// �ƶ�AI
	void    Move();
};
