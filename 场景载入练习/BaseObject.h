#pragma once
#include "Header.h"
#include "BoundingVolume.h"
#include "Input.h"
#include "Terrain.h"

struct NORMALVERTEX  //纹理顶点  且有法线
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	FLOAT u,v;
};

#define D3DXFVF_NORMALVERTEX (D3DFVF_XYZ | D3DFVF_TEX1 |D3DFVF_NORMAL)

class CBaseObject
{
public:
	LPDIRECT3DDEVICE9    m_pDevice;  //D3D驱动
	CBoundingVolume*     m_pBoundingVolume;//外接体
	LPD3DXMESH           m_pSphere;  //碰撞球
	D3DXVECTOR3          m_vPosition;//位置

	D3DXVECTOR3			 m_vDirection;//角色的朝向

	D3DXMATRIX			 m_matBVWorld;//外接体的世界矩阵

public:
	bool	CreateBoundingVolumeMesh();//创建外接体模型
	bool	DrawBoundingVolumeMesh();  //绘制外接体模型
	bool	UpdateBoundingVolumeMesh();//更新外接体模型

	CBoundingVolume*	GetBoundingVolume(){return m_pBoundingVolume;}

	//virtual void Render(bool bBoundingVolume) = 0 ;
	//virtual void CleanUp() ;

	bool IsRoleInRange(D3DXVECTOR3 vPos1,D3DXVECTOR3 vPos2,float fDistance);//角色是否在范围内

	CBaseObject(void);
	~CBaseObject(void);
};
