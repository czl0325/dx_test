#pragma once
#include "Header.h"
#include "Input.h"
#include "Terrain.h"
#include "Role.h"
#include "Plant.h"

const float MINDISTANCE = 0;//最小距离
const float MAXDISTANCE = 200;//最大距离
const float EPSILON = 0.02f;//偏移量
const float DEFAULTDISTANCE = 100.0f;//默认距离
const int BASEBARBOUND = 704.0f;//工具栏


class CCamera
{
private:
	LPDIRECT3DDEVICE9	m_pd3dDevice;

	D3DXVECTOR3			m_vPosition; //位置
	D3DXVECTOR3			m_vViewAt;   //Z向量
	D3DXVECTOR3			m_vUpAxis;   //Y向量
	D3DXVECTOR3			m_vRight;    //X向量
	D3DXVECTOR3			m_vToLook;   //朝向

	D3DXVECTOR3			m_vViewVector;    //位置到视点的矩阵
	D3DXVECTOR3			m_vStrafeVector;  //扫射的矩阵

	FLOAT				m_fRotateY;       //旋转的角度
	FLOAT				m_fDeltaY;        //旋转的速度
	bool				m_bZoom;		  //是否移动的状态

	D3DXMATRIX			m_matView;
	D3DXMATRIX			m_matProj;
	float				m_fDistance;      //视点到摄象机的距离
	float				m_fVArc;          //垂直角度
	float				m_fHArc;          //水平角度


	CTerrain*		    m_pTerrain;		  //地形指针
	CRole*				m_pRole;
	CPlant*             m_pPlant;
	CSceneObject*       m_pSceneObject;
	vector<CPlant*>     m_vPlant;

	int					m_nElapse;        //已经过去的时间

	int          m_nState;

public:
	CCamera(void);
	~CCamera(void);
	static CCamera* m_pCamera ;
	static CCamera* GetInstance()
	{
		if(m_pCamera==NULL)
			m_pCamera=new CCamera();
		return m_pCamera;
	}

	D3DXVECTOR3	GetLookOn() //得到摄象机的朝向
	{
		D3DXVECTOR3	LookOn;
		LookOn = m_vToLook - m_vPosition;
		return LookOn;
	}


	void SetDistance(FLOAT Distance);

	bool InitCamera(LPDIRECT3DDEVICE9 pDevice, CTerrain* pTerrain, float distance, CRole *pRole, CSceneObject* pSceneObject,CPlant* pPlant);
	void SetViewMatrix(D3DXMATRIX*	pViewMatrix);//设置视觉矩阵

	void X_Move(float speed);//x轴移动
	void Y_Move(float speed);//y轴移动
	void Z_Move(float speed);//z轴移动

	void X_Rotate(float angle);//x轴旋转
	void Y_Rotate(float angle);//y轴旋转
	void Z_Rotate(float angle);//z轴旋转
	void Axis_Rotate(D3DXVECTOR3* vec,float angle);//绕任意轴旋转

	void AddVArc(float arc);//增加垂直角度
	void AddHArc(float arc);//增加水平角度
	
	void Run(CInput* pInput);//摄像机的更新

	void DetectBound();//检测与地形碰撞

private:
	bool CheckDistance(BoundingSphere Sphere,
		D3DXVECTOR3 vObject, D3DXVECTOR3 vPlayer);	
	void ChangePos(D3DXVECTOR3 Vector);//改变摄像机的位置
	bool CheckPosInBoudingShere(BoundingSphere Sphere, D3DXVECTOR3 vCameraPos);//判断点是否在球体内


public:
	int GetState(){return m_nState;}
	D3DXVECTOR3 GetCameraPos(){return m_vPosition;}
	D3DXVECTOR3 GetCameraRight(){return m_vRight;}
	D3DXMATRIX GetCameraView(){return m_matView;}
	D3DXMATRIX GetCameraProj(){return m_matProj;}
};
