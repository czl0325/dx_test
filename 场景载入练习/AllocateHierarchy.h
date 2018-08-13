#pragma once
#include "Header.h"
#include "BoundingVolume.h"

// 继承自DXDXFRAME结构的结构
struct D3DXFRAME_DERIVED: public D3DXFRAME
{
	//用来记录从所有的父框架到该框架的初始变换
	D3DXMATRIXA16 CombinedTransformationMatrix;
};

// 继承自D3DXMESHCONTAINER结构的结构

struct D3DXMESHCONTAINER_DERIVED: public D3DXMESHCONTAINER
{
	LPDIRECT3DTEXTURE9*  ppTextures;            //纹理数组
	LPD3DXMESH           pOrigMesh;             //原始网格
	DWORD                NumInfl;               //每个顶点最多受多少骨骼的影响
	DWORD                NumAttributeGroups;    //属性组数量,即子网格数量
	LPD3DXBUFFER         pBoneCombinationBuf;   //骨骼结合表
	D3DXMATRIX**         ppBoneMatrixPtrs;      //存放骨骼的组合变换矩阵
	D3DXMATRIX*          pBoneOffsetMatrices;   //存放骨骼的初始变换矩阵
	DWORD                NumPaletteEntries;     //骨骼数量上限
	bool                 UseSoftwareVP;         //标识是否使用软件顶点处理
};

// 用来从.X文件加载框架层次和网格模型数据
class CAllocateHierarchy : public ID3DXAllocateHierarchy
{
public:
	HRESULT GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED *pMeshContainer);
	HRESULT AllocateName(LPCSTR Name,LPSTR* pNewName);    


	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name,LPD3DXFRAME *ppNewFrame); //创建框架
	STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR  Name,
									const D3DXMESHDATA* pMeshData,
									const D3DXMATERIAL* pMaterials,
									const D3DXEFFECTINSTANCE* pEffectInstances,
									DWORD  NumMaterials,
									const DWORD*   pAdjacency,
									LPD3DXSKININFO pSkinInfo,
									LPD3DXMESHCONTAINER* ppNewMeshContainer);//创建网格容器
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);//销毁框架
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);//销毁网格容器
	CAllocateHierarchy(void);
	~CAllocateHierarchy(void);
};
