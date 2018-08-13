#include "StaticObject.h"

const DWORD MESHVERTEX::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

CStaticObject::CStaticObject(void)
{
	m_pMesh = NULL;
}

CStaticObject::~CStaticObject(void)
{
}

bool CStaticObject::Init( LPDIRECT3DDEVICE9 Device,char* pFileName )
{
	m_pDevice = Device;
	ID3DXBuffer*    pAdjBuffer = NULL;//�����ڽ���Ϣ����
	ID3DXBuffer*    pMtrlBuffer = NULL;//����X�ļ��еĲ���������
	DWORD           dwNumMtrl = 0;//����X�ļ���������

	//����X�ļ�
	if (FAILED(D3DXLoadMeshFromX(pFileName,D3DXMESH_MANAGED,Device,&pAdjBuffer,
		&pMtrlBuffer,NULL,&dwNumMtrl,&m_pMesh)))
	{
		MessageBox(NULL,"��̬ģ�͵���ʧ��","StaticObject error",MB_OK);
		return false;
	}
	
	//������ʺ�����
	if (pMtrlBuffer != NULL && dwNumMtrl != 0)
	{
		D3DXMATERIAL* pMtrl = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
		for (int i = 0;i < dwNumMtrl; i++)
		{
			//���������Ϣ��������
			m_vMtrls.push_back(pMtrl[i].MatD3D);
			if (pMtrl[i].pTextureFilename != NULL)
			{
				//��������
				char cTextureName[256];
				sprintf(cTextureName,"res//StaticMesh//%s",pMtrl[i].pTextureFilename);
				LPDIRECT3DTEXTURE9  m_pTexture = NULL;
				D3DXCreateTextureFromFile(Device,cTextureName,&m_pTexture);

				m_vTextures.push_back(m_pTexture);
			}
			else
				m_vTextures.push_back(NULL);
		}
	}
	//�Ż�����
	HRESULT hr=m_pMesh->OptimizeInplace(
		D3DXMESHOPT_ATTRSORT|D3DXMESHOPT_COMPACT|D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)pAdjBuffer->GetBufferPointer(),
		0,0,0);
	if (FAILED(hr))
	{
		MessageBox(0,"�����Ż�ʧ��",0,0);
		return false;
	}

	//������Χ��
	m_BoundingVolume.Init(Device);
	m_BoundingVolume.Create(m_pMesh);

	//�ͷŽӿ�ָ��
	SAFE_RELEASE(pAdjBuffer);
	SAFE_RELEASE(pMtrlBuffer);

	return true;
}

void CStaticObject::Render()
{
	for (int i=0;i<m_vMtrls.size();i++)
	{
		m_pDevice->SetMaterial(&m_vMtrls[i]);//���ò���
		m_pDevice->SetTexture(0,m_vTextures[i]);//��������
		m_pMesh->DrawSubset(i);//�����Ӽ�
	}
}

void CStaticObject::UpdateBounding( D3DXMATRIX matWord )
{
	m_BoundingVolume.CalSphereByBox(matWord);
}

