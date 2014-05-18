
#include "global.h"
#include "scene.h"
#include "sceneGame.h"
#include "sceneMenu.h"
#include "ui//scenelist.txt"
#include "fileloader.h"
#include "uicontrol.h"


const int MAX_SCENE = 16;
static CScene *s_Scene[ MAX_SCENE];	// ��� ���� �����Ѵ�.
static int s_SceneMap[ 256];	// class id -> scene index
static int s_SceneCount = 0;

CScene::CScene( int Id ) : m_Id(Id),m_pCurScene(NULL), m_pParent(NULL), m_pFocusCtrl(NULL)
{
	static BOOL bInit = FALSE;
	if( !bInit )
	{
		ZeroMemory( s_SceneMap, sizeof(s_SceneMap) );
		bInit = TRUE;
	}

}


CScene::~CScene() 
{

}


//-----------------------------------------------------------------------------//
// �� Ŭ���� ����
// ��ũ��Ʈ���� UI������ �о�鿩 ��Ŭ������ �����Ѵ�. 
// pFileName : UI ��ũ��Ʈ ����
// pParent : �θ� ��
//-----------------------------------------------------------------------------//
BOOL CScene::Create( char *pFileName, CScene *pParent ) // pParent=NULL
{
	SScene *pscene = CFileLoader::LoadUI( pFileName );
	Create( pscene, pParent );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// �� Ŭ���� ����
//-----------------------------------------------------------------------------//
BOOL CScene::Create( SScene *pScene, CScene *pParent ) // pParent=NULL
{
	m_pParent = pParent;
	RegisterScene( this );

	// ��Ʈ�� ����
	for( int i=0; i < pScene->csize; ++i )
	{
		SControl *pc = &pScene->pc[ i];
		AddControl( pc );
	}

	// �ڽ� �� ����
	for( i=0; i < pScene->ssize; ++i )
	{
		SScene *ps = &pScene->ps[ i];
		CScene *s = FindClass( ps->id );
		if( !s ) s = new CScene( ps->id );
		s->Create( ps, this );
		AddChildScene( s );
	}
	return TRUE;
}



//-----------------------------------------------------------------------------//
// ���
//-----------------------------------------------------------------------------//
void CScene::Render()
{
	if( m_pCurScene )
	{
		m_pCurScene->Render();
	}
	else
	{
		std::list< CUIControl* >::iterator it = m_Control.begin();
		while( m_Control.end() != it )
		{
			CUIControl *pctrl = *it++;
			pctrl->Render();
		}
	}

}


//-----------------------------------------------------------------------------//
// ���ϸ��̼� ó�� + ���޼��� ó��
//-----------------------------------------------------------------------------//
void CScene::Animate( int nDelta )
{
	if( m_pCurScene )
	{
		m_pCurScene->Animate( nDelta );
		m_pCurScene->UIMsgProc();
	}
	else
	{
		
	}
}


//-----------------------------------------------------------------------------//
// ������ �޼����� ó���Ѵ�.
//-----------------------------------------------------------------------------//
void CScene::MessageProc( MSG *pMsg )
{
	if( m_pCurScene )
	{
		m_pCurScene->MessageProc( pMsg );
	}
	else
	{
		if( WM_KEYFIRST <= pMsg->message && WM_KEYLAST > pMsg->message )
		{
//			KeyDown( pMsg->wParam );
		}
		if( WM_MOUSEFIRST <= pMsg->message && WM_MOUSELAST > pMsg->message )
		{
			POINT pt;
			pt.x = LOWORD( pMsg->lParam );
			pt.y = HIWORD( pMsg->lParam );

			if( (WM_LBUTTONDOWN == pMsg->message) || (WM_LBUTTONUP == pMsg->message) )
			{
				CUIControl *pui = NULL;
				std::list< CUIControl* >::reverse_iterator it = m_Control.rbegin();
				while( m_Control.rend() != it )
				{
					CUIControl *p = *it++;
					if( p->GetVisible() && p->InRect(&pt) )
					{
						pui = p;
						break;
					}
				}

				if( pui && (WM_LBUTTONDOWN == pMsg->message) )
				{
					pui->SendMessage( &SUIMsg(pui->GetId(), UIMSG_LCLICK, 0, 0) );
					m_pFocusCtrl = pui;
				}
				else if( (WM_LBUTTONUP == pMsg->message) )
				{
					if( pui && (m_pFocusCtrl == pui) )
					{
						pui->SendMessage( &SUIMsg(pui->GetId(), UIMSG_LCLICKUP, 0, 0 ) );
						ButtonUp( pui->GetId() );
					}
					else 
					{
						if( pui ) pui->SendMessage( &SUIMsg(pui->GetId(), UIMSG_LCLICKUP, 0, 0 ) );
						if( m_pFocusCtrl ) m_pFocusCtrl->SendMessage( &SUIMsg(m_pFocusCtrl->GetId(), UIMSG_LCLICKUP, 0, 0 ) );
					}

					m_pFocusCtrl = NULL;
				}
			}
		}
		else if( WM_KEYDOWN == pMsg->message )
		{
			KeyDown( pMsg->wParam );
		}
	}
}


//-----------------------------------------------------------------------------//
// UI �޼����� ó���Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CScene::UIMsgProc()
{
	const int MAX_PROC = 5;
	int count = 0;

	while( !m_MsgQ.empty() && MAX_PROC > count++ )
	{
		DefaultMsgProc( &m_MsgQ.front() );
		m_MsgQ.pop();
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// ���޼����� ó��.
//-----------------------------------------------------------------------------//
BOOL CScene::DefaultMsgProc( SUIMsg *pMsg )
{
	switch( pMsg->msg )
	{
	case UIMSG_COMMAND:
		{
			CUIControl *pui = FindUIControl( pMsg->classid );
			if( !pui ) break;

			switch( pMsg->wparam )
			{
			case UICMD_SETIMAGEINDEX:
					pui->SendMessage( &SUIMsg(pMsg->classid, UIMSG_SETINDEX, pMsg->lparam, 0 ) );
					break;
			}
		}
		break;

	// ��Ʈ�ѿ����� ó���Ѵ�.
	case UIMSG_SETVISIBLE:
		{
			CUIControl *pui = FindUIControl( pMsg->classid );
			if( !pui ) break;
			pui->SetVisible( pMsg->wparam );
		}
		break;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Ű���� �޼��� ó��
//-----------------------------------------------------------------------------//
void CScene::KeyDown( int Key )
{
	// �ڽ� ������ ó���ȴ�.
}


//-----------------------------------------------------------------------------//
// ��ư �޼��� ó��
//-----------------------------------------------------------------------------//
void CScene::ButtonDown( int ButtonId )
{
	// �ڽ� ������ ó���ȴ�.
}


//-----------------------------------------------------------------------------//
// ��ư �޼��� ó��
//-----------------------------------------------------------------------------//
void CScene::ButtonUp( int ButtonId )
{
	// �ڽ� ������ ó���ȴ�.
}


//-----------------------------------------------------------------------------//
// ��Ʈ�� �߰�
//-----------------------------------------------------------------------------//
void CScene::AddControl( SControl *pCtrl )
{
	CUIControl *p = NULL;
	switch( pCtrl->type )
	{
	case UI_IMAGE: p = new CUIImage(); break;
	case UI_BUTTON: p = new CUIButton(); break;
	}
	if( !p ) return;

	p->Create( pCtrl );
	m_Control.push_back( p );
}


//-----------------------------------------------------------------------------//
// ���� ���ŵɶ� ȣ��ȴ�.
// ���ο� �������� ������ ��Ʈ�Ѱ� ������ �����Ѵ�.
//-----------------------------------------------------------------------------//
void CScene::Destroy()
{
	// UIControl ����
	std::list< CUIControl* >::iterator it = m_Control.begin();
	while( m_Control.end() != it )
	{
		CUIControl *pui = *it++;
		SAFE_DELETE( pui );
	}

	// �ڽľ� Destroy
	std::list< CScene* >::iterator i = m_ChildScene.begin();
	while( m_ChildScene.end() != i )
	{
		CScene *ps = *i++;
		ps->Destroy();
		SAFE_DELETE( ps );
	}

}


//-----------------------------------------------------------------------------//
// ���� �޼����� ó���Ѵ�.
//-----------------------------------------------------------------------------//
void CScene::SendMessage( SUIMsg *pMsg )
{
	if( m_pCurScene )
	{
		m_pCurScene->SendMessage( pMsg );
	}
	else
	{
		DefaultMsgProc( pMsg );
	}
}


//-----------------------------------------------------------------------------//
// ���� �޼����� ť�� �������� UIMsgProc()���� ó���ȴ�.
//-----------------------------------------------------------------------------//
void CScene::PostMessage( SUIMsg *pMsg )
{
	if( m_pCurScene )
	{
		m_pCurScene->PostMessage( pMsg );
	}
	else
	{
		m_MsgQ.push( *pMsg );
	}
}


//-----------------------------------------------------------------------------//
// ȭ�鿡 ������ �ڽľ��� ��ü�Ѵ�.
// CurSceneId: ���� �� (�������� �ʾƵ� �ȴ�.)
// NextSceneId: ������ ��
//-----------------------------------------------------------------------------//
void CScene::SwapScene( int CurSceneId, int NextSceneId )
{
//	if( m_pCurScene )
	{
//		m_pCurScene->SwapScene( CurSceneId, NextSceneId );
	}
//	else
	{
		CScene *p = FindClass( NextSceneId );
		if( !p ) return;
		p->SendMessage( &SUIMsg(p->m_Id, UIMSG_SWAPPED, 0, 0) );
		m_pCurScene = p;
	}

}


//-----------------------------------------------------------------------------//
// ���̺��� ���� ã�´�.
//-----------------------------------------------------------------------------//
CScene* CScene::FindClass( int Id )
{
	const int size = sizeof(g_SceneTable) / sizeof(CScene*);
	for( int i=0; i < size; ++i )
	{
		if( g_SceneTable[ i]->GetID() == Id )
			return g_SceneTable[ i];
	}
	int idx = s_SceneMap[ Id];
	if( 0 >= idx ) return NULL;
	return s_Scene[ idx];
}


//-----------------------------------------------------------------------------//
// ���� ��ϵ� UIControl�� �˻��Ѵ�.
//-----------------------------------------------------------------------------//
CUIControl* CScene::FindUIControl( int Id )
{
	std::list< CUIControl* >::iterator it = m_Control.begin();
	while( m_Control.end() != it )
	{
		CUIControl *pui = *it++;
		if( pui->GetId() == Id )
			return pui;
	}
	return NULL;
}


//-----------------------------------------------------------------------------//
// �ڽľ��� �߰�
//-----------------------------------------------------------------------------//
BOOL CScene::AddChildScene( CScene *pScene )
{
	m_ChildScene.push_back( pScene );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// ���� ������ �����ϱ����ؼ� �ӽ� �޸𸮿� �����Ѵ�
// s_Scene[] 1�� �ε������� ���� ����ȴ�. �̴� s_SceneMap[] ���� 0�϶� ��������
// �ʾҴٴ°��� ��Ÿ���� ���ؼ���.
//-----------------------------------------------------------------------------//
void CScene::RegisterScene( CScene *pScene )
{
	s_SceneMap[ pScene->GetID()] = s_SceneCount+1;
	s_Scene[ s_SceneCount+1] = pScene;
	++s_SceneCount;
}
