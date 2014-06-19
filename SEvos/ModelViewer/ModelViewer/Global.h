//-----------------------------------------------------------------------------//
// 2005-12-01  programer: jaejung ��(���բ�)��
// 
//-----------------------------------------------------------------------------//

#if !defined(__GLOBAL_H__)
#define __GLOBAL_H__

#include "stdafx.h"
#include <windows.h>
#pragma warning (disable: 4786)
#include <string>
#include <list>
#include <queue>
#include <map>
#include <Mmsystem.h>
#include "d3dx9.h"
#include "d3dx9tex.h"

#include "math/math.h"

#include "game/combo.h"

#include "common/structdef.h"
#include "common/d3dutility.h"
#include "common/camera.h"
#include "common/frustum.h"
#include "common/linearmemloader.h"
#include "common/fileloader.h"
#include "common/mmtimers.h"
#include "common/dbg.h"

#include "game/scene.h"
#include "scriptlib/script.h"
#include "scriptlib/script_machine.h"

#include "model//mesh.h"



//#define _TEST_MODE	1	// ���Ӱ����� �׽�Ʈ�� ���ؼ� ���ǵ�, ������� ���ŵȴ�.
#define _PRINT_FRAME	1	// ������ ���

extern int g_test;

extern IDirect3DDevice9 *g_pDevice;
extern ns_script::CScript g_Scr;

extern int g_CurTime;	// ���� �ð� (millisecond���� timeGetTime() ��)
extern int g_DeltaTime;	// �������ӿ��� ��� �ð�
extern D3DXMATRIXA16 g_matIdentity;	// �������
extern Vector3 g_DefaultCameraPos;
extern RECT g_ViewPort;
extern CFrustum g_Frustum;


const float VECTOR_EPSILON = 0.05f;

// MACROS
#define SAFE_RELEASE(p)		if(p){ (p)->Release(); p = NULL; }
#define SAFE_DELETE(p)		{if(p){ delete (p); (p) = NULL; }}
#define SAFE_ADELETE(p)		{if(p){ delete[] (p); (p) = NULL; }} // array delete
#define IS_IN_RANGE(value,r0,r1) (( ((r0) <= (value)) && ((value) <= (r1)) ) ? 1 : 0)

// ���� ����� �浹�ڽ� ���
static char *g_CollisionNodeName = "Bip01 Spine1";
static char *g_BoneCenterNodeName = "Bip01";

// Animation Frame ����
// 3DMax ������ 1�ʿ� 30 frame �� �������� �۾��ϱ� ������
// 1�ʿ� 1000frame�� ����ϴ� �������α׷��� �°� �����Ǿ�� �Ѵ�.
const float FRAME = (1000.f / 30.f);



/////////////////////////////////////////////////////////////////////////////////
// definition


// object �Ӽ�
enum OBJ_TYPE
{ 
	OT_TILE,
	OT_MODEL,
	OT_RIGID,
};

// MapEditor ����
enum EDIT_MODE
{
	EM_NORMAL,	// �⺻ ����
	EM_TILE,	// Ÿ�� ���� ���
	EM_TERRAIN,	// ���� ����Ʈ ���
	EM_MODEL,	// �� �߰� ���
};

// MapView Cursor Type
enum CURSOR_TYPE
{
	CT_SQUARE,
	CT_CIRCLE,
};

typedef struct _tagSCrTerrain
{
	int nWidth;
	int nHeight;
	int nVtxPerRow;
	int nVtxPerCol;
	char szTexture[ MAX_PATH];

} SCrTerrain;

#pragma pack(1) // 1byte ����
	typedef struct _tagSTModelLoader
	{
		_tagSTModelLoader() {}
		_tagSTModelLoader( Vector3 v, int n, char *s ) : vPos(v), nModelIdx(n)
		{ if(s) strcpy_s( szObjName, sizeof(szObjName),s ); }

		Vector3 vPos;
		int nModelIdx; // ���� �Ⱦ�
		char szObjName[ 64];

	} STModelLoader;

	typedef struct _tagSTerrainLoader
	{
		int nWidth;
		int nHeight;
		int nVtxPerRow;
		int nVtxPerCol;
		int nCellPerRow;
		int nCellPerCol;
		int nCellSize;
		int nMapSize;
		float *pMap;
		int nRigidSize;
		STModelLoader *pRigid;
		int nDynSize;
		STModelLoader *pDyn;

	} STerrainLoader;
#pragma pack() // default


//class CTerrainPanel;
class CModelView;
class CBasePanelFrame;
//class CTilePanel;
//extern CTerrainPanel *g_pTerrainPanel;
//extern CTilePanel *g_pTilePanel;
extern CBasePanelFrame *g_pBasePanelFrm;
extern CModelView *g_pModelView;

//extern EDIT_MODE g_eEditMode;
//const static char *g_szTilePath = "data//map//tile";
//const static char *g_szModelTable = "data//model";



#endif // __JGLOBAL_H__
