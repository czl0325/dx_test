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
		float _x,_y,_z;//位置
		float fYaw;//雪花绕自身Y旋转的角度
		float fPitch;//雪花绕自身X旋转的角度
		float dSpeed;//雪花下降速度
		float rSpeed;//雪花旋转速度
		int TexIndex;//纹理
	};

	LPDIRECT3DDEVICE9 m_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DTEXTURE9 m_pTexture[3];
	SnowParticle m_Snow[3000];//雪花粒子数组

public:
	bool InitSnow(LPDIRECT3DDEVICE9 pd3dDevice);
	void Update(float fElapsedTime);
	void Render();
	void CleanUp();
	CSnow(void);
	~CSnow(void);
};
