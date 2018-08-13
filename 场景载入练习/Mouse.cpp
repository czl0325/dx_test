#include "Mouse.h"

CMouse::CMouse(void)
{
	m_pTex = 0;
	m_Pos = D3DXVECTOR3(0.0f,0.0f,0.0f);
}

CMouse::~CMouse(void)
{
}

bool CMouse::InitMouse( LPDIRECT3DDEVICE9 pDevice)
{
	m_pDevice = pDevice;
	if(FAILED(D3DXCreateTextureFromFile(m_pDevice,"res\\Mouse\\cursor_m1.dds",&m_pTex)))
		return false;

	if(FAILED(D3DXCreateSprite(m_pDevice,&m_pStr)))
		return false;

	return true;
}

void CMouse::Render()
{
	m_pDevice ->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
	m_pDevice ->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_pDevice ->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	m_pStr->Begin(NULL);
	m_pStr->Draw(m_pTex,NULL,&D3DXVECTOR3(0.0f,0.0f,0.0f),&m_Pos,0xFFFFFFFF);
	m_pStr->End();

	m_pDevice ->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
}

void CMouse::Update()
{
	m_Pos.x = (float)(CInput::GetInstance().GetMousePosX());
	m_Pos.y = (float)(CInput::GetInstance().GetMousePosY());
}

void CMouse::CleanUp()
{
	if (m_pTex)
	{
		m_pTex->Release();
	}
}
