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

//����������������
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

		if (Zoom)//����
		{
			D3DXMatrixIdentity(&matZoom);
			D3DXMatrixScaling(&matZoom,Zoom->x,Zoom->y,Zoom->z);
			m_matWorld = m_matWorld * matZoom;
		}
		if (RotAngle)//��ת
		{
			D3DXMatrixIdentity(&matRotate);
			//Yaw  ����UP����ת  ��Y��ת
			//Pitch����RIGHT�᷽����ת	��X��ת
			//Roll ����LOOK����ת	��Z��ת
			D3DXMatrixRotationYawPitchRoll(&matRotate,RotAngle->y,RotAngle->x,RotAngle->z);
			m_matWorld = m_matWorld * matRotate;
		}
		if (Pos)//ƽ��
		{
			m_vPosition = *Pos;
			D3DXMatrixIdentity(&matTranslation);
			D3DXMatrixTranslation(&matTranslation,Pos->x,Pos->y,Pos->z);
			m_matWorld = m_matWorld * matTranslation;
		}
	}
}

//��ʼ����̬����
bool CHouse::InitStaticObject( LPDIRECT3DDEVICE9 pdevice,CStaticObject*	pStaticObject,D3DXVECTOR3 Pos,D3DXVECTOR3 Rot,D3DXVECTOR3 Zoom )
{
	m_pD3DDevice	= pdevice;
	m_pStaticObject	= pStaticObject;
	m_pMeshInfo		= pStaticObject->GetMeshInformation();

	//��������ģ�͵���ϱ任����
	CalWorldMatrix(&Pos,&Rot,&Zoom);

	//��������ģ�͵İ�Χ��
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
	m_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);		//����Alpha����
	m_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);	//����Alpha���Ժ���
	m_pD3DDevice->SetRenderState(D3DRS_ALPHAREF,0x64);				//����Alpha���Բο�ֵ

	m_pStaticObject->Render();

	m_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
}

//�ж����ߺ������Ƿ��з�������
BOOL CHouse::CheckIntersect( D3DXVECTOR3 vRayOrigin,D3DXVECTOR3 vRayDir,
							D3DXVECTOR3& vHitPoint,float& fDistance,D3DXVECTOR3* vNormal )
{
	//��������۲����������
	D3DXMATRIX InverseWorldMat;
	D3DXMatrixIdentity(&InverseWorldMat);
	D3DXMatrixInverse(&InverseWorldMat,NULL,&m_matWorld);

	//������ת�������屾�ؿռ�---��Ϊ��������������,������ģ�ͻ�����,����Ҫת����һ��.
	D3DXVec3TransformCoord(&vRayOrigin,&vRayOrigin,&InverseWorldMat);
	D3DXVec3TransformNormal(&vRayDir,&vRayDir,&InverseWorldMat);

	float u,v;//��������
	float fDist;
	DWORD FaceIndex;
	DWORD CoundOfHit;
	LPD3DXBUFFER pBuffer = NULL;
	BOOL  bHit;

	//�������ߺ�������ཻ
	//����:��������λ�á����߷����ж��Ƿ��ཻ(�ཻΪtrue)����������ཻ���������������
	//��������u,v�����ߵ��ཻ��ľ��롢���������ཻ�����ε������Ϣ�������ཻ�����ε�����
	if ( FAILED( D3DXIntersect( m_pMeshInfo,&vRayOrigin,&vRayDir,
								&bHit,&FaceIndex,&u,&v,&fDist,
								&pBuffer,&CoundOfHit) ) )
	{
		MessageBox(NULL,"D3DXIntersect() error!",NULL,MB_OK);
		return FALSE;
	}

	if (bHit)//�����ཻ
	{
		fDistance = fDist;//�������ߺ��ཻ��ľ���

		MESHVERTEX* pVertices;
		WORD*		pMeshIndex;

		m_pMeshInfo->LockIndexBuffer(0,(void**)&pMeshIndex);
		m_pMeshInfo->LockVertexBuffer(0,(void**)&pVertices);
		WORD*	iThisTri = &pMeshIndex[3*FaceIndex];
		D3DXVECTOR3 Vertex1 = pVertices[iThisTri[0]].p;
		D3DXVECTOR3 Vertex2 = pVertices[iThisTri[1]].p;
		D3DXVECTOR3 Vertex3 = pVertices[iThisTri[2]].p;
		//ͨ����������,��������
		vHitPoint = Vertex1 + u * (Vertex2 - Vertex1) + v * (Vertex3 - Vertex1);
		//������ת�Ƶ�����ռ�
		D3DXVec3TransformCoord(&vHitPoint,&vHitPoint,&m_matWorld);

		//���㷨��
		if (vNormal)
		{
			//ͨ�������Ĳ����������εķ���
			D3DXVec3Cross(vNormal,&(Vertex2 - Vertex1),&(Vertex3 - Vertex1));
			//������ת�Ƶ�����ռ�
			D3DXVec3TransformNormal(vNormal,vNormal,&m_matWorld);
		}

		m_pMeshInfo->UnlockVertexBuffer();
		m_pMeshInfo->UnlockIndexBuffer();
	}

	return bHit;
}

