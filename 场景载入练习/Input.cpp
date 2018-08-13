#include "Input.h"

CInput::CInput(void)
{
	m_pDI=NULL;
	m_pMouse=NULL;
	m_pKeyboard=NULL;

	m_pKeyControl.m_nDown=FALSE;
	m_pKeyControl.m_nLeft=FALSE;
	m_pKeyControl.m_nRight=FALSE;
	m_pKeyControl.m_nUp=FALSE;

	//鼠标的前一个状态
	m_OldMState.Left=FALSE;
	m_OldMState.Right=FALSE;
	m_OldMState.Pos.x=400;
	m_OldMState.Pos.y=300;
	//鼠标的当前状态
	m_CurMState.Left=FALSE;
	m_CurMState.Right=FALSE;
	m_CurMState.Pos.x=400;
	m_CurMState.Pos.y=300;

}

CInput::~CInput(void)
{
	CleanUp();
}

//初始化DInput对象
BOOL CInput::InitDInput( HWND hWnd,HINSTANCE hInstance )
{
	DirectInput8Create(hInstance,DIRECTINPUT_VERSION,IID_IDirectInput8,
		(LPVOID*)&m_pDI,NULL);
	
	//鼠标的初始化
	m_pDI->CreateDevice(GUID_SysMouse,&m_pMouse,NULL);//创建鼠标设备
	m_pMouse->SetDataFormat(&c_dfDIMouse2);	//设置键盘的数据格式	
	m_pMouse->SetCooperativeLevel(hWnd,DISCL_BACKGROUND|DISCL_NONEXCLUSIVE);//设置交互格式
	m_pMouse->Acquire();//获得输入设备的实际访问权

	//键盘的初始化
	m_pDI->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL);//创建键盘设备
	m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);//设置键盘的数据格式
	m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);//设置交互格式
	m_pKeyboard->Acquire();//获得输入设备的实际访问权

	m_lOldlZ=0;
	m_lNowlZ=0;
	m_bWhell=FALSE;

	return true;
}

//读取鼠标状态
BOOL CInput::ReadMouse()
{
	
	if(m_pMouse==NULL)
	{
		return FALSE;
	}
	ZeroMemory(&dims2,sizeof(dims2));
	//获得鼠标状态
	if((m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE2),&dims2))!=DI_OK)
	{
		return FALSE;
	}
	m_OldMState.Pos.x=m_CurMState.Pos.x;
	m_OldMState.Pos.y=m_CurMState.Pos.y;
	m_OldMState.Left=m_CurMState.Left;
	m_OldMState.Right=m_CurMState.Right;
	//获得当前鼠标的坐标值
	m_CurMState.Pos.x+=dims2.lX;
	m_CurMState.Pos.y+=dims2.lY;
	POINT pt;
	GetCursorPos(&pt);
	MouseX = pt.x;
	MouseY = pt.y;
	if(dims2.rgbButtons[0]&0x80)//左键按下 
	{
		m_CurMState.Left=TRUE;
	}else
	{
		m_CurMState.Left=FALSE;
	}

	if(dims2.rgbButtons[1]&0x80)//右键按下
	{
		m_CurMState.Right=TRUE;
	}else
	{
		m_CurMState.Right=FALSE;
	}
	m_lNowlZ=dims2.lZ;
	if(m_lNowlZ!=m_lOldlZ)
	{
		m_bWhell=TRUE;
	}else
	{
		m_bWhell=FALSE;
	}
	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------
//--读取键盘的状态
//------------------------------------------------------------------------------------------------------------------
BOOL CInput::ReadKeyboard()
{
	HRESULT        result;   //--信息返回句柄

	result = m_pKeyboard->GetDeviceState(sizeof(KeyboardState),(LPVOID)&KeyboardState);
	if(result != DI_OK)
	{
		MessageBox(NULL, "取得键盘设备失败！", NULL, MB_OK);
		return false;
	}
	//--左
	if(KEYDOWN(KeyboardState, DIK_A))
	{
		m_pKeyControl.m_nLeft = true;
	}
	else
	{
		m_pKeyControl.m_nLeft = false;
	}

	//--右
	if(KEYDOWN(KeyboardState, DIK_D))
	{
		m_pKeyControl.m_nRight = true;
	}
	else
	{
		m_pKeyControl.m_nRight = false;
	}

	//--上
	if(KEYDOWN(KeyboardState, DIK_W))
	{
		m_pKeyControl.m_nUp = true;
	}
	else
	{
		m_pKeyControl.m_nUp = false;
	}

	//--下
	if(KEYDOWN(KeyboardState, DIK_S))
	{
		m_pKeyControl.m_nDown = true;
	}
	else
	{
		m_pKeyControl.m_nDown = false;
	}

	return true;
}

bool CInput::IsKeyDown( DWORD dwKeyCode,bool bIgnoreStamp )
{
	if ((KeyboardState[dwKeyCode] & 0x80) != 0x80)
	{
		return false;
	}

	if (bIgnoreStamp)
	{
		return true;
	}
	else
	{
		if ((CGameTime::GetInstance().GetGameTime() - m_nKeyStamp[dwKeyCode]) >= 500)
		{
			m_nKeyStamp[dwKeyCode] = CGameTime::GetInstance().GetGameTime();
			return true;
		}
		else
		{
			return false;
		}
	}	
}

void CInput::CleanUp()
{
	SAFE_RELEASE( m_pDI );
	SAFE_RELEASE( m_pKeyboard );
	SAFE_RELEASE( m_pMouse );
}

