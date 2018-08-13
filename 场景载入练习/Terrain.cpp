#include "Terrain.h"

CTerrain::CTerrain(void)
{
	m_pDevice = NULL;
	m_pTerrainRoot = NULL;
	m_pfHeightValue = NULL;
	m_pVB = NULL;
	m_pIB = NULL;
	m_pVertices = NULL;
	m_pIndicesNear = NULL;
	m_nIndicesNearCurFrame = 0;
	m_pIndicesFar = NULL;
	m_nIndicesFarCurFrame = 0;

	m_nSize = 0;
	m_fCellScale = m_fHeightScale = 0.0f;

	m_fLODC1 = m_fLODC2 = 0.0f;
	m_pbMarkExist = NULL;

	m_ppListBrowseCurr = NULL;
	m_ppListBrowseNext = NULL;

	m_pTexSkeleton = NULL;
	m_pTexArrayDetail[0] = NULL;
	m_pTexArrayDetail[1] = NULL;
	m_pTexArrayDetail[2] = NULL;
	m_pTexAlpha = NULL;

	m_nDetailTexSize = 100;
	m_fAmbient = 0.0f;
	m_fDetailRange = 0.0f;

	m_pFaceNormal = NULL;


	m_bWire = NULL;
}

CTerrain::~CTerrain(void)
{
	CleanUp();
}

void CTerrain::BuildQuadTree( LPTERRAINQUADNODE pNode )//�����Ĳ���
{
	pNode->nNodeSize = pNode->nIndexCorner[1] - pNode->nIndexCorner[0]; //�ڵ�ߴ��㷨
	pNode->nIndexCenter = (pNode->nIndexCorner[0] + pNode->nIndexCorner[3]) / 2;//�ڵ������㷨

	if (pNode->nNodeSize > 2)
	{
		DWORD nTop = (pNode->nIndexCorner[0] + pNode->nIndexCorner[1]) / 2;//���е�
		DWORD nDown = (pNode->nIndexCorner[2] + pNode->nIndexCorner[3]) / 2;//���е�
		DWORD nLeft = (pNode->nIndexCorner[0] + pNode->nIndexCorner[2]) / 2;//���е�
		DWORD nRight = (pNode->nIndexCorner[1] + pNode->nIndexCorner[3]) / 2;//���е�
		DWORD nCenter = (nLeft + nRight) / 2;//���ĵ�

		//�ĸ����ӵĴ���
		pNode->pChild[0] = new TerrainQuadNode;
		pNode->pChild[0]->nIndexCorner[0] = pNode->nIndexCorner[0];
		pNode->pChild[0]->nIndexCorner[1] = nTop;
		pNode->pChild[0]->nIndexCorner[2] = nLeft;
		pNode->pChild[0]->nIndexCorner[3] = nCenter;
		BuildQuadTree(pNode->pChild[0]);

		pNode->pChild[1] = new TerrainQuadNode;
		pNode->pChild[1]->nIndexCorner[0] = nTop;
		pNode->pChild[1]->nIndexCorner[1] = pNode->nIndexCorner[1];
		pNode->pChild[1]->nIndexCorner[2] = nCenter;
		pNode->pChild[1]->nIndexCorner[3] = nRight;
		BuildQuadTree(pNode->pChild[1]);

		pNode->pChild[2] = new TerrainQuadNode;
		pNode->pChild[2]->nIndexCorner[0] = nLeft;
		pNode->pChild[2]->nIndexCorner[1] = nCenter;
		pNode->pChild[2]->nIndexCorner[2] = pNode->nIndexCorner[2];
		pNode->pChild[2]->nIndexCorner[3] = nDown;
		BuildQuadTree(pNode->pChild[2]);

		pNode->pChild[3] = new TerrainQuadNode;
		pNode->pChild[3]->nIndexCorner[0] = nCenter;
		pNode->pChild[3]->nIndexCorner[1] = nRight;
		pNode->pChild[3]->nIndexCorner[2] = nDown;
		pNode->pChild[3]->nIndexCorner[3] = pNode->nIndexCorner[3];
		BuildQuadTree(pNode->pChild[3]);

		pNode->BoundingVolume.Create(pNode->pChild[0]->BoundingVolume,pNode->pChild[1]->BoundingVolume,
			pNode->pChild[2]->BoundingVolume,pNode->pChild[3]->BoundingVolume);

		//ȡ�߶Ȳ��ƽ��ֵ
		float dh0 = fabsf(m_pVertices[nCenter].y - 
			(m_pVertices[pNode->nIndexCorner[1]].y + m_pVertices[pNode->nIndexCorner[2]].y) / 2.0f);
		float dh1 = fabsf(m_pVertices[nTop].y - 
			(m_pVertices[pNode->nIndexCorner[0]].y + m_pVertices[pNode->nIndexCorner[1]].y) / 2.0f);
		float dh2 = fabsf(m_pVertices[nRight].y - 
			(m_pVertices[pNode->nIndexCorner[1]].y + m_pVertices[pNode->nIndexCorner[3]].y) / 2.0f);
		float dh3 = fabsf(m_pVertices[nDown].y - 
			(m_pVertices[pNode->nIndexCorner[2]].y + m_pVertices[pNode->nIndexCorner[3]].y) / 2.0f);
		float dh4 = fabsf(m_pVertices[nLeft].y - 
			(m_pVertices[pNode->nIndexCorner[0]].y + m_pVertices[pNode->nIndexCorner[2]].y) / 2.0f);
		float dh5 = pNode->pChild[0]->fDeltaHeight;// �򻯸߶Ȳ�
		float dh6 = pNode->pChild[1]->fDeltaHeight;
		float dh7 = pNode->pChild[2]->fDeltaHeight;
		float dh8 = pNode->pChild[3]->fDeltaHeight;

		pNode->fDeltaHeight = max(dh0,max(dh1,max(dh2,max(dh3,max(dh4,max(dh5,max(dh6,max(dh7,dh8))))))));
	}
	else
	{
		pNode->pChild[0] = NULL;
		pNode->pChild[1] = NULL;
		pNode->pChild[2] = NULL;
		pNode->pChild[3] = NULL;
		pNode->fDeltaHeight = 0.0f;

		pNode->BoundingVolume.Create((D3DXVECTOR3*)&m_pVertices[pNode->nIndexCorner[0]],
			(D3DXVECTOR3*)&m_pVertices[pNode->nIndexCorner[1]],
			(D3DXVECTOR3*)&m_pVertices[pNode->nIndexCorner[2]],
			(D3DXVECTOR3*)&m_pVertices[pNode->nIndexCorner[3]]);
	}
}

void CTerrain::DestroyQuadTree( LPTERRAINQUADNODE pNode )//�����Ĳ���
{
	if (pNode->pChild[0] != NULL)
	{
		for (int i=0;i!=4;i++)
		{
			DestroyQuadTree(pNode->pChild[i]);
		}
	}

	delete pNode;
}

bool CTerrain::LoadHeightMap(LPCTSTR lpFileName)	// ���ظ߶�ͼ
{
	std::ifstream infile(lpFileName,std::ios_base::in|std::ios_base::binary);
	if (!infile)
		return false;

	int nTotal=m_nSize*m_nSize;
	BYTE* pValue=new BYTE[nTotal];
	if(pValue==NULL)
		return false;
	infile.read((char*)pValue,nTotal);
	infile.close();

	m_pfHeightValue=new float[nTotal];//���θ߶�ֵ����
	if(m_pfHeightValue==NULL){delete[] pValue;return false;}

	for (int i=0;i<nTotal;i++)
	{
		m_pfHeightValue[i]=(float)pValue[i]*m_fHeightScale;
	}

	return true;
}

bool CTerrain::CreateSkeletonAndAlphaTex( DWORD* pdwPixel )// ��������ͼ��ALPHAͼ
{
	D3DLOCKED_RECT rectlocked;//����һ�������ľ�������

	// ����ϸ������
	DWORD* pdwPixelDetail[3];
	for (int i=0;i<3;i++)
	{
		m_pTexArrayDetail[i]->LockRect(0,&rectlocked,0,D3DLOCK_READONLY);// ϸ����������
		pdwPixelDetail[i]=(DWORD*)rectlocked.pBits;
	}

	// ����������������
	if(FAILED(D3DXCreateTexture(m_pDevice,m_nSize-1,m_nSize-1,0,D3DUSAGE_DYNAMIC,
		D3DFMT_UNKNOWN,D3DPOOL_DEFAULT,&m_pTexSkeleton)))// ������������
		return false;

	m_pTexSkeleton->LockRect(0,&rectlocked,0,0);
	DWORD* pdwPixelSkeleton=(DWORD*)rectlocked.pBits;

	// ��������ALPHA����
	if (FAILED(D3DXCreateTexture(m_pDevice,m_nSize-1,m_nSize-1,0,D3DUSAGE_DYNAMIC,
		D3DFMT_UNKNOWN,D3DPOOL_DEFAULT,&m_pTexAlpha)))// ALPHAͼ
		return false;

	m_pTexAlpha->LockRect(0,&rectlocked,0,0);
	DWORD* pdwPixelAlpha=(DWORD*)rectlocked.pBits;

	//Ȩ��
	for (int i=0;i<m_nSize-1;i++)
	{
		for (int j=0;j<m_nSize-1;j++)
		{
			int index = i * (m_nSize - 1) + j;
			pdwPixelAlpha[index] = pdwPixel[index] << 24;//����24λΪaͨ��

			float fMountainRatio = (pdwPixel[index] & 0xff) / 255.0f;//����ɽ��ɫΪ��ɫ
			float fRoadRatio = ((pdwPixel[index] >> 16) & 0xff) / 255.0f;//����·��ɫΪ��ɫ
			float fGrassRatio = 1.0f - fMountainRatio - fRoadRatio;//���ò���ɫΪ��ɫ
			fGrassRatio = fGrassRatio > 0.0f ? fGrassRatio : 0.0f;

			pdwPixelSkeleton[index] = (D3DXCOLOR)pdwPixelDetail[0][index] * fGrassRatio +
				(D3DXCOLOR)pdwPixelDetail[1][index] * fRoadRatio + 
				(D3DXCOLOR)pdwPixelDetail[2][index] * fMountainRatio;
		}
	}

	for (int i=0;i!=3;++i)
	{
		m_pTexArrayDetail[i]->UnlockRect(0);// ϸ���������
	}
	m_pTexSkeleton->UnlockRect(0);// ���������������
	m_pTexAlpha->UnlockRect(0);// ALPHAͼ����

	return true;
}

bool CTerrain::InitVertexBuffer( DWORD* pdwPixel )// ��ʼ�����㻺��
{
	if (FAILED(m_pDevice->CreateVertexBuffer(m_nSize * m_nSize * sizeof(TERRAINVERTEX),
		D3DUSAGE_WRITEONLY,TERRAINVERTEX::FVF,D3DPOOL_MANAGED,&m_pVB,NULL)))
		return false;


	if (FAILED(m_pVB->Lock(0,0,(void**)&m_pVertices,0)))
		return false;


	for (int i=0,m=0;i<m_nSize;++i,++m)
	{
		if (m == m_nSize-1)
		{
			m = m_nSize - 2;
		}
		for (int j=0,n=0;j<m_nSize;++j,++n)
		{
			int index = i * m_nSize + j;
			m_pVertices[index].x = (j - m_nSize / 2) * m_fCellScale;
			m_pVertices[index].z = -(i - m_nSize / 2) * m_fCellScale;
			m_pVertices[index].y = m_pfHeightValue[index];			
			m_pVertices[index].u0 = 1.0f * j / (m_nSize - 1);
			m_pVertices[index].v0 = 1.0f * i / (m_nSize - 1);
			m_pVertices[index].u1 = j * m_fCellScale/m_nDetailTexSize;
			m_pVertices[index].v1 =i * m_fCellScale/m_nDetailTexSize;
		}
	}

	GenerateLightingMap(pdwPixel,D3DXVECTOR3(500.0f,500.0f,0.0f));// ���ɶ������

	m_pVB->Unlock();

	return true;
}


void CTerrain::GenerateLightingMap( DWORD* pdwPixel,D3DXVECTOR3& vLighting )
{
	D3DXVECTOR3 vPosition0,vPosition1,vPosition2,vPosition3,vPosition4,vNormal,vLightingDir;
	float fLightingIntensity;
	float fAlpha;


	for (int i=0,m=0;i!=m_nSize;++i,++m)
	{
		if (m == m_nSize-1)
		{
			m = 0;
		}

		for (int j=0,n=0;j<m_nSize;++j,++n)
		{			
			if (n == m_nSize-1)
			{
				n = m_nSize - 2;
			}

			fAlpha = ((pdwPixel[m * (m_nSize ) + n] >> 16) & 0xff) / 255.0f;//aͨ��

			int index = m_nSize*i+j; 

			vPosition0 = D3DXVECTOR3(m_pVertices[index].x,
				m_pVertices[index].y,m_pVertices[index].z);

			if (j == 0)
			{
				index = i * m_nSize;
			}
			else
			{
				index = i * m_nSize + j - 1;				
			}

			vPosition1 = D3DXVECTOR3(m_pVertices[index].x,
				m_pVertices[index].y,m_pVertices[index].z);

			if (j == m_nSize - 1)
			{
				index = i * m_nSize + j;
			}
			else
			{
				index = i * m_nSize + j + 1;				
			}
			vPosition2 = D3DXVECTOR3(m_pVertices[index].x,
				m_pVertices[index].y,m_pVertices[index].z);

			if (i == 0)
			{
				index = j;
			}
			else
			{
				index = (i - 1) * m_nSize + j;				
			}
			vPosition3 = D3DXVECTOR3(m_pVertices[index].x,
				m_pVertices[index].y,m_pVertices[index].z);

			if (i == m_nSize - 1)
			{
				index = i * m_nSize + j;
			}
			else
			{
				index = (i + 1) * m_nSize + j;				
			}
			vPosition4 = D3DXVECTOR3(m_pVertices[index].x,
				m_pVertices[index].y,m_pVertices[index].z);		


			D3DXVec3Cross(&vNormal,&(vPosition1 - vPosition2),&(vPosition3 - vPosition4));
			D3DXVec3Normalize(&vNormal,&vNormal);
			vLightingDir = vLighting /*- vPosition0*/;
			D3DXVec3Normalize(&vLightingDir,&vLightingDir);
			fLightingIntensity = D3DXVec3Dot(&vNormal,&vLightingDir);
			fLightingIntensity = fLightingIntensity > 0.0f ? fLightingIntensity : 0.0f;
			fLightingIntensity += m_fAmbient;

			index = i * m_nSize + j;
			m_pVertices[index].color = D3DXCOLOR(fLightingIntensity,fLightingIntensity,fLightingIntensity,fAlpha);
		}
	}
}

bool CTerrain::ComputeFaceNormals()// �����淨��
{
	m_pFaceNormal = new D3DXVECTOR3[(m_nSize - 1) * (m_nSize - 1) * 2];//�淨��
	if (m_pFaceNormal == NULL)
	{
		return false;
	}

	D3DXVECTOR3 V1,V2;
	for (int i=0;i!=m_nSize - 1;++i)
	{
		for (int j=0;j!=m_nSize - 1;++j)
		{
			V1 = *(D3DXVECTOR3*)&m_pVertices[i * m_nSize + j + 1] - *(D3DXVECTOR3*)&m_pVertices[i * m_nSize + j];
			V2 = *(D3DXVECTOR3*)&m_pVertices[(i + 1) * m_nSize + j] - *(D3DXVECTOR3*)&m_pVertices[i * m_nSize + j];
			D3DXVec3Cross(&m_pFaceNormal[(i * (m_nSize - 1) + j) * 2],&V1,&V2);
			D3DXVec3Normalize(&m_pFaceNormal[(i * (m_nSize - 1) + j) * 2],&m_pFaceNormal[(i * (m_nSize - 1) + j) * 2]);

			V1 = *(D3DXVECTOR3*)&m_pVertices[(i + 1) * m_nSize + j] - *(D3DXVECTOR3*)&m_pVertices[(i + 1) * m_nSize + j + 1];
			V2 = *(D3DXVECTOR3*)&m_pVertices[i * m_nSize + j + 1] - *(D3DXVECTOR3*)&m_pVertices[(i + 1) * m_nSize + j + 1];			
			D3DXVec3Cross(&m_pFaceNormal[(i * (m_nSize - 1) + j) * 2 + 1],&V1,&V2);
			D3DXVec3Normalize(&m_pFaceNormal[(i * (m_nSize - 1) + j) * 2 + 1],&m_pFaceNormal[(i * (m_nSize - 1) + j) * 2 + 1]);
		}
	}

	return true;
}

bool CTerrain::Initialize( LPDIRECT3DDEVICE9 pdevice )//��ʼ������
{
	m_pDevice=pdevice;
	CConfig::GetInstance().SetConfigFileName("res//ini//initialize.ini");
	m_nSize=CConfig::GetInstance().ReadINI("Terrain","TerrainSize");//��ͼ�ߴ�257
	m_nDetailTexSize=CConfig::GetInstance().ReadINI("Terrain","DetailTextureSize");//ϸ�������С100
	if (m_nSize==0||m_nDetailTexSize==0){return false;}

	m_fCellScale=CConfig::GetInstance().ReadINI("Terrain","CellScale")/100.0f;//��Ԫ�����ű���1000
	m_fHeightScale=CConfig::GetInstance().ReadINI("Terrain","HeightScale")/100.0f;//�߶����ű���100
	m_fLODC1=CConfig::GetInstance().ReadINI("Terrain","LODC1")/100.0f;//LOD1  100
	m_fLODC2=CConfig::GetInstance().ReadINI("Terrain","LODC2")/100.0f;//LOD2  100
	m_fAmbient=CConfig::GetInstance().ReadINI("Terrain","AmbientLight")/100.0f;// ������40
	m_fDetailRange=CConfig::GetInstance().ReadINI("Terrain","DetailRange")/100.0f;//ϸ����ͼ��Χ500000

	// �����߶�ͼ
	TCHAR buff[MAX_PATH];
	CConfig::GetInstance().ReadINI(buff,"Terrain","HeightMapFileName");//��ȡ�߶�ͼ
	if(!LoadHeightMap(buff))
		return false;

	//����ϸ������
	TCHAR lpKey[64];
	for (int i=0;i<3;i++)
	{
		_stprintf_s(lpKey,64,"Detail%dTextureFileName",i);
		CConfig::GetInstance().ReadINI(buff,"Terrain",lpKey);
		if(FAILED(D3DXCreateTextureFromFile(m_pDevice,buff,&m_pTexArrayDetail[i])))//ϸ������
			return false;
	}

	// ����ALPHA����
	LPDIRECT3DTEXTURE9 pTexAlpha;
	CConfig::GetInstance().ReadINI(buff,"Terrain","AlphaTextureFileName");
	if(FAILED(D3DXCreateTextureFromFile(m_pDevice,buff,&pTexAlpha)))
		return false;
	D3DLOCKED_RECT rectTex;
	pTexAlpha->LockRect(0,&rectTex,0,D3DLOCK_READONLY);
	DWORD* pdwPixel=(DWORD*)rectTex.pBits;
	//�������������Alpha����
	if(!CreateSkeletonAndAlphaTex(pdwPixel))
		return false;

	//��ʼ�����㻺��
	if(!InitVertexBuffer(pdwPixel))
		return false;

	pTexAlpha->UnlockRect(0);
	pTexAlpha->Release();

	// �����淨��
	if (!ComputeFaceNormals())
		return false;

	//�����Ĳ���
	m_pTerrainRoot=new TerrainQuadNode;//�����Ĳ����ڵ�
	if(m_pTerrainRoot==NULL)
		return false;

	m_pTerrainRoot->nIndexCorner[0]=0;//��1���Ƕ���
	m_pTerrainRoot->nIndexCorner[1]=m_nSize-1;//��2���Ƕ���
	m_pTerrainRoot->nIndexCorner[2]=(m_nSize-1)*m_nSize;//��3���Ƕ���
	m_pTerrainRoot->nIndexCorner[3]=m_nSize*m_nSize-1;//��4���Ƕ���
	BuildQuadTree(m_pTerrainRoot);

	// ����ÿ֡��Ⱦ�������������
	m_pIndicesNear=new DWORD[(m_nSize-1)*(m_nSize-1)*6];// ÿ֡������Ҫ��Ⱦ�Ķ���������������
	if(m_pIndicesNear==NULL)
		return false;
	m_pIndicesFar=new DWORD[(m_nSize-1)*(m_nSize-1)*6];// ÿ֡������Ҫ��Ⱦ�Ķ���������Զ����
	if(m_pIndicesFar==NULL)
		return false;

	// �����������
	m_pbMarkExist = new bool[m_nSize * m_nSize];// ��������ʾ�ڵ��Ƿ����
	if (m_pbMarkExist == NULL)
		return false;

	// ������������
	m_ppListBrowseCurr = new TerrainQuadNode*[(m_nSize-1)*(m_nSize-1)/4];// ������ǰ��ζ���
	if (m_ppListBrowseCurr == NULL)
		return false;
	m_ppListBrowseNext = new TerrainQuadNode*[(m_nSize-1)*(m_nSize-1)/4];//��һ�����
	if (m_ppListBrowseNext == NULL)
		return false;

	return true;
}

void CTerrain::CleanUp()
{
	if (m_pfHeightValue != NULL)
	{
		delete []m_pfHeightValue;
	}

	if (m_pTexSkeleton != NULL)
	{
		m_pTexSkeleton->Release();
	}

	if (m_pTexAlpha != NULL)
	{
		m_pTexAlpha->Release();
	}

	for (int i=0;i!=3;++i)
	{
		if (m_pTexArrayDetail[i] != NULL)
		{
			m_pTexArrayDetail[i]->Release();
		}
	}

	if (m_pVB != NULL)
	{
		m_pVB->Release();
	}

	if (m_pIndicesNear != NULL)
	{
		delete []m_pIndicesNear;
	}
	if (m_pIndicesFar != NULL)
	{
		delete []m_pIndicesFar;
	}

	if (m_pbMarkExist != NULL)
	{
		delete []m_pbMarkExist;
	}

	if (m_ppListBrowseCurr != NULL)
	{
		delete []m_ppListBrowseCurr;
	}

	if (m_ppListBrowseNext != NULL)
	{
		delete []m_ppListBrowseNext;
	}

	if (m_pFaceNormal != NULL)
	{
		delete []m_pFaceNormal;
	}

	if (m_pTerrainRoot != NULL)
	{
		DestroyQuadTree(m_pTerrainRoot);
	}
}

void CTerrain::Update( D3DXVECTOR3& vCamera,D3DXVECTOR3& vPlayerPos )
{
	D3DXMATRIX matPoj;//�����Ӿ�����
	D3DXMATRIX matView;//����ͶӰ����
	m_nIndicesNearCurFrame = 0;//����������ǰ֡��Ⱦ��������Ϊ0
	m_nIndicesFarCurFrame = 0;//��Զ������ǰ֡��Ⱦ��������Ϊ0
	m_pDevice ->GetTransform(D3DTS_PROJECTION,&matPoj);
	m_pDevice ->GetTransform(D3DTS_VIEW,&matView);
	m_Frustum.Update(matPoj*matView);
	BrowseQuadTree(m_pTerrainRoot,&m_Frustum,vCamera,vPlayerPos);
}

void CTerrain::BrowseQuadTree( LPTERRAINQUADNODE pRoot,CFrustum* pFrustum,D3DXVECTOR3& vCamera,D3DXVECTOR3& vPlayerPos )
//�Ĳ������ڵ㣬ƽ��ͷ��Ķ����������������������
{
	int nListCurrTotal = 1;
	int nListNextTotal = 0;

	m_ppListBrowseCurr[0] = pRoot;
	TerrainQuadNode* pNode;
	LPTERRAINQUADNODE* ppListTemp;
	while (true)
	{
		for (int i=0;i!=nListCurrTotal;++i)
		{
			pNode = m_ppListBrowseCurr[i];
			//if (!pFrustum->IsSphereIn(pNode->BoundingVolume.GetBoundingSphere()))
			//{
			//	continue;
			//}
			//if (!pFrustum->IsBoxIn(pNode->BoundingVolume.GetBoundingBox()))
			//{
			//	continue;
			//}

			if (pNode->nNodeSize > 2)//�ڵ�ߴ�>2
			{
				if (NodeCanDivided(pNode))//�ڵ��Ƿ�����ָ�
				{
					float fDist = D3DXVec3Length(&(vCamera - *(D3DXVECTOR3*)&m_pVertices[pNode->nIndexCenter]));
					float fResult = (pNode->fDeltaHeight * pNode->nNodeSize * m_fCellScale * m_fLODC1 * m_fLODC2) / fDist;
					if (fResult > 1)
					{
						m_ppListBrowseNext[nListNextTotal++] = pNode->pChild[0];
						m_ppListBrowseNext[nListNextTotal++] = pNode->pChild[1];
						m_ppListBrowseNext[nListNextTotal++] = pNode->pChild[2];
						m_ppListBrowseNext[nListNextTotal++] = pNode->pChild[3];

						m_pbMarkExist[pNode->pChild[0]->nIndexCenter] = true;
						m_pbMarkExist[pNode->pChild[1]->nIndexCenter] = true;
						m_pbMarkExist[pNode->pChild[2]->nIndexCenter] = true;
						m_pbMarkExist[pNode->pChild[3]->nIndexCenter] = true;

						continue;					
					}
				}				

				m_pbMarkExist[pNode->pChild[0]->nIndexCenter] = false;
				m_pbMarkExist[pNode->pChild[1]->nIndexCenter] = false;
				m_pbMarkExist[pNode->pChild[2]->nIndexCenter] = false;
				m_pbMarkExist[pNode->pChild[3]->nIndexCenter] = false;
			}

			// �����޳�
			AddNodeToDraw(pNode,vPlayerPos);// �����Ⱦ�ڵ�
		}

		if (nListNextTotal == 0)
		{
			break;
		}

		ppListTemp = m_ppListBrowseCurr;
		m_ppListBrowseCurr = m_ppListBrowseNext;
		m_ppListBrowseNext = ppListTemp;
		nListCurrTotal = nListNextTotal;
		nListNextTotal = 0;
	}	
}

bool CTerrain::NodeCanDivided( LPTERRAINQUADNODE pNode )
{
	DWORD nLeft = pNode->nIndexCenter - pNode->nNodeSize;
	if (nLeft / m_nSize == pNode->nIndexCenter / m_nSize)
	{
		if (!m_pbMarkExist[nLeft])
		{
			return false;
		}
	}

	DWORD nTop = pNode->nIndexCenter - pNode->nNodeSize * m_nSize;
	if (nTop / m_nSize + pNode->nNodeSize == pNode->nIndexCenter / m_nSize)
	{
		if (!m_pbMarkExist[nTop])
		{
			return false;
		}
	}

	DWORD nRight= pNode->nIndexCenter + pNode->nNodeSize;
	if (nRight / m_nSize == pNode->nIndexCenter / m_nSize)
	{
		if (!m_pbMarkExist[nRight])
		{
			return false;
		}
	}

	DWORD nDown = pNode->nIndexCenter + pNode->nNodeSize * m_nSize;
	if (nDown < (DWORD)m_nSize * m_nSize)
	{
		if (!m_pbMarkExist[nDown])
		{
			return false;
		}
	}

	return true;
}

void CTerrain::AddNodeToDraw( LPTERRAINQUADNODE pNode,D3DXVECTOR3& vPlayerPos )
{
	DWORD* pIndices;
	int* pIndicesCurPos;
	float fDist = D3DXVec3Length(&(vPlayerPos - *(D3DXVECTOR3*)&m_pVertices[pNode->nIndexCenter]));
	if (fDist < m_fDetailRange)
	{
		pIndices = m_pIndicesNear;
		pIndicesCurPos = &m_nIndicesNearCurFrame;
	}
	else
	{
		pIndices = m_pIndicesFar;
		pIndicesCurPos = &m_nIndicesFarCurFrame;
	}

	DWORD nTop = (pNode->nIndexCorner[0] + pNode->nIndexCorner[1]) / 2;
	DWORD nDown = (pNode->nIndexCorner[2] + pNode->nIndexCorner[3]) / 2;
	DWORD nLeft = (pNode->nIndexCorner[0] + pNode->nIndexCorner[2]) / 2;
	DWORD nRight = (pNode->nIndexCorner[1] + pNode->nIndexCorner[3]) / 2;

	// �ж���߽ڵ��Ƿ����
	DWORD nLeftMark = pNode->nIndexCenter - pNode->nNodeSize;
	if (nLeftMark / m_nSize == pNode->nIndexCenter / m_nSize
		&& !m_pbMarkExist[nLeftMark])
	{
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCenter;
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCorner[2];
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCorner[0];
	}
	else
	{
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCenter;
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCorner[2];
		pIndices[(*pIndicesCurPos)++] = nLeft;
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCenter;
		pIndices[(*pIndicesCurPos)++] = nLeft;
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCorner[0];
	}
	// �ж��ϱ߽ڵ��Ƿ����
	DWORD nTopMark = pNode->nIndexCenter - pNode->nNodeSize * m_nSize;
	if (nTopMark / m_nSize + pNode->nNodeSize == pNode->nIndexCenter / m_nSize
		&& !m_pbMarkExist[nTopMark])
	{
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCenter;
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCorner[0];
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCorner[1];
	}
	else
	{
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCenter;
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCorner[0];
		pIndices[(*pIndicesCurPos)++] = nTop;
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCenter;
		pIndices[(*pIndicesCurPos)++] = nTop;
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCorner[1];
	}

	// �ж��ұ߽ڵ��Ƿ����
	DWORD nRightMark= pNode->nIndexCenter + pNode->nNodeSize;
	if (nRightMark / m_nSize == pNode->nIndexCenter / m_nSize
		&& !m_pbMarkExist[nRightMark])
	{
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCenter;
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCorner[1];
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCorner[3];
	}
	else
	{
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCenter;
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCorner[1];
		pIndices[(*pIndicesCurPos)++] = nRight;
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCenter;
		pIndices[(*pIndicesCurPos)++] = nRight;
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCorner[3];
	}

	// �ж��±߽ڵ��Ƿ����
	DWORD nDownMark = pNode->nIndexCenter + pNode->nNodeSize * m_nSize;
	if (nDownMark < (DWORD)m_nSize * m_nSize && !m_pbMarkExist[nDownMark])
	{
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCenter;
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCorner[3];
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCorner[2];
	}
	else
	{
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCenter;
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCorner[3];
		pIndices[(*pIndicesCurPos)++] = nDown;
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCenter;
		pIndices[(*pIndicesCurPos)++] = nDown;
		pIndices[(*pIndicesCurPos)++] = pNode->nIndexCorner[2];
	}	
}

void CTerrain::Render()
{
	if (m_bWire == true)
	{
		m_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	}
	else
	{
		m_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	}
	m_pDevice ->SetRenderState(D3DRS_LIGHTING,FALSE);
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	m_pDevice->SetTransform(D3DTS_WORLD,&matWorld);

	m_pDevice->SetStreamSource(0,m_pVB,0,sizeof(TerrainVertex));
	m_pDevice->SetFVF(TerrainVertex::FVF);

	DWORD* pIndices;

	// ��ȾԶ��������
	// ������������	
	if (m_nIndicesFarCurFrame != 0)
	{
		if (FAILED(m_pDevice->CreateIndexBuffer(m_nIndicesFarCurFrame * sizeof(DWORD),
			D3DUSAGE_WRITEONLY,D3DFMT_INDEX32,D3DPOOL_MANAGED,&m_pIB,NULL)))
		{
			return;
		}
		if (FAILED(m_pIB->Lock(0,0,(void**)&pIndices,0)))
		{
			m_pIB->Release();
			m_pIB = NULL;
			return;
		}
		memcpy(pIndices,m_pIndicesFar,m_nIndicesFarCurFrame * sizeof(DWORD));
		m_pIB->Unlock();

		PreRender(false);
		m_pDevice->SetIndices(m_pIB);
		m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,
			m_nSize * m_nSize,0,m_nIndicesFarCurFrame / 3);	

		m_pIB->Release();
	}

	// ��Ⱦ����������
	// ������������	
	if (FAILED(m_pDevice->CreateIndexBuffer(m_nIndicesNearCurFrame * sizeof(DWORD),
		D3DUSAGE_WRITEONLY,D3DFMT_INDEX32,D3DPOOL_MANAGED,&m_pIB,NULL)))
	{
		return;
	}
	if (FAILED(m_pIB->Lock(0,0,(void**)&pIndices,0)))
	{
		m_pIB->Release();
		m_pIB = NULL;
		return;
	}
	memcpy(pIndices,m_pIndicesNear,m_nIndicesNearCurFrame * sizeof(DWORD));
	m_pIB->Unlock();

	PreRender(true);	
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,
		m_nSize * m_nSize,0,m_nIndicesNearCurFrame / 3);	

	m_pIB->Release();
	m_pIB = NULL;
	m_pDevice ->SetRenderState(D3DRS_LIGHTING,FALSE);
	AfterRender();
}

void CTerrain::PreRender( bool bNear )
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING,false);
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);//������Ȼ���
	m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);    //��Ȼ���
	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);//����ģʽ ���߳��κ���
	if (!bNear)
	{
		m_pDevice->SetTexture(0,m_pTexSkeleton);
		m_pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
		m_pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	}
	else
	{
		m_pDevice->SetTexture(0,m_pTexArrayDetail[0]);
		m_pDevice->SetTexture(1,m_pTexArrayDetail[1]);
		m_pDevice->SetTexture(2,m_pTexAlpha);
		m_pDevice->SetTexture(3,m_pTexArrayDetail[2]);

		m_pDevice->SetSamplerState(1,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
		m_pDevice->SetSamplerState(2,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
		m_pDevice->SetSamplerState(3,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);

		m_pDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,1);
		m_pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);

		m_pDevice->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,1);
		m_pDevice->SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_BLENDDIFFUSEALPHA);

		m_pDevice->SetTextureStageState(2,D3DTSS_TEXCOORDINDEX,0);
		m_pDevice->SetTextureStageState(2,D3DTSS_COLORARG2,D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(2,D3DTSS_COLOROP,D3DTOP_SELECTARG2);
		m_pDevice->SetTextureStageState(2,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(2,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);

		m_pDevice->SetTextureStageState(3,D3DTSS_TEXCOORDINDEX,1);
		m_pDevice->SetTextureStageState(3,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(3,D3DTSS_COLORARG2,D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(3,D3DTSS_COLOROP,D3DTOP_BLENDCURRENTALPHA);

		m_pDevice->SetTextureStageState(4,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(4,D3DTSS_COLORARG2,D3DTA_CURRENT);
		m_pDevice->SetTextureStageState(4,D3DTSS_COLOROP,D3DTOP_MODULATE);
	}	
}


void CTerrain::AfterRender()
{
	m_pDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	m_pDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	m_pDevice->SetTextureStageState(2,D3DTSS_COLOROP,D3DTOP_DISABLE);
	m_pDevice->SetTextureStageState(3,D3DTSS_COLOROP,D3DTOP_DISABLE);
	m_pDevice->SetTextureStageState(4,D3DTSS_COLOROP,D3DTOP_DISABLE);

	m_pDevice->SetTextureStageState(2,D3DTSS_ALPHAOP,D3DTOP_DISABLE);

	m_pDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
	m_pDevice->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,1);
}


float CTerrain::GetHeightValue(float x,float z)
{
	float fRow = -z / m_fCellScale + m_nSize / 2;
	float fCol = x / m_fCellScale + m_nSize / 2;
	int nRow =	(int)fRow;
	int nCol = (int)fCol;
	float u = fCol - nCol;
	float v = fRow - nRow;
	if (u + v < 1.0f)
	{
		float fHeight1 = GetHeightMap(nRow,nCol);
		float fHeight2 = GetHeightMap(nRow,nCol + 1);
		float fHeight3 = GetHeightMap(nRow + 1,nCol);
		return fHeight1 + u * (fHeight2 - fHeight1) + v * (fHeight3 - fHeight1);
	}
	else
	{
		float fHeight1 = GetHeightMap(nRow + 1,nCol + 1);
		float fHeight2 = GetHeightMap(nRow + 1,nCol);
		float fHeight3 = GetHeightMap(nRow,nCol + 1);
		u = 1.0f - u;
		v = 1.0f - v;
		return fHeight1 + u * (fHeight2 - fHeight1) + v * (fHeight3 - fHeight1);
	}
}

//�ж����ߺ��������Ƿ��ཻ
bool CTerrain::FixForCamera( OUT D3DXVECTOR3& vDestPos,IN D3DXVECTOR3& vRole,IN D3DXVECTOR3& vCamera,IN FLOAT MinDist )
{
	bool bIntersect = false;
	INT nTriangle = m_nIndicesNearCurFrame / 3;
	float fDist;
	D3DXVECTOR3 vRayDir = vCamera - vRole;
	float u,v;
	D3DXVECTOR3 *vP0,*vP1,*vP2,vNormal;
	for (int i=0;i!=nTriangle;++i)
	{
		//����ÿ��������
		vP0 = (D3DXVECTOR3*)&m_pVertices[m_pIndicesNear[3 * i]];
		vP1 = (D3DXVECTOR3*)&m_pVertices[m_pIndicesNear[3 * i + 1]];
		vP2 = (D3DXVECTOR3*)&m_pVertices[m_pIndicesNear[3 * i + 2]];
		if (D3DXIntersectTri(vP0,vP1,vP2,&vRole,&vRayDir,&u,&v,&fDist))//������ߺ��������ཻ
		{
			if (fDist < MinDist)
			{
				bIntersect = true;
				MinDist = fDist;
				vDestPos = *vP0 + u * (*vP1 - *vP0) + v * (*vP2 - *vP0);
			}
		}
	}
	return bIntersect;
}

//ʰȡ
bool CTerrain::FindDestForPlayMove( OUT D3DXVECTOR3& vDestPos,OUT D3DXMATRIX& matRotation,IN D3DXVECTOR3& vCamera  )
{
	// ��ȡ�󱸻����С
	LPDIRECT3DSURFACE9 pBackBufferSurface;

	if (FAILED(m_pDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBufferSurface)))
		return false;

	D3DSURFACE_DESC backSurfaceDesc;
	if (FAILED(pBackBufferSurface->GetDesc(&backSurfaceDesc)))
		return false;

	int nClientWidth = backSurfaceDesc.Width;
	int nClientHeight = backSurfaceDesc.Height;
	pBackBufferSurface->Release();

	// ������ͶӰƽ�潻��
	D3DXVECTOR3 vProjectionPt;
	vProjectionPt.x = (CInput::GetInstance().GetMousePosX() - nClientWidth / 2.0f) / nClientWidth * 2.0f;
	vProjectionPt.y = -(CInput::GetInstance().GetMousePosY() - nClientHeight / 2.0f) / nClientHeight * 2.0f;
	vProjectionPt.z = 0.0f;

	D3DXMATRIX matProj;
	m_pDevice->GetTransform(D3DTS_PROJECTION,&matProj);
	D3DXMATRIX matProjInv;
	D3DXMatrixInverse(&matProjInv,NULL,&matProj);//��ͶӰ����������
	D3DXMATRIX matView;
	m_pDevice->GetTransform(D3DTS_VIEW,&matView);
	D3DXMATRIX matViewInv;
	D3DXMatrixInverse(&matViewInv,NULL,&matView);//���Ӿ�����������
	D3DXMATRIX matViewProjInv = matProjInv * matViewInv;

	// �������������
	D3DXVECTOR3 vWorldPt;
	D3DXVec3TransformCoord(&vWorldPt,&vProjectionPt,&matViewProjInv);

	// ���߷���Ϊ����-�����
	D3DXVECTOR3 vRayDir = vWorldPt - vCamera;

	bool bIntersect = false;
	float fDist,fMinDist = 100000.0f;
	float u,v;
	D3DXVECTOR3 *vP0,*vP1,*vP2,vNormal;

	// �жϽ����ཻ
	int nTriangle = m_nIndicesNearCurFrame / 3;
	for (int i=0;i!=nTriangle;++i)
	{
		vP0 = (D3DXVECTOR3*)&m_pVertices[m_pIndicesNear[3*i]];
		vP1 = (D3DXVECTOR3*)&m_pVertices[m_pIndicesNear[3*i + 1]];
		vP2 = (D3DXVECTOR3*)&m_pVertices[m_pIndicesNear[3*i + 2]];
		if (D3DXIntersectTri(vP0,vP1,vP2,&vCamera,&vRayDir,&u,&v,&fDist))
		{
			bIntersect = true;
			if (fDist < fMinDist)
			{
				fMinDist = fDist;
				vDestPos = *vP0 + u * (*vP1 - *vP0) + v * (*vP2 - *vP0);
				D3DXVec3Cross(&vNormal,&(*vP1 - *vP0),&(*vP2 - *vP0));//��������Ĳ������
				D3DXMatrixRotationYawPitchRoll(&matRotation,0.0f,D3DX_PI / 2 - atan2f(vNormal.y,vNormal.z),
					atan2f(vNormal.y,vNormal.x) - D3DX_PI / 2);
			}
		}
	}

	if (!bIntersect)
	{
		// �ж�Զ���ཻ
		nTriangle = m_nIndicesFarCurFrame / 3;
		for (int i=0;i!=nTriangle;++i)
		{
			vP0 = (D3DXVECTOR3*)&m_pVertices[m_pIndicesFar[3*i]];
			vP1 = (D3DXVECTOR3*)&m_pVertices[m_pIndicesFar[3*i + 1]];
			vP2 = (D3DXVECTOR3*)&m_pVertices[m_pIndicesFar[3*i + 2]];
			if (D3DXIntersectTri(vP0,vP1,vP2,&vCamera,&vRayDir,&u,&v,&fDist))
			{
				bIntersect = true;
				if (fDist < fMinDist)
				{
					fMinDist = fDist;
					vDestPos = *vP0 + u * (*vP1 - *vP0) + v * (*vP2 - *vP0);
				}
			}
		}
	}

	return bIntersect;	
}

bool CTerrain::CollideWithCamera( D3DXVECTOR3 vRayOrigin,D3DXVECTOR3 vRayDir,D3DXVECTOR3& vHitPoint,float& fMinDist )
{
	D3DXVECTOR3 *pV0,*pV1,*pV2;
	D3DXVECTOR3 vDistance;
	float u,v;
	float fDist,fIntersectDist;
	bool bIntersect = false;
	int m_nNumTriangles = m_nIndicesNearCurFrame / 3;

	for (int i=0;i<m_nNumTriangles;i++)
	{
		pV0 = (D3DXVECTOR3*)&m_pVertices[m_pIndicesNear[3*i]];
		pV1 = (D3DXVECTOR3*)&m_pVertices[m_pIndicesNear[3*i+1]];
		pV2 = (D3DXVECTOR3*)&m_pVertices[m_pIndicesNear[3*i+2]];
		vDistance = *pV0 - vRayOrigin;
		fDist = D3DXVec3Length(&vDistance);
		if (fDist <= 300.0f)
		{
			if (D3DXIntersectTri(pV0,pV1,pV2,&vRayOrigin,&vRayDir,&u,&v,&fIntersectDist))
			{
				if (fIntersectDist < fMinDist)
				{
					bIntersect = true;
					fMinDist = fIntersectDist;
					vHitPoint = *pV0 + u * (*pV1 - *pV0) + v * (*pV2 - *pV0);
				}
			}
		}
	}
	return bIntersect;
}
