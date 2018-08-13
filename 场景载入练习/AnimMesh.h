#pragma once
#include "Header.h"
#include "AllocateHierarchy.h"
#include "BoundingVolume.h"

class CAnimMesh
{
private:
	CAllocateHierarchy*    m_pAlloc;
	CBoundingVolume*       m_pBoundingVolume;
	LPDIRECT3DDEVICE9      m_pd3dDevice;
	LPD3DXFRAME            m_pFrameRoot;



public:
	D3DXVECTOR3 m_vObjectCenter;//����ģ�ͱ߽��������
	float m_fObjectRadius;      //����ģ�ͱ߽���İ뾶
	bool m_bPlayAnim;           //�Ƿ񲥷�ģ���еĶ���
	LPD3DXANIMATIONCONTROLLER m_pAnimControler;//ָ�򶯻��������ӿ�ID3DXAnimationController��ָ��
private:
	HRESULT SetupBoneMatrixPointers( LPD3DXFRAME pFrame );//����������ܵ���ϱ任����
	HRESULT SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase );//����ÿ����������ϱ任����

	HRESULT LoadFromXFile(char* strFileName);//��.x�м�������ģ�ͣ������ض���������
	void UpdateFrameMatrices(LPD3DXFRAME pFrameBase,LPD3DXMATRIX pParentMatrix);//����õ�������ܵ���ϱ任����
	void DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase,LPD3DXFRAME pFrameBase);//��Ⱦһ����������
	void DrawFrame(LPD3DXFRAME pFrame);//��DrawMeshContainer��������Ⱦ�������
public:
	HRESULT OnCreate(LPDIRECT3DDEVICE9 pd3dDevice, char* strFileName);//����
	HRESULT Render(D3DXMATRIXA16* matWorld, double fElapsedAppTime);//��Ⱦ
	HRESULT OnDestroy();//����
	void SetAnimationSetByName( LPCSTR szAnimName );

	CBoundingVolume* GetBoundingVolume(){return m_pBoundingVolume;}
	CAnimMesh(void);
	~CAnimMesh(void);
};
