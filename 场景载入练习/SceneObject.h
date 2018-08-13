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

	std::vector<CStaticObject*>	m_vStaticObject;		//��̬��������---�洢���еľ�̬ģ��
	std::vector<CHouse*>		m_vHouse;				//��̬��������---�洢���еľ�̬����

private:
	bool	LoadStaticMesh();//�������о�̬����ģ��
	bool	LoadStaticObject();//�������о�̬��������

public:
	CSceneObject(void);
	~CSceneObject(void);

	bool	InitScene(LPDIRECT3DDEVICE9 pD3DDevice,CTerrain* pTerrain);
	void	RenderScene();

	int		GetNumObject()	{return m_vHouse.size();}//��ȡ��̬���������

	CHouse*	GetStaticObject(UINT Num)	//����ָ���ľ�̬���
	{
		if (Num > m_vHouse.size())//������Χ
		{
			return NULL;
		}
		else 
			return m_vHouse[Num];
	}
};
