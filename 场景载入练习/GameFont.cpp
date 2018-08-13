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
		40,//�߶�0����Ĭ�ϸ߶�
		0,//���0����Ĭ�Ͽ��
		0,//Ȩֵ	һ��Ĭ��
		0,//��������һ��Ĭ��
		0,//�Ƿ�б��
		DEFAULT_CHARSET,//���������������ַ�����
		OUT_DEFAULT_PRECIS,//����ַ��ľ���
		DEFAULT_QUALITY,//�ַ���������� 
		0,//ָ�������������
		"Arial",//��ʾ�������Ƶ��ַ���
		&m_pFont)))
	{
		MessageBox(0,"���������ʽʧ��",0,0);
		return false;
	}
	return true;
}

void CGameFont::Render( CHAR* txt,int lft,int tp,D3DCOLOR clr,int rht,int btm,DWORD fmt )
{
	RECT rect;				//�ַ������ƾ��ο�
	rect.left	= lft;
	rect.top	= tp;
	rect.right	= rht;
	rect.bottom	= btm;
	m_pFont->DrawTextA(NULL,txt,-1,&rect,fmt,clr);
}
