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
	ID3DXBuffer*    pAdjBuffer = NULL;//保存邻接信息缓存
	ID3DXBuffer*    pMtrlBuffer = NULL;//保存X文件中的材质纹理缓存
	DWORD           dwNumMtrl = 0;//保存X文件材质数量

	//加载X文件
	if (FAILED(D3DXLoadMeshFromX(pFileName,D3DXMESH_MANAGED,Device,&pAdjBuffer,
		&pMtrlBuffer,NULL,&dwNumMtrl,&m_pMesh)))
	{
		MessageBox(NULL,"静态模型导入失败","StaticObject error",MB_OK);
		return false;
	}
	
	//保存材质和纹理
	if (pMtrlBuffer != NULL && dwNumMtrl != 0)
	{
		D3DXMATERIAL* pMtrl = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
		for (int i = 0;i < dwNumMtrl; i++)
		{
			//保存材质信息到容器中
			m_vMtrls.push_back(pMtrl[i].MatD3D);
			if (pMtrl[i].pTextureFilename != NULL)
			{
				//导入纹理
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
	//优化网格
	HRESULT hr=m_pMesh->OptimizeInplace(
		D3DXMESHOPT_ATTRSORT|D3DXMESHOPT_COMPACT|D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)pAdjBuffer->GetBufferPointer(),
		0,0,0);
	if (FAILED(hr))
	{
		MessageBox(0,"网格优化失败",0,0);
		return false;
	}

	//创建包围盒
	m_BoundingVolume.Init(Device);
	m_BoundingVolume.Create(m_pMesh);

	//释放接口指针
	SAFE_RELEASE(pAdjBuffer);
	SAFE_RELEASE(pMtrlBuffer);

	return true;
}

void CStaticObject::Render()
{
	for (int i=0;i<m_vMtrls.size();i++)
	{
		m_pDevice->SetMaterial(&m_vMtrls[i]);//设置材质
		m_pDevice->SetTexture(0,m_vTextures[i]);//设置纹理
		m_pMesh->DrawSubset(i);//绘制子集
	}
}

void CStaticObject::UpdateBounding( D3DXMATRIX matWord )
{
	m_BoundingVolume.CalSphereByBox(matWord);
}

