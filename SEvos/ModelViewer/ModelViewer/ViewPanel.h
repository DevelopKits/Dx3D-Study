#pragma once
#include "afxwin.h"
#include "mfc/panelBase.h"


class CFileListBox;
class CAniFileListBox;
class CModelFielListBox;
class CViewPanel : public CPanelBase
{
public:
	CViewPanel(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CViewPanel();
	enum { IDD = IDD_VIEW_PANEL };

protected:
	CModelFielListBox *m_pModelFileListBox;
	CAniFileListBox *m_pAniFileListBox;

public:
	CString GetSelectModelName();
	CString GetSelectAnimationName();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnLbnSelchangeFilelist(WPARAM wParam, LPARAM lParam);
};
