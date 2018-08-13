#include "AnimMesh.h"

CAnimMesh::CAnimMesh(void)
{
	m_bPlayAnim          = true;
	m_pd3dDevice         = NULL;
	m_pAnimControler     = NULL;
	m_pFrameRoot         = NULL;

	m_pAlloc = new CAllocateHierarchy();
}

CAnimMesh::~CAnimMesh(void)
{
	D3DXFrameDestroy(m_pFrameRoot, m_pAlloc);
	delete m_pAlloc;
}

// 仅在LoadFromXFile中调用。调用子函数SetupBoneMatrixPointersOnMesh()
//       安置好各级框架(实际上是各个骨骼)的组合变换矩阵。
//  在这里其实并没有计算出各个骨骼的组合变换矩阵，只是为每个矩阵开辟了相应
//       的存储空间，真正的计算是在函数CSkinMesh::UpdateFrameMatrices()中完成的。
HRESULT CAnimMesh::SetupBoneMatrixPointers( LPD3DXFRAME pFrame )
{
	HRESULT hr;

	if (pFrame->pMeshContainer != NULL)//该框架所包含的网格容器
	{
		hr = SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer);  //调用子函数
		if (FAILED(hr))
			return hr;
	}

	if (pFrame->pFrameSibling != NULL)//该框架的兄弟框架
	{
		hr = SetupBoneMatrixPointers(pFrame->pFrameSibling);   //递归
		if (FAILED(hr))
			return hr;
	}

	if (pFrame->pFrameFirstChild != NULL)//该框架的子框架
	{
		hr = SetupBoneMatrixPointers(pFrame->pFrameFirstChild);  //递归
		if (FAILED(hr))
			return hr;
	}

	return S_OK;
}

// 仅在SetupBoneMatrixPointers()中被调用，设置每个骨骼的组合变换矩阵
HRESULT CAnimMesh::SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase )
{
	UINT iBone, cBones;  // cBones表示骨骼数量，iBone表示循环变量
	D3DXFRAME_DERIVED *pFrame;

	//先强制转为扩展型
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

	// 只有蒙皮网格模型才有骨骼矩阵----pSkinInfo记录了该网格模型的蒙皮信息
	if (pMeshContainer->pSkinInfo != NULL)
	{
		//得到骨骼数量
		cBones = pMeshContainer->pSkinInfo->GetNumBones();

		//申请存储骨骼矩阵的空间----ppBoneMatrixPtrs存放骨骼的组合变换矩阵
		pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[cBones];     
		if (pMeshContainer->ppBoneMatrixPtrs == NULL)
			return E_OUTOFMEMORY;

		for (iBone = 0; iBone < cBones; iBone++)
		{
			//找到框架
			pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(iBone));
			if (pFrame == NULL)
				return E_FAIL;

			//将框架的组合变换矩阵赋值给对应的骨骼的复合变换矩阵
			//CombinedTransformationMatrix:记录从所有的父框架到该框架自身所积累起来的组合变换矩阵
			pMeshContainer->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformationMatrix;
		}
	}

	return S_OK;
}

// 从文件加载蒙皮网格模型
HRESULT CAnimMesh::LoadFromXFile( char* strFileName )
{	
	HRESULT hr;

	//从.X文件加载层次框架和动画数据
	//参数:
	//1、X文件的文件名;2、指定X文件加载方式的标识符;3、Direct3D设备对象;
	//4、用于分配帧(框架)和网格容器的类;5、允许加载用户数据的接口(该值可以设为NULL);
	//6、要为模型创建的根帧(框架);7、创建的动画控制器
	hr = D3DXLoadMeshHierarchyFromX(strFileName, D3DXMESH_MANAGED, m_pd3dDevice,
		m_pAlloc, NULL, &m_pFrameRoot, &m_pAnimControler);

	if (FAILED(hr))
		return hr;

	//建立各级框架的组合变换矩阵
	hr = SetupBoneMatrixPointers(m_pFrameRoot);

	if (FAILED(hr))
		return hr;

	//计算框架对象的边界球
//	hr = D3DXFrameCalculateBoundingSphere(m_pFrameRoot, 
//	&m_vObjectCenter, &m_fObjectRadius);
	m_pBoundingVolume=new CBoundingVolume();
	m_pBoundingVolume->Init(m_pd3dDevice);

	hr = D3DXFrameCalculateBoundingSphere(m_pFrameRoot, 
		&m_pBoundingVolume->GetBoundingSphere()->vCenter, 
		&m_pBoundingVolume->GetBoundingSphere()->fRadius);
	
	if (FAILED(hr))
		return hr;

	return S_OK;
}

// 创建并加载蒙皮网格模型
HRESULT CAnimMesh::OnCreate(LPDIRECT3DDEVICE9 pd3dDevice, char* strFileName)
{
	HRESULT hr;
	m_pd3dDevice = pd3dDevice;
	hr = LoadFromXFile(strFileName);
	if(FAILED(hr))
		return hr;

	return S_OK;
}

// 计算各个骨骼的组合变换矩阵
void CAnimMesh::UpdateFrameMatrices(LPD3DXFRAME pFrameBase,LPD3DXMATRIX pParentMatrix)
{
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;

	if (pParentMatrix != NULL)
		//两矩阵相乘(当前骨骼的组合变换矩阵 = 该骨骼的初始位置 * 父骨骼的组合变换矩阵)
		D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix, 
		&pFrame->TransformationMatrix, pParentMatrix);
	else
		pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;

	if (pFrame->pFrameSibling != NULL)
		UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);

	if (pFrame->pFrameFirstChild != NULL)
		UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);	
}

//渲染一个具体网格!!!!!!
void CAnimMesh::DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase,LPD3DXFRAME pFrameBase)
{
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*) pMeshContainerBase;

	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*) pFrameBase;
	UINT iMaterial;
	UINT iAttrib;
	LPD3DXBONECOMBINATION pBoneComb;
	UINT iMatrixIndex;
	UINT iPaletteEntry;
	D3DXMATRIXA16 matTemp;
	if (pMeshContainer->pSkinInfo != NULL)
	{
		//检查是否使用软件顶点混合
		HRESULT hr;

		//如果当前硬件不支持，则使用软件顶点处理
		if (pMeshContainer->UseSoftwareVP)
			m_pd3dDevice->SetSoftwareVertexProcessing(true);
		//启用索引顶点混合
		if(pMeshContainer->NumInfl)
			m_pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE,true);
		//根据影响当前网格模型顶点的骨骼数量，设置需要使用的混合矩阵索引数量
		if(pMeshContainer->NumInfl == 1)
			m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_0WEIGHTS);
		else if(pMeshContainer->NumInfl == 2)
			m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_1WEIGHTS);
		else if(pMeshContainer->NumInfl == 3)
			m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_2WEIGHTS);
		else if(pMeshContainer->NumInfl == 4)
			m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_3WEIGHTS);
		//逐个网格进行渲染
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>
			(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0;iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
		{
			//设置混合矩阵
			for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries;iPaletteEntry++)
			{
				iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
				if (iMatrixIndex != UINT_MAX)
				{
					D3DXMatrixMultiply(&matTemp,
						&pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
						pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
					m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(iPaletteEntry),
												&matTemp);
				}
			}
			//设置材质和纹理
			m_pd3dDevice->SetMaterial(&pMeshContainer->
				pMaterials[pBoneComb[iAttrib].AttribId].MatD3D);
			m_pd3dDevice->SetTexture(0,pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);
			//渲染子网格模型
			pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
		}
		//恢复顶点混合状态
		m_pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE,false);
		m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_DISABLE);
		//恢复顶点处理模式
		if(pMeshContainer->UseSoftwareVP)
			m_pd3dDevice->SetSoftwareVertexProcessing(false);
	}
	//如果只是普通网格，在添加材质后就绘制他
	else
	{
		m_pd3dDevice->SetTransform(D3DTS_WORLD, &pFrame->CombinedTransformationMatrix);
		for(UINT iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			m_pd3dDevice->SetMaterial(&pMeshContainer->pMaterials[iMaterial].MatD3D);
			m_pd3dDevice->SetTexture(0, pMeshContainer->ppTextures[iMaterial]);
			pMeshContainer->MeshData.pMesh->DrawSubset(iMaterial);
		}
	}
}

//在DrawMeshContainer基础上渲染各级框架
void CAnimMesh::DrawFrame(LPD3DXFRAME pFrame)
{
	LPD3DXMESHCONTAINER pMeshContainer;

	pMeshContainer = pFrame->pMeshContainer;

	while(pMeshContainer != NULL)
	{
		DrawMeshContainer(pMeshContainer, pFrame);
		pMeshContainer = pMeshContainer->pNextMeshContainer;//下一个网格容器
	}

	if(pFrame->pFrameSibling != NULL)
		DrawFrame(pFrame->pFrameSibling);

	if (pFrame->pFrameFirstChild != NULL)
		DrawFrame(pFrame->pFrameFirstChild);
}

// Desc: 更新框架并绘制框架
//       (1)用m_pAnimController->AdvanceTime()设置时间，m_pAnimController是
//          类LPD3DXANIMATIONCONTROLLER的一个对象
//       (2)用函数CSkinMesh::UpdateFrameMatrices()更新框架
//       (3)用函数CSkinMesh::DrawFrame()绘制框架
HRESULT CAnimMesh::Render(D3DXMATRIXA16* matWorld, double fRlapsedAppTime)
{
	if(fRlapsedAppTime == 0.0f)
		return S_OK;

	if (m_pAnimControler != NULL)
		m_pAnimControler->AdvanceTime( fRlapsedAppTime, NULL );

	UpdateFrameMatrices(m_pFrameRoot, matWorld);

	DrawFrame(m_pFrameRoot);

	return S_OK;
}

HRESULT CAnimMesh::OnDestroy()
{
	delete this;
	return S_OK;
}

void CAnimMesh::SetAnimationSetByName( LPCSTR szAnimName )
{
	assert(szAnimName != NULL);

	LPD3DXANIMATIONSET	pAnimSet;
	m_pAnimControler->GetAnimationSetByName(szAnimName,&pAnimSet);
	m_pAnimControler->SetTrackAnimationSet(0,pAnimSet);
}