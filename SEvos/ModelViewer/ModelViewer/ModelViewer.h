
// ModelViewer.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CModelViewerApp:
// �� Ŭ������ ������ ���ؼ��� ModelViewer.cpp�� �����Ͻʽÿ�.
//

class CModelViewerApp : public CWinAppEx
{
public:
	CModelViewerApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CModelViewerApp theApp;