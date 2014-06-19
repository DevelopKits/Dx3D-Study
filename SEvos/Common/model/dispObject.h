
#ifndef __DISPOBJECT_H__
#define __DISPOBJECT_H__


//------------------------------------------------------------------------
// ȭ�鿡 ����ϴ� ������Ʈ�� �߻�ȭ�� Ŭ������.
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
class CDispObject
{
public:
	CDispObject() { }
	virtual ~CDispObject() { }

public:
	virtual int GetId() = 0;
	virtual void Render() = 0;
	virtual void RenderDepth() = 0;
	virtual BOOL Update( int elapseTime ) = 0;
	virtual Matrix44* GetWorldTM() = 0;
	virtual void Clear() = 0;

};

#endif
