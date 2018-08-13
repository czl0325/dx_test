#include "Role.h"
#include "Camera.h"

CRole::CRole(void)
{
	m_AnimMesh1 = new CAnimMesh();
	m_bPlayAnim = false;

	D3DXMatrixIdentity(&m_matWorld);

	m_SkillTime = 0;
	m_bSkill = false;

	m_bMarker = false;
	m_bMoveByMouse = false;
}

CRole::~CRole(void)
{
	SAFE_DELETE(m_AnimMesh1);
}

//��ʼ��ʰȡͼ��
bool CRole::InitMarker()
{
	if (m_pDevice->CreateVertexBuffer(4*sizeof(MarkerVertex),D3DUSAGE_WRITEONLY,
		MarkerVertex::FVF,D3DPOOL_MANAGED,&m_pVB,NULL))
	{
		return false;
	}
	MarkerVertex* pVertex;
	m_pVB->Lock(0,0,(void**)&pVertex,0);
	pVertex[0] = MarkerVertex(-10.0f,0.0f,-10.0f,0.0f,0.0f);
	pVertex[1] = MarkerVertex(-10.0f,0.0f,10.0f,0.0f,1.0f);
	pVertex[2] = MarkerVertex(10.0f,0.0f,-10.0f,1.0f,0.0f);
	pVertex[3] = MarkerVertex(10.0f,0.0f,10.0f,1.0f,1.0f);
	m_pVB->Unlock();

	return true;
}

bool CRole::OnCreate( LPDIRECT3DDEVICE9 pD3DDevice,CTerrain* pTerrain , D3DXVECTOR3 vRole , CSceneObject* pSceneObject)
{
	m_pDevice = pD3DDevice;
	if (FAILED(m_AnimMesh1->OnCreate(pD3DDevice,"res//mesh//hero2.X")))
	{
		MessageBox(NULL,"����ģ��ʧ��",NULL,MB_OK);
		return false;
	}

	m_AnimMesh1->SetAnimationSetByName("idle");
	m_pTerrain = pTerrain;
	m_vRole = vRole;
	m_pSceneObject = pSceneObject;
	m_vDirection = CCamera::GetInstance()->GetLookOn();

	m_RoleState = STAND;
	m_fSpeed = 0.01f;
	m_bWalk = false;

	m_pBoundingVolume = m_AnimMesh1->GetBoundingVolume();
	m_pBoundingVolume->GetBoundingSphere()->vCenter.x = m_vRole.x;
	m_pBoundingVolume->GetBoundingSphere()->vCenter.y = m_vRole.y + 10.0f;
	m_pBoundingVolume->GetBoundingSphere()->vCenter.z = m_vRole.z;
	m_pBoundingVolume->Zoom(0.1f);

	// ��ʼ�������
	if(!CreateBoundingVolumeMesh())
	{
		return false;
	}

	//��ʼ��ʰȡͼƬ
	if (FAILED(D3DXCreateTextureFromFile(m_pDevice,"res//marker//marker.png",&m_pMarker)))
	{
		return false;
	}

	if(!InitMarker())
	{
		return false;
	}

	return true;
}

void CRole::SetWorldMatrix()
{
	D3DXMATRIX MatScale;
	D3DXMatrixScaling(&MatScale,0.3f,0.3f,0.3f);
	D3DXMATRIX MatRotateY;
	m_vDirection.y=0.0f;

	D3DXVec3Normalize(&m_vDirection,&m_vDirection);
	float  angle=acosf(D3DXVec3Dot(&m_vDirection,&D3DXVECTOR3(0.0f,0.0f,-1.0f)));	
	angle = m_vDirection.x >= 0.0f? -angle:angle;

	D3DXMatrixRotationY(&MatRotateY, angle);
	D3DXMATRIX MatPos;
	D3DXMatrixTranslation(&MatPos,m_vRole.x,m_vRole.y,m_vRole.z);
	m_matWorld = MatRotateY * MatScale * MatPos ;

	m_pBoundingVolume->GetBoundingSphere()->vCenter.x = m_vRole.x;
	m_pBoundingVolume->GetBoundingSphere()->vCenter.y = m_vRole.y + 10.0f;
	m_pBoundingVolume->GetBoundingSphere()->vCenter.z = m_vRole.z;

}



void CRole::KeyBoardDown(float fElaspTime)
{
	m_fAngle = acosf( D3DXVec3Dot(&(CCamera::GetInstance()->GetCameraRight()),&D3DXVECTOR3(-1.0f,0.0f,0.0f)) );
	m_fAngle = CCamera::GetInstance()->GetLookOn().x <= 0.0f ? m_fAngle : -m_fAngle;

	if (m_bWalk)
	{
		if (m_bMoveByWalk)
		{
			m_RoleState = WALK;
			m_AnimMesh1->SetAnimationSetByName("walk");
		}
		else
		{
			m_RoleState = RUN;
			m_AnimMesh1->SetAnimationSetByName("run");
		}
	}
	else
	{
		m_RoleState = STAND;
		m_AnimMesh1->SetAnimationSetByName("idle");
	}

	if (CInput::GetInstance().IsKeyDown(DIK_R,true))
	{
		m_bMoveByWalk = !m_bMoveByWalk;
	}
	else if (CInput::GetInstance().IsKeyDown(DIK_W,true))
	{
		m_bWalk = true;
		D3DXMatrixRotationY(&m_matRotation,m_fAngle);
		m_vDirection	= CCamera::GetInstance()->GetLookOn();
		m_vDirection.y  = 0.0f;
		D3DXVec3Normalize(&m_vDirection,&m_vDirection);
		JudgeRoleState(fElaspTime);
	}
	else if (CInput::GetInstance().IsKeyDown(DIK_S,true))
	{
		m_bWalk = true;
		D3DXMatrixRotationY(&m_matRotation,m_fAngle+D3DX_PI);
		m_vDirection	= CCamera::GetInstance()->GetLookOn() * -1.0f;
		m_vDirection.y  = 0.0f;
		D3DXVec3Normalize(&m_vDirection,&m_vDirection);
		JudgeRoleState(fElaspTime);
	}
	else if (CInput::GetInstance().IsKeyDown(DIK_A,true))
	{
		m_bWalk = true;
		D3DXMatrixRotationY(&m_matRotation,m_fAngle+D3DX_PI/2.0f);
		m_vDirection	= CCamera::GetInstance()->GetLookOn();
		D3DXVec3TransformNormal(&m_vDirection,&m_vDirection,&m_matRotation);
		m_vDirection.y  = 0.0f;
		D3DXVec3Normalize(&m_vDirection,&m_vDirection);
		JudgeRoleState(fElaspTime);
	}
	else if (CInput::GetInstance().IsKeyDown(DIK_D,true))
	{
		m_bWalk = true;
		D3DXMatrixRotationY(&m_matRotation,m_fAngle-D3DX_PI/2.0f);
		m_vDirection	= CCamera::GetInstance()->GetLookOn();
		D3DXVec3TransformNormal(&m_vDirection,&m_vDirection,&m_matRotation);
		m_vDirection.y  = 0.0f;
		D3DXVec3Normalize(&m_vDirection,&m_vDirection);
		JudgeRoleState(fElaspTime);
	}
	else if (CInput::GetInstance().IsKeyDown(DIK_Q,true) )
	{
		m_bPlayAnim = true;
	}
	else if (GetAsyncKeyState('1'))
	{
		m_bSkill = !m_bSkill;
	}

	if (m_bPlayAnim == true)
	{
		if (m_bSkill == false)
		{
			m_SkillTime += CGameTime::GetInstance().GetElapse();
			m_AnimMesh1->SetAnimationSetByName("skill1");
			if (m_SkillTime > 3000)
			{
				m_AnimMesh1->SetAnimationSetByName("idle");
				m_SkillTime = 0;
				m_bPlayAnim = false;
			}
		}
		else
		{
			m_SkillTime += CGameTime::GetInstance().GetElapse();
			m_AnimMesh1->SetAnimationSetByName("skill2");
			if (m_SkillTime > 3000)
			{
				m_AnimMesh1->SetAnimationSetByName("idle");
				m_SkillTime = 0;
				m_bPlayAnim = false;
			}
		}
	}
}

void CRole::KeyBoardUp( WPARAM wParam )//�����ϵ��¼�
{
	if(m_RoleState != STAND)
	{
		m_RoleState = STAND;
		m_bWalk = false;
	}
}

void CRole::Render(bool bBoundingVolume,float fElaspTime,int nState)
{
	m_AnimMesh1->m_bPlayAnim = m_bPlayAnim;//�����Ƿ񲥷Ŷ���

	KeyBoardDown(fElaspTime);
	MouseMove(fElaspTime);
	SetWorldMatrix();
	UpdateBoundingVolumeMesh();


	//���Ʊ�ʶͼƬ
	//if (m_bMarker)
	//{
	//	D3DXMatrixIdentity(&m_matMarker);
	//	D3DXMATRIX matTran;
	//	D3DXMatrixTranslation(&matTran,m_vDestPos.x,m_vDestPos.y,m_vDestPos.z);
	//	m_matMarker = m_matRotationMarker * matTran;
	//	m_pDevice->SetTransform(D3DTS_WORLD,&m_matMarker);
	//	m_pDevice->SetRenderState(D3DRS_ZENABLE,false);
	//	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	//	m_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	//	m_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	//	m_pDevice->SetRenderState(D3DRS_ALPHAREF,0x64);
	//	m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	//	m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	//	m_pDevice->SetTexture(0,m_pMarker);
	//	m_pDevice->SetStreamSource(0,m_pVB,0,sizeof(MarkerVertex));
	//	m_pDevice->SetFVF(MarkerVertex::FVF);
	//	m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
	//	m_pDevice->SetRenderState(D3DRS_ZENABLE,true);
	//	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
	//}
	switch(nState)
	{
	case 1:
		m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCCOLOR);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		//m_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_BLENDFACTOR);
		m_AnimMesh1->Render(&m_matWorld,fElaspTime);
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
		m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	case 2:
		break;
	case 3:
		m_pDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
		m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
		m_AnimMesh1->Render(&m_matWorld,fElaspTime);
		m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
		m_pDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_DISABLE);
	}
	
	if (bBoundingVolume == true)
	{
		DrawBoundingVolumeMesh();
	}

}

void CRole::CleanUp()
{
	m_AnimMesh1->OnDestroy();
}

void CRole::JudgeRoleState(float fElaspTime)
{
	if (m_bWalk == true)
	{
		if (m_bMoveByWalk)
		{
			m_vOldPos = m_vRole;
			m_vRole += m_vDirection * WALKSPEED * fElaspTime;
		}
		else
		{
			m_vOldPos = m_vRole;
			m_vRole += m_vDirection * RUNSPEED * fElaspTime;
		}
	}
	else
		m_vRole = m_vOldPos;

	m_vRole.y = m_pTerrain->GetHeightValue(m_vRole.x, m_vRole.z);

	if (!CanbeMove())
	{
		m_vRole = m_vOldPos;
	}
}

bool CRole::MouseMove(float fElaspTime)
{
	if (CInput::GetInstance().m_CurMState.Left)
	{
		if (m_pTerrain->FindDestForPlayMove(m_vDestPos,m_matRotationMarker,CCamera::GetInstance()->GetCameraPos()))
		{
			m_bWalk = true;
			m_bMarker=true;
			m_bMoveByMouse=true;
			m_vDirection=m_vDestPos-m_vRole;//��ɫ����
			m_vDirection.y=0.0f;
			D3DXVec3Normalize(&m_vDirection,&m_vDirection);
		}
	}
	if (m_bMoveByMouse)
	{
		D3DXVec3Normalize(&m_vDirection,&m_vDirection);
		JudgeRoleState(fElaspTime);
		// �ж��Ƿ��ѵ���
		D3DXVECTOR3 vDest = D3DXVECTOR3(m_vDestPos.x,0.0f,m_vDestPos.z);
		D3DXVECTOR3 vOrigin = D3DXVECTOR3(m_vRole.x,0.0f,m_vRole.z);
		D3DXVECTOR3 vMoveDir = vDest-vOrigin;
		D3DXVec3Normalize(&vMoveDir,&vMoveDir);
		FLOAT b = D3DXVec3Dot(&m_vDirection,&vMoveDir);
		if (b < 0)
		{
			m_vRole = m_vDestPos;
			m_bMoveByMouse = false;
			m_bWalk = false;
		}
	}
	return true;
}

//�жϵ��Ƿ���������
bool CRole::CheckPosInBoudingShere( BoundingSphere Sphere, D3DXVECTOR3 vPlayerPos )
{
	D3DXVECTOR3 vDirect = vPlayerPos - Sphere.vCenter;
	float fDistance = D3DXVec3Length(&vDirect);
	if (fDistance <= Sphere.fRadius)
	{
		return true;
	}
	return false;
}

//���������ģ�͵���ײ���
bool CRole::MeshCollisionTest( CHouse* pStaticObject,D3DXVECTOR3& NewPos, 
							  float HeightOrig,D3DXVECTOR3 vLookat,float AngleByY, 
							  D3DXVECTOR3* NewPosOnMesh /*= NULL*/ )
{
	D3DXMATRIX matRotY;
	D3DXMatrixIdentity(&matRotY);
	D3DXMatrixRotationY(&matRotY,AngleByY*D3DX_PI/180);//��ת����ĽǶ�
	D3DXVECTOR3 vDirection = vLookat;//����
	D3DXVECTOR3 vHitPoint;
	float		fDistance;
	D3DXVec3Normalize(&vDirection,&vDirection);
	D3DXVec3TransformNormal(&vDirection,&vDirection,&matRotY);//��ת��ĳ���

	//����������ĵ�ȥ�����������ײ���
	if ( pStaticObject->CheckIntersect( D3DXVECTOR3(NewPos.x,HeightOrig,NewPos.z),//���ߵ�ԭ��
										vDirection,	//����ĳ���
										vHitPoint,	//�����ཻ��
										fDistance,	//�������ߺ��ཻ��ľ���
										NULL ) )
	{
		if (NewPosOnMesh)//����������ģ����
		{
			*NewPosOnMesh = vHitPoint;
			return true;
		}
		else 
		{
			//������;�̬�������̫����ʱ��,���ܽ����ƶ�
			//��������Ҫ���������ƶ�һ�εľ���,��Ȼ����Ϳ��ܴ���ǽ��
			//if (fDistance < m_fMoveSpeed)
			if (fDistance < 6.0f)
				return true;
			else 
				return false;
		}
	}
}

//��ײ���
bool CRole::CheckCollision( CHouse* pStaticObject )
{
	BoundingSphere TempBoundingSphere = pStaticObject->GetBoundingSphere();
	//�Ⱥ;�̬����İ�Χ������ж�
	//���������İ�Χ�嶼û�з�����ײҲ��û��Ҫ���Ǻ���������ײ��
	if (CheckPosInBoudingShere(TempBoundingSphere, m_vRole))//��ɫ�ڰ�Χ����
	{
		//��ǰ�����
		if (MeshCollisionTest(pStaticObject,m_vRole,m_vOldPos.y+9.0f,m_vDirection,0.0f)) 
			return true;

		//��ǰ�����---��Ҫ�Ǳ���ͬʱʹ��ǰ���������ƶ����ֵ���ײ����
		if (MeshCollisionTest(pStaticObject,m_vRole,m_vOldPos.y+9.0f,m_vDirection,45.0f)) 
			return true;

		if (MeshCollisionTest(pStaticObject,m_vRole,m_vOldPos.y+9.0f,m_vDirection,-45.0f)) 
			return true;

		//�·����---��Ϊ¥�ݿ����Ǵ�ֱ��,���Է���ȡD3DXVECTOR3(0.0f,-1.0f,0.0f)���ܳ���û�н��������
		//�������µķ���ֻ����΢��б��,������¥���Ա߻��ǻ�������,����¥�����������ƽ���,�Ͳ�������������鷳������
		D3DXVECTOR3 NewPosOnMesh = D3DXVECTOR3(0.0f,0.0f,0.0f);
		if (MeshCollisionTest(pStaticObject,m_vRole,m_vOldPos.y+10.0f,D3DXVECTOR3(0.0f,-1.0f,0.0f),0.0f,&NewPosOnMesh))
		{
			if (NewPosOnMesh.y > m_vRole.y)//�����������������ĵ���߶�
			{
				if (abs(NewPosOnMesh.y - m_vOldPos.y) < 20.0f)//�߶����̫��ĵط�,����������ֱ����ȥ
				{
					m_vRole.y = NewPosOnMesh.y;
					return false;
				}
				else 
					return true;
			}
		}
		else
		{
			if (MeshCollisionTest(pStaticObject,m_vRole,m_vOldPos.y+10.0f,D3DXVECTOR3(0.1f,-0.9f,0.1f),0.0f,&NewPosOnMesh))
			{
				if (NewPosOnMesh.y > m_vRole.y)//�����������������ĵ���߶�
				{
					if (abs(NewPosOnMesh.y - m_vOldPos.y) < 20.0f)//�߶����̫��ĵط�,����������ֱ����ȥ
					{
						m_vRole.y = NewPosOnMesh.y;
						return false;
					}
					else 
						return true;
				}
			}
		}
	}

	return false;
}

//�ж������Ƿ����ƶ�
bool CRole::CanbeMove()
{
	//�ж����������̬�������ײ
	int n_NumObject = m_pSceneObject->GetNumObject();
	for (int i=0; i < n_NumObject; i++)
	{
		//�����ж�

		if (CheckCollision(m_pSceneObject->GetStaticObject(i)))
		{
			return false;
		}
	}
	return true;
}
