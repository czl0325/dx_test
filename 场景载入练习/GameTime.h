#pragma once
#include "Header.h"
class CGameTime
{
private:
	unsigned int m_nGameTime;
	unsigned int m_nElapse;
	unsigned int m_nFPS;
	unsigned int m_nFrameCount;
public:
	CGameTime(void);
	~CGameTime(void);
	static CGameTime& GetInstance()
	{
		static CGameTime timer;
		return timer;
	}
	void Update();
	unsigned int GetElapse(){return m_nElapse;}
	unsigned int GetFPS(){return m_nFPS;}
	unsigned int GetGameTime(){return m_nGameTime;}
};
