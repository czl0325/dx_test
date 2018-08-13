#include "header.h"
#include "D3DInit.h"

CD3DInit* g_D3DInit = new CD3DInit();


//--windows的消息回调函数
long FAR PASCAL WindowProc(HWND hWnd,UINT message,
						   WPARAM wParam,LPARAM lParam)
{
	switch(message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:       //--按键消息
		g_D3DInit->KeyDown(wParam);
		switch(wParam)		
		{			
		case VK_ESCAPE:                         //--当按下ESC键时候触发关闭窗口的消息
			PostMessage(hWnd, WM_CLOSE,0,0);
			break;
		}
		break;
	case WM_KEYUP:
		g_D3DInit->KeyUp(wParam);
		break;
	}
	return DefWindowProc(hWnd,message,wParam,lParam);
}

//--windows的主函数
int CALLBACK WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
					 LPSTR lpCmdLine,int nCmdShow)
{
	MSG      msg;                                //--定义一个消息的对象
	HWND     hWnd;                               //--窗口句柄
	WNDCLASS wc;                                 //--定义窗口类
	//--创建窗口类
	wc.style       = CS_HREDRAW | CS_VREDRAW;    //--支持水平和垂直重绘
	wc.lpfnWndProc = WindowProc;                 //--相应的消息处理函数
	wc.cbClsExtra  = 0;                          //--附加内存空间
	wc.cbWndExtra  = 0;                          //--附加内存空间
	wc.hInstance   = hInstance;                  //--窗口的实例句柄
	wc.hIcon       = NULL;                       //--窗口的小图标
	wc.hCursor     = NULL;                       //--窗口的鼠标形状
	//--背景颜色
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;                     //--窗口菜单
	wc.lpszClassName = "陈昭良作品";          //--窗口名称

	RegisterClass(&wc);                          //--注册窗口句柄
	hWnd = CreateWindowEx(WS_EX_APPWINDOW,         //--窗口总显示在顶部
		"陈昭良作品",       //--窗口的类名
		"陈昭良作品", //--窗口的标题
		WS_OVERLAPPEDWINDOW,   //--窗口的风格
		50,                    //--X轴的初始化设定坐标
		50,                    //--Y轴的初始化设定坐标
		800,                   //--高度初始化
		600,                   //--宽度初始化
		NULL,                  //--父窗口句柄
		NULL,                  //--窗口菜单的句柄 
		hInstance,             //--实例句柄
		NULL);                 //--附加信息
	if(!hWnd)
	{
		return FALSE;
	}
	g_D3DInit->GameInit(hWnd,hInstance);

	ShowWindow(hWnd,nCmdShow);                   //--显示窗口
	UpdateWindow(hWnd);                          //--更新窗口
	while(true)
	{
		//--接受系统消息(&msg为MSG类型的信息结构体,NULL窗口句柄),
		//--0,0表示接受所有的窗口信息)
		if(PeekMessage(&msg,NULL,0U,0U,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);              //--转化信息
			DispatchMessage(&msg);               //--分配信息到相应的消息处理函数windowProc中
		}
		else
		{
			g_D3DInit->Render();                 //--渲染D3D
		}
	}

	return 1;
}
