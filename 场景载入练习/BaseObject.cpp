#include "BaseObject.h"

CBaseObject::CBaseObject(void)
{
}

CBaseObject::~CBaseObject(void)
{
}

//创建外接体模型
bool CBaseObject::CreateBoundingVolumeMesh()
{
	//创建球型外接体
	if (FAILED(D3DXCreateSphere(m_pDevice,m_pBoundingVolume->GetBoundingSphere()->fRadius,
		20,20,&m_pSphere,NULL)))
	{
		return false;
	}
	return true;
}

//绘制外接体模型
bool CBaseObject::DrawBoundingVolumeMesh()
{
	m_pDevice->SetTransform(D3DTS_WORLD,&m_matBVWorld);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xb0ff0000);

	m_pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);
	m_pDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);
	m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);

	HRESULT hr = m_pSphere->DrawSubset(0);

	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
	m_pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);

	return true;
}

//更新外接体模型
bool CBaseObject::UpdateBoundingVolumeMesh()
{
	D3DXMATRIX m_matTranslation;
	
	D3DXMatrixIdentity(&m_matBVWorld);
	D3DXMatrixTranslation(&m_matTranslation,
		m_pBoundingVolume->GetBoundingSphere()->vCenter.x,
		m_pBoundingVolume->GetBoundingSphere()->vCenter.y,
		m_pBoundingVolume->GetBoundingSphere()->vCenter.z);
	
	m_matBVWorld = m_matTranslation;

	return true;
}

//角色是否在范围内
bool CBaseObject::IsRoleInRange( D3DXVECTOR3 vPos1,D3DXVECTOR3 vPos2,float fDistance )
{
	D3DXVECTOR3 vTemp = vPos1 - vPos2;
	float fTemp = D3DXVec3Length(&vTemp);
	if (fTemp < fDistance)
	{
		return true;
	}
	else
		return false;
}
