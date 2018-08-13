#pragma once
#include "Header.h"
#include "Config.h"

//太阳及光晕的顶点坐标系
struct VERTEX_LENSFLARE
{
	D3DXVECTOR3 pos;	//位置
	float		rhw;	//投影校正系数
	D3DCOLOR	color;	//颜色
	float		u, v;	//纹理坐标
};

#define D3DFVF_LENSFLARE (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

//太阳及光晕的纹理和相对位置
struct SunFlare 
{
	INT			NumofTex;//纹理(记录用的是哪一层纹理)
	float		fSize;	 //相对尺寸(光晕的大小,以屏幕中的百分比表示,1.0表示和屏幕一样宽）
	float		fLinePos;//各光晕在太阳和屏幕中心连线上的相对位置
	D3DXCOLOR	color;
};

class CSun2
{
private:
	LPDIRECT3DDEVICE9		m_pD3DDevice;
	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;

	LPDIRECT3DTEXTURE9		m_pSunTex[7];
	D3DXVECTOR3	m_SunPos;//太阳位置
	float		m_fIntensityBorder;//衰减区域的宽度
	float		m_fIntensity;//光的强度
	std::vector<SunFlare>	m_vSunFlare;
	int			m_nSunCoordX;//太阳的屏幕坐标
	int			m_nSunCoordY;
	int			m_nSunCoordW;

private:
	//增加太阳光晕
	void AddSpot(int nTexNum,float fSize,float fLinePos,D3DXCOLOR Color);
	//计算太阳的屏幕坐标
	void Transfrom3Dto2D();
	//计算光的亮度
	void CalSunIntensity();
	//计算太阳和各光的顶点
	void CreateVertices();

public:
	CSun2(LPDIRECT3DDEVICE9 pD3DDevice);
	~CSun2(void);
	bool InitSun();
	void UpdateSun();
	void RenderSun();
};
