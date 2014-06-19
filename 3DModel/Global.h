//-----------------------------------------------------------------------------//
// 2005-12-01  programer: jaejung ��(���բ�)��
// 
//-----------------------------------------------------------------------------//

#if !defined(__GLOBAL_H__)
#define __GLOBAL_H__

#define _AFX_SECURE_NO_WARNINGS 
#define _ATL_SECURE_NO_WARNINGS

#pragma warning (disable: 4996)	// strcpy ��� ����


#include <windows.h>
#pragma warning (disable: 4786)
#include <string>
#include <list>
#include <queue>
#include <map>
#include "d3dx9.h"
#include "d3dx9tex.h"
#include "math.h"
#include "structdef.h"
#include "d3dutility.h"
#include "combo.h"
//#include "control.h"
#include "script.h"
#include "script_machine.h"
#include "terrain.h"

#include "dbg.h"

//#define _TEST_MODE	1	// ���Ӱ����� �׽�Ʈ�� ���ؼ� ���ǵ�, ������� ���ŵȴ�.
#define _PRINT_FRAME	1	// ������ ���

extern int g_test;

extern IDirect3DDevice9 *g_pDevice;
extern ns_script::CScript g_Scr;

extern int g_CurTime;	// ���� �ð� (millisecond���� timeGetTime() ��)
extern int g_DeltaTime;	// �������ӿ��� ��� �ð�
extern D3DXMATRIXA16 g_matIdentity;	// �������
extern Vector3 g_DefaultCameraPos;


const float VECTOR_EPSILON = 0.05f;

// MACROS
#define SAFE_RELEASE(p)		if(p){ (p)->Release(); p = NULL; }
#define SAFE_DELETE(p)		{if(p){ delete (p); (p) = NULL; }}
#define SAFE_ADELETE(p)		{if(p){ delete[] (p); (p) = NULL; }} // array delete


// ���� ����� �浹�ڽ� ���
static char *g_CollisionNodeName = "Bip01 Spine1";
static char *g_BoneCenterNodeName = "Bip01";

// Animation Frame ����
// 3DMax ������ 1�ʿ� 30 frame �� �������� �۾��ϱ� ������
// 1�ʿ� 1000frame�� ����ϴ� �������α׷��� �°� �����Ǿ�� �Ѵ�.
const float FRAME = (1000.f / 30.f);



#endif // __JGLOBAL_H__
