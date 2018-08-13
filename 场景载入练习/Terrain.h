#pragma once
#include "Header.h"
#include "SkyBox.h"
#include "Config.h"
#include "BoundingVolume.h"
#include "Frustum.h"
#include "Input.h"
typedef struct TerrainVertex 
{
	float	x,y,z;
	D3DCOLOR	color;
	float	u0,v0;
	float	u1,v1;
	enum
	{
		FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2,
	};

}TERRAINVERTEX,*LPTERRAINVERTEX;

typedef struct TerrainQuadNode
{
	DWORD	nIndexCorner[4];	// �ĸ��Ƕ���
	TerrainQuadNode*	pChild[4];	// �ĸ�����
	CBoundingVolume	BoundingVolume;	// ��Χ��
	DWORD	nIndexCenter;	// �ڵ���������
	DWORD	nNodeSize;	// �ڵ�ߴ�
	float	fDeltaHeight;	// �򻯸߶Ȳ�
}TERRAINQUADNODE,*LPTERRAINQUADNODE;

class CTerrain
{
private:
	LPDIRECT3DDEVICE9	m_pDevice;	// �豸ָ��
	LPTERRAINQUADNODE	m_pTerrainRoot;	// �����Ĳ������ڵ�
	LPDIRECT3DVERTEXBUFFER9	m_pVB;	// ���㻺��
	LPDIRECT3DINDEXBUFFER9	m_pIB;	// ��������
	TerrainVertex*	m_pVertices;	// ��������
	DWORD*	m_pIndicesNear;	// ÿ֡������Ҫ��Ⱦ�Ķ���������������
	int		m_nIndicesNearCurFrame;	// ��ǰ֡��Ⱦ����������������
	DWORD*	m_pIndicesFar;	// ÿ֡������Ҫ��Ⱦ�Ķ���������Զ����
	int		m_nIndicesFarCurFrame;	// ��ǰ֡��Ⱦ����������Զ����

	float*	m_pfHeightValue;	// ���θ߶�ֵ����
	int		m_nSize;	// ���γߴ�
	float	m_fCellScale;	// ��Ԫ�����ű���
	float	m_fHeightScale;	// �߶����ű���

	float	m_fLODC1,m_fLODC2;	// LOD����
	bool*	m_pbMarkExist;		// ��������ʾ�ڵ��Ƿ����

	LPTERRAINQUADNODE*	m_ppListBrowseCurr;	// ������ǰ��ζ���
	LPTERRAINQUADNODE*	m_ppListBrowseNext;	// ��һ�����

	LPDIRECT3DTEXTURE9	m_pTexSkeleton;	// ������������
	LPDIRECT3DTEXTURE9	m_pTexArrayDetail[3];	// ϸ������
	LPDIRECT3DTEXTURE9	m_pTexAlpha;	// ALPHAͼ

	int		m_nDetailTexSize;	// ϸ����ͼ��С
	float	m_fAmbient;	// ������
	float	m_fDetailRange;	// ϸ����ͼ��Χ

	D3DXVECTOR3* m_pFaceNormal;	// �淨��
	CFrustum     m_Frustum;
public:
	bool         m_bWire;     //�Ƿ�Ϊ�߿�ģʽ

	CTerrain(void);
	~CTerrain(void);
	void BuildQuadTree(LPTERRAINQUADNODE pNode);	// �����Ĳ���
	void DestroyQuadTree(LPTERRAINQUADNODE pNode);	// �����Ĳ���
	bool LoadHeightMap(LPCTSTR lpFileName);	// ���ظ߶�ͼ
	bool CreateSkeletonAndAlphaTex(DWORD* pdwPixel);	// ��������ͼ��ALPHAͼ
	bool InitVertexBuffer(DWORD* pdwPixel);	// ��ʼ�����㻺��
	bool ComputeFaceNormals();	// �����淨��
	void GenerateLightingMap(DWORD* pdwPixel,D3DXVECTOR3& vLighting);// ���ɶ������
	bool Initialize(LPDIRECT3DDEVICE9 pdevice);// ��ʼ��
	void CleanUp();	// �������


	void Update(D3DXVECTOR3& vCamera,D3DXVECTOR3& vPlayerPos);	// ����
	void BrowseQuadTree(LPTERRAINQUADNODE pRoot,CFrustum* pFrustum,D3DXVECTOR3& vCamera,D3DXVECTOR3& vPlayerPos);	// �����Ĳ���
	bool NodeCanDivided(LPTERRAINQUADNODE pNode);	// �ڵ��ܷ�����ָ�
	void AddNodeToDraw(LPTERRAINQUADNODE pNode,D3DXVECTOR3& vPlayerPos);	// �����Ⱦ�ڵ�


	void Render();	// ��Ⱦ
	void PreRender(bool bNear);	// ��Ⱦǰ״̬����
	void AfterRender();	// ��Ⱦ��״̬��ԭ


	//�ж����ߺ��������Ƿ��ཻ
	bool FixForCamera(OUT D3DXVECTOR3& vDestPos,IN D3DXVECTOR3& vRole,IN D3DXVECTOR3& vCamera,IN FLOAT MinDist);
	bool CollideWithCamera(D3DXVECTOR3 vRayOrigin,D3DXVECTOR3 vRayDir,D3DXVECTOR3& vHitPoint,float& fMinDist); 
	//ʰȡ
	bool FindDestForPlayMove( OUT D3DXVECTOR3& vDestPos,OUT D3DXMATRIX& matRotation,IN D3DXVECTOR3& vCamera );


	int GetTriangleNumToDraw()// ��ȡ��ǰ֡��Ⱦ����������
	{return (m_nIndicesNearCurFrame + m_nIndicesFarCurFrame) / 3;}
	bool IsOutOfRange(D3DXVECTOR3& vPosition)// �ж��Ƿ񳬳����α߽�
	{return vPosition.x<-m_nSize/2*m_fCellScale||vPosition.x>m_nSize/2*m_fCellScale
	||vPosition.z<-m_nSize/2*m_fCellScale||vPosition.z>m_nSize/2*m_fCellScale;}
	float GetHeightMap(int nRow, int nColumn)// ��ȡָ�����и߶�ֵ
	{return m_pfHeightValue[nRow * m_nSize + nColumn];}
	float GetHeightValue(float x,float z);// ��ȡָ������߶�

	float GetTerrainWidth()	//��ȡ���εĿ��
	{
		return (m_nSize - 1) * m_fCellScale;
	}
};
