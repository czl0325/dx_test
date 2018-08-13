#include "Sun.h"

CSun::CSun(void)
{
	for (int i=0;i<7;i++)
	{
		m_pTexture[i]=NULL;
	}
	m_Alpha=1.0f;
	m_Color=D3DXCOLOR(1.0f,1.0f,1.0f,m_Alpha);
}

CSun::~CSun(void)
{
	CleanUp();
}

bool CSun::InitSun( LPDIRECT3DDEVICE9 pDevice,float Width,float Height )
{
	m_SunPos.x=100;
	m_SunPos.y=300;
	m_SunPos.z=100;
	m_pd3dDevice=pDevice;
	m_nWidth=Width;
	m_nHeight=Height;
	if (FAILED(D3DXCreateSprite(m_pd3dDevice,&m_pSprite)))
	{
		return false;
	}
	int SunNum=7;
	for (int i=0;i<SunNum;i++)
	{
		LPLENFANLRE Buffer=new LenFanlre();
		m_vLensFanare.push_back(Buffer);
		m_vLensFanare[i]->Cerent.x=64;
		m_vLensFanare[i]->Cerent.y=64;
		m_vLensFanare[i]->Cerent.z=0;
	}
	D3DXCreateTextureFromFile(m_pd3dDevice,"pic\\Sun\\Sun01.dds",&m_pTexture[0]);
	D3DXCreateTextureFromFile(m_pd3dDevice,"pic\\Sun\\Sun02.dds",&m_pTexture[1]);
	D3DXCreateTextureFromFile(m_pd3dDevice,"pic\\Sun\\Sun03.dds",&m_pTexture[2]);
	m_pTexture[3]=m_pTexture[0];
	m_pTexture[4]=m_pTexture[1];
	m_pTexture[5]=m_pTexture[2];
	D3DXCreateTextureFromFile(m_pd3dDevice,"pic\\Sun\\Sun.dds",&m_pTexture[6]);
	return true;
}

void CSun::Render()
{
	m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,false);//更新深度缓冲
	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);    //深度缓冲
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF,0x00000000);
	m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
	D3DXVECTOR3  Center = D3DXVECTOR3((FLOAT)(m_nWidth>>1),(FLOAT)(m_nHeight>>1),0);
	D3DXVECTOR3  PosBuffer = m_Pos;
	m_pSprite->Begin(NULL);
	m_pSprite->Draw(m_pTexture[6],NULL,&D3DXVECTOR3(128.0f,128.0f,0.0f),&m_Pos,0xFFFFFFFF);
	PosBuffer.z = 0;
	//设定世界矩阵
	D3DXMATRIX  matWorld;
	D3DXMatrixIdentity(&matWorld);
	m_pd3dDevice->SetTransform(D3DTS_WORLD,&matWorld);
	for (int i = 0; i != 6;++i)
	{
		D3DXMATRIX matChange,mat;
		PosBuffer.x = PosBuffer.x + (Center.x - m_Pos.x) / 3;
		PosBuffer.y = PosBuffer.y + (Center.y - m_Pos.y) / 3;
		m_pSprite->Draw(m_pTexture[i],NULL,&m_vLensFanare[i]->Cerent,&PosBuffer,m_Color);
	}
	m_pSprite->End();
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,true);//更新深度缓冲
	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, true);    //深度缓冲
}

void CSun::Transform3Dto2DSunPos()
{
	D3DXMATRIX   matView;
	D3DXMATRIX   matProj;
	D3DXMATRIX   matTrans;
	m_pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
	m_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);
	matTrans = matView * matProj;
	m_Pos = m_SunPos;
	D3DXVec3TransformCoord(&m_Pos,&m_SunPos,&matTrans);
	D3DXMATRIX  matViewPot;
	matViewPot._11 = (FLOAT)(m_nWidth>>1);
	matViewPot._12 = 0;
	matViewPot._13 = 0;
	matViewPot._14 = 0;

	matViewPot._21 = 0;
	matViewPot._22 = (FLOAT)((-m_nHeight)>>1);
	matViewPot._23 = 0;
	matViewPot._24 = 0;

	matViewPot._31 = 0;
	matViewPot._32 = 0;
	matViewPot._33 = 1;
	matViewPot._34 = 0;

	matViewPot._41 = (FLOAT)(m_nWidth>>1);
	matViewPot._42 = (FLOAT)(m_nHeight>>1);
	matViewPot._43 = 0;
	matViewPot._44 = 1;

	D3DXVec3TransformCoord(&m_Pos,&m_Pos,&matViewPot);
}

void CSun::UpDate()
{
	Transform3Dto2DSunPos();
	FLOAT AlphaX;
	if (m_Pos.x < 0)
	{
		AlphaX = -m_Pos.x;
	}
	else
	{
		if (m_Pos.x > m_nWidth)
		{
			AlphaX = m_Pos.x - m_nWidth;
		}
		else
		{
			AlphaX = 0;
		}
	}
	//Y轴和边界问题
	FLOAT AlphaY;
	if (m_Pos.y < 0)
	{
		AlphaY = -m_Pos.y;  
	} 
	else
	{
		if (m_Pos.y > m_nHeight)
		{
			AlphaY = m_Pos.y - m_nHeight;
		}
		else
		{
			AlphaY = 0;
		}
	}
	//取2者最大值
	m_Alpha = AlphaX > AlphaY ? AlphaX : AlphaY;
	if (m_Alpha > BORDE || m_Pos.z > 1.0f)
	{
		m_Alpha = 0;
		m_Color = D3DXCOLOR( 1.0f,1.0f,1.0f,m_Alpha);
		return ;
	}
	m_Alpha = 1.0f - (m_Alpha / BORDE);
	m_Color = D3DXCOLOR(1.0f,1.0f,1.0f,m_Alpha);
}

void CSun::CleanUp()
{
	for (int i = 0; i != 7; ++i)
	{
		m_pTexture[i]->Release();
	}
}
