#include "SceneObject.h"

CSceneObject::CSceneObject(void)
{
}

CSceneObject::~CSceneObject(void)
{
	for (int i=0; i != m_vStaticObject.size(); i++)
	{
		SAFE_DELETE(m_vStaticObject[i]);
	}
	for (int i=0; i != m_vHouse.size(); i++)
	{
		SAFE_DELETE(m_vHouse[i]);
	}
}

//载入所有静态网格模型
bool CSceneObject::LoadStaticMesh()
{
	CConfig::GetInstance().SetConfigFileName("res//ini//SceneObject.ini");
	int nTotal = CConfig::GetInstance().ReadINI("StaticMesh","Total");
	if (nTotal == 0)
	{
		return false;
	}
	TCHAR buff[MAX_PATH],szFileName[MAX_PATH];
	CStaticObject* pMesh;
	pMesh = NULL;

	//载入所有的模型
	for (int i=0; i < nTotal; i++)
	{
		pMesh = new CStaticObject();
		if (pMesh == NULL)
		{
			return false;
		}
		_stprintf_s(buff,MAX_PATH,"Mesh%d",i);
		CConfig::GetInstance().ReadINI("StaticMesh",buff,szFileName,MAX_PATH);
		if (!pMesh->Init(m_pD3DDevice,szFileName))
		{
			return false;
		}
		m_vStaticObject.push_back(pMesh);
	}
	pMesh = NULL;
	return true;
}

//载入所有静态物体数据
bool CSceneObject::LoadStaticObject()
{
	CConfig::GetInstance().SetConfigFileName("res//ini//SceneObject.ini");
	int nTotal = CConfig::GetInstance().ReadINI("StaticObject","Total");
	if (nTotal == 0)
	{
		return false;
	}

	int nIndex;
	D3DXVECTOR3 vPos;
	D3DXVECTOR3	vRot;
	D3DXVECTOR3	vZoom;

	TCHAR filename[MAX_PATH];
	CConfig::GetInstance().ReadINI("StaticObject","StaticObjectData",filename,MAX_PATH);

	//从文件中载入静态物体的数据
	std::ifstream infile(filename);
	if (!infile)
	{
		return false;
	}

	char buff[256];
	infile.getline(buff,256);
	CHouse* pStaticObject;
	for (int i=0; i < nTotal; i++)
	{
		pStaticObject = new CHouse();
		infile>>nIndex>>vPos.x>>vPos.y>>vPos.z>>vRot.x>>vRot.y>>vRot.z>>vZoom.x>>vZoom.y>>vZoom.z;
//		vPos.y = m_pTerrain->GetHeightValue(vPos.x,vPos.z);	//通过地形获取静态物体的Y值
		vRot.x = vRot.x * D3DX_PI/180.0f;
		vRot.y = vRot.y * D3DX_PI/180.0f;
		vRot.z = vRot.z * D3DX_PI/180.0f;
		pStaticObject->InitStaticObject(m_pD3DDevice,m_vStaticObject[nIndex],vPos,vRot,vZoom);

		m_vHouse.push_back(pStaticObject);
	}

	infile.clear();
	infile.close();
	pStaticObject = NULL;

	return true;
}

bool CSceneObject::InitScene( LPDIRECT3DDEVICE9 pD3DDevice,CTerrain* pTerrain )
{
	m_pD3DDevice	= pD3DDevice;
	m_pTerrain		= pTerrain;

	if (!LoadStaticMesh())
	{
		MessageBox(NULL,"载入静态网格模型失败!","error",NULL);
		return false;
	}

	if (!LoadStaticObject())
	{
		MessageBox(NULL,"载入静态物体数据失败!","error",NULL);
		return false;
	}

	return true;
}

void CSceneObject::RenderScene()
{
	//渲染所有的静态物体
	for (int i=0; i != m_vHouse.size(); i++)
	{
		m_vHouse[i]->RenderStaticObject();
	}
}	

