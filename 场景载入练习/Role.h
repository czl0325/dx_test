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
	CAnimMesh*  m_AnimMesh1;//蒙皮动画网格模型
	CTerrain*   m_pTerrain;
	CSceneObject*	m_pSceneObject;//用于检测人物和静态物件的碰撞

	D3DXVECTOR3 m_vRole;     //角色的位置
	D3DXVECTOR3 m_vOldPos;   //角色上一桢的位置
	Role_State	m_RoleState; //角色状态;

	D3DXMATRIXA16 m_matWorld;//人物的世界矩阵
	D3DXMATRIX m_matRotation;//人物的旋转矩阵

	float m_fAngle;		//角色旋转角度
	float m_fSpeed;     //角色的移动速度
	float m_SkillTime;	// 控制技能的播放时间

	bool m_bWalk;       //人物是否在移动
	bool m_bMoveByWalk;	//人物走还是跑
	bool m_bPlayAnim;   //标志是否播放模型动画
	bool m_bSkill;      //播放哪个技能


//*******************关于拾取******************************//
	LPDIRECT3DTEXTURE9	m_pMarker;//拾取的标识
	LPDIRECT3DVERTEXBUFFER9	m_pVB;//拾取的顶点
	D3DXMATRIX m_matMarker;//标识符的世界矩阵
	D3DXMATRIX m_matRotationMarker;//标识符的旋转矩阵
	D3DXVECTOR3 m_vDestPos;//拾取的坐标

	bool m_bMarker;//是否绘制拾取图片
	bool m_bMoveByMouse;//是否是鼠标移动
//*********************************************************//

public:
	bool InitMarker();//初始化拾取图标
	bool OnCreate(LPDIRECT3DDEVICE9	pD3DDevice,CTerrain* pTerrain , D3DXVECTOR3 vRole,CSceneObject* pSceneObject);
	void SetWorldMatrix();//设置英雄自己的世界矩阵

	void KeyBoardDown(float fElaspTime);//键盘下按事件	
	void KeyBoardUp(WPARAM wParam);//键盘上弹事件
	void JudgeRoleState(float fElaspTime);//判断人物状态
	bool MouseMove(float fElaspTime);//鼠标移动

	void Render(bool bBoundingVolume,float fElaspTime,int nState);
	void CleanUp();

	CRole(void);
	~CRole(void);


//***********************************射线碰撞检测*********************************//
private:
	//判断点是否在球体内
	bool CheckPosInBoudingShere(BoundingSphere Sphere, D3DXVECTOR3 vPlayerPos);
	//人物和网格模型的碰撞检测
	bool MeshCollisionTest( CHouse* pStaticObject,D3DXVECTOR3& NewPos,
							float HeightOrig,D3DXVECTOR3 vLookat,float AngleByY,
							D3DXVECTOR3* NewPosOnMesh = NULL);
	//碰撞检测
	bool CheckCollision(CHouse* pStaticObject);
	//判断人物是否能移动
	bool CanbeMove();
//********************************************************************************//
};
