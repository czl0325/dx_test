#pragma once
#include "Header.h"

#define BORDE 100.0f

typedef struct LenFanlre
{
	D3DXVECTOR3 Cerent;
	float Zoom;
	enum
	{
		FVF=D3DFVF_XYZ|D3DFVF_TEX1,
	};
}LENFANLRE,*LPLENFANLRE;

class CSun
{
private:
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	LPDIRECT3DTEXTURE9 m_pTexture[7];
	LPD3DXSPRITE m_pSprite;
	D3DXVECTOR3 m_SunPos,m_Pos;
	D3DXCOLOR m_Color;
	float m_Alpha;
	int m_nWidth,m_nHeight;
	vector<LPLENFANLRE> m_vLensFanare;
public:
	void Transform3Dto2DSunPos();
	bool InitSun(LPDIRECT3DDEVICE9 pDevice,float Width,float Height);
	void Render();
	void UpDate();
	void CleanUp();
	CSun(void);
	~CSun(void);
};
