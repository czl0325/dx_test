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

//������ֵ֮��ȡ�漴ֵ
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

//����̫���ĺ�����Ϊ���ɼ�----��box
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

//����̫Զ��̫���ĺ�����Ϊ���ɼ�----��sphere
bool CButterfly::CheckPosInInSphere( D3DXVECTOR3 View,D3DXVECTOR3 Pos )
{
	float flen = D3DXVec3Length(&(Pos - View));

	//����̫Զ��̫���ĺ�����Ϊ���ɼ�---����̫�����Ļ���Ӱ����������Ӿ�
	if ( flen <= 150.0f || flen >= 1000.0f )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//��ʼ������
bool CButterfly::InitBufferfly()
{
	CConfig::GetInstance().SetConfigFileName("res//ini//Butterfly.ini");

	TCHAR FileName[MAX_PATH];
	CConfig::GetInstance().ReadINI("Butterfly","ButterflyEffect",FileName,MAX_PATH);

	//����Ч������
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
		MessageBox(NULL,"����BufferflyЧ��ʧ��!","Error!",MB_OK);
		return false;
	}

	//��ȡ������������
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

	//��ʼ������״̬
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

		//���ݴ�С�Ժ�����������
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

		//���ݽǶȶԺ���������ת
		D3DXMatrixIdentity(&n_Temp);
		D3DXMatrixRotationY(&n_Temp,m_ButterflyColony[i].nAngle*D3DX_PI/180);
		D3DXMatrixMultiply( &m_ButterflyColony[i].matZoomRot,
			&m_ButterflyColony[i].matZoomRot,
			&n_Temp);

		//����λ�öԺ��������ƶ�
		D3DXMatrixIdentity(&m_ButterflyColony[i].matWorld);
		D3DXMatrixTranslation(	&m_ButterflyColony[i].matWorld,
			m_ButterflyColony[i].Pos.x,
			m_ButterflyColony[i].Pos.y,
			m_ButterflyColony[i].Pos.z);

		//��ȡ�������������
		D3DXMatrixMultiply(&m_ButterflyColony[i].matWorld,&m_ButterflyColony[i].matZoomRot,&m_ButterflyColony[i].matWorld);

		m_ButterflyColony[i].bVisible = TRUE;
		float fTempAngle = m_ButterflyColony[i].fWingAngle*D3DX_PI/180;

		//�Ӵ�ĺ����������漴ȡһС�ĺ�������
		D3DXVECTOR2 n_TexCoord=D3DXVECTOR2((FLOAT)(rand()%4)/4,(FLOAT)(rand()%4)/4);

		//���4���Ƕ�Ӧ����������
		pVertex[4*i].Tex	= D3DXVECTOR2(n_TexCoord.x,n_TexCoord.y);
		pVertex[4*i+1].Tex	= D3DXVECTOR2(n_TexCoord.x+0.25f,n_TexCoord.y);
		pVertex[4*i+2].Tex	= D3DXVECTOR2(n_TexCoord.x,n_TexCoord.y+0.25f);
		pVertex[4*i+3].Tex	= D3DXVECTOR2(n_TexCoord.x+0.25f,n_TexCoord.y+0.25f);

		//ͨ����������ʹ��������۵�---����z���۵�����
		FLOAT X=cos(fTempAngle);
		FLOAT Y=sin(fTempAngle);
		D3DXVec3TransformCoord(&pVertex[4*i].Pos,  &D3DXVECTOR3(-X,Y,0),&m_ButterflyColony[i].matWorld);
		D3DXVec3TransformCoord(&pVertex[4*i+1].Pos,&D3DXVECTOR3(0,0,1), &m_ButterflyColony[i].matWorld);
		D3DXVec3TransformCoord(&pVertex[4*i+2].Pos,&D3DXVECTOR3(0,0,-1),&m_ButterflyColony[i].matWorld);
		D3DXVec3TransformCoord(&pVertex[4*i+3].Pos,&D3DXVECTOR3(X,Y,0), &m_ButterflyColony[i].matWorld);
	}
	m_pVertexBuffer->Unlock();

	//��������Ĵ�����
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
	//Ϊshader������ϱ任����
	D3DXMATRIX matViewProj = CCamera::GetInstance()->GetCameraView() * CCamera::GetInstance()->GetCameraProj();
	m_ButterflyEffect->SetMatrix("g_matWorldViewProj",&matViewProj);

	BUTTERFLY_VERTEX* pVertex;

	m_pVertexBuffer->Lock(0, 0, (void**)&pVertex, 0);

	//�޶������Ŀ��ӷ�Χ
	D3DXVECTOR3 vViewPos = CCamera::GetInstance()->GetCameraPos();
	ViewBox	CameraBox;
	CameraBox.s_Max = vViewPos+D3DXVECTOR3(400,1000,100);
	CameraBox.s_Min = vViewPos-D3DXVECTOR3(400,1000,100);

	//�����޸ĺ�������
	for (INT i=0; i<m_NumButterfly; i++)
	{
		D3DXVECTOR3 vLook;

		D3DXVec3TransformNormal(&vLook,&D3DXVECTOR3(0,0,1),&m_ButterflyColony[i].matWorld);
		D3DXVECTOR3 n_NewPos=m_ButterflyColony[i].Pos;

		//����ֻ��xzƽ�����ƶ�
		FLOAT fSecond = TimeDelta/1000.0f;
		n_NewPos.x += vLook.x*fSecond*abs(m_ButterflyColony[i].fWingSpeed);
		n_NewPos.z += vLook.z*fSecond*abs(m_ButterflyColony[i].fWingSpeed);

		m_ButterflyColony[i].Pos.x=n_NewPos.x;
		m_ButterflyColony[i].Pos.z=n_NewPos.z;
		//��������ɵ��˵��α߽�,�Ͱ�λ�����µ��ص����������
		if (   m_ButterflyColony[i].Pos.x >= 0.9*1200.0f
			|| m_ButterflyColony[i].Pos.z >= 0.9*1200.0f
			|| m_ButterflyColony[i].Pos.x <= -0.9*1200.0f
			|| m_ButterflyColony[i].Pos.z <= -0.9*1200.0f
			|| m_ButterflyColony[i].Pos.y >= 500 )
		{
			//�ú����ص�ԭ�㸽��
			m_ButterflyColony[i].Pos = D3DXVECTOR3((FLOAT)(rand()%100-50),(FLOAT)(rand()%10+10),(FLOAT)(rand()%100-50));

			m_ButterflyColony[i].fWingSpeed = GetRandFloat(1.0f,4.0f);

			continue;
		}

		//������y��λ��Ϊ�����ķ��и߶�(��Ҫ�ǿ��ƺ���������ˮ����)+��ǰλ�õĵ���߶�
		m_ButterflyColony[i].Pos.y =m_ButterflyColony[i].fFlyHeight+pTerrain->GetHeightValue(m_ButterflyColony[i].Pos.x,m_ButterflyColony[i].Pos.z);

		//���ƺ��������˶���Χ
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

		//�жϺ����ǲ�����ƽ��Ͷ����
		//if (pFrustum->IsSphereIn(&m_ButterflyColony[i].Pos))
		//{
			if ( CheckPosInInSphere(vViewPos,m_ButterflyColony[i].Pos) )//ͨ��sphere���ж�
			{
				m_ButterflyColony[i].bVisible = false;
			}
			else
			{
				m_ButterflyColony[i].bVisible = true;
			}

			//����Щ��������ת�������
			if (rand()%1000 <= 50)
			{
				m_ButterflyColony[i].nAngle = rand()%360-180;
				D3DXMATRIX n_Temp;
				D3DXMatrixIdentity(&m_ButterflyColony[i].matZoomRot);//Ӧ����תһ�¾Ϳ�����,û��Ҫ�����½���������
				//����
				D3DXMatrixIdentity(&n_Temp);
				D3DXMatrixScaling(&n_Temp,
					m_ButterflyColony[i].fSize,
					m_ButterflyColony[i].fSize,
					m_ButterflyColony[i].fSize);
				D3DXMatrixMultiply(&m_ButterflyColony[i].matZoomRot,
					&m_ButterflyColony[i].matZoomRot,
					&n_Temp);
				//��ת
				D3DXMatrixIdentity(&n_Temp);
				D3DXMatrixRotationY(&n_Temp,m_ButterflyColony[i].nAngle*D3DX_PI/180);
				D3DXMatrixMultiply( &m_ButterflyColony[i].matZoomRot,
					&m_ButterflyColony[i].matZoomRot,
					&n_Temp);
			}
			//ƽ��
			D3DXMATRIX matTemp;
			D3DXMatrixIdentity(&matTemp);
			D3DXMatrixTranslation(	&matTemp,
				m_ButterflyColony[i].Pos.x,
				m_ButterflyColony[i].Pos.y,
				m_ButterflyColony[i].Pos.z);
			D3DXMatrixMultiply(&m_ButterflyColony[i].matWorld,&m_ButterflyColony[i].matZoomRot,&matTemp);

			float fTempAngle=m_ButterflyColony[i].fWingAngle*D3DX_PI/180;

			//���ݳ��ĽǶ��������������Ķ�������
			FLOAT X = cos(fTempAngle);
			FLOAT Y = sin(fTempAngle);

			D3DXVec3TransformCoord(&pVertex[4*i].Pos  ,&D3DXVECTOR3(-X,Y,0),&m_ButterflyColony[i].matWorld);
			D3DXVec3TransformCoord(&pVertex[4*i+1].Pos,&D3DXVECTOR3(0,0,1), &m_ButterflyColony[i].matWorld);
			D3DXVec3TransformCoord(&pVertex[4*i+2].Pos,&D3DXVECTOR3(0,0,-1),&m_ButterflyColony[i].matWorld);
			D3DXVec3TransformCoord(&pVertex[4*i+3].Pos,&D3DXVECTOR3(X,Y,0), &m_ButterflyColony[i].matWorld);
		//}
		//else
		//{
		//	//����ƽ��Ͷ��Ϳ��ӷ�Χ�ڵĺ���������ʾ
		//	m_ButterflyColony[i].bVisible = false;
		//}
	}

	m_pVertexBuffer->Unlock();
}

void CButterfly::RenderBufferfly()
{
	//��ȾЧ��
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
			//���ڿ��ӷ�Χ�ڵĺ���,������Ⱦ
			if (m_ButterflyColony[i].bVisible)
			{
				m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,4*i,2);
			}
		}
		m_ButterflyEffect->EndPass();
	}
	m_ButterflyEffect->End();
}
