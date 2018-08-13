#pragma once
#include "Header.h"
#include "AllocateHierarchy.h"
#include "BoundingVolume.h"

class CAnimMesh
{
private:
	CAllocateHierarchy*    m_pAlloc;
	CBoundingVolume*       m_pBoundingVolume;
	LPDIRECT3DDEVICE9      m_pd3dDevice;
	LPD3DXFRAME            m_pFrameRoot;



public:
	D3DXVECTOR3 m_vObjectCenter;//网格模型边界球的球心
	float m_fObjectRadius;      //网格模型边界球的半径
	bool m_bPlayAnim;           //是否播放模型中的动画
	LPD3DXANIMATIONCONTROLLER m_pAnimControler;//指向动画控制器接口ID3DXAnimationController的指针
private:
	HRESULT SetupBoneMatrixPointers( LPD3DXFRAME pFrame );//建立各级框架的组合变换矩阵
	HRESULT SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase );//设置每个骨骼的组合变换矩阵

	HRESULT LoadFromXFile(char* strFileName);//从.x中加载网格模型，并加载动画控制器
	void UpdateFrameMatrices(LPD3DXFRAME pFrameBase,LPD3DXMATRIX pParentMatrix);//计算得到各级框架的组合变换矩阵
	void DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase,LPD3DXFRAME pFrameBase);//渲染一个具体网格
	void DrawFrame(LPD3DXFRAME pFrame);//在DrawMeshContainer基础上渲染各级框架
public:
	HRESULT OnCreate(LPDIRECT3DDEVICE9 pd3dDevice, char* strFileName);//创建
	HRESULT Render(D3DXMATRIXA16* matWorld, double fElapsedAppTime);//渲染
	HRESULT OnDestroy();//销毁
	void SetAnimationSetByName( LPCSTR szAnimName );

	CBoundingVolume* GetBoundingVolume(){return m_pBoundingVolume;}
	CAnimMesh(void);
	~CAnimMesh(void);
};
