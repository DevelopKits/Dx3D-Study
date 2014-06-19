
#include "stdafx.h"
#include "../Common/global.h"
#include "messagedispatch.h"

#include "messagesystem.h"

CMessageSystem *CMessageSystem::m_pInstance = NULL;

//
// �޼����� ����Ʈ�� �����Ѵ�.
void CMessageSystem::SendMessage( const SMessage &msg )
{
	m_MsgList.push_back(msg);
}


//
//
void CMessageSystem::PostMessage( const SMessage &msg )
{
	m_MsgList.push_back(msg);
}


//------------------------------------------------------------------------
// 
// [2011/1/14 jjuiddong]
//------------------------------------------------------------------------
SMessage CMessageSystem::GetNextMessage()
{
	if (m_MsgList.empty()) 
	{
		return SMessage(MSG_NONE);
	}
	SMessage msg = m_MsgList.front();
	m_MsgList.pop_front();
	return msg;
}


//------------------------------------------------------------------------
// ����� �޼����� ��� ó���Ѵ�.
// [2011/1/14 jjuiddong]
//------------------------------------------------------------------------
void CMessageSystem::Update( const float timeDelta )
{
	while (!m_MsgList.empty())
	{
		if (m_MsgList.front().preceiver)
		{
			m_MsgList.front().preceiver->MsgDipatch(m_MsgList.front());
		}
		m_MsgList.pop_front();
	}

}


//------------------------------------------------------------------------
// �޼��� �ý��� �ʱ�ȭ
// [2011/1/14 jjuiddong]
//------------------------------------------------------------------------
void CMessageSystem::Clear()
{
	m_MsgList.clear();

}

