// EditPanel.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ModelViewer.h"
#include "EditPanel.h"


// CEditPanel ��ȭ �����Դϴ�.

CEditPanel::CEditPanel(CWnd* pParent /*=NULL*/)
	: CPanelBase(MODEL, CEditPanel::IDD, pParent)
{

}

CEditPanel::~CEditPanel()
{
}

void CEditPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditPanel, CDialog)
END_MESSAGE_MAP()


// CEditPanel �޽��� ó�����Դϴ�.
