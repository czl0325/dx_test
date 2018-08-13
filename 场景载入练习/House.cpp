#include "House.h"

CHouse::CHouse(void)
{
	m_pStaticObject	= NULL;
	m_pMeshInfo		= NULL;
	D3DXMatrixIdentity(&m_matWorld);
}

CHouse::~CHouse(void)
{
}

//设置物件的世界矩阵
void CHouse::CalWorldMatrix( D3DXVECTOR3* Pos,D3DXVECTOR3* RotAngle,D3DXVECTOR3* Zoom )
{
	if (Zoom==NULL && RotAngle==NULL && Pos==NULL)
	{
		return ;
	}

	else
	{
		D3DXMATRIX	matZoom;
		D3DXMATRIX	matRotate;
		D3DXMATRIX	matTranslation;

		if (Zoom)//缩放
		{
			D3DXMatrixIdentity(&matZoom);
			D3DXMatrixScaling(&matZoom,Zoom->x,Zoom->y,Zoom->z);
			m_matWorld = m_matWorld * matZoom;
		}
		if (RotAngle)//旋转
		{
			D3DXMatrixIdentity(&matRotate);
			//Yaw  饶着UP轴旋转  饶Y旋转
			//Pitch饶着RIGHT轴方向旋转	饶X旋转
			//Roll 饶着LOOK轴旋转	饶Z旋转
			D3DXMatrixRotationYawPitchRoll(&matRotate,RotAngle->y,RotAngle->x,RotAngle->z);
			m_matWorld = m_matWorld * matRotate;
		}
		if (Pos)//平移
		{
			m_vPosition = *Pos;
			D3DXMatrixIdentity(&matTranslation);
			D3DXMatrixTranslation(&matTranslation,Pos->x,Pos->y,Pos->z);
			m_matWorld = m_matWorld * matTranslation;
		}
	}
}

//初始化静态物体
bool CHouse::InitStaticObject( LPDIRECT3DDEVICE9 pdevice,CStaticObject*	pStaticObject,D3DXVECTOR3 Pos,D3DXVECTOR3 Rot,D3DXVECTOR3 Zoom )
{
	m_pD3DDevice	= pdevice;
	m_pStaticObject	= pStaticObject;
	m_pMeshInfo		= pStaticObject->GetMeshInformation();

	//计算网格模型的组合变换矩阵
	CalWorldMatrix(&Pos,&Rot,&Zoom);

	//更新网格模型的包围体
	pStaticObject->UpdateBounding(m_matWorld);

	return true;
}

void CHouse::RenderStaticObject()
{
	m_pD3DDevice->SetTransform(D3DTS_WORLD,&m_matWorld);

	m_pD3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pD3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3DDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	m_pD3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	m_pD3DDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
	m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);		//启用Alpha测试
	m_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);	//设置Alpha测试函数
	m_pD3DDevice->SetRenderState(D3DRS_ALPHAREF,0x64);				//设置Alpha测试参考值

	m_pStaticObject->Render();

	m_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
}

//判断射线和网格是否有发生交叉
BOOL CHouse::CheckIntersect( D3DXVECTOR3 vRayOrigin,D3DXVECTOR3 vRayDir,
							D3DXVECTOR3& vHitPoint,float& fDistance,D3DXVECTOR3* vNormal )
{
	//计算世界观察矩阵的逆矩阵
	D3DXMATRIX InverseWorldMat;
	D3DXMatrixIdentity(&InverseWorldMat);
	D3DXMatrixInverse(&InverseWorldMat,NULL,&m_matWorld);

	//把射线转换到物体本地空间---因为射线是世界坐标,而网格模型还不是,所以要转化成一致.
	D3DXVec3TransformCoord(&vRayOrigin,&vRayOrigin,&InverseWorldMat);
	D3DXVec3TransformNormal(&vRayDir,&vRayDir,&InverseWorldMat);

	float u,v;//重心坐标
	float fDist;
	DWORD FaceIndex;
	DWORD CoundOfHit;
	LPD3DXBUFFER pBuffer = NULL;
	BOOL  bHit;

	//测试射线和网格的相交
	//参数:网格、射线位置、射线方向、判断是否相交(相交为true)、返回最近相交的三角形面的索引
	//重心坐标u,v、射线到相交点的距离、返回所有相交三角形的相关信息、返回相交三角形的数量
	if ( FAILED( D3DXIntersect( m_pMeshInfo,&vRayOrigin,&vRayDir,
								&bHit,&FaceIndex,&u,&v,&fDist,
								&pBuffer,&CoundOfHit) ) )
	{
		MessageBox(NULL,"D3DXIntersect() error!",NULL,MB_OK);
		return FALSE;
	}

	if (bHit)//存在相交
	{
		fDistance = fDist;//传回射线和相交点的距离

		MESHVERTEX* pVertices;
		WORD*		pMeshIndex;

		m_pMeshInfo->LockIndexBuffer(0,(void**)&pMeshIndex);
		m_pMeshInfo->LockVertexBuffer(0,(void**)&pVertices);
		WORD*	iThisTri = &pMeshIndex[3*FaceIndex];
		D3DXVECTOR3 Vertex1 = pVertices[iThisTri[0]].p;
		D3DXVECTOR3 Vertex2 = pVertices[iThisTri[1]].p;
		D3DXVECTOR3 Vertex3 = pVertices[iThisTri[2]].p;
		//通过重心坐标,求出交叉点
		vHitPoint = Vertex1 + u * (Vertex2 - Vertex1) + v * (Vertex3 - Vertex1);
		//将重心转移到世界空间
		D3DXVec3TransformCoord(&vHitPoint,&vHitPoint,&m_matWorld);

		//计算法线
		if (vNormal)
		{
			//通过向量的叉乘求出三角形的法线
			D3DXVec3Cross(vNormal,&(Vertex2 - Vertex1),&(Vertex3 - Vertex1));
			//将法线转移到世界空间
			D3DXVec3TransformNormal(vNormal,vNormal,&m_matWorld);
		}

		m_pMeshInfo->UnlockVertexBuffer();
		m_pMeshInfo->UnlockIndexBuffer();
	}

	return bHit;
}

