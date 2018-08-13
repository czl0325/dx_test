#pragma once
#include "Header.h"
#include "Config.h"

//̫�������εĶ�������ϵ
struct VERTEX_LENSFLARE
{
	D3DXVECTOR3 pos;	//λ��
	float		rhw;	//ͶӰУ��ϵ��
	D3DCOLOR	color;	//��ɫ
	float		u, v;	//��������
};

#define D3DFVF_LENSFLARE (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

//̫�������ε���������λ��
struct SunFlare 
{
	INT			NumofTex;//����(��¼�õ�����һ������)
	float		fSize;	 //��Գߴ�(���εĴ�С,����Ļ�еİٷֱȱ�ʾ,1.0��ʾ����Ļһ����
	float		fLinePos;//��������̫������Ļ���������ϵ����λ��
	D3DXCOLOR	color;
};

class CSun2
{
private:
	LPDIRECT3DDEVICE9		m_pD3DDevice;
	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;

	LPDIRECT3DTEXTURE9		m_pSunTex[7];
	D3DXVECTOR3	m_SunPos;//̫��λ��
	float		m_fIntensityBorder;//˥������Ŀ��
	float		m_fIntensity;//���ǿ��
	std::vector<SunFlare>	m_vSunFlare;
	int			m_nSunCoordX;//̫������Ļ����
	int			m_nSunCoordY;
	int			m_nSunCoordW;

private:
	//����̫������
	void AddSpot(int nTexNum,float fSize,float fLinePos,D3DXCOLOR Color);
	//����̫������Ļ����
	void Transfrom3Dto2D();
	//����������
	void CalSunIntensity();
	//����̫���͸���Ķ���
	void CreateVertices();

public:
	CSun2(LPDIRECT3DDEVICE9 pD3DDevice);
	~CSun2(void);
	bool InitSun();
	void UpdateSun();
	void RenderSun();
};
