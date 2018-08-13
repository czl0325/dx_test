#include "GameFont.h"

CGameFont::CGameFont(void)
{
}

CGameFont::~CGameFont(void)
{
	SAFE_RELEASE(m_pFont);
}

bool CGameFont::InitFont( LPDIRECT3DDEVICE9 pdevice)
{
	if (FAILED(D3DXCreateFontA(pdevice,
		40,//高度0代表默认高度
		0,//宽度0代表默认宽度
		0,//权值	一般默认
		0,//过滤属性一般默认
		0,//是否斜体
		DEFAULT_CHARSET,//创建字体所属的字符集合
		OUT_DEFAULT_PRECIS,//输出字符的精度
		DEFAULT_QUALITY,//字符的输出自量 
		0,//指定字体的索引号
		"Arial",//表示字体名称的字符串
		&m_pFont)))
	{
		MessageBox(0,"创建字体格式失败",0,0);
		return false;
	}
	return true;
}

void CGameFont::Render( CHAR* txt,int lft,int tp,D3DCOLOR clr,int rht,int btm,DWORD fmt )
{
	RECT rect;				//字符串绘制矩形框
	rect.left	= lft;
	rect.top	= tp;
	rect.right	= rht;
	rect.bottom	= btm;
	m_pFont->DrawTextA(NULL,txt,-1,&rect,fmt,clr);
}
