#include "Water.h"

CWater::CWater(void)
{
	m_nSize=80;
	m_nScellpace=33;
	m_nNumVertex=m_nSize*m_nSize;
	m_nNumTriangle=(m_nSize-1)*(m_nSize-1)*2;
	m_nRow=m_nSize-1;
	m_nCol=m_nSize-1;
	m_nNumIndex=m_nNumTriangle*3;
	m_nWidth=m_nRow*m_nScellpace;
	m_nDepth=m_nCol*m_nScellpace;
}

CWater::~CWater(void)
{
}

BOOL CWater::CreateVertex()
{
	//创建顶点
	if(FAILED(m_pD3DDevice->CreateVertexBuffer(m_nNumVertex*sizeof(CUSTOMVERTEX),
		D3DUSAGE_WRITEONLY,CUSTOMVERTEX::FVF,D3DPOOL_MANAGED,&m_pVB,NULL)))
	{
		return FALSE;
	}
	int nStarX=-(m_nWidth/2);
	int nStarZ=m_nDepth/2;

	float	uCoordIncrementSize=1.0f/(float)m_nSize;
	float	vCoordIncrementSize=1.0f/(float)m_nSize;
	CUSTOMVERTEX*	pVertex;
	m_pVB->Lock(0,0,(void**)&pVertex,0);
	for(int z=0;z<m_nSize;z++)
		for(int x=0;x<m_nSize;x++)
		{
			int index=(m_nSize*z)+x;
			pVertex[index].x=(float)nStarX+m_nScellpace*x;
			pVertex[index].y=0.0f;
			pVertex[index].z=(float)nStarZ-m_nScellpace*z;
			pVertex[index].u=uCoordIncrementSize*x*10;
			pVertex[index].v=vCoordIncrementSize*z*10;
			pVertex[index].u2=uCoordIncrementSize*x*100;
			pVertex[index].v2=vCoordIncrementSize*z*100;
		}
		m_pVB->Unlock();
		return TRUE;
}

BOOL CWater::CreateIndex()
{
	if(FAILED(m_pD3DDevice->CreateIndexBuffer(m_nNumIndex*sizeof(DWORD),
		D3DUSAGE_WRITEONLY,D3DFMT_INDEX32,::D3DPOOL_MANAGED,&m_pIB,NULL)))
	{
		return FALSE;
	}
	DWORD*	indices=0;
	m_pIB->Lock(0,0,(void**)&indices,0);
	int		nBaseIndex=0;
	for(int z=0;z<m_nRow;z++)
		for(int x=0;x<m_nCol;x++)
		{
			indices[nBaseIndex]=z*m_nSize+x;
			indices[nBaseIndex+1]=z*m_nSize+x+1;
			indices[nBaseIndex+2]=(z+1)*m_nSize+x;

			indices[nBaseIndex+3]=(z+1)*m_nSize+x;
			indices[nBaseIndex+4]=z*m_nSize+x+1;
			indices[nBaseIndex+5]=(z+1)*m_nSize+x+1;

			nBaseIndex+=6;
		}
		m_pIB->Unlock();

		return TRUE;
}

BOOL CWater::InitWater( LPDIRECT3DDEVICE9 pDevice,CRole* pRole,CTerrain* pTerrain )
{
	m_pD3DDevice=pDevice;
	m_pRole=pRole;
	m_pTerrain=pTerrain;
	CConfig::GetInstance().SetConfigFileName("res//ini//Water.ini");

	//创建顶点
	if (!CreateVertex())
	{
		return false;
	}

	//创建索引
	if (!CreateIndex())
	{
		return false;
	}

	TCHAR FileName[MAX_PATH];
	CConfig::GetInstance().ReadINI("Water","WaterEffect",FileName,MAX_PATH);

	/**创建效果文件*/
	LPD3DXBUFFER	pError=NULL;
	D3DXCreateEffectFromFile(m_pD3DDevice,FileName,NULL,
		NULL,D3DXSHADER_DEBUG,NULL,&m_pEffect,&pError);
	if(pError)
	{
		MessageBox(NULL,(char*)pError->GetBufferPointer(),NULL,MB_OK);
		pError->Release();
	}
	/**创建纹理*/
	CConfig::GetInstance().ReadINI("Water","Texture1",FileName,MAX_PATH);
	D3DXCreateTextureFromFile(m_pD3DDevice,FileName,&m_pTexture);
	CConfig::GetInstance().ReadINI("Water","Texture2",FileName,MAX_PATH);
	D3DXCreateTextureFromFile(m_pD3DDevice,FileName,&m_pTexture2);
	D3DXMatrixIdentity(&m_matWorld);

	return TRUE;
}

BOOL CWater::Render( float fTime )
{
	if(m_pD3DDevice)
	{
		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);

		/*********************************************************/
		D3DXMATRIX I;
		D3DXMatrixIdentity(&I);

		m_pD3DDevice->SetRenderState(D3DRS_STENCILENABLE,TRUE);//打开模版缓冲区
		
		m_pD3DDevice->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_ALWAYS);//模版测试函数一直为真
		
		m_pD3DDevice->SetRenderState(D3DRS_STENCILREF,0x1);//设置模版参考值为1
		
		m_pD3DDevice->SetRenderState(D3DRS_STENCILPASS,D3DSTENCILOP_REPLACE);//如果通过测试用测试值代替模版值
		
		m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,false);//关闭z-buffer防止深度写入

		//--透明的混合方式
		m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		m_pD3DDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
		m_pD3DDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		m_pD3DDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
		m_pD3DDevice->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,1);
		m_pD3DDevice->SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		m_pD3DDevice->SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_CURRENT);
		m_pD3DDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_ADD);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
		//镜面贴图
		m_pD3DDevice->SetStreamSource(0,m_pVB,0,sizeof(CUSTOMVERTEX));
		m_pD3DDevice->SetFVF(CUSTOMVERTEX::FVF);
		m_pD3DDevice->SetTexture(0,m_pTexture);
		m_pD3DDevice->SetTexture(1,m_pTexture2);
		m_pD3DDevice->SetTransform(D3DTS_WORLD,&I);
		m_pD3DDevice->SetIndices(m_pIB);
		m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,
			m_nNumVertex,0,m_nNumTriangle);
		m_pD3DDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
		m_pD3DDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);

		m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

		/*************************/
		//写入完后打开深度值
		m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
		//第二次模版测试
		m_pD3DDevice->SetRenderState(D3DRS_STENCILFUNC,D3DCMP_EQUAL);
		m_pD3DDevice->SetRenderState(D3DRS_STENCILPASS,D3DSTENCILOP_KEEP);

		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		//计算关于镜面的反射矩阵
		D3DXMATRIXA16	W,T,R,S;
		D3DXPLANE	plane(0.0f,-1.0f,0.0f,0.0f);
		D3DXMatrixIdentity(&R);
		D3DXMatrixReflect(&R,&plane);
		D3DXMatrixScaling(&S,0.1f,0.1f,0.1f);
		D3DXMatrixTranslation(&T,m_pRole->m_vRole.x,m_pRole->m_vRole.y,
			m_pRole->m_vRole.z);

		D3DXMATRIX MatRotate;
		float  angle=acosf(D3DXVec3Dot(&m_pRole->m_vDirection,&D3DXVECTOR3(0.0f,0.0f,-1.0f)));
		angle=m_pRole->m_vDirection.x>=0.0f? -angle:angle;
		D3DXMatrixRotationY(&MatRotate,angle);

		W=m_pRole->m_matRotation*MatRotate*R*S*T;

		m_pD3DDevice->SetTransform(D3DTS_WORLD, &W);
		m_pD3DDevice->SetRenderState(D3DRS_ZENABLE ,false);

		m_pRole->m_matWorld=W;

//		m_pRole->Render(false,fTime);
		m_pD3DDevice->SetRenderState(D3DRS_ZENABLE ,true);

		// 恢复渲染状态
		m_pD3DDevice->SetRenderState(D3DRS_STENCILENABLE, false);

	}
	return TRUE;
}

BOOL CWater::Update( float fTime )
{
	CUSTOMVERTEX* pVertices = NULL;
	m_pVB->Lock(0,0,(void**)&pVertices,0);	
	for(int i=0; i<m_nNumVertex; i++)
	{
		float A=3.5f;
		float DirX=1.0f;
		float DirZ=1.0f;
		float x = pVertices[i].x;
		float z = pVertices[i].z;
		static float T=0.0f;
		T+=fTime*0.00005f;
		if(T>=1.0f)
		{
			T=0.0f;
		}
		//pVertices[i].y=A*sin(x+T*2*3.1415926f)+A*sin(z+T*2*3.1415926f)+5;
		pVertices[i].y=sin(x+T*2*3.1415926f)+sin(z+T*2*3.1415926f)+5.0f;
		pVertices[i].u+=0.0005f;
		pVertices[i].v+=0.0005f;
	}
	m_pVB->Unlock();



	return TRUE;
}