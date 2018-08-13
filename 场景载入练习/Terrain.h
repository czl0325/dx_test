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
	DWORD	nIndexCorner[4];	// 四个角顶点
	TerrainQuadNode*	pChild[4];	// 四个孩子
	CBoundingVolume	BoundingVolume;	// 包围体
	DWORD	nIndexCenter;	// 节点中心索引
	DWORD	nNodeSize;	// 节点尺寸
	float	fDeltaHeight;	// 简化高度差
}TERRAINQUADNODE,*LPTERRAINQUADNODE;

class CTerrain
{
private:
	LPDIRECT3DDEVICE9	m_pDevice;	// 设备指针
	LPTERRAINQUADNODE	m_pTerrainRoot;	// 地形四叉树根节点
	LPDIRECT3DVERTEXBUFFER9	m_pVB;	// 顶点缓存
	LPDIRECT3DINDEXBUFFER9	m_pIB;	// 索引缓存
	TerrainVertex*	m_pVertices;	// 顶点数据
	DWORD*	m_pIndicesNear;	// 每帧更新需要渲染的顶点索引（近处）
	int		m_nIndicesNearCurFrame;	// 当前帧渲染顶点总数（近处）
	DWORD*	m_pIndicesFar;	// 每帧更新需要渲染的顶点索引（远处）
	int		m_nIndicesFarCurFrame;	// 当前帧渲染顶点总数（远处）

	float*	m_pfHeightValue;	// 地形高度值缓存
	int		m_nSize;	// 地形尺寸
	float	m_fCellScale;	// 单元格缩放比例
	float	m_fHeightScale;	// 高度缩放比例

	float	m_fLODC1,m_fLODC2;	// LOD常量
	bool*	m_pbMarkExist;		// 标记数组表示节点是否存在

	LPTERRAINQUADNODE*	m_ppListBrowseCurr;	// 遍历当前层次队列
	LPTERRAINQUADNODE*	m_ppListBrowseNext;	// 下一层队列

	LPDIRECT3DTEXTURE9	m_pTexSkeleton;	// 地形轮廓纹理
	LPDIRECT3DTEXTURE9	m_pTexArrayDetail[3];	// 细节纹理
	LPDIRECT3DTEXTURE9	m_pTexAlpha;	// ALPHA图

	int		m_nDetailTexSize;	// 细节贴图大小
	float	m_fAmbient;	// 环境光
	float	m_fDetailRange;	// 细节贴图范围

	D3DXVECTOR3* m_pFaceNormal;	// 面法线
	CFrustum     m_Frustum;
public:
	bool         m_bWire;     //是否为线框模式

	CTerrain(void);
	~CTerrain(void);
	void BuildQuadTree(LPTERRAINQUADNODE pNode);	// 创建四叉树
	void DestroyQuadTree(LPTERRAINQUADNODE pNode);	// 销毁四叉树
	bool LoadHeightMap(LPCTSTR lpFileName);	// 加载高度图
	bool CreateSkeletonAndAlphaTex(DWORD* pdwPixel);	// 创建轮廓图和ALPHA图
	bool InitVertexBuffer(DWORD* pdwPixel);	// 初始化顶点缓存
	bool ComputeFaceNormals();	// 计算面法线
	void GenerateLightingMap(DWORD* pdwPixel,D3DXVECTOR3& vLighting);// 生成顶点光照
	bool Initialize(LPDIRECT3DDEVICE9 pdevice);// 初始化
	void CleanUp();	// 结束清空


	void Update(D3DXVECTOR3& vCamera,D3DXVECTOR3& vPlayerPos);	// 更新
	void BrowseQuadTree(LPTERRAINQUADNODE pRoot,CFrustum* pFrustum,D3DXVECTOR3& vCamera,D3DXVECTOR3& vPlayerPos);	// 遍历四叉树
	bool NodeCanDivided(LPTERRAINQUADNODE pNode);	// 节点能否继续分割
	void AddNodeToDraw(LPTERRAINQUADNODE pNode,D3DXVECTOR3& vPlayerPos);	// 添加渲染节点


	void Render();	// 渲染
	void PreRender(bool bNear);	// 渲染前状态设置
	void AfterRender();	// 渲染后状态还原


	//判断射线和三角形是否相交
	bool FixForCamera(OUT D3DXVECTOR3& vDestPos,IN D3DXVECTOR3& vRole,IN D3DXVECTOR3& vCamera,IN FLOAT MinDist);
	bool CollideWithCamera(D3DXVECTOR3 vRayOrigin,D3DXVECTOR3 vRayDir,D3DXVECTOR3& vHitPoint,float& fMinDist); 
	//拾取
	bool FindDestForPlayMove( OUT D3DXVECTOR3& vDestPos,OUT D3DXMATRIX& matRotation,IN D3DXVECTOR3& vCamera );


	int GetTriangleNumToDraw()// 获取当前帧渲染的三角形数
	{return (m_nIndicesNearCurFrame + m_nIndicesFarCurFrame) / 3;}
	bool IsOutOfRange(D3DXVECTOR3& vPosition)// 判断是否超出地形边界
	{return vPosition.x<-m_nSize/2*m_fCellScale||vPosition.x>m_nSize/2*m_fCellScale
	||vPosition.z<-m_nSize/2*m_fCellScale||vPosition.z>m_nSize/2*m_fCellScale;}
	float GetHeightMap(int nRow, int nColumn)// 获取指定行列高度值
	{return m_pfHeightValue[nRow * m_nSize + nColumn];}
	float GetHeightValue(float x,float z);// 获取指定坐标高度

	float GetTerrainWidth()	//获取地形的宽度
	{
		return (m_nSize - 1) * m_fCellScale;
	}
};
