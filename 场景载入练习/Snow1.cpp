#include "Snow1.h"

CSnow1::CSnow1(void)
{
	m_MaxNum     = 1100;
	m_ChangeTime = 1;
	m_VbOffset   = 0;   //起始点编号
	m_Vbset      = 500; //一次绘制几个点
	m_pTextrue   = NULL;
}

CSnow1::~CSnow1(void)
{
}

BOOL CSnow1::InitParticle( IDirect3DDevice9* pD3dDevice )
{
	m_pD3dDevice = pD3dDevice;
	if( FAILED( m_pD3dDevice ->CreateVertexBuffer(m_MaxNum*sizeof(PARTICLE),
		D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY|D3DUSAGE_POINTS,
		FVF,
		D3DPOOL_DEFAULT,
		&m_pVB,
		NULL) ) )
	{
		return FALSE;
	}

	for(INT i = 0; i < m_MaxNum; ++i )
	{
		LPATTRBULE pAttrbule = new ATTRBULE;
		m_Paticle.push_back(pAttrbule);

		m_Paticle[i]->Age = 0;
		m_Paticle[i]->Color = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
		m_Paticle[i]->IsAlife = FALSE;

		m_Paticle[i]->Velocity = D3DXVECTOR3(0.0f,-0.0f,0.0f);
		m_Paticle[i]->Acceleration = D3DXVECTOR3(0.0f,-0.01f,0.0f);		
		m_Paticle[i]->FadeColor = m_Paticle[i]->Color;
	}

	if ( !CreateTexture("res//Snow//snow1.bmp") )
		return false;

	return TRUE;
}

BOOL CSnow1::CreateTexture( char* FileName )
{
	if( FAILED( D3DXCreateTextureFromFile(m_pD3dDevice,FileName,&m_pTextrue) ) )
	{
		return FALSE;
	}

	return TRUE;
}

VOID CSnow1::RenderState()
{
	FLOAT PointSize = 3.0f;
	FLOAT PointMinSize = 2.0f;
	FLOAT Point_A = 0.0f;
	FLOAT Point_B = 0.0f;
	FLOAT Point_C = 1.0f;

	m_pD3dDevice ->SetRenderState(D3DRS_POINTSPRITEENABLE,true);
	m_pD3dDevice ->SetRenderState(D3DRS_POINTSCALEENABLE,true);

	m_pD3dDevice ->SetRenderState(D3DRS_POINTSIZE,*(DWORD*)&(PointSize));
	m_pD3dDevice ->SetRenderState(D3DRS_POINTSIZE_MIN,*(DWORD*)&(PointMinSize));

	m_pD3dDevice ->SetRenderState(D3DRS_POINTSCALE_A,*(DWORD*)&(Point_A));
	m_pD3dDevice ->SetRenderState(D3DRS_POINTSCALE_B,*(DWORD*)&(Point_B));
	m_pD3dDevice ->SetRenderState(D3DRS_POINTSCALE_C,*(DWORD*)&(Point_C));

	m_pD3dDevice ->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	m_pD3dDevice ->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);

	m_pD3dDevice ->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	m_pD3dDevice ->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_pD3dDevice ->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_BLENDFACTOR);
}

VOID CSnow1::Update( CTerrain* Terrain,D3DXVECTOR3 vRlose /*= D3DXVECTOR3(0.0f,0.0f,0.0f)*/ )
{
	for(INT i = 0;i<m_MaxNum;++i)
	{
		if(m_Paticle[i]->IsAlife)
		{
			FLOAT fTerrainHeight = Terrain->GetHeightValue(m_Paticle[i]->Position.x,m_Paticle[i]->Position.z);
			if( (m_Paticle[i]->Position.y) <= (fTerrainHeight) )
			{
				m_Paticle[i]->Age = 0;
				m_Paticle[i]->IsAlife = FALSE;
				m_Paticle[i]->Position += (m_Paticle[i]->Velocity) + (m_Paticle[i]->Acceleration)*(FLOAT)(m_Paticle[i]->Age);
			}
			else
			{		
				m_Paticle[i]->Position += (m_Paticle[i]->Velocity) + (m_Paticle[i]->Acceleration)*(FLOAT)(m_Paticle[i]->Age);
				m_Paticle[i]->Age += 1.0f;	
			}
		}
		else
		{
			if (m_Paticle[i]->Age == 0)
			{
				m_Paticle[i]->IsAlife = TRUE;
				m_Paticle[i]->Position = vRlose+D3DXVECTOR3(GetRand(-1000,1000),GetRand(10,600),GetRand(-1000,1000));
			}
		}
	}
}

VOID CSnow1::Render()
{
	RenderState();

	m_pD3dDevice->SetTexture(0,m_pTextrue);//m_pTextrue
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	m_pD3dDevice->SetTransform(D3DTS_WORLD,&matWorld);
	m_pD3dDevice->SetFVF(FVF);
	m_pD3dDevice->SetStreamSource(0,m_pVB,0,sizeof(PARTICLE));

	if (m_VbOffset>m_MaxNum)//起始点编号大于最大数目
	{
		m_VbOffset = 0;
	}

	PARTICLE* VbBuffer = 0;

	if( FAILED( m_pVB ->Lock(m_VbOffset*sizeof(PARTICLE),
		m_Vbset*sizeof(PARTICLE),
		(VOID**)&VbBuffer,
		m_VbOffset?D3DLOCK_NOOVERWRITE:D3DLOCK_DISCARD) ) )
	{
		return ;
	}

	INT NumParticle = 0;

	for(INT i = m_VbOffset;i < m_MaxNum; ++i)
	{
		if(m_Paticle[i]->IsAlife)
		{
			if(m_Paticle[i]->Age >= m_Paticle[i] ->LifeTime)
			{
				VbBuffer ->Color = m_Paticle[i] ->FadeColor; 
				VbBuffer ->Position = m_Paticle[i]->Position;
			}
			else
			{
				VbBuffer ->Color = m_Paticle[i] ->Color;
				VbBuffer ->Position = m_Paticle[i]->Position;
			}

			++NumParticle;	
			++VbBuffer;

			if (NumParticle == m_Vbset)
			{
				m_pVB ->Unlock();
				NumParticle = 0;
				m_pD3dDevice ->DrawPrimitive(D3DPT_POINTLIST,m_VbOffset,m_Vbset);
				m_VbOffset += m_Vbset;

				if (m_VbOffset >= m_MaxNum)
				{
					m_VbOffset = 0;
				}
				if( FAILED( m_pVB ->Lock(m_VbOffset*sizeof(PARTICLE),
					m_Vbset*sizeof(PARTICLE),
					(VOID**)&VbBuffer,
					m_VbOffset?D3DLOCK_NOOVERWRITE:D3DLOCK_DISCARD) ) )
				{
					return ;
				}
			}
		}
	}

	m_pVB ->Unlock();
	if (NumParticle)
	{
		m_pD3dDevice ->DrawPrimitive(D3DPT_POINTLIST,m_VbOffset,NumParticle);
	}
	m_VbOffset += m_Vbset;

	m_pD3dDevice ->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
}

VOID CSnow1::SetD3dMaterial()
{
	D3DMATERIAL9 Material;
	ZeroMemory(&Material,sizeof(D3DMATERIAL9));
	Material.Ambient = D3DXCOLOR(1.0f,1.0f,1.0f,0.5f);
	Material.Diffuse = D3DXCOLOR(1.0f,1.0f,1.0f,0.5f);
	Material.Power = 1.0f;
	m_pD3dDevice ->SetMaterial(&Material);
}

VOID CSnow1::Clearup()
{
	if (m_pTextrue)
	{
		m_pTextrue->Release();
	}

	m_Paticle.clear();
}

FLOAT CSnow1::GetRand( FLOAT Min,FLOAT Max )
{
	if (Min == 0 && Max != 0)
	{
		return (FLOAT)(rand()%(INT)Max);
	}
	if (Min != 0 && Max == 0)
	{
		return (FLOAT)(-rand()%(INT)Min);
	}
	if (Min == 0 && Max == 0)
	{
		return 0;
	}
	return Min+rand()%(INT)(Max - Min);
}
