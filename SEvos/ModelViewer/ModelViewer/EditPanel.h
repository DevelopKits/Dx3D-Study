#pragma once
#include "mfc/panelBase.h"


// CEditPanel ��ȭ �����Դϴ�.

class CEditPanel : public CPanelBase
{

public:
	CEditPanel(CWnd* pParent = NULL);
	virtual ~CEditPanel();
	enum { IDD = IDD_EDIT_PANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
