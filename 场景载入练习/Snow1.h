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
		D3DXVECTOR3	Position;//点的位置
		D3DXVECTOR3	Velocity;//初始速度
		D3DXVECTOR3	Acceleration;//加速度

		D3DXCOLOR	Color;//点颜色
		D3DXCOLOR	FadeColor;//点消失时的颜色

		INT			LifeTime;//生存时间
		FLOAT		Age;//已经生存的时间
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
	INT							m_VbOffset;//起始点编号
	INT							m_Vbset;//一次绘制几个点
};
