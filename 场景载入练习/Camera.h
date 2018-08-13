#pragma once
#include "Header.h"
#include "Input.h"
#include "Terrain.h"
#include "Role.h"
#include "Plant.h"

const float MINDISTANCE = 0;//��С����
const float MAXDISTANCE = 200;//������
const float EPSILON = 0.02f;//ƫ����
const float DEFAULTDISTANCE = 100.0f;//Ĭ�Ͼ���
const int BASEBARBOUND = 704.0f;//������


class CCamera
{
private:
	LPDIRECT3DDEVICE9	m_pd3dDevice;

	D3DXVECTOR3			m_vPosition; //λ��
	D3DXVECTOR3			m_vViewAt;   //Z����
	D3DXVECTOR3			m_vUpAxis;   //Y����
	D3DXVECTOR3			m_vRight;    //X����
	D3DXVECTOR3			m_vToLook;   //����

	D3DXVECTOR3			m_vViewVector;    //λ�õ��ӵ�ľ���
	D3DXVECTOR3			m_vStrafeVector;  //ɨ��ľ���

	FLOAT				m_fRotateY;       //��ת�ĽǶ�
	FLOAT				m_fDeltaY;        //��ת���ٶ�
	bool				m_bZoom;		  //�Ƿ��ƶ���״̬

	D3DXMATRIX			m_matView;
	D3DXMATRIX			m_matProj;
	float				m_fDistance;      //�ӵ㵽������ľ���
	float				m_fVArc;          //��ֱ�Ƕ�
	float				m_fHArc;          //ˮƽ�Ƕ�


	CTerrain*		    m_pTerrain;		  //����ָ��
	CRole*				m_pRole;
	CPlant*             m_pPlant;
	CSceneObject*       m_pSceneObject;
	vector<CPlant*>     m_vPlant;

	int					m_nElapse;        //�Ѿ���ȥ��ʱ��

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

	D3DXVECTOR3	GetLookOn() //�õ�������ĳ���
	{
		D3DXVECTOR3	LookOn;
		LookOn = m_vToLook - m_vPosition;
		return LookOn;
	}


	void SetDistance(FLOAT Distance);

	bool InitCamera(LPDIRECT3DDEVICE9 pDevice, CTerrain* pTerrain, float distance, CRole *pRole, CSceneObject* pSceneObject,CPlant* pPlant);
	void SetViewMatrix(D3DXMATRIX*	pViewMatrix);//�����Ӿ�����

	void X_Move(float speed);//x���ƶ�
	void Y_Move(float speed);//y���ƶ�
	void Z_Move(float speed);//z���ƶ�

	void X_Rotate(float angle);//x����ת
	void Y_Rotate(float angle);//y����ת
	void Z_Rotate(float angle);//z����ת
	void Axis_Rotate(D3DXVECTOR3* vec,float angle);//����������ת

	void AddVArc(float arc);//���Ӵ�ֱ�Ƕ�
	void AddHArc(float arc);//����ˮƽ�Ƕ�
	
	void Run(CInput* pInput);//������ĸ���

	void DetectBound();//����������ײ

private:
	bool CheckDistance(BoundingSphere Sphere,
		D3DXVECTOR3 vObject, D3DXVECTOR3 vPlayer);	
	void ChangePos(D3DXVECTOR3 Vector);//�ı��������λ��
	bool CheckPosInBoudingShere(BoundingSphere Sphere, D3DXVECTOR3 vCameraPos);//�жϵ��Ƿ���������


public:
	int GetState(){return m_nState;}
	D3DXVECTOR3 GetCameraPos(){return m_vPosition;}
	D3DXVECTOR3 GetCameraRight(){return m_vRight;}
	D3DXMATRIX GetCameraView(){return m_matView;}
	D3DXMATRIX GetCameraProj(){return m_matProj;}
};
