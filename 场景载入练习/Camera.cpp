#include "Camera.h"

CCamera* CCamera::m_pCamera=NULL;

CCamera::CCamera()
{ 
	m_nElapse     = 0;
	
	m_nState = 3;

	m_pTerrain    = new CTerrain();
	m_pRole       = new CRole();
	m_pPlant      = new CPlant();
}

CCamera::~CCamera(void)
{
	delete m_pTerrain;
	delete m_pRole;
	delete m_pPlant;
}

bool CCamera::InitCamera( LPDIRECT3DDEVICE9 pDevice,CTerrain* pTerrain,float distance , CRole *pRole , CSceneObject* pSceneObject ,CPlant* pPlant)
{
	m_pd3dDevice=pDevice;
	m_pTerrain=pTerrain;
	m_pRole=pRole;
	m_pSceneObject=pSceneObject;
	m_pPlant = pPlant;

	m_vUpAxis = D3DXVECTOR3(0.0f,1.0f,0.0f);
	m_vRight  = D3DXVECTOR3(1.0f,0.0f,0.0f);
	m_vViewAt = D3DXVECTOR3(0.0f,0.0f,1.0f);

	m_fDistance=distance;

	m_fVArc=0.0f;
	m_fHArc=0.0f;

	return true;
}

void CCamera::SetViewMatrix( D3DXMATRIX* pViewMatrix )
{
	D3DXVec3Normalize(&m_vViewAt,&m_vViewAt);
	D3DXVec3Cross(&m_vUpAxis,&m_vViewAt,&m_vRight);
	D3DXVec3Normalize(&m_vUpAxis,&m_vUpAxis);
	D3DXVec3Cross(&m_vRight,&m_vUpAxis,&m_vViewAt);
	D3DXVec3Normalize(&m_vRight,&m_vRight);

	float x = -D3DXVec3Dot(&m_vRight,&m_vPosition);
	float y = -D3DXVec3Dot(&m_vUpAxis,&m_vPosition);
	float z = -D3DXVec3Dot(&m_vViewAt,&m_vPosition);

	pViewMatrix->_11=m_vRight.x;	    pViewMatrix->_21=m_vRight.y;
	pViewMatrix->_12=m_vUpAxis.x;		pViewMatrix->_22=m_vUpAxis.y;
	pViewMatrix->_13=m_vViewAt.x;		pViewMatrix->_23=m_vViewAt.y;
	pViewMatrix->_14=0.0f;			    pViewMatrix->_24=0.0f;

	pViewMatrix->_31=m_vRight.z;	    pViewMatrix->_41=x;
	pViewMatrix->_32=m_vUpAxis.z;		pViewMatrix->_42=y;
	pViewMatrix->_33=m_vViewAt.z;		pViewMatrix->_43=z;
	pViewMatrix->_34=0.0f;			    pViewMatrix->_44=1.0f;
}

//X轴移动
void CCamera::X_Move(float speed)
{
	m_vPosition += m_vRight*speed;
}

//Y轴移动
void CCamera::Y_Move(float speed)
{
	m_vPosition += m_vUpAxis*speed;
}

//Z轴移动
void CCamera::Z_Move(float speed)
{
	m_vPosition += m_vViewAt*speed;
}

//X轴旋转
void CCamera::X_Rotate(float angle)
{
	D3DXMATRIX mat;
	D3DXMatrixRotationAxis(&mat,&m_vRight,angle);
	D3DXVec3TransformCoord(&m_vUpAxis,&m_vUpAxis,&mat);
	D3DXVec3TransformCoord(&m_vViewAt,&m_vViewAt,&mat);
}

//Y轴旋转
void CCamera::Y_Rotate(float angle)
{
	D3DXMATRIX mat;
	//D3DXMatrixRotationAxis(&mat,&m_Up,angle);
	D3DXMatrixRotationY(&mat,angle);
	D3DXVec3TransformCoord(&m_vRight,&m_vRight,&mat);
	D3DXVec3TransformCoord(&m_vViewAt,&m_vViewAt,&mat);
}

//Z轴旋转
void CCamera::Z_Rotate(float angle)
{
	D3DXMATRIX mat;
	D3DXMatrixRotationAxis(&mat,&m_vViewAt,angle);
	D3DXVec3TransformCoord(&m_vUpAxis,&m_vUpAxis,&mat);
	D3DXVec3TransformCoord(&m_vRight,&m_vRight,&mat);
}

//自由角度旋转
void CCamera::Axis_Rotate(D3DXVECTOR3 *vec, float angle)
{
	D3DXMATRIX mat;
	D3DXMatrixRotationAxis(&mat,vec,angle);
	D3DXVec3TransformCoord(&m_vViewAt,&m_vViewAt,&mat);
	D3DXVec3TransformCoord(&m_vPosition,&m_vPosition,&mat);
}

//增加水平角度
void CCamera::AddHArc(float arc)
{
	m_fHArc -= arc;
}
//增加垂直角度
void CCamera::AddVArc(float arc)
{
	m_fVArc -= arc;
	if(m_fVArc >= D3DX_PI/2.2f)//判断垂直角度
	{
		m_fVArc = D3DX_PI/2.2f;
	}
	if(m_fVArc <= -D3DX_PI/3.0f)
	{
		m_fVArc = -D3DX_PI/3.0f;
	}
}

void CCamera::SetDistance(FLOAT Distance)
{
	m_fDistance += Distance;
	if(m_fDistance<=(MINDISTANCE))
		m_fDistance = (MINDISTANCE);
	if(m_fDistance>=MAXDISTANCE)
		m_fDistance = MAXDISTANCE;
}

void CCamera::Run( CInput* pInput )
{
	//m_fDistance-=pInput->GetMouseWheel()*(CGameTime::GetInstance().GetElapse()*0.01f);
	//m_vToLook = m_pRole->m_vRole;
	////m_fDistance=m_fDistance<0? 0.0f:m_fDistance;
	//if (m_fDistance < 2.0f)
	//{
	//	m_fDistance = 2.0f;
	//	m_pRole->m_bTransparent = true;
	//}
	//else if (m_fDistance > 500.0f)
	//{
	//	m_fDistance = 500.0f;
	//}
	//else
	//	m_pRole->m_bTransparent = false;
	//SetDistance(-pInput->GetMouseWheel() / 10.0f);

	static int LastTime=timeGetTime();
	int		CurTime=timeGetTime();
	m_nElapse=CurTime-LastTime;
	LastTime=CurTime;
	if(pInput->m_CurMState.Right)
	{
		this->AddHArc(0.0007f*m_nElapse*(pInput->m_CurMState.Pos.x-pInput->m_OldMState.Pos.x));
		this->AddVArc(0.0007f*m_nElapse*(pInput->m_CurMState.Pos.y-pInput->m_OldMState.Pos.y));
	}

	m_vToLook = m_pRole->m_vRole;
	m_vToLook.y += 20;

	//根据角色坐标来计算摄像机的坐标
	m_vPosition.x = m_vToLook.x - m_fDistance * cos(m_fVArc) * cos(m_fHArc);
	m_vPosition.y = m_vToLook.y - m_fDistance * sin(m_fVArc) + 20;
	m_vPosition.z = m_vToLook.z - m_fDistance * cos(m_fVArc) * sin(m_fHArc);


	DetectBound();

	D3DXMatrixLookAtLH(&m_matView,&m_vPosition,&m_vToLook,&m_vUpAxis);
	m_pd3dDevice->SetTransform(D3DTS_VIEW,&m_matView);
	//设置投影矩阵
	D3DXMatrixPerspectiveFovLH(&m_matProj,D3DX_PI/4,1.0f,
		1.0f,10000.0f);
	m_pd3dDevice->SetTransform(D3DTS_PROJECTION,&m_matProj);
}

void CCamera::DetectBound()
{
	D3DXVECTOR3 vDestPos;
	D3DXVECTOR3 vRole = m_pRole->m_vRole;//人物的位置
	static float fObstacleDistance = m_fDistance;


	vRole.y +=10.0f;
	if (m_pTerrain ->FixForCamera(vDestPos,vRole,m_vPosition,m_fDistance))
	{
		D3DXVECTOR3 vTemp = vDestPos - vRole;
		if(D3DXVec3Length(&vTemp) < m_fDistance)
		{
			m_vPosition = vDestPos;
			m_vPosition.y += 5.0f;
		}	
	}

	//**********************判断摄象机射线和静态物件网格碰撞*********************//
	D3DXVECTOR3 n_Direct = m_vPosition - vRole;
	D3DXVec3Normalize(&n_Direct,&n_Direct);
	int n_NumObject = m_pSceneObject->GetNumObject();

	D3DXVECTOR3 vHitPoint;
	float fDist = 0.0f;

	for (int i=0;i<n_NumObject;i++)
	{
		//if ( CheckDistance( m_pSceneObject->GetStaticObject(i)->GetBoundingSphere(),
		//					m_pSceneObject->GetStaticObject(i)->GetObjPosition(),vRole) )
		//{
			D3DXVECTOR3 vTemp1 = vRole - m_vPosition;
			if (m_pSceneObject->GetStaticObject(i)->CheckIntersect(vRole,n_Direct,vHitPoint,fDist,NULL))
			{
				if (m_fDistance >= fDist)
				{
					m_vPosition = vHitPoint - n_Direct*3.0f;//调整摄像机的位置(交点稍微往人一点的位置)
					m_vPosition.y += 5.0f;//避免视点紧贴网格的情况(不然会看到网格下面)
					fObstacleDistance = fDist;
				}
			}
			//}
	}

	//有些树在判断交点时,会有问题,具体原因暂时不明确
	//for (int i=0;i<m_pPlant->m_vPlant.size();i++)
	//{
	//	if (m_pPlant->m_vPlant[i]->PickMesh(&vRole,&n_Direct,vHitPoint,fDist,NULL))
	//	{
	//		if (m_fDistance >= fDist)
	//		{
	//			m_vPosition = vHitPoint - n_Direct*3.0f;//调整摄像机的位置(交点稍微往人一点的位置)
	//			m_vPosition.y += 5.0f;//避免视点紧贴网格的情况(不然会看到网格下面)
	//			fObstacleDistance = fDist;
	//		}
	//	}
	//}

	D3DXVECTOR3 vDistance = m_vPosition - vRole;
	float f = D3DXVec3Length(&vDistance);
	if (f < 25.0f && f > 12.0f)
	{
		m_nState = 1;
	}
	else if (f <= 12.0f)
	{
		m_nState = 2;
	}
	else
		m_nState = 3;
}

//判断点是否在球体内
bool CCamera::CheckDistance( BoundingSphere Sphere, D3DXVECTOR3 vObject, D3DXVECTOR3 vPlayer )
{
	D3DXVECTOR3 vDistance = vObject - vPlayer;
	vDistance.y = 0.0f;
	float fDistance = D3DXVec3Length(&vDistance);
	fDistance = fDistance - Sphere.fRadius;
	//在摄像机活动范围外的可以不用去考虑
	if (fDistance <= m_fDistance)
	{
		return true;
	}
	return false;
}

//改变摄像机的位置
void CCamera::ChangePos( D3DXVECTOR3 Vector )
{
	D3DXMATRIX MatTrans;
	D3DXMatrixTranslation(&MatTrans,Vector.x,Vector.y,Vector.z);
	D3DXVec3TransformCoord(&m_vPosition,&m_vPosition,&MatTrans);
}

//判断点是否在球体内
bool CCamera::CheckPosInBoudingShere( BoundingSphere Sphere, D3DXVECTOR3 vCameraPos )
{
	D3DXVECTOR3 vDirect = vCameraPos - Sphere.vCenter;
	float fDistance = D3DXVec3Length(&vDirect);
	if (fDistance <= Sphere.fRadius)
	{
		return true;
	}
	return false;
}

