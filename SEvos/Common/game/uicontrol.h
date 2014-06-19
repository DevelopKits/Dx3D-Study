//-----------------------------------------------------------------------------//
// 2008-03-29  programer: jaejung ��(���բ�)��
// 
//-----------------------------------------------------------------------------//

#if !defined(__UICONTROL_H__)
#define __UICONTROL_H__



// ��Ʈ�� �Ϲ�ȭ Ŭ����
class CUIControl
{
public:
	CUIControl();
	virtual ~CUIControl();
protected:
	int m_Type;
	int m_Id;
	RECT m_Rect;
	BOOL m_bVisible;

public:
	virtual BOOL Create( SControl *pCtrl );
	virtual void Render() {}
	virtual void Animate( int nTimeDelta ) {}
	virtual BOOL MsgProc( SUIMsg *pMsg ) { return TRUE; }
	virtual void SetVisible( BOOL bVisible ) { m_bVisible=bVisible; }
	virtual void SetRect( RECT *pRect ) { m_Rect = *pRect; }

	void SendMessage( SUIMsg *pMsg );
	int GetId() const { return m_Id; }
	int GetType() const { return m_Type; }
	BOOL InRect( POINT *pt );
	BOOL GetVisible() const { return m_bVisible; }
	void GetRect( RECT *pRect ) { *pRect = m_Rect; }
	
};



// �̹��� ��� Ŭ����
class CUIImage : public CUIControl
{
public:
	CUIImage();
	virtual ~CUIImage();
protected:
	IDirect3DTexture9 *m_pTex;
	int m_nIdx;					// �̹��� �ε��� (default = 0)
	SVtxRhwTex m_V[ 6];
	int m_Width;				// �̹��� �ѳʺ�
	int m_Height;				// �̹��� �ѳ���

public:
	virtual BOOL Create( SControl *pCtrl );
	virtual void Render();
	virtual void Animate( int nTimeDelta );
	virtual BOOL MsgProc( SUIMsg *pMsg );
	virtual void SetRect( RECT *pRect );

	void SetImageIndex( int nIdx );
	int GetImageIndex() const { return m_nIdx; }

};



// ��ư Ŭ����
class CUIButton : public CUIControl
{
public:
	CUIButton();
	virtual ~CUIButton();
protected:
	CUIImage *m_pImg;

public:
	virtual BOOL Create( SControl *pCtrl );
	virtual void Render();
	virtual void Animate( int nTimeDelta );
	virtual BOOL MsgProc( SUIMsg *pMsg );

};


#endif // __UICONTROL_H__
