#include "Grass.h"

CGrass::CGrass(void)
{
	m_fWagSpeed = 20.0f;
	m_XOffset	= 0.0f;
	m_ZOffset	= 0.0f;
	m_NumGrass	= 0;
	m_pVertexBuffer = NULL;
}

CGrass::~CGrass(void)
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pGrassTex);
}

//创建草的顶点
bool CGrass::CreateGrassVertexBuffer()
{
	float fXPos = 0;
	float fZPos = 0;
	float fYPos = 0;

	float fWidth	= m_pTerrain->GetTerrainWidth() / 2 * 0.8f;

	D3DXVECTOR4* GrassPos	= new D3DXVECTOR4[m_NumGrass];

	//获取每个草的坐标位置
	for (int i=0; i<m_NumGrass; ++i)
	{
	randpos:
		fXPos = GetRandFloat(-fWidth,fWidth);
		fZPos = GetRandFloat(-fWidth,fWidth);

		fYPos = m_pTerrain->GetHeightValue(fXPos,fZPos);

		if (fYPos > 5.0f)//不让高地上有草---可以通过高度图来规定草的分布范围,暂时用此方法
		{
			goto randpos;
		}

		GrassPos[i].x	= fXPos;
		GrassPos[i].y	= fYPos;
		GrassPos[i].z	= fZPos;
		GrassPos[i].w	= (rand()%360) * D3DX_PI/180.0f;//设置草的摆放角度
	}

	D3DXMATRIX matZoom;
	D3DXMATRIX matWorld;
	D3DXMATRIX matRot;

	if(FAILED(m_pD3DDevice->CreateVertexBuffer( 8*m_NumGrass*sizeof(GRASS_VERTEX),
												D3DUSAGE_WRITEONLY,
												D3DFVF_GRASS,
												D3DPOOL_MANAGED,
												&m_pVertexBuffer,
												NULL)))
	{
		MessageBox(NULL,"创建草的顶点失败!","Error!",MB_OK);
		return false;
	}

	GRASS_VERTEX* pVertex;
	//m_pVertexBuffer->Lock(0,8*m_NumGrass*sizeof(GRASS_VERTEX),(VOID**)&pVertex,D3DLOCK_DISCARD);
	m_pVertexBuffer->Lock(0,8*m_NumGrass*sizeof(GRASS_VERTEX),(VOID**)&pVertex,0);

	int VerIndex = 0;
	for (int i=0; i<m_NumGrass; i++)
	{
		//缩放
		D3DXMatrixIdentity(&matZoom);
		D3DXMatrixScaling(&matZoom,m_vGrassSize.x*GetRandFloat(0.8,1.2),m_vGrassSize.y*GetRandFloat(0.8,1.5),m_vGrassSize.z*GetRandFloat(0.8,1.2));
		VerIndex = 8*i;
		//旋转
		D3DXMatrixIdentity(&matRot);
		D3DXMatrixRotationY(&matRot,GrassPos[i].w);
		//平移
		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixTranslation(&matWorld,GrassPos[i].x,GrassPos[i].y,GrassPos[i].z);

		//得出组合变换矩阵
		matWorld = matZoom*matRot*matWorld;

		//求出十字交叉的四个顶点
		D3DXVec3TransformCoord(&pVertex[VerIndex].Pos,	&D3DXVECTOR3(-1,1,-1),&matWorld);
		D3DXVec3TransformCoord(&pVertex[VerIndex+1].Pos,&D3DXVECTOR3(1,1,-1), &matWorld);
		D3DXVec3TransformCoord(&pVertex[VerIndex+2].Pos,&D3DXVECTOR3(-1,0,-1),&matWorld);
		D3DXVec3TransformCoord(&pVertex[VerIndex+3].Pos,&D3DXVECTOR3(1,0,-1), &matWorld);

		D3DXVec3TransformCoord(&pVertex[VerIndex+4].Pos,&D3DXVECTOR3(0,1,0),  &matWorld);
		D3DXVec3TransformCoord(&pVertex[VerIndex+5].Pos,&D3DXVECTOR3(0,1,-2), &matWorld);
		D3DXVec3TransformCoord(&pVertex[VerIndex+6].Pos,&D3DXVECTOR3(0,0,0),  &matWorld);
		D3DXVec3TransformCoord(&pVertex[VerIndex+7].Pos,&D3DXVECTOR3(0,0,-2), &matWorld);

		//重新调整y轴值,下方点紧贴地面
		pVertex[VerIndex+2].Pos.y = m_pTerrain->GetHeightValue(pVertex[VerIndex+2].Pos.x,pVertex[VerIndex+2].Pos.z);
		pVertex[VerIndex+3].Pos.y = m_pTerrain->GetHeightValue(pVertex[VerIndex+3].Pos.x,pVertex[VerIndex+3].Pos.z);
		pVertex[VerIndex+6].Pos.y = m_pTerrain->GetHeightValue(pVertex[VerIndex+6].Pos.x,pVertex[VerIndex+6].Pos.z);
		pVertex[VerIndex+7].Pos.y = m_pTerrain->GetHeightValue(pVertex[VerIndex+7].Pos.x,pVertex[VerIndex+7].Pos.z);

		//避免下方顶点高于或者是太接近上方顶点的情况
		if (pVertex[VerIndex+2].Pos.y >= pVertex[VerIndex].Pos.y - 15.0f)
		{
			pVertex[VerIndex].Pos.y = pVertex[VerIndex+2].Pos.y + 15.0f;
		}
		if (pVertex[VerIndex+3].Pos.y >= pVertex[VerIndex+1].Pos.y - 15.0f)
		{
			pVertex[VerIndex+1].Pos.y = pVertex[VerIndex+3].Pos.y + 15.0f;
		}
		if (pVertex[VerIndex+6].Pos.y >= pVertex[VerIndex+4].Pos.y - 15.0f)
		{
			pVertex[VerIndex+4].Pos.y = pVertex[VerIndex+6].Pos.y + 15.0f;
		}
		if (pVertex[VerIndex+7].Pos.y >= pVertex[VerIndex+5].Pos.y - 15.0f)
		{
			pVertex[VerIndex+5].Pos.y = pVertex[VerIndex+7].Pos.y + 15.0f;
		}

		//随机获取纹理
		D3DXVECTOR2 n_TexCoord=D3DXVECTOR2((FLOAT)(rand()%4)/4,(FLOAT)(rand()%4)/4);
		//确定各个顶点的纹理坐标
		pVertex[VerIndex].Tex	= pVertex[VerIndex+4].Tex = D3DXVECTOR2(n_TexCoord.x,n_TexCoord.y);
		pVertex[VerIndex+1].Tex = pVertex[VerIndex+5].Tex = D3DXVECTOR2(n_TexCoord.x+0.25f,n_TexCoord.y);
		pVertex[VerIndex+2].Tex = pVertex[VerIndex+6].Tex = D3DXVECTOR2(n_TexCoord.x,n_TexCoord.y+0.25f);
		pVertex[VerIndex+3].Tex = pVertex[VerIndex+7].Tex = D3DXVECTOR2(n_TexCoord.x+0.25f,n_TexCoord.y+0.25f);

		//摇摆的上方2点 和方向  方向要向着平面的法线方向来回摆动
		D3DXVECTOR3 vOffSet;
		D3DXVECTOR3 vDirect;
		vDirect = D3DXVECTOR3(GetRandFloat(-1,1),0,GetRandFloat(-1,1));//随机获取一个方向
		D3DXVec3Normalize(&vDirect,&vDirect);
		D3DXVec3TransformNormal(&vOffSet,&vDirect,&matWorld);
		vOffSet.y=0;
		D3DXVec3Normalize(&vOffSet,&vOffSet);
		pVertex[VerIndex].Dir	= pVertex[VerIndex+4].Dir = D3DXVECTOR2(vOffSet.x,vOffSet.z);
		pVertex[VerIndex+1].Dir	= pVertex[VerIndex+5].Dir = D3DXVECTOR2(vOffSet.x,vOffSet.z);
		pVertex[VerIndex+2].Dir	= pVertex[VerIndex+6].Dir = D3DXVECTOR2(0,0);
		pVertex[VerIndex+3].Dir	= pVertex[VerIndex+7].Dir = D3DXVECTOR2(0,0);
	}
	m_pVertexBuffer->Unlock();

	SAFE_DELETE_ARRAY(GrassPos);

	return true;
}

//创建草的索引
bool CGrass::CreateGrassIndexBuffer()
{
	if( FAILED(m_pD3DDevice->CreateIndexBuffer( 12*m_NumGrass*sizeof(DWORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&m_pIndexBuffer,
		NULL)))
	{
		MessageBox(NULL,"创建草的索引失败!","Error!",MB_OK);
		return false;
	}

	DWORD* pIndex;
	m_pIndexBuffer->Lock(0,0,(VOID**)&pIndex,0);
	DWORD dwIndex = 0;
	DWORD dwVertexIndex = 0;
	for (int i=0; i<m_NumGrass; i++)
	{
		dwVertexIndex = i * 8;
		pIndex[dwIndex]		= dwVertexIndex;
		pIndex[dwIndex+1]	= dwVertexIndex + 1;
		pIndex[dwIndex+2]	= dwVertexIndex + 2;

		pIndex[dwIndex+3]	= dwVertexIndex + 2;
		pIndex[dwIndex+4]	= dwVertexIndex + 1;
		pIndex[dwIndex+5]	= dwVertexIndex + 3;

		pIndex[dwIndex+6]	= dwVertexIndex + 4;
		pIndex[dwIndex+7]	= dwVertexIndex + 5;
		pIndex[dwIndex+8]	= dwVertexIndex + 6;

		pIndex[dwIndex+9]	= dwVertexIndex + 6;
		pIndex[dwIndex+10]	= dwVertexIndex + 5;
		pIndex[dwIndex+11]	= dwVertexIndex + 7;
		dwIndex += 12;
	}
	m_pIndexBuffer->Unlock();

	return true;
}

//在两个值之间取随即值
float CGrass::GetRandFloat( float fMin,float fMax )
{
	if (fMin >= fMax)
	{
		return fMin;
	}
	//在0到1之间取值
	float f = (rand() % 10000) * 0.0001f; 

	// return float in [lowBound, highBound] interval. 
	return (f * (fMax - fMin)) + fMin; 
}

bool CGrass::TimeMode( float &StartTime, float TimeDelta, float TimeELapce )
{
	StartTime += TimeDelta;
	if (StartTime >= TimeELapce)
	{
		return true;
	}
	else 
		return false;
}

bool CGrass::CreateGrassEffectFromFile()
{
	CConfig::GetInstance().SetConfigFileName("res//ini//Grass.ini");
	TCHAR FileName[MAX_PATH];

	//创建效果对象
	CConfig::GetInstance().ReadINI("Grass","GrassEffect",FileName,MAX_PATH);
	HRESULT hr = NULL;
	LPD3DXBUFFER n_Error = NULL;
	hr = D3DXCreateEffectFromFileA(	m_pD3DDevice,
		FileName,
		NULL,
		NULL,
		D3DXSHADER_DEBUG,
		NULL,
		&m_GrassEffect,
		&n_Error );
	if (n_Error)
	{
		MessageBox(NULL,(char*)n_Error->GetBufferPointer(),"Error!",MB_OK);
		SAFE_RELEASE(n_Error);
		return false;
	}

	if (FAILED(hr))
	{
		MessageBox(NULL,"创建Grass效果失败!","Error!",MB_OK);
		return false;
	}

	//载入草的大纹理
	CConfig::GetInstance().ReadINI("Grass","GrassMap",FileName,MAX_PATH);
	if ( FAILED(D3DXCreateTextureFromFileA( m_pD3DDevice,
		FileName,
		&m_pGrassTex)))
	{
		MessageBox(NULL,"载入草的纹理失败!","Error!",MB_OK);
		return false;
	}
	m_GrassEffect->SetTexture("GrassMap",m_pGrassTex);

	return true;
}

bool CGrass::InitGrass( LPDIRECT3DDEVICE9 pD3DDevice, CTerrain* pTerrain )
{
	m_pD3DDevice	= pD3DDevice;
	m_pTerrain		= pTerrain;

	CConfig::GetInstance().SetConfigFileName("res//ini//Grass.ini");
	//获取文件中获取草的相关参数
	m_NumGrass		= CConfig::GetInstance().ReadINI("Grass","Total");
	m_fWagSpeed		= CConfig::GetInstance().ReadINI("Grass","WagSpeed");
	m_vGrassSize.x	= CConfig::GetInstance().ReadINI("Grass","GrassSizeX") * 1.0f;
	m_vGrassSize.y	= CConfig::GetInstance().ReadINI("Grass","GrassSizeY") * 1.0f;
	m_vGrassSize.z	= CConfig::GetInstance().ReadINI("Grass","GrassSizeZ") * 1.0f;

	if (!CreateGrassEffectFromFile())
	{
		return false;
	}

	if (!CreateGrassVertexBuffer())
	{
		return false;
	}

	if (!CreateGrassIndexBuffer())
	{
		return false;
	}

	return true;
}

void CGrass::UpdateGrass( float TimeDelta )
{
	static int nNumTime		= 0;
	static float fTimeLeave	= 0.0f;
	float fSecond = TimeDelta * 0.001f;

	//求草在X,Z上的摆动量
	m_XOffset += m_fWagSpeed * fSecond;
	m_ZOffset += m_fWagSpeed * fSecond;

	//通过时间间隔控制摆动的方向
	if (TimeMode(fTimeLeave,fSecond,0.5))
	{
		nNumTime++;
		if (nNumTime%2)
		{
			m_fWagSpeed = -m_fWagSpeed;
			fTimeLeave = 0.0f;
		}
		else fTimeLeave = 0.0f;

		if (nNumTime == 100) nNumTime = 0;
	}

	//传入草的在X,Z方向上的摆动量
	m_GrassEffect->SetFloat("g_XOffset",m_XOffset);
	m_GrassEffect->SetFloat("g_ZOffset",m_ZOffset);
}	

void CGrass::RenderGrass()
{
	D3DXMATRIX	matWorld;
	D3DXMATRIX	matProj;
	D3DXMATRIX	matView;

	if ( m_GrassEffect == NULL )
		return;

	//传入转化矩阵
	D3DXMatrixIdentity(&matWorld);
	m_pD3DDevice->GetTransform(D3DTS_VIEW,&matView);
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION,&matProj);
	m_pD3DDevice->SetTransform(D3DTS_WORLD,&matWorld);
	matWorld = matWorld * matView * matProj;
	m_GrassEffect->SetMatrix("g_matWorldViewProj",&matWorld);

	//渲染效果
	D3DXHANDLE	n_Technique= m_GrassEffect->GetTechniqueByName("Grass");
	m_GrassEffect->SetTechnique(n_Technique);

	UINT nPasses;
	m_GrassEffect->Begin(&nPasses,0);
	for (UINT i=0; i<nPasses; i++)
	{
		m_GrassEffect->BeginPass(0);
		m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(GRASS_VERTEX));
		m_pD3DDevice->SetFVF(D3DFVF_GRASS);
		m_pD3DDevice->SetIndices(m_pIndexBuffer);
		m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8*m_NumGrass, 0, m_NumGrass*4);
		m_GrassEffect->EndPass();
	}
	m_GrassEffect->End();
}
