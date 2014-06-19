
#ifndef __MOUSEEVENTLISTNER_H__
#define __MOUSEEVENTLISTNER_H__

//------------------------------------------------------------------------
// ���콺 �̺�Ʈ�� ó���ϴ� �����ʴ�.
// �� Ŭ������ �ƹ��ϵ� ���� �ʴ´�.
// [2011/2/25 jjuiddong]
//------------------------------------------------------------------------

class CMouseEventListner
{
public:
	CMouseEventListner(CWnd *parent) : m_pParent(parent) {}
	virtual~CMouseEventListner() {}

protected:
	CWnd *m_pParent;

public:
	CWnd* GetParent() { return m_pParent; }
	virtual void Update(int elapsTick) {}
	virtual void OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {}
	virtual void OnLButtonDown(UINT nFlags, CPoint point) {}
	virtual void OnLButtonUp(UINT nFlags, CPoint point) {}
	virtual void OnMouseMove(UINT nFlags, CPoint point) {}
	virtual void OnRButtonDown(UINT nFlags, CPoint point) {}
	virtual void OnRButtonUp(UINT nFlags, CPoint point) {}

};

#endif
