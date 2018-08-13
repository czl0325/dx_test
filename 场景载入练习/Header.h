#ifndef _HEADER
#define _HEADER

#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <cmath>
#include <math.h>
#include <list>
#include <string>
//D3D�õ�ͷ�ļ�
#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>
#include <assert.h>
#include "dinput.h"
#include "d3dx9anim.h"//��Ƥ��������
using namespace std;
//D3D�õ�lib��
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Dinput8.lib")
#pragma comment(lib,"dxguid.lib")

//#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code)&0x80000)?1:0)
//#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code)&0x8000)?0:1)

#define WINDOW_WIDTH	1024	//��Ϸ���ڵĿ�
#define WINDOW_HEIGHT	768	//��Ϸ���ڵĸ�

#define DINPUT_BUFFERSIZE    16                    //--DI������ģʽ�еĻ������Ĵ�С
#define KEYDOWN(name, key) (name[key] & 0x80)      //--���̵İ����¼�����

#define SAFE_DELETE(p) { if(p)   { delete (p);  (p) = NULL;} }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p) = NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

#endif
