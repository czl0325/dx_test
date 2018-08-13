#include "D3DInit.h"
#include "Camera.h"
#include "GameFont.h"

CD3DInit::CD3DInit(void)
{
	g_pD3D = NULL;
	g_pd3dDevice = NULL;
	m_bSnow = false;
	m_bFog = false;
	m_bSkySphere = true;
	m_bDrawBoundingVolume = false;
}

CD3DInit::~CD3DInit(void)
{
	delete g_SkyBox;
	delete g_Terrain;
	delete g_Sun;
	delete g_Role;
	delete g_Snow;
	delete g_Snow1;
	delete g_SkySphere;
	delete g_Plant;
	delete g_Mouse;
	delete g_Monster;
}

HRESULT CD3DInit::InitD3D(HWND hWnd)
{
	if(NULL==(g_pD3D=Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;
	D3DDISPLAYMODE d3ddm;
	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddm)))
	{
		return E_FAIL;
	}
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));               //--把结构体清空
	d3dpp.Windowed         = true;                  //--指明不是窗口模式,而是全局独占模式
	d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;  //--设定换页效果为丢弃后台缓存
	d3dpp.BackBufferFormat = d3ddm.Format;           //--色彩深度为桌面的色彩深度
 	d3dpp.BackBufferWidth  = d3ddm.Width;            //--后缓冲宽度
 	d3dpp.BackBufferHeight = d3ddm.Height;           //--后缓冲高度
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT; //--屏幕刷新率
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_ONE; //--翻转模式
	d3dpp.EnableAutoDepthStencil     = true; //开启深度缓冲
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;//开启深度缓冲
	if(FAILED( g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,          //--使用当前显卡作注设备
		D3DDEVTYPE_HAL,             //--硬件抽象层设备
		hWnd,        
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, //--用软件的方法处理
		//--顶点数据参数若为D3DCREATE_HARDWARE_VERTEXPROCESSING则
		//--使用硬件
		&d3dpp,
		&g_pd3dDevice)))
	{
		return E_FAIL;
	}
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return S_OK;
}

void CD3DInit::Render()
{
	if(NULL == g_pd3dDevice)
		return;
	//--将填充后台缓冲区置为黑色
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255),1.0f,0);
	g_pd3dDevice->BeginScene(); //--开始通知显卡要进行渲染
	CGameTime::GetInstance().Update();
	ReadInput();
	CCamera::GetInstance()->Run(&CInput::GetInstance());

	g_SkyDome->UpdateSkyDome(CGameTime::GetInstance().GetElapse() * 1.0f);

	g_SkyDome->Render();

	g_Mouse->Update();
	g_Sun2->UpdateSun();
	g_Snow->Update(CGameTime::GetInstance().GetGameTime());
	g_Terrain->Update(CCamera::GetInstance()->GetCameraPos(),g_Role->m_vRole);
	g_Snow1->Update(g_Terrain);
	g_Butterfly->UpdateBufferfly(CGameTime::GetInstance().GetElapse(),g_Terrain);
	g_Grass->UpdateGrass(CGameTime::GetInstance().GetElapse());
	g_Water->Update(CGameTime::GetInstance().GetElapse() * 0.001f);


	//渲染地形
	g_Terrain->Render();

	//渲染太阳
	g_Sun2->RenderSun();

	for (int i = 0; i < g_Plant->m_vPlant.size(); i++)
	{
		g_Plant->m_vPlant[i]->Render(m_bDrawBoundingVolume);
	}

	//渲染场景内的物件
	g_SceneObject->RenderScene();

	if (m_bSnow == true)
	{
		g_Snow1->Render();
	}

	g_Butterfly->RenderBufferfly();
	g_Grass->RenderGrass();
	g_Water->Render(0.03f);
	g_Role->Render(m_bDrawBoundingVolume,0.03f,CCamera::GetInstance()->GetState());
//	g_Monster->Render(m_bDrawBoundingVolume);


	//if (m_bFog == true)
	//{
	//	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE,true);
	//	g_pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE,true);
	//}

	ShowFontInfo();

	for (int i=0;i<g_Plant->m_vPlant.size();i++)
	{
		if (CCollision::InterectTwoSphere(g_Plant->m_vPlant[i]->GetBoundingVolume()->Getboundingspehere(),
			g_Role->GetBoundingVolume()->Getboundingspehere()))
		{
			g_Role->m_vRole = g_Role->m_vOldPos;
		}
	}

	//--中间将来是整个图形的绘制部分
	g_pd3dDevice->EndScene();  //--结束图形的渲染
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);//--翻页
}

bool CD3DInit::GameInit(HWND hWnd,HINSTANCE hInstance)
{
	if(SUCCEEDED(this->InitD3D(hWnd)))
	{
		g_SkyBox    = new CSkyBox(g_pd3dDevice);
		g_Terrain   = new CTerrain();
		g_Sun       = new CSun();
		g_Sun2      = new CSun2(g_pd3dDevice);
		g_Role      = new CRole();
		g_Snow      = new CSnow();
		g_Snow1     = new CSnow1();
		g_SkySphere = new CSkySphere(g_pd3dDevice);
		g_Plant     = new CPlant();
		g_Mouse     = new CMouse();
		g_Monster   = new CMonster();
		g_SkyDome   = new CSkyDome(g_pd3dDevice);
		g_Butterfly = new CButterfly(g_pd3dDevice);
		g_Grass     = new CGrass();
		g_Water     = new CWater();
		g_SceneObject = new CSceneObject();



		/**初始化输入*/
		if(!CInput::GetInstance().InitDInput(hWnd,hInstance))
			return false;

		/**初始化鼠标*/
		if(!g_Mouse->InitMouse(g_pd3dDevice))
			return false;

		/**初始化天空盒*/
		if(FAILED(g_SkyBox->InitSkyBox(5000,5000,5000,D3DXVECTOR3(0,0,0))))
			return false;

		/**初始化天空球*/
		if(FAILED(g_SkySphere->SetSkyDomeTexture("pic//SkySphere.jpg")))
			return false;

		/**初始化天空球*/
		if(!g_SkyDome->InitSkyDome())
			return false;

		/**初始化太阳*/
		if(!g_Sun->InitSun(g_pd3dDevice,500,500))
			return false;

		/**初始化太阳*/
		if(!g_Sun2->InitSun())
			return false;

		/**初始化地形*/
		if(!g_Terrain->Initialize(g_pd3dDevice))
			return false;

		/**初始化字体*/
		if(!CGameFont::GetInstance().InitFont(g_pd3dDevice))
			return false;

		/**初始化人物*/
		if(!g_Role->OnCreate(g_pd3dDevice,g_Terrain,D3DXVECTOR3(0.0f,0.0f,0.0f),g_SceneObject))
			return false;

		/**初始化摄象机*/
		if(!CCamera::GetInstance()->InitCamera(g_pd3dDevice,g_Terrain,70.0f,g_Role,g_SceneObject,g_Plant))
			return false;

		/**初始化雪*/
		if(!g_Snow->InitSnow(g_pd3dDevice))
			return false;

		/**初始化雪*/
		if(!g_Snow1->InitParticle(g_pd3dDevice))
			return false;

		/**初始化静态物体*/
		if(!LoadObjectMeshList(g_pd3dDevice,g_Terrain))
			return false;

		/**初始化蝴蝶*/
		if(!g_Butterfly->InitBufferfly())
			return false;

		/**初始化草*/
		if(!g_Grass->InitGrass(g_pd3dDevice,g_Terrain))
			return false;

		/**初始化水*/
		if(!g_Water->InitWater(g_pd3dDevice,g_Role,g_Terrain))
			return false;

		/**初始化怪物*/
		if(!g_Monster->InitMonster(g_pd3dDevice,g_Terrain,g_Role))
			return false;

		/**初始化静态物件*/
		if(!g_SceneObject->InitScene(g_pd3dDevice,g_Terrain))
			return false;


		return true;

	}
	else
	{
		return false;
	}

	return true;
}

void CD3DInit::ReadInput()
{
	CInput::GetInstance().ReadKeyboard();
	CInput::GetInstance().ReadMouse();
}

void CD3DInit::KeyUp( WPARAM wParam )
{
	g_Role->KeyBoardUp(wParam);
}

void CD3DInit::KeyDown( WPARAM wParam )
{
	switch(wParam)
	{
	case VK_F1:
		g_Terrain->m_bWire = !g_Terrain->m_bWire;
		break;
	case VK_F2:
		m_bSnow = !m_bSnow;
		break;
	case VK_F3:
		break;
	case VK_F4:
		m_bFog = !m_bFog;
		break;
	case VK_F5:
		m_bDrawBoundingVolume = !m_bDrawBoundingVolume;
		break;
	}
}

bool CD3DInit::LoadObjectMeshList( LPDIRECT3DDEVICE9 pDevice,CTerrain* pTerrain )
{
	int m_nTreeNum = CConfig::GetInstance().ReadINI("Tree","TreeTotal","res//ini//StaticMeshObject.ini");
	for (int i = 0;i < m_nTreeNum;i++)
	{
		CStaticObject* m_pStaticObject;
		char cKeyName[256];
		sprintf(cKeyName,"Tree%d",i+1);
		char cFileName[256];
		CConfig::GetInstance().ReadINI("Tree",cKeyName,"NULL",cFileName,256,"res//ini//StaticMeshObject.ini");
		m_pStaticObject = new CStaticObject();
		m_pStaticObject->Init(g_pd3dDevice,cFileName);
		g_Plant->m_vecObjectMesh.push_back(m_pStaticObject);
		char cScale[256];
		sprintf(cScale,"Scale%d",i+1);
		char cPosition[256];
		sprintf(cPosition,"Position%d",i+1);
		char cPos_x[256];
		sprintf(cPos_x,"%s_x",cPosition);
		char cPos_z[256];
		sprintf(cPos_z,"%s_z",cPosition);
		D3DXVECTOR3 vPos;
		float fScale;
		vPos.x = CConfig::GetInstance().ReadINI("Tree",cPos_x,"res//ini//StaticMeshObject.ini");
		vPos.z = CConfig::GetInstance().ReadINI("Tree",cPos_z,"res//ini//StaticMeshObject.ini");
		vPos.y = pTerrain->GetHeightValue(vPos.x,vPos.z);
		fScale = CConfig::GetInstance().ReadINI("Tree",cScale,"res//ini//StaticMeshObject.ini");		
		CPlant* pPlant;
		pPlant = new CPlant();
		pPlant->Init(g_pd3dDevice,m_pStaticObject,fScale/100.0f,true,true,vPos,pTerrain);
		g_Plant->m_vPlant.push_back(pPlant);
	}

	return true;
}

void CD3DInit::ShowFontInfo()
{
	//显示帧数
	TCHAR buff[256];
	_stprintf_s(buff,256,"帧数:%d",CGameTime::GetInstance().GetFPS());
	CGameFont::GetInstance().Render(buff,0,0,0xffffff00);

	//显示渲染的人物高度
	_stprintf_s(buff,256,_T("人物坐标:%f,%f,%f"),g_Role->m_vRole.x,
		g_Role->m_vRole.y,
		g_Role->m_vRole.z);
	CGameFont::GetInstance().Render(buff,0,40,0xffffff00);
}