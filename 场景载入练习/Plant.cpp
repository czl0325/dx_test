#include "Plant.h"

CPlant::CPlant(void)
{
}

CPlant::~CPlant(void)
{
	
}

bool CPlant::Init( LPDIRECT3DDEVICE9 pDevice,CStaticObject* pStaticObject,
				  float fScale, bool bIgnoreCollision,bool bAlphaBlend,
				  D3DXVECTOR3 vPosition,CTerrain* pTerrain )
{
	m_pDevice = pDevice;
	m_pStaticObject = pStaticObject;
	m_fScale = fScale;
	m_bIgnoreCollision = bIgnoreCollision;
	m_bAlphaBlend = bAlphaBlend;
	m_vPosition = vPosition;
	m_pBoundingVolume = &m_pStaticObject->m_BoundingVolume;
	m_pBoundingVolume->GetBoundingSphere()->vCenter.x = m_vPosition.x ;
	m_pBoundingVolume->GetBoundingSphere()->vCenter.y = m_vPosition.y + 10.0f;
	m_pBoundingVolume->GetBoundingSphere()->vCenter.z = m_vPosition.z ;
	m_pBoundingVolume->Zoom(0.06f*fScale);

	if(!CreateBoundingVolumeMesh())//创建外接体模型
		return false;

	return true;
}

void CPlant::Render(bool bBoundingVolume)
{
	//设置物体世界坐标系并绘制
	D3DXMATRIX matPosition,matScale;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation(&matPosition,m_vPosition.x,m_vPosition.y,m_vPosition.z);
	D3DXMatrixScaling(&matScale,m_fScale,m_fScale,m_fScale);
	matWorld = matScale * matPosition ;
	m_pDevice->SetTransform(D3DTS_WORLD,&matWorld);
	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	if (m_bAlphaBlend)
	{
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
		m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,true);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		m_pDevice->SetRenderState(D3DRS_ALPHAREF,0x00000000);
		m_pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
		//m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
		m_pStaticObject->Render();
		//m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
		m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,false);
	}
	else
		m_pStaticObject->Render();


	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
	
	if (bBoundingVolume == true)
	{
		UpdateBoundingVolumeMesh();
		DrawBoundingVolumeMesh();
	}
}

void CPlant::CleanUp()
{
	
}

bool CPlant::PickMesh( D3DXVECTOR3* PickOrig,D3DXVECTOR3* PickDirect, D3DXVECTOR3& OutPickPosition,FLOAT& Dis,D3DXVECTOR3* Normal/*=NULL*/ )
{
	D3DXMATRIX n_InverceWorldMat;
	D3DXMatrixIdentity(&n_InverceWorldMat);
	D3DXMatrixInverse(&n_InverceWorldMat,NULL,&matWorld);

	D3DXVECTOR3 n_PickOrig=*PickOrig;
	D3DXVECTOR3 n_PickDirect=*PickDirect;

	//把射线转换到物体本地空间   
	D3DXVec3TransformCoord(&n_PickOrig,&n_PickOrig,&n_InverceWorldMat);
	D3DXVec3TransformNormal(&n_PickDirect,&n_PickDirect,&n_InverceWorldMat);

	//重心坐标
	FLOAT U;
	FLOAT V;
	FLOAT n_Distance;
	DWORD n_pFaceIndex;
	DWORD  n_CoundOfHit;
	LPD3DXBUFFER n_pBuffer=NULL;
	BOOL  n_BePick;
	D3DXVECTOR3 n_ResultPos;
	HRESULT hr=NULL;

	hr = D3DXIntersect(m_pStaticObject->m_pMesh,&n_PickOrig,&n_PickDirect,&n_BePick,&n_pFaceIndex,
		&U,&V,&n_Distance,&n_pBuffer,&n_CoundOfHit);

	if (n_BePick)
	{
		n_BePick = true;
		Dis = n_Distance;
		NORMALVERTEX* pVertices;
		WORD*   n_pMeshIndex;
		m_pStaticObject->m_pMesh->LockIndexBuffer(0,(void**)&n_pMeshIndex);
		m_pStaticObject->m_pMesh->LockVertexBuffer(0,(void**)&pVertices);
		WORD*	iThisTri=&n_pMeshIndex[3*n_pFaceIndex];
		D3DXVECTOR3 n_Vertex1=pVertices[iThisTri[0]].Position;
		D3DXVECTOR3 n_Vertex2=pVertices[iThisTri[1]].Position;
		D3DXVECTOR3 n_Vertex3=pVertices[iThisTri[2]].Position;
		n_ResultPos =n_Vertex1 + U *(n_Vertex2-n_Vertex1) +V *(n_Vertex3-n_Vertex1);
		D3DXVec3TransformCoord(&n_ResultPos,&n_ResultPos,&matWorld);

		//计算法线
		if (Normal)
		{
			D3DXVec3Cross(Normal,&(n_Vertex2-n_Vertex1),&(n_Vertex3-n_Vertex1));
			D3DXVec3TransformNormal(Normal,Normal,&matWorld);
		}

		m_pStaticObject->m_pMesh->UnlockVertexBuffer();
		m_pStaticObject->m_pMesh->UnlockIndexBuffer();
		OutPickPosition=n_ResultPos;
	}
	return n_BePick;
}

