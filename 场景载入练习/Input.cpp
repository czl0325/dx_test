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

	//����ǰһ��״̬
	m_OldMState.Left=FALSE;
	m_OldMState.Right=FALSE;
	m_OldMState.Pos.x=400;
	m_OldMState.Pos.y=300;
	//���ĵ�ǰ״̬
	m_CurMState.Left=FALSE;
	m_CurMState.Right=FALSE;
	m_CurMState.Pos.x=400;
	m_CurMState.Pos.y=300;

}

CInput::~CInput(void)
{
	CleanUp();
}

//��ʼ��DInput����
BOOL CInput::InitDInput( HWND hWnd,HINSTANCE hInstance )
{
	DirectInput8Create(hInstance,DIRECTINPUT_VERSION,IID_IDirectInput8,
		(LPVOID*)&m_pDI,NULL);
	
	//���ĳ�ʼ��
	m_pDI->CreateDevice(GUID_SysMouse,&m_pMouse,NULL);//��������豸
	m_pMouse->SetDataFormat(&c_dfDIMouse2);	//���ü��̵����ݸ�ʽ	
	m_pMouse->SetCooperativeLevel(hWnd,DISCL_BACKGROUND|DISCL_NONEXCLUSIVE);//���ý�����ʽ
	m_pMouse->Acquire();//��������豸��ʵ�ʷ���Ȩ

	//���̵ĳ�ʼ��
	m_pDI->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL);//���������豸
	m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);//���ü��̵����ݸ�ʽ
	m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);//���ý�����ʽ
	m_pKeyboard->Acquire();//��������豸��ʵ�ʷ���Ȩ

	m_lOldlZ=0;
	m_lNowlZ=0;
	m_bWhell=FALSE;

	return true;
}

//��ȡ���״̬
BOOL CInput::ReadMouse()
{
	
	if(m_pMouse==NULL)
	{
		return FALSE;
	}
	ZeroMemory(&dims2,sizeof(dims2));
	//������״̬
	if((m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE2),&dims2))!=DI_OK)
	{
		return FALSE;
	}
	m_OldMState.Pos.x=m_CurMState.Pos.x;
	m_OldMState.Pos.y=m_CurMState.Pos.y;
	m_OldMState.Left=m_CurMState.Left;
	m_OldMState.Right=m_CurMState.Right;
	//��õ�ǰ��������ֵ
	m_CurMState.Pos.x+=dims2.lX;
	m_CurMState.Pos.y+=dims2.lY;
	POINT pt;
	GetCursorPos(&pt);
	MouseX = pt.x;
	MouseY = pt.y;
	if(dims2.rgbButtons[0]&0x80)//������� 
	{
		m_CurMState.Left=TRUE;
	}else
	{
		m_CurMState.Left=FALSE;
	}

	if(dims2.rgbButtons[1]&0x80)//�Ҽ�����
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
//--��ȡ���̵�״̬
//------------------------------------------------------------------------------------------------------------------
BOOL CInput::ReadKeyboard()
{
	HRESULT        result;   //--��Ϣ���ؾ��

	result = m_pKeyboard->GetDeviceState(sizeof(KeyboardState),(LPVOID)&KeyboardState);
	if(result != DI_OK)
	{
		MessageBox(NULL, "ȡ�ü����豸ʧ�ܣ�", NULL, MB_OK);
		return false;
	}
	//--��
	if(KEYDOWN(KeyboardState, DIK_A))
	{
		m_pKeyControl.m_nLeft = true;
	}
	else
	{
		m_pKeyControl.m_nLeft = false;
	}

	//--��
	if(KEYDOWN(KeyboardState, DIK_D))
	{
		m_pKeyControl.m_nRight = true;
	}
	else
	{
		m_pKeyControl.m_nRight = false;
	}

	//--��
	if(KEYDOWN(KeyboardState, DIK_W))
	{
		m_pKeyControl.m_nUp = true;
	}
	else
	{
		m_pKeyControl.m_nUp = false;
	}

	//--��
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

