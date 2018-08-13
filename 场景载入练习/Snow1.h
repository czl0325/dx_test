#pragma once
#include "ParticleSystem.h"

class CSnow1 : public CParticleSystem
{
public:
	CSnow1(void);
	~CSnow1(void);


	BOOL InitParticle(IDirect3DDevice9* pD3dDevice);

	VOID Update(CTerrain* Terrain,D3DXVECTOR3 vRlose = D3DXVECTOR3(0.0f,0.0f,0.0f));
	VOID Render();
	VOID Clearup();




	BOOL CreateTexture(char* FileName);
	BOOL DetectBound(D3DXVECTOR3 vPosition);

	VOID SetD3dMaterial();
	VOID RenderState();

	FLOAT GetRand(FLOAT Min,FLOAT Max);





public:
	typedef struct PARTICLE
	{
		D3DXVECTOR3	Position;
		D3DCOLOR	Color;
	}PARTICLE,*LPPARTICLE;


	typedef struct ATTRBULE
	{
		D3DXVECTOR3	Position;//���λ��
		D3DXVECTOR3	Velocity;//��ʼ�ٶ�
		D3DXVECTOR3	Acceleration;//���ٶ�

		D3DXCOLOR	Color;//����ɫ
		D3DXCOLOR	FadeColor;//����ʧʱ����ɫ

		INT			LifeTime;//����ʱ��
		FLOAT		Age;//�Ѿ������ʱ��
		BOOL		IsAlife;//if Life the value is TRUE

	}ATTRBULE,*LPATTRBULE;

protected:
	static const DWORD FVF = (D3DFVF_XYZ|D3DFVF_DIFFUSE);

	vector<LPATTRBULE>			m_Paticle;

	IDirect3DVertexBuffer9*		m_pVB;
	IDirect3DTexture9*			m_pTextrue;

	D3DXVECTOR3					m_Origin;

	INT							m_MaxNum;
	INT							m_ChangeTime;
	INT							m_VbOffset;//��ʼ����
	INT							m_Vbset;//һ�λ��Ƽ�����
};
