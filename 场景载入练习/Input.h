#pragma once
#include "Header.h"
#include "GameTime.h"

//--���̵�״̬��ֻ����up,down,right,left���ĸ���
struct CKeyControl
{
	int m_nRight;
	int m_nLeft;
	int m_nUp;
	int m_nDown;
	POINT  pos;
};

//--����״ֻ̬�������Ҽ����º���굱ǰλ��
struct CMouseState
{
	BOOL Left;
	BOOL Right;
	POINT Pos;
	int WheelZ;
};

enum{ LBUTTON = 0,RBUTTON = 1,MBUTTON = 2 };

class CInput
{
public:
	LPDIRECTINPUT8          m_pDI;         //--DI����
	LPDIRECTINPUTDEVICE8    m_pMouse;      //--����豸
	LPDIRECTINPUTDEVICE8    m_pKeyboard;   //--�����豸
	CKeyControl             m_pKeyControl;  //--����״̬
	CMouseState             m_OldMState;   //--���ǰһ��״̬
	CMouseState             m_CurMState;   //--��굱ǰ״̬

	DIMOUSESTATE2			dims2;
	LONG					m_lOldlZ;
	LONG					m_lNowlZ;
	BOOL					m_bWhell;//�����Ƿ��ж�

	BYTE                    KeyboardState[256]; //--��������ϵ����м���
	unsigned int	        m_nKeyStamp[256];
	unsigned int            m_nMouseStamp[4];


	int                     MouseX;        //--���X��������
	int                     MouseY;        //--���Y��������
	HWND	                m_hWnd;


public:
	CInput(void);
	~CInput(void);
	static CInput& GetInstance()
	{
		static CInput input;
		return input;
	}
	BOOL InitDInput(HWND hWnd,HINSTANCE hInstance);//��ʼ��DIput�豸
	BOOL ReadMouse();//��ȡ���״̬
	BOOL ReadKeyboard();//��ȡ����״̬
	void CleanUp();
	bool IsKeyDown(DWORD dwKeyCode,bool bIgnoreStamp);


	long GetMousePosX()
	{
		POINT pt;
		::GetCursorPos(&pt);
		::ScreenToClient(m_hWnd,&pt);
		return pt.x;
	}

	long GetMousePosY()
	{
		POINT pt;
		::GetCursorPos(&pt);
		::ScreenToClient(m_hWnd,&pt);
		return pt.y;
	}

	long GetMouseWheel(){return dims2.lZ;}

};
