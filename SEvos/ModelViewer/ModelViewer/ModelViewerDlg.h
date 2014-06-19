
// CModelViewerDlg.h : ��� ����
//

#pragma once


// CModelViewerDlg ��ȭ ����
class CModelViewerDlg : public CDialog
{
// �����Դϴ�.
public:
	CModelViewerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MODELVIEWER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

protected:
	BOOL m_bLoop;
	int m_nDeltaTime;
	char m_CurrentDirectory[ MAX_PATH];

public:
	BOOL Init();
	void MainProc();
	BOOL Setup();


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};
