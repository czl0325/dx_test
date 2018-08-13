#include "Butterfly.h"

CButterfly::CButterfly(LPDIRECT3DDEVICE9 pD3DDevice)
{
	m_pD3DDevice = pD3DDevice;
}

CButterfly::~CButterfly(void)
{
	SAFE_DELETE_ARRAY(m_ButterflyColony);
	SAFE_RELEASE(m_ButterflyTexture);
	SAFE_RELEASE(m_ButterflyEffect);
	SAFE_RELEASE(m_pVertexBuffer);
}

//在两个值之间取随即值
float CButterfly::GetRandFloat( float fMin,float fMax )
{
	if (fMin >= fMax)
	{
		return fMin;
	}
	// get random float in [0, 1] interval
	float f = (rand() % 10000) * 0.0001f; 

	// return float in [lowBound, highBound] interval. 
	return (f * (fMax - fMin)) + fMin;
}

//控制太近的蝴蝶都为不可见----用box
bool CButterfly::CheckPosInBoudingBox( ViewBox Box,D3DXVECTOR3 Pos )
{
	if (Pos.x > Box.s_Max.x ||
		Pos.y > Box.s_Max.y ||
		Pos.z > Box.s_Max.z ||
		Pos.x < Box.s_Min.x ||
		Pos.y < Box.s_Min.y ||
		Pos.z < Box.s_Min.z)
	{
		return false;
	}

	return true;
}

//控制太远和太近的蝴蝶都为不可见----用sphere
bool CButterfly::CheckPosInInSphere( D3DXVECTOR3 View,D3DXVECTOR3 Pos )
{
	float flen = D3DXVec3Length(&(Pos - View));

	//控制太远和太近的蝴蝶都为不可见---蝴蝶太靠近的话会影响摄像机的视觉
	if ( flen <= 150.0f || flen >= 1000.0f )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//初始化蝴蝶
bool CButterfly::InitBufferfly()
{
	CConfig::GetInstance().SetConfigFileName("res//ini//Butterfly.ini");

	TCHAR FileName[MAX_PATH];
	CConfig::GetInstance().ReadINI("Butterfly","ButterflyEffect",FileName,MAX_PATH);

	//创建效果对象
	HRESULT hr = NULL;
	LPD3DXBUFFER n_Error = NULL;
	hr = D3DXCreateEffectFromFileA(	m_pD3DDevice,
		FileName,
		NULL,
		NULL,
		D3DXSHADER_DEBUG,
		NULL,
		&m_ButterflyEffect,
		&n_Error );
	if (n_Error)
	{
		MessageBox(NULL,(char*)n_Error->GetBufferPointer(),"Error!",MB_OK);
		SAFE_RELEASE(n_Error);
		return false;
	}

	if (FAILED(hr))
	{
		MessageBox(NULL,"创建Bufferfly效果失败!","Error!",MB_OK);
		return false;
	}

	//获取蝴蝶的总数量
	m_NumButterfly = CConfig::GetInstance().ReadINI("Butterfly","Total");

	if (FAILED(m_pD3DDevice->CreateVertexBuffer(4*m_NumButterfly*sizeof(BUTTERFLY_VERTEX),
		D3DUSAGE_WRITEONLY,
		D3DFVF_BUTTERFLY,
		D3DPOOL_MANAGED,
		&m_pVertexBuffer,
		NULL)))
	{	
		MessageBox(NULL,"Create Butterfly VertexBuffer Failed!","Error!",MB_OK);
		return false;
	}

	//初始化蝴蝶状态
	BUTTERFLY_VERTEX* pVertex = NULL;
	m_ButterflyColony =new Butterfly[m_NumButterfly];
	if (m_ButterflyColony == NULL) return false;
	m_pVertexBuffer->Lock(0, 4*m_NumButterfly*sizeof(BUTTERFLY_VERTEX), (void**)&pVertex, 0);
	//m_pVertexBuffer->Lock(0,4*m_NumButterfly*sizeof(BUTTERFLY_VERTEX),(VOID**)&pVertex,D3DLOCK_NOOVERWRITE|D3DLOCK_DONOTWAIT);

	for (INT i=0; i<m_NumButterfly; i++)
	{
		m_ButterflyColony[i].Pos.x = GetRandFloat(-1000,1000);
		m_ButterflyColony[i].Pos.z = GetRandFloat(-1000,1000);
		m_ButterflyColony[i].Pos.y = GetRandFloat(20,60);
		m_ButterflyColony[i].nAngle = (int)GetRandFloat(-180,180);
		m_ButterflyColony[i].fWingSpeed	= GetRandFloat(1.0f,4.0f);
		m_ButterflyColony[i].fFlyHeight	= GetRandFloat(30,200);
		m_ButterflyColony[i].fWingAngle	= GetRandFloat(5,85);
		m_ButterflyColony[i].fSize		= (FLOAT)(rand()%5+10);
		m_ButterflyColony[i].bVisible	= true;

		//根据大小对蝴蝶进行缩放
		D3DXMATRIX n_Temp;
		D3DXMatrixIdentity(&m_ButterflyColony[i].matZoomRot);
		D3DXMatrixIdentity(&n_Temp);
		D3DXMatrixScaling(  &n_Temp,
			m_ButterflyColony[i].fSize,
			m_ButterflyColony[i].fSize,
			m_ButterflyColony[i].fSize);
		D3DXMatrixMultiply( &m_ButterflyColony[i].matZoomRot,
			&m_ButterflyColony[i].matZoomRot,
			&n_Temp);

		//根据角度对蝴蝶进行旋转
		D3DXMatrixIdentity(&n_Temp);
		D3DXMatrixRotationY(&n_Temp,m_ButterflyColony[i].nAngle*D3DX_PI/180);
		D3DXMatrixMultiply( &m_ButterflyColony[i].matZoomRot,
			&m_ButterflyColony[i].matZoomRot,
			&n_Temp);

		//根据位置对蝴蝶进行移动
		D3DXMatrixIdentity(&m_ButterflyColony[i].matWorld);
		D3DXMatrixTranslation(	&m_ButterflyColony[i].matWorld,
			m_ButterflyColony[i].Pos.x,
			m_ButterflyColony[i].Pos.y,
			m_ButterflyColony[i].Pos.z);

		//获取蝴蝶的世界矩阵
		D3DXMatrixMultiply(&m_ButterflyColony[i].matWorld,&m_ButterflyColony[i].matZoomRot,&m_ButterflyColony[i].matWorld);

		m_ButterflyColony[i].bVisible = TRUE;
		float fTempAngle = m_ButterflyColony[i].fWingAngle*D3DX_PI/180;

		//从大的蝴蝶纹理中随即取一小的蝴蝶纹理
		D3DXVECTOR2 n_TexCoord=D3DXVECTOR2((FLOAT)(rand()%4)/4,(FLOAT)(rand()%4)/4);

		//求出4个角对应的纹理坐标
		pVertex[4*i].Tex	= D3DXVECTOR2(n_TexCoord.x,n_TexCoord.y);
		pVertex[4*i+1].Tex	= D3DXVECTOR2(n_TexCoord.x+0.25f,n_TexCoord.y);
		pVertex[4*i+2].Tex	= D3DXVECTOR2(n_TexCoord.x,n_TexCoord.y+0.25f);
		pVertex[4*i+3].Tex	= D3DXVECTOR2(n_TexCoord.x+0.25f,n_TexCoord.y+0.25f);

		//通过坐标设置使纹理产生折叠---沿着z轴折叠蝴蝶
		FLOAT X=cos(fTempAngle);
		FLOAT Y=sin(fTempAngle);
		D3DXVec3TransformCoord(&pVertex[4*i].Pos,  &D3DXVECTOR3(-X,Y,0),&m_ButterflyColony[i].matWorld);
		D3DXVec3TransformCoord(&pVertex[4*i+1].Pos,&D3DXVECTOR3(0,0,1), &m_ButterflyColony[i].matWorld);
		D3DXVec3TransformCoord(&pVertex[4*i+2].Pos,&D3DXVECTOR3(0,0,-1),&m_ButterflyColony[i].matWorld);
		D3DXVec3TransformCoord(&pVertex[4*i+3].Pos,&D3DXVECTOR3(X,Y,0), &m_ButterflyColony[i].matWorld);
	}
	m_pVertexBuffer->Unlock();

	//载入蝴蝶的大纹理
	CConfig::GetInstance().ReadINI("Butterfly","ButterflyMap",FileName,MAX_PATH);

	if (FAILED(D3DXCreateTextureFromFileA(m_pD3DDevice,
		FileName,
		&m_ButterflyTexture)))
	{
		MessageBox(NULL,"Load BufferTexture Failed!","Error!",MB_OK);
		return false;
	}
	m_ButterflyEffect->SetTexture("ButterflyMap",m_ButterflyTexture);

	return true;
}

void CButterfly::UpdateBufferfly( FLOAT TimeDelta,CTerrain* pTerrain )
{
	//为shader传入组合变换矩阵
	D3DXMATRIX matViewProj = CCamera::GetInstance()->GetCameraView() * CCamera::GetInstance()->GetCameraProj();
	m_ButterflyEffect->SetMatrix("g_matWorldViewProj",&matViewProj);

	BUTTERFLY_VERTEX* pVertex;

	m_pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

	//限定蝴蝶的可视范围
	D3DXVECTOR3 vViewPos = CCamera::GetInstance()->GetCameraPos();
	ViewBox	CameraBox;
	CameraBox.s_Max = vViewPos+D3DXVECTOR3(400,1000,100);
	CameraBox.s_Min = vViewPos-D3DXVECTOR3(400,1000,100);

	//重新修改蝴蝶顶点
	for (INT i=0; i<m_NumButterfly; i++)
	{
		D3DXVECTOR3 vLook;

		D3DXVec3TransformNormal(&vLook,&D3DXVECTOR3(0,0,1),&m_ButterflyColony[i].matWorld);
		D3DXVECTOR3 n_NewPos=m_ButterflyColony[i].Pos;

		//蝴蝶只在xz平面做移动
		FLOAT fSecond = TimeDelta/1000.0f;
		n_NewPos.x += vLook.x*fSecond*abs(m_ButterflyColony[i].fWingSpeed);
		n_NewPos.z += vLook.z*fSecond*abs(m_ButterflyColony[i].fWingSpeed);

		m_ButterflyColony[i].Pos.x=n_NewPos.x;
		m_ButterflyColony[i].Pos.z=n_NewPos.z;
		//如果蝴蝶飞到了地形边界,就把位置重新调回到摄像机附近
		if (   m_ButterflyColony[i].Pos.x >= 0.9*1200.0f
			|| m_ButterflyColony[i].Pos.z >= 0.9*1200.0f
			|| m_ButterflyColony[i].Pos.x <= -0.9*1200.0f
			|| m_ButterflyColony[i].Pos.z <= -0.9*1200.0f
			|| m_ButterflyColony[i].Pos.y >= 500 )
		{
			//让蝴蝶回到原点附近
			m_ButterflyColony[i].Pos = D3DXVECTOR3((FLOAT)(rand()%100-50),(FLOAT)(rand()%10+10),(FLOAT)(rand()%100-50));

			m_ButterflyColony[i].fWingSpeed = GetRandFloat(1.0f,4.0f);

			continue;
		}

		//蝴蝶的y轴位置为蝴蝶的飞行高度(主要是控制蝴蝶不会在水下面)+当前位置的地面高度
		m_ButterflyColony[i].Pos.y =m_ButterflyColony[i].fFlyHeight+pTerrain->GetHeightValue(m_ButterflyColony[i].Pos.x,m_ButterflyColony[i].Pos.z);

		//控制蝴蝶翅膀的运动范围
		if (m_ButterflyColony[i].fWingAngle > 85)
		{
			m_ButterflyColony[i].fWingAngle = 85;
			m_ButterflyColony[i].fWingSpeed = -m_ButterflyColony[i].fWingSpeed;
		}
		else if (m_ButterflyColony[i].fWingAngle < 5)
		{
			m_ButterflyColony[i].fWingAngle = 5;
			m_ButterflyColony[i].fWingSpeed = -m_ButterflyColony[i].fWingSpeed;
		}
		m_ButterflyColony[i].fWingAngle += m_ButterflyColony[i].fWingSpeed * fSecond * 100;

		//判断蝴蝶是不是在平截投体内
		//if (pFrustum->IsSphereIn(&m_ButterflyColony[i].Pos))
		//{
			if ( CheckPosInInSphere(vViewPos,m_ButterflyColony[i].Pos) )//通过sphere来判断
			{
				m_ButterflyColony[i].bVisible = false;
			}
			else
			{
				m_ButterflyColony[i].bVisible = true;
			}

			//让有些蝴蝶可以转方向飞行
			if (rand()%1000 <= 50)
			{
				m_ButterflyColony[i].nAngle = rand()%360-180;
				D3DXMATRIX n_Temp;
				D3DXMatrixIdentity(&m_ButterflyColony[i].matZoomRot);//应该旋转一下就可以了,没必要在重新进行缩放了
				//缩放
				D3DXMatrixIdentity(&n_Temp);
				D3DXMatrixScaling(&n_Temp,
					m_ButterflyColony[i].fSize,
					m_ButterflyColony[i].fSize,
					m_ButterflyColony[i].fSize);
				D3DXMatrixMultiply(&m_ButterflyColony[i].matZoomRot,
					&m_ButterflyColony[i].matZoomRot,
					&n_Temp);
				//旋转
				D3DXMatrixIdentity(&n_Temp);
				D3DXMatrixRotationY(&n_Temp,m_ButterflyColony[i].nAngle*D3DX_PI/180);
				D3DXMatrixMultiply( &m_ButterflyColony[i].matZoomRot,
					&m_ButterflyColony[i].matZoomRot,
					&n_Temp);
			}
			//平移
			D3DXMATRIX matTemp;
			D3DXMatrixIdentity(&matTemp);
			D3DXMatrixTranslation(	&matTemp,
				m_ButterflyColony[i].Pos.x,
				m_ButterflyColony[i].Pos.y,
				m_ButterflyColony[i].Pos.z);
			D3DXMatrixMultiply(&m_ButterflyColony[i].matWorld,&m_ButterflyColony[i].matZoomRot,&matTemp);

			float fTempAngle=m_ButterflyColony[i].fWingAngle*D3DX_PI/180;

			//根据翅膀的角度重新修正蝴蝶的顶点坐标
			FLOAT X = cos(fTempAngle);
			FLOAT Y = sin(fTempAngle);

			D3DXVec3TransformCoord(&pVertex[4*i].Pos  ,&D3DXVECTOR3(-X,Y,0),&m_ButterflyColony[i].matWorld);
			D3DXVec3TransformCoord(&pVertex[4*i+1].Pos,&D3DXVECTOR3(0,0,1), &m_ButterflyColony[i].matWorld);
			D3DXVec3TransformCoord(&pVertex[4*i+2].Pos,&D3DXVECTOR3(0,0,-1),&m_ButterflyColony[i].matWorld);
			D3DXVec3TransformCoord(&pVertex[4*i+3].Pos,&D3DXVECTOR3(X,Y,0), &m_ButterflyColony[i].matWorld);
		//}
		//else
		//{
		//	//不在平截投体和可视范围内的蝴蝶不必显示
		//	m_ButterflyColony[i].bVisible = false;
		//}
	}

	m_pVertexBuffer->Unlock();
}

void CButterfly::RenderBufferfly()
{
	//渲染效果
	D3DXHANDLE	n_Technique= m_ButterflyEffect->GetTechniqueByName("Butterfly");
	m_ButterflyEffect->SetTechnique(n_Technique);
	m_pD3DDevice->SetStreamSource(0,m_pVertexBuffer,0,sizeof(BUTTERFLY_VERTEX));
	m_pD3DDevice->SetFVF(D3DFVF_BUTTERFLY);

	UINT nPasses = 0;
	m_ButterflyEffect->Begin(&nPasses,0);
	for (UINT iPass = 0; iPass < nPasses; iPass++)
	{
		m_ButterflyEffect->BeginPass( iPass );

		for (int i=0; i < m_NumButterfly; i++)
		{
			//不在可视范围内的蝴蝶,不必渲染
			if (m_ButterflyColony[i].bVisible)
			{
				m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,4*i,2);
			}
		}
		m_ButterflyEffect->EndPass();
	}
	m_ButterflyEffect->End();
}
