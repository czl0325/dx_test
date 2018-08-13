#include "AllocateHierarchy.h"

CAllocateHierarchy::CAllocateHierarchy(void)
{
}

CAllocateHierarchy::~CAllocateHierarchy(void)
{
}


HRESULT CAllocateHierarchy::AllocateName( LPCSTR Name, LPSTR *pNewName )
{
	UINT cbLength;

	if( Name != NULL )
	{
		cbLength = (UINT)strlen(Name) + 1;
		*pNewName = new CHAR[cbLength];
		if (*pNewName == NULL)
			return E_OUTOFMEMORY;
		memcpy( *pNewName, Name, cbLength*sizeof(CHAR) );
	}
	else
		*pNewName = NULL;

	return S_OK;
}


//生成蒙皮网格模型(含有每个顶点的混合权重、索引和一个骨骼组合表)
HRESULT CAllocateHierarchy::GenerateSkinnedMesh( D3DXMESHCONTAINER_DERIVED *pMeshContainer )
{
	HRESULT hr = S_OK;

	if (pMeshContainer->pSkinInfo == NULL)//判断当前网格容器是否包含有蒙皮信息
		return hr;

	SAFE_RELEASE(pMeshContainer->MeshData.pMesh);
	SAFE_RELEASE(pMeshContainer->pBoneCombinationBuf);

	//获取原始网格模型索引缓冲区
	LPDIRECT3DINDEXBUFFER9 pIB;
	if (FAILED(hr = pMeshContainer->pOrigMesh->GetIndexBuffer(&pIB)))//pOrigMesh为原始网格模型
		return E_FAIL;

	//获取影响一个面(三角形)的矩阵数量
	DWORD NumMaxFaceInfl;
	hr = pMeshContainer->pSkinInfo->GetMaxFaceInfluences(pIB, 
		pMeshContainer->pOrigMesh->GetNumFaces(),//原始网格数目
		&NumMaxFaceInfl);
	pIB->Release();
	if (FAILED(hr))
		return E_FAIL;

	//设置影响一个面的矩阵数量不能超过12
	NumMaxFaceInfl = min(NumMaxFaceInfl, 12);

	//获取当前设备的能力
	IDirect3DDevice9* pd3dDevice;
	pMeshContainer->pOrigMesh->GetDevice(&pd3dDevice);//得到一个指向设备的接口
	D3DCAPS9 d3dCaps;//描述设备硬件的能力
	pd3dDevice->GetDeviceCaps( &d3dCaps );//获取硬件渲染能力

	//根据当前设备能力和NumMaxFaceInfl, 设置在进行骨骼蒙皮调色时使用骨骼数量的上限
	//d3dCaps.MaxVertexBlendMatrixIndex表示当前设备对索引顶点混合的支持,为0表示当前设备不支持索引顶点混合
	//d3dCaps.MaxVertexBlendMatrixIndex+1)/2表示矩阵调色板中矩阵的最大数量(因为还有一半要包含法线信息)
	if( (d3dCaps.MaxVertexBlendMatrixIndex+1)/2 < NumMaxFaceInfl )
	{
		//如果使用软件顶点处理模式,则矩阵调色板的最大容量为256,不管是否需要对法线向量进行混合
		pMeshContainer->NumPaletteEntries = min(256, pMeshContainer->pSkinInfo->GetNumBones());
		pMeshContainer->UseSoftwareVP = true;
	}
	else
	{
		pMeshContainer->NumPaletteEntries = min( (d3dCaps.MaxVertexBlendMatrixIndex+1) / 2, 
			pMeshContainer->pSkinInfo->GetNumBones() );
		pMeshContainer->UseSoftwareVP = false;
	}

	//根据原初始网格模型提供的相应参数生成索引蒙皮网格模型(使用索引顶点混合)
	hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
		( pMeshContainer->pOrigMesh,//表示输入的原网格
		0,//当前未使用
		pMeshContainer->NumPaletteEntries,//表示可以使用的矩阵调色板容量
		pMeshContainer->pAdjacency, NULL,//分别表示输入和输出的网格邻接信息
		NULL, NULL,//表示新生成的蒙皮网格中面和顶点将如何射到原网格模型中的面和顶点
		&pMeshContainer->NumInfl,//输出一个顶点最多可能受多少块骨骼的影响
		&pMeshContainer->NumAttributeGroups,//输出骨骼结合表中的骨骼数目,也就是属性组的数目
		&pMeshContainer->pBoneCombinationBuf,//输出骨骼结合表
		&pMeshContainer->MeshData.pMesh);//指向最终生成的蒙皮网格


	return hr;

}


//-----------------------------------------------------------------------------
// Desc: 创建框架, 仅仅是分配内存和初始化,还没有对其成员赋予合适的值
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateFrame(LPCSTR Name,LPD3DXFRAME *ppNewFrame)
{
	HRESULT hr = S_OK;
	D3DXFRAME_DERIVED *pFrame;

	*ppNewFrame = NULL;

	pFrame = new D3DXFRAME_DERIVED;  //创建框架结构对象
	if (pFrame == NULL) 
	{
		return E_OUTOFMEMORY;
	}

	//为框架指定名称
	hr = AllocateName(Name, (LPSTR*)&pFrame->Name);
	if (FAILED(hr))
	{
		delete pFrame;
		return hr;
	}

	//初始化D3DXFRAME_DERIVED结构其它成员变量
	D3DXMatrixIdentity(&pFrame->TransformationMatrix);
	D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);

	pFrame->pMeshContainer = NULL;
	pFrame->pFrameSibling = NULL;
	pFrame->pFrameFirstChild = NULL;

	*ppNewFrame = pFrame;
	pFrame = NULL;

	return hr;
}


//-----------------------------------------------------------------------------
// Desc: 在这里是调用了成员函数 GenerateSkinnedMesh(pMeshContainer);
//       是在这里加载了蒙皮信息
//		 前7个参数为输入参数,最后一个是输出参数,存放指向最终生成的网格容器的指针的地址。
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateMeshContainer(THIS_ LPCSTR  Name,
												const D3DXMESHDATA* pMeshData,
												const D3DXMATERIAL* pMaterials,
												const D3DXEFFECTINSTANCE* pEffectInstances,
												DWORD  NumMaterials,
												const DWORD*   pAdjacency,
												LPD3DXSKININFO pSkinInfo,
												LPD3DXMESHCONTAINER* ppNewMeshContainer)//创建网格容器
{
	HRESULT hr;
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = NULL;
	UINT NumFaces;       //网格中的面数,在填充网格容器结构的邻接信息成员时使用
	UINT iMaterial;      //纹理操作时的循环变量
	UINT cBones;         //当前网格模型骨骼总数
	LPDIRECT3DDEVICE9 pd3dDevice = NULL;
	LPD3DXMESH pMesh    = NULL;
	*ppNewMeshContainer = NULL;

	if (pMeshData->Type != D3DXMESHTYPE_MESH)
	{
		return E_FAIL;
	}

	pMesh = pMeshData->pMesh;

	if (pMesh->GetFVF() == 0)
	{
		return E_FAIL;
	}

	//为网格容器分配内存
	pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	if (pMeshContainer == NULL)
	{
		return E_OUTOFMEMORY;
	}
	memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));

	//填充网格容器结构D3DXMESHCONTAINER_DERIVED的成员

	//为网格指定名称
	hr = AllocateName(Name, &pMeshContainer->Name);
	if (FAILED(hr))
	{
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}

	pMesh->GetDevice(&pd3dDevice);
	NumFaces = pMesh->GetNumFaces();

	//确保网格顶点包含法线-----主要是和GenerateSkinnedMesh()中的设备能力相呼应
	if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
	{
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
		hr = pMesh->CloneMeshFVF( pMesh->GetOptions(), 
			pMesh->GetFVF() | D3DFVF_NORMAL, 
			pd3dDevice, 
			&pMeshContainer->MeshData.pMesh );
		if (FAILED(hr))
		{
			SAFE_RELEASE(pd3dDevice);
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}

		pMesh = pMeshContainer->MeshData.pMesh;
		D3DXComputeNormals( pMesh, NULL );//计算网格顶点的单位法线
	}
	else 
	{
		pMeshContainer->MeshData.pMesh = pMesh;
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
		pMesh->AddRef();
	}

	//为网格模型准备材质和纹理
	pMeshContainer->NumMaterials = max(1, NumMaterials); 
	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
	pMeshContainer->pAdjacency = new DWORD[NumFaces*3];
	if ((pMeshContainer->pAdjacency == NULL) || (pMeshContainer->pMaterials == NULL)
		|| (pMeshContainer->ppTextures == NULL))
	{
		hr = E_OUTOFMEMORY;
		SAFE_RELEASE(pd3dDevice);
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}

	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces*3);  
	memset(pMeshContainer->ppTextures, 0, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);

	if (NumMaterials > 0)            
	{    
		//复制材质属性, 设置材质环境光属性
		memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials); 
		pMeshContainer->pMaterials->MatD3D.Ambient = pMeshContainer->pMaterials->MatD3D.Diffuse;

		//提取纹理文件, 创建纹理对象
		for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++) 
		{
			if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL)
			{
				//WCHAR strTexturePath[MAX_PATH];
				//WCHAR wszBuf[MAX_PATH];
				////从纹理文件路径提取纹理文件名
				//RemovePathFromFileName(pMeshContainer->pMaterials[iMaterial].pTextureFilename, wszBuf);
				//////根据纹理文件名从事先指定的路径查找纹理文件
				//DXUTFindDXSDKMediaFileCch( strTexturePath, MAX_PATH, wszBuf );

				TCHAR strTexturePath[MAX_PATH];
				_stprintf_s(strTexturePath,MAX_PATH,_T("Res\\Mesh\\%s"),pMeshContainer->pMaterials[iMaterial].pTextureFilename);
				if( FAILED( D3DXCreateTextureFromFile( pd3dDevice, strTexturePath,
					&pMeshContainer->ppTextures[iMaterial] ) ) )
				{
					pMeshContainer->ppTextures[iMaterial] = NULL;
				}

				pMeshContainer->pMaterials[iMaterial].pTextureFilename = NULL;
			}
		}
	}
	else
	{
		pMeshContainer->pMaterials[0].pTextureFilename = NULL;
		memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
		pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
	}

	SAFE_RELEASE(pd3dDevice);

	//如果当前网格包含蒙皮信息
	if (pSkinInfo != NULL)
	{
		//加载蒙皮网格信息
		pMeshContainer->pSkinInfo = pSkinInfo; 
		pSkinInfo->AddRef();

		//保留原网格信息
		pMeshContainer->pOrigMesh = pMesh;
		pMesh->AddRef();

		//获取骨骼数量
		cBones = pSkinInfo->GetNumBones();

		//为每块骨骼分配保存初始变换矩阵的内存空间
		pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
		if (pMeshContainer->pBoneOffsetMatrices == NULL) 
		{
			hr = E_OUTOFMEMORY;
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}

		//保存每块骨骼的初始变换矩阵
		for (UINT iBone = 0; iBone < cBones; iBone++)
		{
			pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
		}

		//生成蒙皮网格模型
		hr = GenerateSkinnedMesh(pMeshContainer); 
		if (FAILED(hr))
		{
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}
	}

	*ppNewMeshContainer = pMeshContainer;
	pMeshContainer = NULL;
	return hr;
}

HRESULT CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	if (pFrameToFree != NULL )
	{
		SAFE_DELETE_ARRAY(pFrameToFree->Name);
		SAFE_DELETE(pFrameToFree);
	}
	return S_OK;
}

HRESULT CAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	if (pMeshContainerBase == NULL)
		return S_OK;
	UINT iMaterial;
	//先转为扩展型以免内存泄露
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = 
		(D3DXMESHCONTAINER_DERIVED*) pMeshContainerBase;
	SAFE_DELETE_ARRAY(pMeshContainer->Name);
	SAFE_DELETE_ARRAY(pMeshContainer->pAdjacency);
	SAFE_DELETE_ARRAY(pMeshContainer->pMaterials);
	if (pMeshContainer->ppTextures != NULL)
	{
		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			SAFE_RELEASE(pMeshContainer->ppTextures[iMaterial]);
		}
	}
	SAFE_DELETE_ARRAY(pMeshContainer->ppTextures);
	SAFE_RELEASE(pMeshContainer->MeshData.pMesh);
	SAFE_RELEASE(pMeshContainer->pSkinInfo);
	SAFE_DELETE(pMeshContainer);
	return S_OK;
}