#include "SkyBox.h"

CSkyBox::CSkyBox(LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;
	m_pVB        = NULL;
}

CSkyBox::~CSkyBox(void)
{
}

HRESULT CSkyBox::InitSkyBox(FLOAT Length, FLOAT Width, FLOAT Height, D3DXVECTOR3 Center)
{
	D3DXVECTOR3  ps;  //--盒子的顶点坐标
	m_Length   = Length;  //--长度
	m_Width    = Width;   //--宽度
	m_Height   = Height;  //--高度
	m_Center   = Center;  //--中心点坐标

	ps.x = m_Center.x - m_Width/2;
	ps.y = m_Center.y - m_Height/2;
	ps.z = m_Center.z - m_Length/2;

	SKYBOXVERTEX g_Vertices[] = 
	{
		//--Front
		{ ps.x,           ps.y,            ps.z + m_Length, 1.0, 1.0},
		{ ps.x,           ps.y + m_Height, ps.z + m_Length, 1.0, 0.0},
		{ ps.x + m_Width, ps.y,            ps.z + m_Length, 0.0, 1.0},
		{ ps.x + m_Width, ps.y + m_Height, ps.z + m_Length, 0.0, 0.0},
		//--Back
		{ ps.x + m_Width, ps.y,            ps.z,            1.0, 1.0},
		{ ps.x + m_Width, ps.y + m_Height, ps.z,            1.0, 0.0},
		{ ps.x,           ps.y,            ps.z,            0.0, 1.0},
		{ ps.x,           ps.x + m_Height, ps.z,            0.0, 0.0},
		//--Left
		{ ps.x,           ps.y,            ps.z,            1.0, 1.0},
		{ ps.x,           ps.y + m_Height, ps.z,            1.0, 0.0},
		{ ps.x,           ps.y,            ps.z + m_Length, 0.0, 1.0},
		{ ps.x,           ps.y + m_Height, ps.z + m_Length, 0.0, 0.0},
		//--Right
		{ ps.x + m_Width, ps.y,            ps.z + m_Length, 1.0, 1.0},
		{ ps.x + m_Width, ps.y + m_Height, ps.z + m_Length, 1.0, 0.0},
		{ ps.x + m_Width, ps.y,            ps.z,            0.0, 1.0},
		{ ps.x + m_Width, ps.y + m_Height, ps.z,            0.0, 0.0},
		//--Top
		{ ps.x,           ps.y + m_Height, ps.z + m_Length, 1.0, 1.0},
		{ ps.x,           ps.y + m_Height, ps.z,            1.0, 0.0},
		{ ps.x + m_Width, ps.y + m_Height, ps.z + m_Length, 0.0, 1.0},
		{ ps.x + m_Width, ps.y + m_Height, ps.z,            0.0, 0.0},

		//--Bottom
		{ ps.x,           ps.y,            ps.z,            1.0, 1.0},
		{ ps.x,           ps.y,            ps.z + m_Length, 1.0, 0.0},
		{ ps.x + m_Width, ps.y,            ps.z,            0.0, 1.0},
		{ ps.x + m_Width, ps.y,            ps.z + m_Length, 0.0, 0.0},
	};
	//--创建保存天空盒的顶点缓冲区
	if(FAILED(m_pd3dDevice->CreateVertexBuffer(24*sizeof(SKYBOXVERTEX),
		0,
		D3DFVF_SKYBOXVERTEX,
		D3DPOOL_DEFAULT,
		&m_pVB,NULL)))
	{
		return E_FAIL;
	}

	VOID* pVertices;
	if(FAILED(m_pVB->Lock(0,sizeof(g_Vertices),(void**)&pVertices,0)))
	{
		return E_FAIL;
	}
	memcpy(pVertices, g_Vertices, sizeof(g_Vertices));  //--拷贝数据
	m_pVB->Unlock();  //解锁

	SetTexture("pic//Back.bmp",   0);
	SetTexture("pic//Front.bmp",  1);
	SetTexture("pic//terr_dirt-grass.jpg", 2);
	SetTexture("pic//Top.bmp",    3);
	SetTexture("pic//Left.bmp",   4);
	SetTexture("pic//Right.bmp",  5);

	return S_OK;
}

BOOL CSkyBox::SetTexture(const char *TextureFilePath, int ps)
{
	if(FAILED(D3DXCreateTextureFromFile(m_pd3dDevice,
		TextureFilePath,&m_pTexture[ps])))
	{
		return false;
	}
	return true;
}

void CSkyBox::Render()
{
	//--物体沿X、Z轴平移
	m_pd3dDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	m_pd3dDevice->SetTransform(D3DTS_WORLD,&matWorld);
	/*MatricesTrans();*/
	m_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(SKYBOXVERTEX));
	m_pd3dDevice->SetFVF(D3DFVF_SKYBOXVERTEX);
	//--front
	m_pd3dDevice->SetTexture(0, m_pTexture[SKYBOX_FRONT]);
	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	//--back
	m_pd3dDevice->SetTexture(0, m_pTexture[SKYBOX_BACK]);
	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4 ,2);
	//--left
	m_pd3dDevice->SetTexture(0, m_pTexture[SKYBOX_LEFT]);
	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 8, 2);
	//--right
	m_pd3dDevice->SetTexture(0, m_pTexture[SKYBOX_RIGHT]);
	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 12, 2);
	//--top
	m_pd3dDevice->SetTexture(0, m_pTexture[SKYBOX_TOP]);
	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 16, 2);
	//--bottom
	m_pd3dDevice->SetTexture(0, m_pTexture[SKYBOX_BOTTOM]);
	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 20, 2);
}
