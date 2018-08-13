#include "Snow.h"

CSnow::CSnow(void)
{
}

CSnow::~CSnow(void)
{
	CleanUp();
}

bool CSnow::InitSnow( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	if(FAILED(m_pd3dDevice->CreateVertexBuffer(4*sizeof(CustomVretex),
		D3DUSAGE_WRITEONLY, CustomVretex::FVF, D3DPOOL_MANAGED, &m_pVB, NULL)))
		return false;
	
	CustomVretex *pVretices;
	m_pVB->Lock(0,0,(void**)&pVretices,0);
	pVretices[0] = CustomVretex(-1.0f,0.0f,0.0f,0.0f,1.0f);
	pVretices[1] = CustomVretex(-1.0f,5.0f,0.0f,0.0f,0.0f);
	pVretices[2] = CustomVretex(1.0f,5.0f,0.0f,1.0f,0.0f);
	pVretices[3] = CustomVretex(1.0f,0.0f,0.0f,1.0f,1.0f);
	m_pVB->Unlock();

	//创建雪花的纹理
	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice,"res//Snow//snow1.bmp",&m_pTexture[0])))
		return false;
	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice,"res//Snow//snow2.bmp",&m_pTexture[1])))
		return false;
	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice,"res//Snow//snow3.bmp",&m_pTexture[2])))
		return false;

	srand(GetTickCount());
	for (int i=0;i<3000;i++)
	{
		m_Snow[i]._x = float(rand()%2600-1300);
		m_Snow[i]._y = 100.0f;
		m_Snow[i]._z = float(rand()%2600-1300);

		m_Snow[i].fPitch = 2 * D3DX_PI;//雪花绕自身X旋转的角度
		m_Snow[i].fYaw = 2 * D3DX_PI;//雪花绕自身Y旋转的角度
		m_Snow[i].dSpeed = 10.0f;//雪花下降速度
		m_Snow[i].rSpeed = 1.0f;//雪花旋转速度
		m_Snow[i].TexIndex = rand()%3;//纹理
	}

	return true;
}

void CSnow::Render()
{
	//设置纹理状态
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);

	m_pd3dDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);

	//设置Alpha混合系数
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);

	//不剔除任何面
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	for (int i=0;i<3000;i++)
	{
		D3DXMATRIXA16 matYaw,matPitch,matTrans,	matSnowWorld;

		D3DXMatrixIdentity(&matSnowWorld);
		D3DXMatrixRotationY(&matYaw,m_Snow[i].fYaw);
		D3DXMatrixRotationX(&matPitch,m_Snow[i].fPitch);
		D3DXMatrixTranslation(&matTrans,m_Snow[i]._x,m_Snow[i]._y,m_Snow[i]._z);

		matSnowWorld = matYaw * matPitch * matTrans;
		m_pd3dDevice->SetTransform(D3DTS_WORLD,&matSnowWorld);
		
		//渲染当前粒子
		m_pd3dDevice->SetTexture(0,m_pTexture[m_Snow[i].TexIndex]);
		m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
		m_pd3dDevice->SetStreamSource(0,m_pVB,0,sizeof(CustomVretex));
		m_pd3dDevice->SetFVF(CustomVretex::FVF);
		m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);

	}

	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);//剔除背面
}

void CSnow::Update( float fElapsedTime )
{
	for (int i=0;i<3000;i++)
	{
		m_Snow[i]._y -= m_Snow[i].dSpeed * fElapsedTime;
		if (m_Snow[i]._y < 0)
		{
			m_Snow[i]._x = float(rand()%2600-1300);
			m_Snow[i]._y = 100.0f;
			m_Snow[i]._z = float(rand()%2600-1300);
		}
		m_Snow[i].fPitch += m_Snow[i].rSpeed * fElapsedTime;
		m_Snow[i].fYaw += m_Snow[i].rSpeed * fElapsedTime;
	}
}

void CSnow::CleanUp()
{
	for (int i=0;i<3;i++)
	{
		if (m_pTexture[i])
		{
			m_pTexture[i]->Release();
		}
	}
}