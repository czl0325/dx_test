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


//������Ƥ����ģ��(����ÿ������Ļ��Ȩ�ء�������һ��������ϱ�)
HRESULT CAllocateHierarchy::GenerateSkinnedMesh( D3DXMESHCONTAINER_DERIVED *pMeshContainer )
{
	HRESULT hr = S_OK;

	if (pMeshContainer->pSkinInfo == NULL)//�жϵ�ǰ���������Ƿ��������Ƥ��Ϣ
		return hr;

	SAFE_RELEASE(pMeshContainer->MeshData.pMesh);
	SAFE_RELEASE(pMeshContainer->pBoneCombinationBuf);

	//��ȡԭʼ����ģ������������
	LPDIRECT3DINDEXBUFFER9 pIB;
	if (FAILED(hr = pMeshContainer->pOrigMesh->GetIndexBuffer(&pIB)))//pOrigMeshΪԭʼ����ģ��
		return E_FAIL;

	//��ȡӰ��һ����(������)�ľ�������
	DWORD NumMaxFaceInfl;
	hr = pMeshContainer->pSkinInfo->GetMaxFaceInfluences(pIB, 
		pMeshContainer->pOrigMesh->GetNumFaces(),//ԭʼ������Ŀ
		&NumMaxFaceInfl);
	pIB->Release();
	if (FAILED(hr))
		return E_FAIL;

	//����Ӱ��һ����ľ����������ܳ���12
	NumMaxFaceInfl = min(NumMaxFaceInfl, 12);

	//��ȡ��ǰ�豸������
	IDirect3DDevice9* pd3dDevice;
	pMeshContainer->pOrigMesh->GetDevice(&pd3dDevice);//�õ�һ��ָ���豸�Ľӿ�
	D3DCAPS9 d3dCaps;//�����豸Ӳ��������
	pd3dDevice->GetDeviceCaps( &d3dCaps );//��ȡӲ����Ⱦ����

	//���ݵ�ǰ�豸������NumMaxFaceInfl, �����ڽ��й�����Ƥ��ɫʱʹ�ù�������������
	//d3dCaps.MaxVertexBlendMatrixIndex��ʾ��ǰ�豸�����������ϵ�֧��,Ϊ0��ʾ��ǰ�豸��֧������������
	//d3dCaps.MaxVertexBlendMatrixIndex+1)/2��ʾ�����ɫ���о�����������(��Ϊ����һ��Ҫ����������Ϣ)
	if( (d3dCaps.MaxVertexBlendMatrixIndex+1)/2 < NumMaxFaceInfl )
	{
		//���ʹ��������㴦��ģʽ,������ɫ����������Ϊ256,�����Ƿ���Ҫ�Է����������л��
		pMeshContainer->NumPaletteEntries = min(256, pMeshContainer->pSkinInfo->GetNumBones());
		pMeshContainer->UseSoftwareVP = true;
	}
	else
	{
		pMeshContainer->NumPaletteEntries = min( (d3dCaps.MaxVertexBlendMatrixIndex+1) / 2, 
			pMeshContainer->pSkinInfo->GetNumBones() );
		pMeshContainer->UseSoftwareVP = false;
	}

	//����ԭ��ʼ����ģ���ṩ����Ӧ��������������Ƥ����ģ��(ʹ������������)
	hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
		( pMeshContainer->pOrigMesh,//��ʾ�����ԭ����
		0,//��ǰδʹ��
		pMeshContainer->NumPaletteEntries,//��ʾ����ʹ�õľ����ɫ������
		pMeshContainer->pAdjacency, NULL,//�ֱ��ʾ���������������ڽ���Ϣ
		NULL, NULL,//��ʾ�����ɵ���Ƥ��������Ͷ��㽫����䵽ԭ����ģ���е���Ͷ���
		&pMeshContainer->NumInfl,//���һ�������������ܶ��ٿ������Ӱ��
		&pMeshContainer->NumAttributeGroups,//���������ϱ��еĹ�����Ŀ,Ҳ�������������Ŀ
		&pMeshContainer->pBoneCombinationBuf,//���������ϱ�
		&pMeshContainer->MeshData.pMesh);//ָ���������ɵ���Ƥ����


	return hr;

}


//-----------------------------------------------------------------------------
// Desc: �������, �����Ƿ����ڴ�ͳ�ʼ��,��û�ж����Ա������ʵ�ֵ
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateFrame(LPCSTR Name,LPD3DXFRAME *ppNewFrame)
{
	HRESULT hr = S_OK;
	D3DXFRAME_DERIVED *pFrame;

	*ppNewFrame = NULL;

	pFrame = new D3DXFRAME_DERIVED;  //������ܽṹ����
	if (pFrame == NULL) 
	{
		return E_OUTOFMEMORY;
	}

	//Ϊ���ָ������
	hr = AllocateName(Name, (LPSTR*)&pFrame->Name);
	if (FAILED(hr))
	{
		delete pFrame;
		return hr;
	}

	//��ʼ��D3DXFRAME_DERIVED�ṹ������Ա����
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
// Desc: �������ǵ����˳�Ա���� GenerateSkinnedMesh(pMeshContainer);
//       ���������������Ƥ��Ϣ
//		 ǰ7������Ϊ�������,���һ�����������,���ָ���������ɵ�����������ָ��ĵ�ַ��
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateMeshContainer(THIS_ LPCSTR  Name,
												const D3DXMESHDATA* pMeshData,
												const D3DXMATERIAL* pMaterials,
												const D3DXEFFECTINSTANCE* pEffectInstances,
												DWORD  NumMaterials,
												const DWORD*   pAdjacency,
												LPD3DXSKININFO pSkinInfo,
												LPD3DXMESHCONTAINER* ppNewMeshContainer)//������������
{
	HRESULT hr;
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = NULL;
	UINT NumFaces;       //�����е�����,��������������ṹ���ڽ���Ϣ��Աʱʹ��
	UINT iMaterial;      //�������ʱ��ѭ������
	UINT cBones;         //��ǰ����ģ�͹�������
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

	//Ϊ�������������ڴ�
	pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	if (pMeshContainer == NULL)
	{
		return E_OUTOFMEMORY;
	}
	memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));

	//������������ṹD3DXMESHCONTAINER_DERIVED�ĳ�Ա

	//Ϊ����ָ������
	hr = AllocateName(Name, &pMeshContainer->Name);
	if (FAILED(hr))
	{
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}

	pMesh->GetDevice(&pd3dDevice);
	NumFaces = pMesh->GetNumFaces();

	//ȷ�����񶥵��������-----��Ҫ�Ǻ�GenerateSkinnedMesh()�е��豸�������Ӧ
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
		D3DXComputeNormals( pMesh, NULL );//�������񶥵�ĵ�λ����
	}
	else 
	{
		pMeshContainer->MeshData.pMesh = pMesh;
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
		pMesh->AddRef();
	}

	//Ϊ����ģ��׼�����ʺ�����
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
		//���Ʋ�������, ���ò��ʻ���������
		memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials); 
		pMeshContainer->pMaterials->MatD3D.Ambient = pMeshContainer->pMaterials->MatD3D.Diffuse;

		//��ȡ�����ļ�, �����������
		for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++) 
		{
			if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL)
			{
				//WCHAR strTexturePath[MAX_PATH];
				//WCHAR wszBuf[MAX_PATH];
				////�������ļ�·����ȡ�����ļ���
				//RemovePathFromFileName(pMeshContainer->pMaterials[iMaterial].pTextureFilename, wszBuf);
				//////���������ļ���������ָ����·�����������ļ�
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

	//�����ǰ���������Ƥ��Ϣ
	if (pSkinInfo != NULL)
	{
		//������Ƥ������Ϣ
		pMeshContainer->pSkinInfo = pSkinInfo; 
		pSkinInfo->AddRef();

		//����ԭ������Ϣ
		pMeshContainer->pOrigMesh = pMesh;
		pMesh->AddRef();

		//��ȡ��������
		cBones = pSkinInfo->GetNumBones();

		//Ϊÿ��������䱣���ʼ�任������ڴ�ռ�
		pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
		if (pMeshContainer->pBoneOffsetMatrices == NULL) 
		{
			hr = E_OUTOFMEMORY;
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}

		//����ÿ������ĳ�ʼ�任����
		for (UINT iBone = 0; iBone < cBones; iBone++)
		{
			pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
		}

		//������Ƥ����ģ��
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
	//��תΪ��չ�������ڴ�й¶
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