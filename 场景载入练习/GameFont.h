#pragma once
#include "Header.h"
class CGameFont
{
private:
	D3DXFONT_DESC  m_FontDesc;//������Ϣ�ӿ�
	ID3DXFont*     m_pFont;   //����ӿ�

public:
	static CGameFont& GetInstance()
	{
		static CGameFont GameFont;
		return GameFont;
	}

	//��ʼ������
	bool InitFont(LPDIRECT3DDEVICE9 pdevice);

	//����
	void Render(CHAR* txt,int lft,int tp,D3DCOLOR clr,int rht=WINDOW_WIDTH,
		int btm=WINDOW_HEIGHT,DWORD fmt=DT_LEFT|DT_TOP);

	CGameFont(void);
	~CGameFont(void);
};
