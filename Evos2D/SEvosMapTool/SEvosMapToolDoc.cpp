// SEvosMapToolDoc.cpp : CSEvosMapToolDoc Ŭ������ ����
//

#include "stdafx.h"
#include "SEvosMapTool.h"

#include "SEvosMapToolDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSEvosMapToolDoc

IMPLEMENT_DYNCREATE(CSEvosMapToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CSEvosMapToolDoc, CDocument)
END_MESSAGE_MAP()


// CSEvosMapToolDoc ����/�Ҹ�

CSEvosMapToolDoc::CSEvosMapToolDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.

}

CSEvosMapToolDoc::~CSEvosMapToolDoc()
{
}

BOOL CSEvosMapToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ ���ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	return TRUE;
}




// CSEvosMapToolDoc serialization

void CSEvosMapToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}


// CSEvosMapToolDoc ����

#ifdef _DEBUG
void CSEvosMapToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSEvosMapToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSEvosMapToolDoc ���
