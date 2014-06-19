// ModelPanel.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool2.h"
#include "modelFileListBox.h"
#include "MapView.h"
#include "ModelPannel.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelPanel dialog


CModelPanel::CModelPanel(CWnd* pParent /*=NULL*/)
	: CPanelBase(MODEL, CModelPanel::IDD, pParent)
{
	m_pModelFileListBox = NULL;

}


CModelPanel::~CModelPanel()
{
	SAFE_DELETE(m_pModelFileListBox);

}


void CModelPanel::DoDataExchange(CDataExchange* pDX)
{
	CPanelBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelPanel)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModelPanel, CPanelBase)
	//{{AFX_MSG_MAP(CModelPanel)
	ON_BN_CLICKED(IDC_BUTTON_REFLESH, OnButtonReflesh)
	ON_MESSAGE(WM_NOTIFY_SELECTLIST, &CModelPanel::OnLbnSelchangeFilelist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelPanel message handlers

BOOL CModelPanel::OnInitDialog() 
{
	CPanelBase::OnInitDialog();

	CRect modelRect = GetChildRect(IDC_MODELLIST_RECT);
	m_pModelFileListBox = new CModelFielListBox();
	m_pModelFileListBox->Create(CModelFielListBox::IDD, this);
	m_pModelFileListBox->MoveWindow(modelRect);
	m_pModelFileListBox->ShowWindow(SW_SHOW);

	return TRUE;
}


void CModelPanel::OnButtonReflesh() 
{

}


void CModelPanel::LoadTable()
{
/*
	list<string> extlist, filelist;
	extlist.push_back( "bmp" );
	extlist.push_back( "tga" );
	extlist.push_back( "jpg" );
	extlist.push_back( "tng" );

	CollectFile( &extlist, szDirectory, &filelist );

	while( LB_ERR != m_TileList.DeleteString(0) ) {} // List Clear

	list<string>::iterator it = filelist.begin();
	while( filelist.end() != it )
		m_TileList.AddString( it++->c_str() );
/**/

//	m_FileTree.OpenModelTable( (char*)g_szModelTable );
//	SD_CTableMgr::LoadModelTable( (char*)g_szModelTable );

}


//------------------------------------------------------------------------
// ���ϸ���Ʈ�� ���õǸ� ȣ��ȴ�.
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
LRESULT CModelPanel::OnLbnSelchangeFilelist(WPARAM wParam, LPARAM lParam)
{
	Update();
	return 0;
}

//------------------------------------------------------------------------
// 
// [2011/2/22 jjuiddong]
//------------------------------------------------------------------------
CString CModelPanel::GetSelectModelName()
{
	return m_pModelFileListBox->GetSelectItemName();
}


//------------------------------------------------------------------------
// 
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
void CModelPanel::OnShowWindow()
{

}


//------------------------------------------------------------------------
// 
// [2011/3/1 jjuiddong]
//------------------------------------------------------------------------
void CModelPanel::OnHideWindow()
{
	// ���õ� ����Ʈ�� �ʱ�ȭ �Ѵ�.
	// �г��� ���� �� ���õ� ���������� �ε��ϱ⶧����
	// �г��� ���� �� �ʱ�ȭ�� ���־�� �Ѵ�.
	CListBox *fileListBox = m_pModelFileListBox->GetFileListBox();
	fileListBox->SetCurSel(-1);

	UpdateData(FALSE);
}
