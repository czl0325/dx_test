#pragma once
#include "Header.h"


const int SKYBOX_BACK     = 0;    //--����
const int SKYBOX_FRONT    = 1;    //--ǰ��
const int SKYBOX_BOTTOM   = 2;    //--����
const int SKYBOX_TOP      = 3;    //--����
const int SKYBOX_LEFT     = 4;    //--����
const int SKYBOX_RIGHT    = 5;    //--����

struct SKYBOXVERTEX
{
	FLOAT x,y,z;
	FLOAT tu,tv;  //--��ʾ�������������
};


#define D3DFVF_SKYBOXVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

class CSkyBox
{
private:
	LPDIRECT3DDEVICE9       m_pd3dDevice;    //--D3D�豸
	LPDIRECT3DVERTEXBUFFER9 m_pVB;           //--���涥��Ļ�����

	FLOAT                   m_Length;        //--���ӵĳ���
	FLOAT                   m_Width;         //--���ӵĿ��
	FLOAT                   m_Height;        //--���ӵĸ߶�
	D3DXVECTOR3             m_Center;        //--���ӵ����ĵ�����
public:
	LPDIRECT3DTEXTURE9      m_pTexture[6];   //--������պ������������
	CSkyBox(LPDIRECT3DDEVICE9 pd3dDevice);
	virtual ~CSkyBox();
	HRESULT      InitSkyBox(FLOAT Length, FLOAT Width, FLOAT Height,
		D3DXVECTOR3 Center);//��ʼ����պ�
	//--��������
	BOOL         SetTexture(const char *TextureFilePath, int ps);
	void         Render();   //--��Ⱦ����
};
