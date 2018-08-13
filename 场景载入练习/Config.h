#pragma once
#include "Header.h"
class CConfig
{
private:
	TCHAR m_szFileName[MAX_PATH];
public:
	CConfig(void);
	~CConfig(void);

	static CConfig& GetInstance(){static CConfig config;return config;}
	
	void SetConfigFileName(LPCTSTR szFileName)
	{
		_tcscpy_s(m_szFileName,MAX_PATH,szFileName);
	}

	LPCTSTR GetConfigFileName(void)const
	{
		return m_szFileName;
	}

	int ReadINI(IN LPCTSTR lpSection,IN LPCTSTR lpKey,IN INT nDefault = 0 OPTIONAL)
	{
		return GetPrivateProfileInt(lpSection,lpKey,nDefault,m_szFileName);
	}

	int ReadINI(LPCSTR lpAppName,LPCSTR lpKeyName,LPCSTR fileName,INT nDefault = 0)
	{
		return GetPrivateProfileInt(lpAppName,lpKeyName,nDefault,fileName);
	}

	void ReadINI(LPCSTR lpAppName,LPCSTR lpKeyName,LPCSTR lpDefault,
		LPSTR lpReturnedString,DWORD nSize,LPCSTR fileName)
	{
		GetPrivateProfileString(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize,fileName);
	}

	void WriteINI(IN LPCTSTR lpSection,IN LPCTSTR lpKey,IN int nValue)
	{
		TCHAR szBuff[16];
		_stprintf_s(szBuff,16,TEXT("%d"),nValue);	
		WriteINI(lpSection,lpKey,szBuff);
	}

	void ReadINI(OUT LPTSTR lpRetValue,IN LPCTSTR lpSection,IN LPCTSTR lpKey,IN LPCTSTR lpDefault = _TEXT("") OPTIONAL)
	{
		GetPrivateProfileString(lpSection,lpKey,lpDefault,lpRetValue,MAX_PATH,m_szFileName);
	}

	void WriteINI(IN LPCTSTR lpSection,IN LPCTSTR lpKey,IN LPCTSTR lpValue)
	{
		WritePrivateProfileString(lpSection,lpKey,lpValue,m_szFileName);
	}

	void ReadINI( LPCTSTR lpSection,LPCTSTR lpKey,LPTSTR lpReturnString,int nSize,LPCTSTR lpDefault = "")
	{
		GetPrivateProfileString(lpSection,lpKey,lpDefault,lpReturnString,nSize,m_szFileName);
	}
};
