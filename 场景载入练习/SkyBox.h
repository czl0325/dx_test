#pragma once
#include "Header.h"


const int SKYBOX_BACK     = 0;    //--后面
const int SKYBOX_FRONT    = 1;    //--前面
const int SKYBOX_BOTTOM   = 2;    //--下面
const int SKYBOX_TOP      = 3;    //--上面
const int SKYBOX_LEFT     = 4;    //--左面
const int SKYBOX_RIGHT    = 5;    //--右面

struct SKYBOXVERTEX
{
	FLOAT x,y,z;
	FLOAT tu,tv;  //--表示顶点的纹理坐标
};


#define D3DFVF_SKYBOXVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

class CSkyBox
{
private:
	LPDIRECT3DDEVICE9       m_pd3dDevice;    //--D3D设备
	LPDIRECT3DVERTEXBUFFER9 m_pVB;           //--保存顶点的缓冲区

	FLOAT                   m_Length;        //--盒子的长度
	FLOAT                   m_Width;         //--盒子的宽度
	FLOAT                   m_Height;        //--盒子的高度
	D3DXVECTOR3             m_Center;        //--盒子的中心点坐标
public:
	LPDIRECT3DTEXTURE9      m_pTexture[6];   //--保存天空盒六个面的纹理
	CSkyBox(LPDIRECT3DDEVICE9 pd3dDevice);
	virtual ~CSkyBox();
	HRESULT      InitSkyBox(FLOAT Length, FLOAT Width, FLOAT Height,
		D3DXVECTOR3 Center);//初始化天空盒
	//--设置纹理
	BOOL         SetTexture(const char *TextureFilePath, int ps);
	void         Render();   //--渲染场景
};
