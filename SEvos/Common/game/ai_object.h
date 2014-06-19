//-----------------------------------------------------------------------------//
// 2008-02-28  programer: jaejung ��(���բ�)��
// 
//	�ΰ������� ����ϴ� Ŭ������.
//	�ڽ��� ĳ���͸� Control���� ���� ���۷����� ������, ��Ȳ�� 
//	�°� CCharacterŬ�������� �ٷ� �Է��� ���ϰų�, ControlŬ������
//	���� ����� ������.
//
//	���ѻ��¸ӽ��� ��ũ��Ʈ���� �����ǰ�, CAIObject�� �� ���¿�
//	���� �������� �۾��� �ô´�. 
//	������� Wait���¿����� ��ġ�̵� ���� ������ �ְ�,
//	Walk���¿����� ��ǥ�������� ������ �ӵ��� �����̰� �ϸ�,
//	Attack���¿����� ��ǥ���� ���⿡ ���� �޺������̳�, �����ڼ���
//	���ϰ� �Ѵ�.
//
//	CAIObject�� ���ΰ��� CharacterŬ������ ������, ��Ȳ�� ����
//	���ΰ��� ����, ��ġ, ü���� �м��Ҽ� �ִ�.
//
//-----------------------------------------------------------------------------//

#if !defined(__AIOBJECT_H__)
#define __AIOBJECT_H__


// CAIObject
class CControl;
class CCharacter;
class CAIObject
{
public:
	CAIObject();
	virtual ~CAIObject() {}

	enum AISTATE
	{ 
		AI_WAIT=1, AI_WATCH, AI_ATTACK, AI_COMBOATTACK, AI_ATTACKWAIT,
		AI_OUTMOVE, AI_INMOVE, AI_CHASE, AI_RUNAWAY, AI_DEAD 
	};

protected:
	CControl *m_pControl;
	CCharacter *m_pHero;	// ���ΰ� ĳ����
	BOOL m_bScriptLoad;

public:
	AISTATE m_State;			// �ΰ����� ����
	AISTATE m_PrevState;		// �� ����
	CCharacter *m_pCharacter;	// ĳ����
	int m_IncTime;				// �����ð� (Animate ó���Ǹ� �ʱ�ȭ�ȴ�)
	int m_Elapse;				// ���°�� �ð�
	float m_Distance;			// ������ �Ÿ�(���ΰ����� �Ÿ�)
	float m_AttackDistance;		// ������ �ϱ����� �ּҰŸ�

public:
	BOOL Create( CControl *pControl, CCharacter *pCharacter, char *pAIFileName );
	BOOL Animate( int nDelta );
	void ChangeState( int State );
	void Command( SMsg Msg );

protected:
	void Action( int nDelta );

};

#endif //  __AIOBJECT_H__
