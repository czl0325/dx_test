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
	ZeroMemory(&d3dpp, sizeof(d3dpp));               //--�ѽṹ�����
	d3dpp.Windowed         = true;                  //--ָ�����Ǵ���ģʽ,����ȫ�ֶ�ռģʽ
	d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;  //--�趨��ҳЧ��Ϊ������̨����
	d3dpp.BackBufferFormat = d3ddm.Format;           //--ɫ�����Ϊ�����ɫ�����
 	d3dpp.BackBufferWidth  = d3ddm.Width;            //--�󻺳���
 	d3dpp.BackBufferHeight = d3ddm.Height;           //--�󻺳�߶�
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT; //--��Ļˢ����
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_ONE; //--��תģʽ
	d3dpp.EnableAutoDepthStencil     = true; //������Ȼ���
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;//������Ȼ���
	if(FAILED( g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,          //--ʹ�õ�ǰ�Կ���ע�豸
		D3DDEVTYPE_HAL,             //--Ӳ��������豸
		hWnd,        
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, //--������ķ�������
		//--�������ݲ�����ΪD3DCREATE_HARDWARE_VERTEXPROCESSING��
		//--ʹ��Ӳ��
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
	//--������̨��������Ϊ��ɫ
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255),1.0f,0);
	g_pd3dDevice->BeginScene(); //--��ʼ֪ͨ�Կ�Ҫ������Ⱦ
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


	//��Ⱦ����
	g_Terrain->Render();

	//��Ⱦ̫��
	g_Sun2->RenderSun();

	for (int i = 0; i < g_Plant->m_vPlant.size(); i++)
	{
		g_Plant->m_vPlant[i]->Render(m_bDrawBoundingVolume);
	}

	//��Ⱦ�����ڵ����
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

	//--�м佫��������ͼ�εĻ��Ʋ���
	g_pd3dDevice->EndScene();  //--����ͼ�ε���Ⱦ
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);//--��ҳ
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



		/**��ʼ������*/
		if(!CInput::GetInstance().InitDInput(hWnd,hInstance))
			return false;

		/**��ʼ�����*/
		if(!g_Mouse->InitMouse(g_pd3dDevice))
			return false;

		/**��ʼ����պ�*/
		if(FAILED(g_SkyBox->InitSkyBox(5000,5000,5000,D3DXVECTOR3(0,0,0))))
			return false;

		/**��ʼ�������*/
		if(FAILED(g_SkySphere->SetSkyDomeTexture("pic//SkySphere.jpg")))
			return false;

		/**��ʼ�������*/
		if(!g_SkyDome->InitSkyDome())
			return false;

		/**��ʼ��̫��*/
		if(!g_Sun->InitSun(g_pd3dDevice,500,500))
			return false;

		/**��ʼ��̫��*/
		if(!g_Sun2->InitSun())
			return false;

		/**��ʼ������*/
		if(!g_Terrain->Initialize(g_pd3dDevice))
			return false;

		/**��ʼ������*/
		if(!CGameFont::GetInstance().InitFont(g_pd3dDevice))
			return false;

		/**��ʼ������*/
		if(!g_Role->OnCreate(g_pd3dDevice,g_Terrain,D3DXVECTOR3(0.0f,0.0f,0.0f),g_SceneObject))
			return false;

		/**��ʼ�������*/
		if(!CCamera::GetInstance()->InitCamera(g_pd3dDevice,g_Terrain,70.0f,g_Role,g_SceneObject,g_Plant))
			return false;

		/**��ʼ��ѩ*/
		if(!g_Snow->InitSnow(g_pd3dDevice))
			return false;

		/**��ʼ��ѩ*/
		if(!g_Snow1->InitParticle(g_pd3dDevice))
			return false;

		/**��ʼ����̬����*/
		if(!LoadObjectMeshList(g_pd3dDevice,g_Terrain))
			return false;

		/**��ʼ������*/
		if(!g_Butterfly->InitBufferfly())
			return false;

		/**��ʼ����*/
		if(!g_Grass->InitGrass(g_pd3dDevice,g_Terrain))
			return false;

		/**��ʼ��ˮ*/
		if(!g_Water->InitWater(g_pd3dDevice,g_Role,g_Terrain))
			return false;

		/**��ʼ������*/
		if(!g_Monster->InitMonster(g_pd3dDevice,g_Terrain,g_Role))
			return false;

		/**��ʼ����̬���*/
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
	//��ʾ֡��
	TCHAR buff[256];
	_stprintf_s(buff,256,"֡��:%d",CGameTime::GetInstance().GetFPS());
	CGameFont::GetInstance().Render(buff,0,0,0xffffff00);

	//��ʾ��Ⱦ������߶�
	_stprintf_s(buff,256,_T("��������:%f,%f,%f"),g_Role->m_vRole.x,
		g_Role->m_vRole.y,
		g_Role->m_vRole.z);
	CGameFont::GetInstance().Render(buff,0,40,0xffffff00);
}