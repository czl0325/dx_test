#include "Sun2.h"

CSun2::CSun2(LPDIRECT3DDEVICE9 pD3DDevice)
{
	m_pD3DDevice	=   pD3DDevice;
	m_pVertexBuffer	=	NULL;
	for (int i = 0;i < 7; i++)
	{
		m_pSunTex[i] = NULL;
	}
	
}

CSun2::~CSun2(void)
{
	m_vSunFlare.clear();
	SAFE_RELEASE(m_pVertexBuffer);
	for (int i = 0;i < 7; i++)
	{
		SAFE_RELEASE(m_pSunTex[i]);
	}
}

//增加太阳光晕
void CSun2::AddSpot( int nTexNum,float fSize,float fLinePos,D3DXCOLOR Color )
{
	SunFlare TempSunFlare;
	TempSunFlare.NumofTex	= nTexNum;
	TempSunFlare.fSize		= fSize;
	TempSunFlare.fLinePos	= fLinePos;
	TempSunFlare.color		= Color;
	m_vSunFlare.push_back(TempSunFlare);
}

void CSun2::Transfrom3Dto2D()
{
	D3DXMATRIX matView, matConcat, matProj, matViewport;
	D3DXVECTOR4 vResult;

	//视区变换
	matViewport = D3DXMATRIX
		(
		static_cast<float>(WINDOW_WIDTH/2), 0, 0, 0,
		0, static_cast<float>(-WINDOW_HEIGHT/2), 0, 0,
		0, 0, 1, 0,
		static_cast<float>(WINDOW_WIDTH/2), static_cast<float>(WINDOW_HEIGHT/2), 0, 1
		);

	D3DXMatrixIdentity(&matConcat); 
	m_pD3DDevice->GetTransform(D3DTS_VIEW,&matView);
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION,&matProj);

	matConcat *= matView;
	matConcat *= matProj;
	matConcat *= matViewport;

	D3DXVec3Transform(&vResult, &m_SunPos, &matConcat);

	//太阳的屏幕坐标
	m_nSunCoordX = static_cast<int>(vResult.x/vResult.w);
	m_nSunCoordY = static_cast<int>(vResult.y/vResult.w);
	m_nSunCoordW = static_cast<int>(vResult.w);
}

//计算光的亮度
void CSun2::CalSunIntensity()
{
	//------------------------------------------------------
	//根据给定强度和太阳大屏幕边缘的距离计算真实强度t
	//原理可以参照《地形制作全攻略》文档中的光晕章节
	//------------------------------------------------------
	int nAwayX,nAwayY;
	//计算X方向和边界的距离
	if (m_nSunCoordX < 0) nAwayX = -m_nSunCoordX;
	else
	{
		if (m_nSunCoordX > WINDOW_WIDTH) nAwayX = m_nSunCoordX - WINDOW_WIDTH;
		else nAwayX = 0;
	}
	//计算Y方向和边界的距离
	if (m_nSunCoordY < 0) nAwayY = -m_nSunCoordY;
	else 
	{
		if(m_nSunCoordY > WINDOW_HEIGHT) nAwayY = m_nSunCoordY - WINDOW_HEIGHT;
		else nAwayY = 0;
	}

	//取两者最大的
	float fAway = static_cast<float>((nAwayX > nAwayY) ? nAwayX : nAwayY);

	if (fAway > m_fIntensityBorder) fAway = m_fIntensityBorder;

	//计算出光的强度---fAway越大,光的亮度越低
	m_fIntensity = 1.0f - (fAway / m_fIntensityBorder);
}

//计算太阳和各光的顶点
void CSun2::CreateVertices()
{
	//算出窗口中心点坐标
	int iCenterOfScreenX = WINDOW_WIDTH/2;
	int iCenterOfScreenY = WINDOW_HEIGHT/2;

	//中心点和太阳的距离
	int iDistanceX = iCenterOfScreenX - m_nSunCoordX;
	int iDistanceY = iCenterOfScreenY - m_nSunCoordY;


	VERTEX_LENSFLARE *pVertices;
	int iVBSize=m_vSunFlare.size();
	if(FAILED(m_pVertexBuffer->Lock( 0, iVBSize*6*sizeof(VERTEX_LENSFLARE), (VOID**)&pVertices, 0)))
		return;

	for (std::vector<SunFlare>::iterator i = m_vSunFlare.begin(); i != m_vSunFlare.end(); i++) 
	{
		SunFlare &spot = (*i);

		// 计算光斑中心的位置
		float iSpotCenterPosX = iCenterOfScreenX - ((float)iDistanceX * spot.fLinePos);
		float iSpotCenterPosY = iCenterOfScreenY - ((float)iDistanceY * spot.fLinePos);
		// 计算光斑的半径
		float iSizeDiv2 = WINDOW_WIDTH * spot.fSize/2.0f;

		//通过光的亮度系数和alpha值来控制光的强度
		D3DXCOLOR color = spot.color;
		color.a *= m_fIntensity;

		//0-1
		//|\|
		//2-3
		//0-1-3
		pVertices->color = color;
		pVertices->pos = D3DXVECTOR3(iSpotCenterPosX-iSizeDiv2, iSpotCenterPosY-iSizeDiv2, 0);
		pVertices->u = 0.0f; pVertices->v = 0.0f; pVertices->rhw = 1.0f;
		pVertices++;

		pVertices->color = color;
		pVertices->pos = D3DXVECTOR3(iSpotCenterPosX+iSizeDiv2, iSpotCenterPosY-iSizeDiv2, 0);
		pVertices->u = 1.0f; pVertices->v = 0.0f; pVertices->rhw = 1.0f;
		pVertices++;

		pVertices->color = color;
		pVertices->pos = D3DXVECTOR3(iSpotCenterPosX+iSizeDiv2, iSpotCenterPosY+iSizeDiv2, 0);
		pVertices->u = 1.0f; pVertices->v = 1.0f; pVertices->rhw = 1.0f;
		pVertices++;

		//0-3-2
		pVertices->color = color;
		pVertices->pos = D3DXVECTOR3(iSpotCenterPosX-iSizeDiv2, iSpotCenterPosY-iSizeDiv2, 0);
		pVertices->u = 0.0f; pVertices->v = 0.0f; pVertices->rhw = 1.0f;
		pVertices++;

		pVertices->color = color;
		pVertices->pos = D3DXVECTOR3(iSpotCenterPosX+iSizeDiv2, iSpotCenterPosY+iSizeDiv2, 0);
		pVertices->u = 1.0f; pVertices->v = 1.0f; pVertices->rhw = 1.0f;
		pVertices++;

		pVertices->color = color;
		pVertices->pos = D3DXVECTOR3(iSpotCenterPosX-iSizeDiv2, iSpotCenterPosY+iSizeDiv2, 0);
		pVertices->u = 0.0f; pVertices->v = 1.0f; pVertices->rhw = 1.0f;
		pVertices++;
	} 
	m_pVertexBuffer->Unlock();
}

bool CSun2::InitSun()
{
	CConfig::GetInstance().SetConfigFileName("res//ini//Sun.ini");
	//设置太阳的位置及衰减区域
	m_SunPos.x	= CConfig::GetInstance().ReadINI("Sun","SunPosX");
	m_SunPos.y	= CConfig::GetInstance().ReadINI("Sun","SunPosY");
	m_SunPos.z	= CConfig::GetInstance().ReadINI("Sun","SunPosZ");
	m_fIntensityBorder = CConfig::GetInstance().ReadINI("Sun","IntensityBorder");

	//创建太阳的顶点缓冲
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(6*13*sizeof(VERTEX_LENSFLARE),
		D3DUSAGE_WRITEONLY,D3DFVF_LENSFLARE,D3DPOOL_MANAGED,
		&m_pVertexBuffer,NULL)))
	{
		MessageBox(NULL, "创建太阳顶点缓冲失败!", "SunBuffer", MB_OK);
		return false;
	}

	//载入太阳及光晕的纹理
	TCHAR SunTexPathName[MAX_PATH];
	TCHAR Temp[20];
	for (int i = 0; i < 7; i++)
	{
		_stprintf_s(Temp,20,"SunFileName%d",i);
		CConfig::GetInstance().ReadINI("Sun",Temp,SunTexPathName,MAX_PATH);

		if(FAILED(D3DXCreateTextureFromFile(m_pD3DDevice,SunTexPathName,&m_pSunTex[i])))
		{
			MessageBox(NULL, "创建太阳纹理失败!", "Texture.exe", MB_OK);
			return false;
		}
	}

	//增加太阳及光晕
	AddSpot(0, 0.50f,  1.0f, D3DXCOLOR(1.0f, 1.0f, 0.5f, 1.0f));
	AddSpot(2, 0.12f,  0.8f, D3DXCOLOR(0.7f, 0.5f, 0.0f, 0.3f));
	AddSpot(5, 0.08f,  0.7f, D3DXCOLOR(0.8f, 0.7f, 0.0f, 0.3f));
	AddSpot(1, 0.08f,  0.6f, D3DXCOLOR(0.8f, 1.0f, 0.0f, 0.5f));
	AddSpot(3, 0.11f,  0.5f, D3DXCOLOR(0.8f, 1.0f, 0.0f, 0.3f));
	AddSpot(1, 0.08f,  0.4f, D3DXCOLOR(0.8f, 0.5f, 0.0f, 1.0f));
	AddSpot(6, 0.08f,  0.1f, D3DXCOLOR(0.8f, 1.0f, 0.5f, 0.5f));
	AddSpot(1, 0.08f, -0.2f, D3DXCOLOR(0.8f, 0.5f, 0.0f, 1.0f));
	AddSpot(2, 0.08f, -0.3f, D3DXCOLOR(0.8f, 1.0f, 0.6f, 1.0f));
	AddSpot(1, 0.13f, -0.4f, D3DXCOLOR(0.8f, 0.7f, 0.0f, 0.3f));
	AddSpot(3, 0.13f, -0.7f, D3DXCOLOR(0.8f, 0.5f, 0.0f, 0.2f));
	AddSpot(4, 0.20f, -1.0f, D3DXCOLOR(0.8f, 0.7f, 0.0f, 0.2f));
	AddSpot(3, 0.30f, -1.3f, D3DXCOLOR(0.5f, 0.5f, 0.0f, 0.4f));

	return true;
}

void CSun2::UpdateSun()
{
	Transfrom3Dto2D();
	if (m_nSunCoordW >= 0)
	{
		CalSunIntensity();
		CreateVertices();
	}
}

void CSun2::RenderSun()
{
	if (m_nSunCoordW >= 0)
	{
		m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		// 启用加法alpha混合
		m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);

		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);

		m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, false);
		m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);

		m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(VERTEX_LENSFLARE));
		m_pD3DDevice->SetFVF(D3DFVF_LENSFLARE);

		for (int i=0; i < m_vSunFlare.size(); i++) 
		{
			m_pD3DDevice->SetTexture(0, m_pSunTex[m_vSunFlare[i].NumofTex]);				
			m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, i*6, 2);
		}
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		m_pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false);
		m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, true);
		m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	}
}