
#include "global.h"
#include "control.h"
#include "character.h"
#include "ai_object.h"


float g_flag2 = 12.f;


//-----------------------------------------------------------------------------//
// ĳ���� �ΰ����� ��ũ��Ʈ���� ȣ���ϴ� �ݹ��Լ���.
// nFuncID : �Լ� Ÿ���� ��Ÿ����.
//		LoadVariable:  class pointer, id (import Ŭ���� ������� �� ���)
//					   id				 (import ���� ����)
//		StoreVariable: class pointer, value, id (import Ŭ���� ������� �� ���)
//					   value, id				(import ���� ����)
// pProcessor : ��ũ��Ʈ ���μ���, �� ��ü�� ���ؼ� ����,���ϰ��� �����Ҽ� �ִ�.
//-----------------------------------------------------------------------------//
void CharacterAICallBack( int nFuncID, ns_script::CProcessor *pProcessor )
{
	switch( nFuncID )
	{
	case ns_script::LoadPointer:
		{
			CAIObject *pai = (CAIObject*)pProcessor->GetArgumentClass( 0 );
			int id = (int)pProcessor->GetArgumentFloat( 1 );

			switch( id )
			{
			// global flag
			case 1000:
				pProcessor->SetReturnValue( &g_flag2 );
				break;

			case 200:	// AI::state
				pProcessor->SetReturnValue( &pai->m_State );
				break;
			case 201:	// AI::elapse
				pProcessor->SetReturnValue( &pai->m_Elapse );
				break;
			case 202:	// AI::distance
				pProcessor->SetReturnValue( &pai->m_Distance );
				break;
			case 203:	// AI::hp
				pProcessor->SetReturnValue( (DWORD)&pai->m_pCharacter->GetHP() );
				break;
			case 204:	// AI::attack_distance
				pProcessor->SetReturnValue( &pai->m_AttackDistance );
				break;
			case 205:	// AI::level
				pProcessor->SetReturnValue( (DWORD)&pai->m_pCharacter->GetLevel() );
				break;
			}
		}
		break;

	// AI::ChangeAIState( state )
	case 211:
		{
			CAIObject *pai = (CAIObject*)pProcessor->GetArgumentClass( 0 );
			pai->ChangeState( (int)pProcessor->GetArgumentFloat(1) );
		}
		break;

	// AI::IsDead()
	case 212:
		{
			CAIObject *pai = (CAIObject*)pProcessor->GetArgumentClass( 0 );
			pProcessor->SetReturnValue( pai->m_pCharacter->IsDead() );
		}
		break;

	// GetAI()
	case 1021:
		{
			char *pstr = pProcessor->GetArgumentStr( 0 );
			CAIObject *pai = g_Control.GetAI( pstr );
			pProcessor->SetReturnValue( (DWORD)pai );
		}
		break;
	}

}


CAIObject::CAIObject()
{
	ChangeState( WAIT );

}


//-----------------------------------------------------------------------------//
// �ΰ����� ����� ���� ĳ���� Ŭ������ �����ϰ�, �ΰ����� ��ũ��Ʈ�� �ε��Ѵ�. 
// ��ũ��Ʈ�� ���ٸ� Animate() �Լ����� �ƹ�ó�� ���� �ٷ� ���ϵȴ�.
// pControl: ��Ʈ�� Ŭ����, ĳ����Ŭ�������� ����� �������� ��Ʈ���� ���ؾ��Ѵ�.
// pCharacter: �ΰ����ɿ� ���ؼ� ���۵� ĳ���� Ŭ����
// pAIFileName: �ΰ����� ��ũ��Ʈ ���ϸ�
//-----------------------------------------------------------------------------//
BOOL CAIObject::Create( CControl *pControl, CCharacter *pCharacter, char *pAIFileName )
{
	m_bScriptLoad = (BOOL)pAIFileName;
	m_pCharacter = pCharacter;
	m_pControl = pControl;
	m_pHero = pControl->GetCharacter( "hero" );
	m_IncTime = 0;
	m_AttackDistance = pCharacter->GetAttackLength();
	m_State = AI_WAIT;
	m_PrevState = AI_WAIT;

	if( m_pHero )
	{
		Vector3 v0 = m_pCharacter->GetPos();
		Vector3 v1 = m_pHero->GetPos();
		m_Distance = sqrtf( v0.LengthRoughly( v1 ) );
	}

	if( pAIFileName )
	{
		// ��ũ��Ʈ ���� ���ڰ����� ĳ�����̸��� �����ϱ� ������ 
		// ��ũ��Ʈ������ �ش� ĳ���� �����Ϳ� ���� �� �� �ִ�.
		g_Scr.Execute( pAIFileName, CharacterAICallBack, pCharacter->GetName() );
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// ���ϸ��̼�
// ���ΰ��� �ΰ������� ó������ �ʴ´�.
//-----------------------------------------------------------------------------//
BOOL CAIObject::Animate( int nDelta )
{
	if( !m_bScriptLoad ) return TRUE;
	int skipstate = JUMP | KO | DAMAGE | DEAD | STATE5 | STATE6;

	m_IncTime += nDelta;
	m_Elapse += nDelta;
	if( m_IncTime < 200 )
		return TRUE;

	m_IncTime = 0;

	// ���ΰ����� �Ÿ��� ����Ѵ�.
	Vector3 v0 = m_pCharacter->GetPos();
	Vector3 v1 = m_pHero->GetPos();
	m_Distance = sqrtf( v0.LengthRoughly( v1 ) );

	switch( m_State )
	{
	case AI_WAIT:
		// ��ġ�̵� ���� ���ڸ��� ������
		break;

	case AI_WATCH:	// ������
		{
			// ���ΰ� ������ ���ƴٴѴ�.
		}
		break;

	case AI_ATTACK:
	case AI_COMBOATTACK:
		{
			if( skipstate & m_pCharacter->GetState() )
				break;

			Vector3 dir = v1 - v0;
			dir.y = 0;
			dir.Normalize();
	
			// �������� �ٷ� AttackWait ���·� �ٲ��.			
			if( m_pCharacter->Attack(dir, KEY_A) )
				ChangeState( AI_ATTACKWAIT );
		}
		break;

	case AI_CHASE:
		{
			if( skipstate & m_pCharacter->GetState() )
				break;

			if( 15.f < m_Distance && 35.f > m_Distance )
			{
				Vector3 v = v1 - v0;
				v.y = 0;
				v.Normalize();
				m_pCharacter->SetDirection( v );
			}
			else
			{
				if( m_PrevState == m_State )
				{
					if( m_pCharacter->GetDestPos().EqualEpsilon(3.f,v0) || (m_Elapse > 4000) )
					{
						Vector3 v = v1 - v0;
						v.y = 0;
						v.Normalize();
						v = v0 + (v * (m_Distance - 30.f));
						m_pCharacter->Move( v );
					}
				}
				else
				{
					Vector3 v = v1 - v0;
					v.y = 0;
					v.Normalize();
					v = v0 + (v * (m_Distance - 30.f));
					m_pCharacter->Move( v );
				}
			}
		}
		break;

	case AI_OUTMOVE:
		{
			if( skipstate & m_pCharacter->GetState() )
				break;

			Vector3 v = v1 - v0;
			v.y = 0;
			v.Normalize();
			v = v0 + (v * (m_Distance - 130.f));
			m_pCharacter->Move( v );
		}
		break;
	
	case AI_INMOVE:
		{
			if( skipstate & m_pCharacter->GetState() )
				break;

			if( 57.f < m_Distance && 63.f > m_Distance )
			{
				Vector3 v = v1 - v0;
				v.y = 0;
				v.Normalize();
				m_pCharacter->SetDirection( v );
			}
			else
			{
				Vector3 v = v1 - v0;
				v.y = 0;
				v.Normalize();
				v = v0 + (v * (m_Distance - 80.f));
				m_pCharacter->Move( v );
			}
		}
		break;

	case AI_RUNAWAY:
		break;

	case AI_DEAD:
		return (m_Elapse < 10000);
	}

	m_PrevState = m_State;
	return TRUE;
}


//-----------------------------------------------------------------------------//
// �ΰ����� ��ũ��Ʈ���� ���°� �ٲ�� ���Լ��� ȣ���Ѵ�.
// State : ��ȯ�� ���°�
//-----------------------------------------------------------------------------//
void CAIObject::ChangeState( int State )
{
	m_PrevState = m_State;
	m_State = (AISTATE)State;

	m_Elapse = 0;
}


//-----------------------------------------------------------------------------//
// �ΰ�����Ŭ������ ����ϴ� ĳ���Ϳ� ����� �������� ����
// �� �Լ��� ȣ��ȴ�. CControl Ŭ�������� ȣ��ȴ�.
//-----------------------------------------------------------------------------//
void CAIObject::Command( SMsg Msg )
{
	switch( Msg.type )
	{
	case MSG_KEYDOWN:
//		KeyProc( Msg.lparam, Msg.wparam );
		break;

	case MSG_SETPOS:
		{
			Vector3 *pv = (Vector3*)Msg.lparam;
			if( m_pHero )
			{
				Vector3 v1 = m_pHero->GetPos();
				m_Distance = sqrtf( pv->LengthRoughly( v1 ) );
			}
		}
		break;

	case MSG_MOVPOS:
		{

		}
		break;
	}

}
