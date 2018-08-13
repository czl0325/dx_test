#pragma once
#include "Header.h"
#include "BoundingVolume.h"

// �̳���DXDXFRAME�ṹ�Ľṹ
struct D3DXFRAME_DERIVED: public D3DXFRAME
{
	//������¼�����еĸ���ܵ��ÿ�ܵĳ�ʼ�任
	D3DXMATRIXA16 CombinedTransformationMatrix;
};

// �̳���D3DXMESHCONTAINER�ṹ�Ľṹ

struct D3DXMESHCONTAINER_DERIVED: public D3DXMESHCONTAINER
{
	LPDIRECT3DTEXTURE9*  ppTextures;            //��������
	LPD3DXMESH           pOrigMesh;             //ԭʼ����
	DWORD                NumInfl;               //ÿ����������ܶ��ٹ�����Ӱ��
	DWORD                NumAttributeGroups;    //����������,������������
	LPD3DXBUFFER         pBoneCombinationBuf;   //������ϱ�
	D3DXMATRIX**         ppBoneMatrixPtrs;      //��Ź�������ϱ任����
	D3DXMATRIX*          pBoneOffsetMatrices;   //��Ź����ĳ�ʼ�任����
	DWORD                NumPaletteEntries;     //������������
	bool                 UseSoftwareVP;         //��ʶ�Ƿ�ʹ��������㴦��
};

// ������.X�ļ����ؿ�ܲ�κ�����ģ������
class CAllocateHierarchy : public ID3DXAllocateHierarchy
{
public:
	HRESULT GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED *pMeshContainer);
	HRESULT AllocateName(LPCSTR Name,LPSTR* pNewName);    


	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name,LPD3DXFRAME *ppNewFrame); //�������
	STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR  Name,
									const D3DXMESHDATA* pMeshData,
									const D3DXMATERIAL* pMaterials,
									const D3DXEFFECTINSTANCE* pEffectInstances,
									DWORD  NumMaterials,
									const DWORD*   pAdjacency,
									LPD3DXSKININFO pSkinInfo,
									LPD3DXMESHCONTAINER* ppNewMeshContainer);//������������
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);//���ٿ��
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);//������������
	CAllocateHierarchy(void);
	~CAllocateHierarchy(void);
};
