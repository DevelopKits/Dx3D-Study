// SEvosMapTool.h : SEvosMapTool ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CSEvosMapToolApp:
// �� Ŭ������ ������ ���ؼ��� SEvosMapTool.cpp�� �����Ͻʽÿ�.
//

class CSEvosMapToolApp : public CWinApp
{
public:
	CSEvosMapToolApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSEvosMapToolApp theApp;