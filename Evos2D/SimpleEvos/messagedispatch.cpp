
#include "stdafx.h"

#include "messagesystem.h"
#include "messagedispatch.h"


//------------------------------------------------------------------------
// 
// [2011/1/14 jjuiddong]
//------------------------------------------------------------------------
void CMessageDispatch::SendMsg(const SMessage &msg)
{
	CMessageSystem::Get()->SendMessage(msg);
}

//------------------------------------------------------------------------
// true�� �����ϸ� �Ļ��� Ŭ�������� ���� ���޵ȴ�.
// [2011/1/14 jjuiddong]
//------------------------------------------------------------------------
BOOL CMessageDispatch::MsgDipatch(const SMessage &msg)
{
	// ��ӹ޴� Ŭ�������� �����Ѵ�.
	return TRUE;
}
