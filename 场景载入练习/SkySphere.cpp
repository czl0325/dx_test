#include "SkySphere.h"

CSkySphere::CSkySphere(LPDIRECT3DDEVICE9 pD3DDevice, int nRings, int nSegments)
{
	m_pD3DDevice     =   pD3DDevice;
	m_pVertexBuffer  =   NULL;
	m_pIndexBuffer   =   NULL;
	m_pTexture       =   NULL;
	m_pSkyEffect     =   NULL;

	m_nRings         =   nRings;
	m_nSegments      =   nSegments;

	m_dwNumOfVertices =  (m_nRings + 1) * (m_nSegments + 1); //��������
	m_dwNumOfIndices  =  2*m_nRings*(m_nSegments+1);         //��������()
	m_dwNumOfPolygons =  m_dwNumOfIndices - 2;               //����������(Լ������������)

	UpdateVertices();
}

CSkySphere::~CSkySphere(void)
{
}

bool CSkySphere::UpdateVertices()
{
	WORD*  pIndices;    //ָ�룬��lock����������ʱ�ᱻ�ض���
	SKYSPHEREVERTEX* pVertex;

	WORD wVertexIndex = 0;
	int  nCurrentRing;
	int  nCurrentSegment;
	D3DXVECTOR3    vNormal;


	//�������㻺��
	if(FAILED(m_pD3DDevice->CreateVertexBuffer(m_dwNumOfVertices* sizeof(SKYSPHEREVERTEX),
		0,D3DFVF_SKYSPHEREVERTEX,D3DPOOL_DEFAULT, 
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
		return false;



	if(FAILED(m_pIndexBuffer->Lock(0,m_dwNumOfIndices,(VOID**)&pIndices,0)))
		return false;


	FLOAT rDeltaRingAngle = (0.6f * D3DX_PI / m_nRings);
	FLOAT rDeltaSegAngle  = (2.0f * D3DX_PI / m_nSegments);

	for(nCurrentRing = 0; nCurrentRing < m_nRings + 1; nCurrentRing++)
	{
		FLOAT r0 = sinf(nCurrentRing * rDeltaRingAngle);
		FLOAT y0 = cosf(nCurrentRing * rDeltaRingAngle);

		//��������Ƭ����Ŀ
		for(nCurrentSegment = 0; nCurrentSegment < m_nSegments + 1; nCurrentSegment++)
		{
			FLOAT x0 = r0 * cosf(nCurrentSegment * rDeltaSegAngle);
			FLOAT z0 = r0 * sinf(nCurrentSegment * rDeltaSegAngle);
			vNormal.x = x0;
			vNormal.y = y0;
			vNormal.z = z0;
			D3DXVec3Normalize(&vNormal, &vNormal);

			pVertex->x = x0;
			pVertex->y = y0;
			pVertex->z = z0;
			//����
			pVertex->nx = -vNormal.x;
			pVertex->ny = -vNormal.y;
			pVertex->nz = -vNormal.z;
			pVertex->u = 1.0f - ((FLOAT)nCurrentSegment / (FLOAT)m_nSegments);
			//pVertex->v = 2 * (FLOAT)nCurrentRing / (FLOAT)m_nRings;
			pVertex->v = (FLOAT)nCurrentRing / (FLOAT)m_nRings;
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
		return false;

	if(FAILED(m_pVertexBuffer->Unlock()))
		return false;



	return true;
}

bool CSkySphere::SetSkyDomeTexture( const char* szTextureFilePath )
{
	if(FAILED(D3DXCreateTextureFromFile(m_pD3DDevice,szTextureFilePath,&m_pTexture)))
	{
		MessageBox(NULL, "��������ʧ��", "Texture.exe", MB_OK);
		return false;
	}
	return true;
}

bool CSkySphere::Render()
{

	D3DXMATRIX	matWorld;
	D3DXMATRIX  matMove;          //�ƶ�����
	D3DXMATRIX  matRotationUser;  //�Զ������
	D3DXMATRIX  matScale;
	//�Զ���һ����ֱ����(��Yƽ��)����ת��
	D3DXMatrixRotationAxis(&matRotationUser, &D3DXVECTOR3(0.0f,1.0f,0.0f),0);
	D3DXMatrixScaling(&matScale,1200.0f,400.0f,1200.0f);
	D3DXMatrixTranslation(&matMove,0.0f,0.0f,0.0f);
	D3DXMatrixMultiply(&matWorld, &matRotationUser, &matScale);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matMove);

	m_pD3DDevice->SetTransform(D3DTS_WORLD,&matWorld);//Ӧ��������Ͼ���
	m_pD3DDevice->SetStreamSource(0,m_pVertexBuffer,0,sizeof(SKYSPHEREVERTEX));
	m_pD3DDevice->SetFVF(D3DFVF_SKYSPHEREVERTEX);

	if(m_pTexture != NULL)
	{
		m_pD3DDevice->SetTexture(0,m_pTexture);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	}
	else
	{
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	}

	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	m_pD3DDevice->SetIndices(m_pIndexBuffer);
	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,0,0,m_dwNumOfVertices,0,m_dwNumOfPolygons);
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

	return true;
}

bool CSkySphere::SetEffectValue()
{
	//��������������·��
	if(FAILED(D3DXCreateTextureFromFile(m_pD3DDevice,"pic//Sky//skydome.dds",&m_pSkyBoxTex)))
	{
		MessageBox(NULL, "�������������ʧ��!", "Texture.exe", MB_OK);
		return false;
	}

	//�Ʋ�����
	if (FAILED(D3DXCreateTextureFromFile(m_pD3DDevice,"pic//Sky//Cloud03.dds",&m_pCloudTex)))
	{
		MessageBox(NULL, "�����Ʋ�����ʧ��!", "Texture.exe", MB_OK);
		return false;
	}

	m_pSkyEffect->SetTexture("pic//Sky//skydome.dds",m_pSkyBoxTex);
	m_pSkyEffect->SetTexture("pic//Sky//Cloud03.dds",m_pCloudTex);

	return true;
}