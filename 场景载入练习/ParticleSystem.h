#pragma once
#include "Header.h"
#include "Terrain.h"

class CParticleSystem
{
protected:
	IDirect3DDevice9*			m_pD3dDevice;

public:
	CParticleSystem(void);
	~CParticleSystem(void);

	virtual BOOL InitParticle(IDirect3DDevice9* pD3dDevice) = 0;

	virtual VOID Update(CTerrain* Terrain,D3DXVECTOR3 vRlose) = 0;
	virtual VOID Render() = 0;
	virtual VOID Clearup() = 0;
};
