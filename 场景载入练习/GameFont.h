#pragma once
#include "Header.h"
class CGameFont
{
private:
	D3DXFONT_DESC  m_FontDesc;//字体信息接口
	ID3DXFont*     m_pFont;   //字体接口

public:
	static CGameFont& GetInstance()
	{
		static CGameFont GameFont;
		return GameFont;
	}

	//初始化字体
	bool InitFont(LPDIRECT3DDEVICE9 pdevice);

	//绘制
	void Render(CHAR* txt,int lft,int tp,D3DCOLOR clr,int rht=WINDOW_WIDTH,
		int btm=WINDOW_HEIGHT,DWORD fmt=DT_LEFT|DT_TOP);

	CGameFont(void);
	~CGameFont(void);
};
