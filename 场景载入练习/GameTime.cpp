#include "GameTime.h"

CGameTime::CGameTime(void)
{
	m_nGameTime    = 0;
	m_nElapse      = 0;
	m_nFPS         = 1;
	m_nFrameCount  = 0;
}

CGameTime::~CGameTime(void)
{
}

void CGameTime::Update()
{
	static int nLastTime = timeGetTime();
	int nCurTime = timeGetTime();
	m_nElapse = nCurTime - nLastTime;
	nLastTime = nCurTime;
	m_nGameTime += m_nElapse;
	++m_nFrameCount;

	if ((m_nGameTime % 1000) < ((m_nGameTime - m_nElapse) % 1000))
	{
		m_nFPS = m_nFrameCount;
		m_nFrameCount = 0;
	}

	//if(m_nGameTime > 1.0f)
	//{
	//	m_nFPS = m_nFrameCount / m_nGameTime;
	//	m_nGameTime = 0;
	//	m_nFrameCount = 0;
	//}
}
