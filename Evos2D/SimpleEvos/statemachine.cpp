
#include "stdafx.h"
#include "state.h"
#include "dbg.h"
#include "unit.h"
#include "statemachine.h"


CStateMachine::CStateMachine()
{

}


CStateMachine::~CStateMachine()
{
	Clear();

}


//------------------------------------------------------------------------
// ������Ʈ �߰�
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::PushState( CState *pstate )
{
	g_Dbg.Print("%d %s\n", pstate->GetUnit()->GetId(), pstate->GetStateName());

	m_StateStack.push_back(pstate);
	pstate->StateBegin();

}


//------------------------------------------------------------------------
// Stack�� ����� ��� State�� �����Ѵ�.
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::PopAllState()
{
	PopState(0);
}


//------------------------------------------------------------------------
// Stack�� ����� State�� �ε��� ���� idx���� ���ų� ũ�� ��� �����.
// stack �� bottom �� �ε������� 0 �̰� top���� �ö󰡸鼭 Ŀ����.
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::PopState( int idx )
{
	const int size = (int)m_StateStack.size();
	for (int i = idx; i < size; ++i)
	{
		CState *p = m_StateStack.back();
		p->StateEnd();
		m_StateStack.pop_back();
		m_GarbageStack.push_back(p);
	}
}


//------------------------------------------------------------------------
// pstate�� sub State �� child State �� ��� �����Ѵ�. pstate�� �״�� �д�.
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::PopSubState( CState *pstate )
{
	int stackidx = GetStateStackIndex(pstate);
	if (stackidx < 0) return;

	PopState(stackidx + 1);
}


//------------------------------------------------------------------------
// pstate�� ������ sub state(child state) �� ��� �����Ѵ�.
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::PopCurrentState( CState *pstate )
{
	int stackidx = GetStateStackIndex(pstate);
	if (stackidx < 0) return;

	PopState(stackidx);
}


//------------------------------------------------------------------------
// pstate �� StackIndex�� �����Ѵ�. ���ٸ� -1�� �����Ѵ�.
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
int CStateMachine::GetStateStackIndex( CState *pstate )
{
	int idx = 0;
	StateItor itor = m_StateStack.begin();
	while (m_StateStack.end() != itor)
	{
		CState *p = *itor++;
		if (pstate == p)
		{
			return idx;
		}
		++idx;
	}
	return -1;
}


//------------------------------------------------------------------------
// Ŭ���� �ʱ�ȭ
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::Clear()
{
	PopAllState();
}


//------------------------------------------------------------------------
// stack�� top �� ����ȴ�. 
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::Update( float timeDelta )
{
	if (m_StateStack.empty()) return;

	CState *pstate = m_StateStack.back();
	pstate->Update(timeDelta);

	ClearGarbage();
}


//------------------------------------------------------------------------
// 
// [2011/1/20 jjuiddong]
//------------------------------------------------------------------------
BOOL CStateMachine::MsgDipatch(const SMessage &msg)
{
	StateItor itor = m_StateStack.begin();
	while (m_StateStack.end() != itor)
	{
		if (!(*itor++)->MsgDipatch(msg))
		{
			break;
		}
	}
	return TRUE;
}


//------------------------------------------------------------------------
// 
// [2011/1/21 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::Render()
{
	if (m_StateStack.empty()) return;

	CState *pstate = m_StateStack.back();
	pstate->Render();

}


//------------------------------------------------------------------------
// 
// [2011/1/21 jjuiddong]
//------------------------------------------------------------------------
void CStateMachine::ClearGarbage()
{
	StateItor itor = m_GarbageStack.begin();
	while (m_GarbageStack.end() != itor)
	{
		CState *p = *itor++;
		delete p;
	}
	m_GarbageStack.clear();
}
