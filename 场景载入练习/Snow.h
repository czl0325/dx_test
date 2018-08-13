#pragma once
#include "Header.h"
class CSnow
{
private:
	struct CustomVretex
	{
		float _x, _y, _z;
		float _u, _v;
		CustomVretex(float x,float y,float z,float u,float v)
		{
			_x = x; _y = y; _z = z; _u = u; _v = v;
		}
		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1 ; 
	};

	struct SnowParticle
	{
		float _x,_y,_z;//λ��
		float fYaw;//ѩ��������Y��ת�ĽǶ�
		float fPitch;//ѩ��������X��ת�ĽǶ�
		float dSpeed;//ѩ���½��ٶ�
		float rSpeed;//ѩ����ת�ٶ�
		int TexIndex;//����
	};

	LPDIRECT3DDEVICE9 m_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DTEXTURE9 m_pTexture[3];
	SnowParticle m_Snow[3000];//ѩ����������

public:
	bool InitSnow(LPDIRECT3DDEVICE9 pd3dDevice);
	void Update(float fElapsedTime);
	void Render();
	void CleanUp();
	CSnow(void);
	~CSnow(void);
};
