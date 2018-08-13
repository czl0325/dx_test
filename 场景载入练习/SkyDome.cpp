#include "SkyDome.h"

CSkyDome::CSkyDome(LPDIRECT3DDEVICE9 pD3DDevice)
{
	m_pD3DDevice	=   pD3DDevice;
	m_pIndexBuffer	=	NULL;
	m_pVertexBuffer	=   NULL;
	m_pSkyBoxTex	=   NULL;
	m_pCloudTex		=   NULL;
	m_pSkyEffect	=	NULL;
	m_nRings		=	10;
	m_nSegments		=	10;
	m_vCloudOffset	=	D3DXVECTOR2(0,0);
	CConfig::GetInstance().SetConfigFileName("res//ini//SkyDome.ini");
}

CSkyDome::~CSkyDome(void)
{
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pSkyBoxTex);
	SAFE_RELEASE(m_pCloudTex);
	SAFE_RELEASE(m_pSkyEffect);
}

bool CSkyDome::InitSkyDome()
{
	TCHAR SkyShaderFileName[MAX_PATH];
	//����Բ�İ뾶�����ֵȷ�
	m_fRadius	= CConfig::GetInstance().ReadINI("SkyDome","Radius") * 1.0f;
	m_nRings	= CConfig::GetInstance().ReadINI("SkyDome","Rings");
	m_nSegments	= CConfig::GetInstance().ReadINI("SkyDome","Segments");

	m_dwNumOfVertices = (m_nRings + 1) * (m_nSegments + 1);	//��������
	m_dwNumOfIndices  = 2*m_nRings*(m_nSegments+1);			//��������
	m_dwNumOfPolygons = m_dwNumOfIndices - 2;				//����������(Լ������������)

	CConfig::GetInstance().ReadINI("SkyDome","SkyEffect",SkyShaderFileName,MAX_PATH);

	//����Ч������
	HRESULT hr = NULL;
	LPD3DXBUFFER n_Error = NULL;
	hr = D3DXCreateEffectFromFileA(	m_pD3DDevice,
		SkyShaderFileName,
		NULL,
		NULL,
		D3DXSHADER_DEBUG,
		NULL,
		&m_pSkyEffect,
		&n_Error );
	if (n_Error)
	{
		MessageBox(NULL,(char*)n_Error->GetBufferPointer(),NULL,NULL);
		SAFE_RELEASE(n_Error);
		hr=E_FAIL;
	}

	if (FAILED(hr))
	{
		return false;
	}

	//�������������
	if (!CreateVertices())
	{
		return false;
	}

	//����Ч������
	if (!SetEffectValue())
	{
		return false;
	}

	return true;
}

bool CSkyDome::CreateVertices()
{
	WORD*  pIndices;    //ָ�룬��lock����������ʱ�ᱻ�ض���
	SKYDOMEVERTEX* pVertex;

	WORD wVertexIndex = 0;
	int  nCurrentRing;
	int  nCurrentSegment;

	//�������㻺��
	if(FAILED(m_pD3DDevice->CreateVertexBuffer(m_dwNumOfVertices* sizeof(SKYDOMEVERTEX),
		0,D3DFVF_SKYDOMEVERTEX,D3DPOOL_DEFAULT, 
		&m_pVertexBuffer,NULL)))
	{
		return false;
	}

	//��������������
	if(FAILED(m_pD3DDevice->CreateIndexBuffer(m_dwNumOfIndices *sizeof(DWORD),
		0,D3DFMT_INDEX16,D3DPOOL_MANAGED,
		&m_pIndexBuffer,NULL)))
	{
		return false;
	}

	if(FAILED(m_pVertexBuffer->Lock(0,0,(VOID**)&pVertex,0)))
	{
		return false;
	}

	if(FAILED(m_pIndexBuffer->Lock(0,m_dwNumOfIndices,(VOID**)&pIndices,0)))
	{
		return false;
	}
	FLOAT rDeltaRingAngle = (0.6f * D3DX_PI / m_nRings);
	FLOAT rDeltaSegAngle  = (2.0f * D3DX_PI / m_nSegments);

	for(nCurrentRing = 0; nCurrentRing < m_nRings + 1; nCurrentRing++)
	{
		//�����γ��Բ�İ뾶�͸�γ�ȵĶ�Ӧy����
		FLOAT r0 = m_fRadius * sinf(nCurrentRing * rDeltaRingAngle);
		FLOAT y0 = m_fRadius * cosf(nCurrentRing * rDeltaRingAngle);

		//��������Ƭ����Ŀ
		for(nCurrentSegment = 0; nCurrentSegment < m_nSegments + 1; nCurrentSegment++)
		{
			FLOAT x0 = r0 * cosf(nCurrentSegment * rDeltaSegAngle);
			FLOAT z0 = r0 * sinf(nCurrentSegment * rDeltaSegAngle);

			pVertex->pos.x=x0;
			pVertex->pos.y=y0;
			pVertex->pos.z=z0;

			pVertex->tex.x = 1.0f - ((FLOAT)nCurrentSegment / (FLOAT)m_nSegments);
			pVertex->tex.y = (FLOAT)nCurrentRing / (FLOAT)m_nRings;
			pVertex++;
			if(nCurrentRing != m_nRings)
			{
				*pIndices = wVertexIndex;
				pIndices++;
				*pIndices = wVertexIndex + (WORD)(m_nSegments + 1);
				pIndices++;
				wVertexIndex++;
			}
		}
	}
	if(FAILED(m_pIndexBuffer->Unlock()))
	{
		return false;
	}
	if(FAILED(m_pVertexBuffer->Unlock()))
	{
		return false;
	}
	return true;
}

bool CSkyDome::SetEffectValue()
{
	//��������������·��
	TCHAR SkyTexPathName[MAX_PATH];
	CConfig::GetInstance().ReadINI("SkyDome","SkyTex",SkyTexPathName,MAX_PATH);

	if(FAILED(D3DXCreateTextureFromFile(m_pD3DDevice,SkyTexPathName,&m_pSkyBoxTex)))
	{
		MessageBox(NULL, "�������������ʧ��!", "Texture.exe", MB_OK);
		return false;
	}

	//�Ʋ�����
	CConfig::GetInstance().ReadINI("SkyDome","CloudTex",SkyTexPathName,MAX_PATH);
	if (FAILED(D3DXCreateTextureFromFile(m_pD3DDevice,SkyTexPathName,&m_pCloudTex)))
	{
		MessageBox(NULL, "�����Ʋ�����ʧ��!", "Texture.exe", MB_OK);
		return false;
	}

	m_pSkyEffect->SetTexture("SkyTex",m_pSkyBoxTex);
	m_pSkyEffect->SetTexture("CloudMap",m_pCloudTex);

	return true;
}

void CSkyDome::UpdateSkyDome( float fTimeDelta )
{
	static D3DXVECTOR2 vCloudMoveRate = D3DXVECTOR2(0.02f,0.04f);//����x��,z����ƶ��ٶ�
	m_vCloudOffset += vCloudMoveRate * fTimeDelta * 0.001f;//�Ʋ���һ������ѭ�������е��ƶ���
	if (m_vCloudOffset.x>1.0f) m_vCloudOffset.x=0.0f;
	if (m_vCloudOffset.y>1.0f) m_vCloudOffset.y=0.0f;

	m_pSkyEffect->SetValue("g_cloudTexOffset",m_vCloudOffset,sizeof(D3DXVECTOR2));
}

void CSkyDome::Render()
{
	D3DXMATRIX	matWorld;
	D3DXMATRIX	matProj;
	D3DXMATRIX	matView;

	if ( m_pSkyEffect == NULL )
		return;

	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixScaling(&matWorld,100.0f,40.0f,100.0f);

	m_pD3DDevice->GetTransform(D3DTS_VIEW,&matView);
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION,&matProj);
	m_pD3DDevice->SetTransform(D3DTS_WORLD,&matWorld);
	matWorld = matWorld * matView * matProj;

	m_pSkyEffect->SetMatrix("g_matWorldViewProj",&matWorld);

	D3DXHANDLE	n_Technique = m_pSkyEffect->GetTechniqueByName("Sky");
	m_pSkyEffect->SetTechnique(n_Technique);


	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	m_pD3DDevice->SetStreamSource(0,m_pVertexBuffer,0,sizeof(SKYDOMEVERTEX));
	m_pD3DDevice->SetFVF(D3DFVF_SKYDOMEVERTEX);
	m_pD3DDevice->SetIndices(m_pIndexBuffer);

	UINT nPasses = 0;
	m_pSkyEffect->Begin( &nPasses, 0 );
	for( UINT iPass = 0; iPass < nPasses; ++iPass)
	{
		m_pSkyEffect->BeginPass( iPass );
		m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,0,0,m_dwNumOfVertices,0,m_dwNumOfPolygons);
		m_pSkyEffect->EndPass();
	}
	m_pSkyEffect->End();
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
}
