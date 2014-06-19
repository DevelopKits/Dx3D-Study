
#ifndef __MESSAGESYSTEM_H__
#define __MESSAGESYSTEM_H__

#include <list>
#include "message.h"


// �޼��� ����
class CMessageDispatch;
typedef struct SMessage
{
	Message msg;
	CMessageDispatch *preceiver;
	CMessageDispatch *psender;
	int param1;

	SMessage() {}
	SMessage(Message m):msg(m) { }
	SMessage(Message m, CMessageDispatch *receiver, CMessageDispatch *sender, int _param1 ) :
	msg(m), preceiver(receiver), psender(sender), param1(_param1) { }
} SMessage;


// �޼��� �ý���
class CMessageSystem
{
protected:
	CMessageSystem() {} // �̱����̶� �ν��Ͻ��� �ܺο��� ������ �� ����.
public:
	virtual ~CMessageSystem() {}

protected:
	typedef std::list<SMessage> MsgList;
	typedef MsgList::iterator MsgItor;

	static CMessageSystem *m_pInstance;
	MsgList m_MsgList;

public:
	void SendMessage( const SMessage &msg );
	void PostMessage( const SMessage &pmsg );
	SMessage GetNextMessage();
	void Update( const float timeDelta );
	void Clear();

	// �̱��� ����
	static CMessageSystem* Get()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CMessageSystem();
		}
		return m_pInstance;
	}

	static void Release()
	{
		if (m_pInstance)
		{
			m_pInstance->Clear();
			delete m_pInstance;
		}
		m_pInstance = NULL;
	}

};

#endif
