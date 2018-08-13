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

// ����LoadFromXFile�е��á������Ӻ���SetupBoneMatrixPointersOnMesh()
//       ���úø������(ʵ�����Ǹ�������)����ϱ任����
//  ��������ʵ��û�м����������������ϱ任����ֻ��Ϊÿ�����󿪱�����Ӧ
//       �Ĵ洢�ռ䣬�����ļ������ں���CSkinMesh::UpdateFrameMatrices()����ɵġ�
HRESULT CAnimMesh::SetupBoneMatrixPointers( LPD3DXFRAME pFrame )
{
	HRESULT hr;

	if (pFrame->pMeshContainer != NULL)//�ÿ������������������
	{
		hr = SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer);  //�����Ӻ���
		if (FAILED(hr))
			return hr;
	}

	if (pFrame->pFrameSibling != NULL)//�ÿ�ܵ��ֵܿ��
	{
		hr = SetupBoneMatrixPointers(pFrame->pFrameSibling);   //�ݹ�
		if (FAILED(hr))
			return hr;
	}

	if (pFrame->pFrameFirstChild != NULL)//�ÿ�ܵ��ӿ��
	{
		hr = SetupBoneMatrixPointers(pFrame->pFrameFirstChild);  //�ݹ�
		if (FAILED(hr))
			return hr;
	}

	return S_OK;
}

// ����SetupBoneMatrixPointers()�б����ã�����ÿ����������ϱ任����
HRESULT CAnimMesh::SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase )
{
	UINT iBone, cBones;  // cBones��ʾ����������iBone��ʾѭ������
	D3DXFRAME_DERIVED *pFrame;

	//��ǿ��תΪ��չ��
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

	// ֻ����Ƥ����ģ�Ͳ��й�������----pSkinInfo��¼�˸�����ģ�͵���Ƥ��Ϣ
	if (pMeshContainer->pSkinInfo != NULL)
	{
		//�õ���������
		cBones = pMeshContainer->pSkinInfo->GetNumBones();

		//����洢��������Ŀռ�----ppBoneMatrixPtrs��Ź�������ϱ任����
		pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[cBones];     
		if (pMeshContainer->ppBoneMatrixPtrs == NULL)
			return E_OUTOFMEMORY;

		for (iBone = 0; iBone < cBones; iBone++)
		{
			//�ҵ����
			pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(iBone));
			if (pFrame == NULL)
				return E_FAIL;

			//����ܵ���ϱ任����ֵ����Ӧ�Ĺ����ĸ��ϱ任����
			//CombinedTransformationMatrix:��¼�����еĸ���ܵ��ÿ��������������������ϱ任����
			pMeshContainer->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformationMatrix;
		}
	}

	return S_OK;
}

// ���ļ�������Ƥ����ģ��
HRESULT CAnimMesh::LoadFromXFile( char* strFileName )
{	
	HRESULT hr;

	//��.X�ļ����ز�ο�ܺͶ�������
	//����:
	//1��X�ļ����ļ���;2��ָ��X�ļ����ط�ʽ�ı�ʶ��;3��Direct3D�豸����;
	//4�����ڷ���֡(���)��������������;5����������û����ݵĽӿ�(��ֵ������ΪNULL);
	//6��ҪΪģ�ʹ����ĸ�֡(���);7�������Ķ���������
	hr = D3DXLoadMeshHierarchyFromX(strFileName, D3DXMESH_MANAGED, m_pd3dDevice,
		m_pAlloc, NULL, &m_pFrameRoot, &m_pAnimControler);

	if (FAILED(hr))
		return hr;

	//����������ܵ���ϱ任����
	hr = SetupBoneMatrixPointers(m_pFrameRoot);

	if (FAILED(hr))
		return hr;

	//�����ܶ���ı߽���
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

// ������������Ƥ����ģ��
HRESULT CAnimMesh::OnCreate(LPDIRECT3DDEVICE9 pd3dDevice, char* strFileName)
{
	HRESULT hr;
	m_pd3dDevice = pd3dDevice;
	hr = LoadFromXFile(strFileName);
	if(FAILED(hr))
		return hr;

	return S_OK;
}

// ���������������ϱ任����
void CAnimMesh::UpdateFrameMatrices(LPD3DXFRAME pFrameBase,LPD3DXMATRIX pParentMatrix)
{
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;

	if (pParentMatrix != NULL)
		//���������(��ǰ��������ϱ任���� = �ù����ĳ�ʼλ�� * ����������ϱ任����)
		D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix, 
		&pFrame->TransformationMatrix, pParentMatrix);
	else
		pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;

	if (pFrame->pFrameSibling != NULL)
		UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);

	if (pFrame->pFrameFirstChild != NULL)
		UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);	
}

//��Ⱦһ����������!!!!!!
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
		//����Ƿ�ʹ�����������
		HRESULT hr;

		//�����ǰӲ����֧�֣���ʹ��������㴦��
		if (pMeshContainer->UseSoftwareVP)
			m_pd3dDevice->SetSoftwareVertexProcessing(true);
		//��������������
		if(pMeshContainer->NumInfl)
			m_pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE,true);
		//����Ӱ�쵱ǰ����ģ�Ͷ���Ĺ���������������Ҫʹ�õĻ�Ͼ�����������
		if(pMeshContainer->NumInfl == 1)
			m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_0WEIGHTS);
		else if(pMeshContainer->NumInfl == 2)
			m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_1WEIGHTS);
		else if(pMeshContainer->NumInfl == 3)
			m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_2WEIGHTS);
		else if(pMeshContainer->NumInfl == 4)
			m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_3WEIGHTS);
		//������������Ⱦ
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>
			(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0;iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
		{
			//���û�Ͼ���
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
			//���ò��ʺ�����
			m_pd3dDevice->SetMaterial(&pMeshContainer->
				pMaterials[pBoneComb[iAttrib].AttribId].MatD3D);
			m_pd3dDevice->SetTexture(0,pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);
			//��Ⱦ������ģ��
			pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
		}
		//�ָ�������״̬
		m_pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE,false);
		m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND,D3DVBF_DISABLE);
		//�ָ����㴦��ģʽ
		if(pMeshContainer->UseSoftwareVP)
			m_pd3dDevice->SetSoftwareVertexProcessing(false);
	}
	//���ֻ����ͨ��������Ӳ��ʺ�ͻ�����
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

//��DrawMeshContainer��������Ⱦ�������
void CAnimMesh::DrawFrame(LPD3DXFRAME pFrame)
{
	LPD3DXMESHCONTAINER pMeshContainer;

	pMeshContainer = pFrame->pMeshContainer;

	while(pMeshContainer != NULL)
	{
		DrawMeshContainer(pMeshContainer, pFrame);
		pMeshContainer = pMeshContainer->pNextMeshContainer;//��һ����������
	}

	if(pFrame->pFrameSibling != NULL)
		DrawFrame(pFrame->pFrameSibling);

	if (pFrame->pFrameFirstChild != NULL)
		DrawFrame(pFrame->pFrameFirstChild);
}

// Desc: ���¿�ܲ����ƿ��
//       (1)��m_pAnimController->AdvanceTime()����ʱ�䣬m_pAnimController��
//          ��LPD3DXANIMATIONCONTROLLER��һ������
//       (2)�ú���CSkinMesh::UpdateFrameMatrices()���¿��
//       (3)�ú���CSkinMesh::DrawFrame()���ƿ��
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