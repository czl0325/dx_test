#pragma once
#include "Header.h"
#include "GameTime.h"

//--键盘的状态，只给出up,down,right,left这四个键
struct CKeyControl
{
	int m_nRight;
	int m_nLeft;
	int m_nUp;
	int m_nDown;
	POINT  pos;
};

//--鼠标的状态只给出左右键按下和鼠标当前位置
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
	LPDIRECTINPUT8          m_pDI;         //--DI对象
	LPDIRECTINPUTDEVICE8    m_pMouse;      //--鼠标设备
	LPDIRECTINPUTDEVICE8    m_pKeyboard;   //--键盘设备
	CKeyControl             m_pKeyControl;  //--键盘状态
	CMouseState             m_OldMState;   //--鼠标前一个状态
	CMouseState             m_CurMState;   //--鼠标当前状态

	DIMOUSESTATE2			dims2;
	LONG					m_lOldlZ;
	LONG					m_lNowlZ;
	BOOL					m_bWhell;//滚轮是否有动

	BYTE                    KeyboardState[256]; //--保存键盘上的所有键码
	unsigned int	        m_nKeyStamp[256];
	unsigned int            m_nMouseStamp[4];


	int                     MouseX;        //--鼠标X方向坐标
	int                     MouseY;        //--鼠标Y方向坐标
	HWND	                m_hWnd;


public:
	CInput(void);
	~CInput(void);
	static CInput& GetInstance()
	{
		static CInput input;
		return input;
	}
	BOOL InitDInput(HWND hWnd,HINSTANCE hInstance);//初始化DIput设备
	BOOL ReadMouse();//读取鼠标状态
	BOOL ReadKeyboard();//读取键盘状态
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
