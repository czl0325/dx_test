#include "StartUI.h"

CStartUI::CStartUI(void)
{
	m_pDevice = NULL;
	m_pTex = NULL;
	m_pSpr = NULL;
	m_Time = 0;
	m_Alpha = 255;
	m_Color = D3DCOLOR_ARGB(255,255,255,m_Alpha);
}

CStartUI::~CStartUI(void)
{
	CleanUp();
}

bool CStartUI::Initialize( LPDIRECT3DDEVICE9 pDevice,LPD3DXSPRITE pStr )
{
	m_pDevice = pDevice;
	m_pSpr = pStr;
	if (FAILED(D3DXCreateTextureFromFile(m_pDevice,"res//UI//LG_Loading_1024.dds",&m_pTex)))
		return false;

	return true;
}

void CStartUI::Update(float fTime)
{
	m_Time += fTime;
	if (m_Time < 120)
	{
		return;
	}
	m_Time = 0;
	m_Alpha -= 10;
	if (m_Alpha <= 0)
	{
		m_Alpha = 0;
	}
	m_Color = D3DCOLOR_ARGB(255,255,255,m_Alpha);
}

void CStartUI::Render()
{
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	m_pSpr->Begin(NULL);
	m_pSpr->Draw(m_pTex,NULL,NULL,&D3DXVECTOR3(0.0f,0.0f,0.0f),m_Color);
	m_pSpr->End();

	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
}

void CStartUI::CleanUp()
{
	SAFE_RELEASE(m_pTex);
}