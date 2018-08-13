#include "Monster.h"

CMonster::CMonster(void)
{
	m_bMove = false;
	m_bMoveByWalk = false;
	m_pAnimMesh = new CAnimMesh();
	D3DXMatrixIdentity(&m_matWorld);
	
}

CMonster::~CMonster(void)
{
}

bool CMonster::InitMonster( LPDIRECT3DDEVICE9 pdevice,CTerrain* pTerrain,CRole* pRole)
{
	m_pDevice = pdevice;
	m_pTerrain = pTerrain;
	m_pRole = pRole;

	CConfig::GetInstance().SetConfigFileName("res//ini//Monster.ini");
	m_fWalkSpeed = CConfig::GetInstance().ReadINI("Monster","WalkSpeed") * 1.0f;
	m_fRunSpeed = CConfig::GetInstance().ReadINI("Monster","RunSpeed") * 1.0f;
	m_fInterval = CConfig::GetInstance().ReadINI("Monster","Interval") * 1.0f;
	m_fMoveElapse = CConfig::GetInstance().ReadINI("Monster","MoveElapse") / 100.0f;
	
	if (FAILED(m_pAnimMesh->OnCreate(m_pDevice,"res//Monster//dragon.X")))
	{
		MessageBox(NULL,"创造怪物模型失败",NULL,MB_OK);
		return false;
	}

	m_vMonster = D3DXVECTOR3(m_pRole->m_vRole.x,0.0f,m_pRole->m_vRole.z-20.0f);
	m_vLook = D3DXVECTOR3((rand() % 200 - 100) / 100.0f,0.0f,(rand() % 200 - 100) / 100.0f);
	m_fTimeForMove = (rand() % (int)(m_fMoveElapse * 100.0f)) / 100.0f;

	m_pAnimMesh->SetAnimationSetByName("walk");

	return true;
}

void CMonster::Update( float fElapse,D3DXVECTOR3 vPos )
{
	
}

void CMonster::Render( bool bBoundingVolume )
{
	MoveAI();
//	m_vMonster.y = m_pTerrain->GetHeightValue(m_vMonster.x,m_vMonster.z);
	m_vLook = m_vMonster - m_pRole->m_vRole;
	Move();
	SetWorldMatrix();
//	UpdateBoundingVolumeMesh();
	m_pAnimMesh->Render(&m_matWorld,0.03f);
}

void CMonster::SetWorldMatrix()
{
	D3DXMATRIX MatScale;
	D3DXMatrixScaling(&MatScale,2.0f,2.0f,2.0f);
	D3DXMATRIX MatRotateY;
	m_vDirection.y=0.0f;

	D3DXVec3Normalize(&m_vDirection,&m_vDirection);
	float  angle=acosf(D3DXVec3Dot(&m_vDirection,&D3DXVECTOR3(0.0f,0.0f,-1.0f)));	
	angle = m_vDirection.x >= 0.0f? -angle:angle;

	D3DXMatrixRotationY(&MatRotateY, angle);
	D3DXMATRIX MatPos;
	D3DXMatrixTranslation(&MatPos,m_vMonster.x,m_vMonster.y,m_vMonster.z);
	m_matWorld = MatRotateY * MatScale * MatPos ;
}

void CMonster::CleanUp()
{

}

// 移动AI
void CMonster::MoveAI()
{
	m_fTimeForMove += CGameTime::GetInstance().GetElapse() * 0.01f;
	if (m_fTimeForMove > m_fMoveElapse)
	{
		m_fTimeForMove = 0.0f;
		m_bMove = true;
		if (m_bMove)
		{
			if (m_pRole->m_RoleState == RUN || m_pRole->m_RoleState == WALK)
			{
				m_vLook = m_pRole->m_vRole - m_vMonster;
			}
			else
				m_vLook = D3DXVECTOR3((rand()%200-100)/100.0f,0.0f,(rand()%200-100)/100.0f);
		}
	}
	if (m_bMove == true)
	{
		m_vOldPos = m_vMonster;
		m_vMonster += m_vLook * 0.01f;
	}
	else
		m_vMonster = m_vOldPos;

	D3DXVECTOR3 vTemp = m_pRole->m_vRole - m_vMonster;
	if (D3DXVec3Length(&vTemp) == 20.0f)
	{
		m_bMove = false;
	}
}

void CMonster::Move()
{
	if (m_pRole->m_RoleState == STAND)
	{
		m_vMonster = m_vOldPos;
	}
	else if (m_pRole->m_RoleState == RUN || m_pRole->m_RoleState == WALK)
	{
		m_bMove = true;
	}

	if (m_bMove == true)
	{
		m_vMonster = m_pRole->m_vOldPos - (m_vLook * WALKSPEED * 0.03f);
		//if (m_pRole->m_RoleState == RUN)
		//{			
		//	m_vMonster += m_vLook * 80.0f ;	
		//}
		//else if(m_pRole->m_RoleState == WALK)
		//{
		//	m_vMonster += m_vLook * 50.0f ;
		//}
	}
}