#pragma once
#include "Header.h"
#include "StaticObject.h"
#include "House.h"
#include "Terrain.h"
#include "Config.h"

class CSceneObject
{
private:
	LPDIRECT3DDEVICE9	m_pD3DDevice;
	CTerrain*			m_pTerrain;

	std::vector<CStaticObject*>	m_vStaticObject;		//静态网格容器---存储所有的静态模型
	std::vector<CHouse*>		m_vHouse;				//静态物体容器---存储所有的静态数据

private:
	bool	LoadStaticMesh();//载入所有静态网格模型
	bool	LoadStaticObject();//载入所有静态物体数据

public:
	CSceneObject(void);
	~CSceneObject(void);

	bool	InitScene(LPDIRECT3DDEVICE9 pD3DDevice,CTerrain* pTerrain);
	void	RenderScene();

	int		GetNumObject()	{return m_vHouse.size();}//获取静态物件的数量

	CHouse*	GetStaticObject(UINT Num)	//返回指定的静态物件
	{
		if (Num > m_vHouse.size())//超出范围
		{
			return NULL;
		}
		else 
			return m_vHouse[Num];
	}
};
