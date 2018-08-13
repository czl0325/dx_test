#pragma once
#include "Header.h"
#include "GameTime.h"

class CStartUI
{
private:
	LPDIRECT3DDEVICE9 m_pDevice;
	LPDIRECT3DTEXTURE9 m_pTex;
	LPD3DXSPRITE m_pSpr;

	D3DXCOLOR m_Color;
	int m_Alpha;
	int m_Time;
public:
	CStartUI(void);
	~CStartUI(void);
	bool Initialize(LPDIRECT3DDEVICE9 pDevice,LPD3DXSPRITE pStr);
	void Update(float fTime);
	void Render();
	void CleanUp();
};
