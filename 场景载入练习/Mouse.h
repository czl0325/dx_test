#pragma once
#include "Header.h"
#include "Input.h"

class CMouse
{
private:
	LPDIRECT3DDEVICE9  m_pDevice;
	LPDIRECT3DTEXTURE9 m_pTex;
	LPD3DXSPRITE       m_pStr;

	D3DXVECTOR3        m_Pos;
public:
	bool InitMouse(LPDIRECT3DDEVICE9 pDevice);
	void Render();
	void Update();
	void CleanUp();
	CMouse(void);
	~CMouse(void);
};
