#pragma once
#include "BaseObject.h"
#include "AnimMesh.h"
#include "GameTime.h"
#include "SceneObject.h"
#include "House.h"

#define RUNSPEED 80.0f
#define WALKSPEED 50.0f

struct MarkerVertex
{
	float x,y,z;
	float u,v;
	MarkerVertex(float _x,float _y,float _z,float _u,float _v)
	{
		x=_x;y=_y;z=_z;
		u=_u;v=_v;
	}
	static const DWORD FVF=D3DFVF_XYZ|D3DFVF_TEX1;
};

enum Role_State{STAND,WALK,RUN,SKILL};


class CRole : public CBaseObject
{
public:
	CAnimMesh*  m_AnimMesh1;//��Ƥ��������ģ��
	CTerrain*   m_pTerrain;
	CSceneObject*	m_pSceneObject;//���ڼ������;�̬�������ײ

	D3DXVECTOR3 m_vRole;     //��ɫ��λ��
	D3DXVECTOR3 m_vOldPos;   //��ɫ��һ���λ��
	Role_State	m_RoleState; //��ɫ״̬;

	D3DXMATRIXA16 m_matWorld;//������������
	D3DXMATRIX m_matRotation;//�������ת����

	float m_fAngle;		//��ɫ��ת�Ƕ�
	float m_fSpeed;     //��ɫ���ƶ��ٶ�
	float m_SkillTime;	// ���Ƽ��ܵĲ���ʱ��

	bool m_bWalk;       //�����Ƿ����ƶ�
	bool m_bMoveByWalk;	//�����߻�����
	bool m_bPlayAnim;   //��־�Ƿ񲥷�ģ�Ͷ���
	bool m_bSkill;      //�����ĸ�����


//*******************����ʰȡ******************************//
	LPDIRECT3DTEXTURE9	m_pMarker;//ʰȡ�ı�ʶ
	LPDIRECT3DVERTEXBUFFER9	m_pVB;//ʰȡ�Ķ���
	D3DXMATRIX m_matMarker;//��ʶ�����������
	D3DXMATRIX m_matRotationMarker;//��ʶ������ת����
	D3DXVECTOR3 m_vDestPos;//ʰȡ������

	bool m_bMarker;//�Ƿ����ʰȡͼƬ
	bool m_bMoveByMouse;//�Ƿ�������ƶ�
//*********************************************************//

public:
	bool InitMarker();//��ʼ��ʰȡͼ��
	bool OnCreate(LPDIRECT3DDEVICE9	pD3DDevice,CTerrain* pTerrain , D3DXVECTOR3 vRole,CSceneObject* pSceneObject);
	void SetWorldMatrix();//����Ӣ���Լ����������

	void KeyBoardDown(float fElaspTime);//�����°��¼�	
	void KeyBoardUp(WPARAM wParam);//�����ϵ��¼�
	void JudgeRoleState(float fElaspTime);//�ж�����״̬
	bool MouseMove(float fElaspTime);//����ƶ�

	void Render(bool bBoundingVolume,float fElaspTime,int nState);
	void CleanUp();

	CRole(void);
	~CRole(void);


//***********************************������ײ���*********************************//
private:
	//�жϵ��Ƿ���������
	bool CheckPosInBoudingShere(BoundingSphere Sphere, D3DXVECTOR3 vPlayerPos);
	//���������ģ�͵���ײ���
	bool MeshCollisionTest( CHouse* pStaticObject,D3DXVECTOR3& NewPos,
							float HeightOrig,D3DXVECTOR3 vLookat,float AngleByY,
							D3DXVECTOR3* NewPosOnMesh = NULL);
	//��ײ���
	bool CheckCollision(CHouse* pStaticObject);
	//�ж������Ƿ����ƶ�
	bool CanbeMove();
//********************************************************************************//
};
