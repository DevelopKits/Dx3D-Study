
// ModelViewer.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "ModelViewer.h"
#include "ModelViewerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CModelViewerApp

BEGIN_MESSAGE_MAP(CModelViewerApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CModelViewerApp ����

CModelViewerApp::CModelViewerApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}


// ������ CModelViewerApp ��ü�Դϴ�.

CModelViewerApp theApp;


// CModelViewerApp �ʱ�ȭ

BOOL CModelViewerApp::InitInstance()
{
//	CWinAppEx::InitInstance();

	CModelViewerDlg *pDlg = new CModelViewerDlg();
	pDlg->Create( IDD_MODELVIEWER_DIALOG );
	if( !pDlg->Init() )
	{
		delete pDlg;
		return FALSE;
	}

	pDlg->ShowWindow( SW_SHOW );
	pDlg->MainProc();

	pDlg->DestroyWindow();
	g_pDevice->Release();
	delete pDlg;

	return FALSE;
}
