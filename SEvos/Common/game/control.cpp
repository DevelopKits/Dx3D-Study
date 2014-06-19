
#include "stdafx.h"
#include "control.h"
#include "character.h"
#include "ai_object.h"
#include "../model/collision.h"

using namespace std;

CControl g_Control;	// ��������

CControl::CControl() : m_pHero(NULL)
{
	m_pCollision = new CollisionList;
//	m_pTerrain = new CTerrain;
 
}


CControl::~CControl() 
{
	Clear();

}


//-----------------------------------------------------------------------------//
// ���ϸ��̼�
//-----------------------------------------------------------------------------//
void CControl::Animate( int nDelta )
{
	CharMap::iterator it = m_ModelList.begin();
	while( m_ModelList.end() != it )
	{
		// ���� ������Ʈ�� DeadList�� ���Եȴ�.
		it->second.pchar->Animate( nDelta );
		if( !it->second.pai->Animate(nDelta) )
		{
			m_DeadList.push_back( it->second );
			it = m_ModelList.erase( it );
		}
		else
		{
			++it;
		}
	}

	m_pCollision->UpdateCollisionBox();
	for( int k=1; k < 3; ++k )
	{
		int collisioncnt = m_pCollision->CollisionTest( k );
		if( 0 < collisioncnt )
		{
			g_Dbg.Console( "Collision\n" );
			for( int i=0; i < collisioncnt; ++i )
			{
				m_pCollision->m_ChkTable[ i][ 0]->Collision( k, m_pCollision->m_ChkTable[ i][ 1] );
				m_pCollision->m_ChkTable[ i][ 1]->Collision( k, m_pCollision->m_ChkTable[ i][ 0] );
			}
		}
	}

	MsgProc();
	AIGroupProc( nDelta );

	// ���ΰ� ��ġ�� ���� ī�޶� ��ġ�� �̵��Ѵ�.
	if( m_pHero )
	{
		Vector3 pos = m_pHero->GetPos() + g_DefaultCameraPos;
//		Vector3 cp = g_Camera.GetPosition();
//		if( cp != pos )
		{
			g_Camera.SetPosition( pos );
		}
	}
}


//-----------------------------------------------------------------------------//
// ���
//-----------------------------------------------------------------------------//
void CControl::Render()
{
	CharMap::iterator it = m_ModelList.begin();
	while( m_ModelList.end() != it )
	{
		it->second.pchar->Render();
		++it;
	}
//	m_pTerrain->Render();

}


//-----------------------------------------------------------------------------//
// ĳ���� �߰�
//-----------------------------------------------------------------------------//
CCharacter* CControl::AddCharacter( CHARACTER_TYPE Type, char *pName, char *pModelFile, 
							        char *pAniFile, char *pWeaponFile, char *pComboFile, char *pAIFile )
{
	if( !pModelFile ) return NULL;

	CharMap::iterator it = m_ModelList.find( pName );	// ���� ���̵� �����ϸ� ���ϵȴ�.
	if( m_ModelList.end() != it ) return it->second.pchar;

	// ĳ���� Ŭ���� ����
	CCharacter *pchar = new CCharacter;
	pchar->SetName( pName );
	pchar->LoadBody( pModelFile );
	if( pWeaponFile ) pchar->LoadWeapon( pWeaponFile );
	if( pAniFile ) pchar->LoadAni( pAniFile );
	if( pComboFile ) pchar->LoadCombo( pComboFile );
	pchar->SetCollision( Type, m_pCollision );
	pchar->SetTerrain( m_pTerrain );

	// �ΰ����� Ŭ���� ����
	CAIObject *pai = new CAIObject;
	pai->Create( this, pchar, pAIFile );

	// ���
	m_ModelList.insert( CharMap::value_type(pName,SObject(pchar,pai)) );

	if( !strcmp("hero", pName) )
		m_pHero = pchar;

	return pchar;
}




//-----------------------------------------------------------------------------//
// �޸� �Ұ�, �ʱ�ȭ
//-----------------------------------------------------------------------------//
void CControl::Clear()
{
	CharMap::iterator it = m_ModelList.begin();
	while( m_ModelList.end() != it )
	{
		SObject s = it++->second;
		delete s.pchar;
		delete s.pai;
	}
	m_ModelList.clear();

	CharList::iterator i = m_DeadList.begin();
	while( m_DeadList.end() != i )
	{
		SObject s = *i++;
		delete s.pchar;
		delete s.pai;
	}
	m_DeadList.clear();

	SAFE_DELETE( m_pCollision );
//	SAFE_DELETE( m_pTerrain );
}


//-----------------------------------------------------------------------------//
// �޼����� �޾Ƽ� �޼���ť�� �����Ѵ�.
//-----------------------------------------------------------------------------//
void CControl::Command( SMsg &Msg )
{
	m_MsgQ.push( Msg );
}


//-----------------------------------------------------------------------------//
// �޼����� ó���Ѵ�.
//-----------------------------------------------------------------------------//
void CControl::MsgProc()
{
	const int MAX_PROC = 10;
	int proccnt = 0;

	while( !m_MsgQ.empty() && proccnt < MAX_PROC )
	{
		const SMsg &msg = m_MsgQ.front();

		// Control���� ó���� �޼���
		if( MSG_CONTROL == msg.type )
		{

		}
		else
		{
			// id�� NULL�̸� ���ΰ��� ���Ѵ�.
			char *key = "hero";
			if( msg.id ) key = msg.id;

			CharMap::iterator it = m_ModelList.find( key );
			if( m_ModelList.end() != it ) 
			{
				SObject obj = it->second;
				obj.pchar->Command( msg );
				obj.pai->Command( msg );
			}
		}

		proccnt++;
		m_MsgQ.pop();
	}
}


//-----------------------------------------------------------------------------//
// ĳ����, �ΰ����� Ŭ���� ã��
//-----------------------------------------------------------------------------//
CCharacter* CControl::GetCharacter( char *pCharacterName )
{
	CharMap::iterator it = m_ModelList.find( pCharacterName );
	if( it == m_ModelList.end() ) return NULL;
	return it->second.pchar;
}
CAIObject* CControl::GetAI( char *pCharacterName )
{
	CharMap::iterator it = m_ModelList.find( pCharacterName );
	if( it == m_ModelList.end() ) return NULL;
	return it->second.pai;
}


//-----------------------------------------------------------------------------//
// ������ Ÿ���� ��ġ�� �����Ѵ�. Ÿ���� ���ٸ� FALSE�� �����Ѵ�.
// pCharacter : ������ ĳ���� 
// pReval : Ÿ����ġ ���� �������� ���ϵ� ����
//-----------------------------------------------------------------------------//
CCharacter* CControl::GetAttackTarget( CCharacter *pCharacter, Vector3 *pReval )
{
	Vector3 p0 = pCharacter->GetPos();
	CHARACTER_TYPE type = pCharacter->GetCharacterType();

	// ���ΰ��� �����Ұ�� ���ΰ��� �ٶ󺸴� �������� Ÿ�ٸ� �����ϸ�, �ڿ� �ִ� 
	// Ÿ���� �������� ���Ѵ�.
	// �׷��� ���� ��� Ÿ���� �տ� �ֵ� �ڿ� �ֵ� ������� Ÿ���� �����Ҽ� �ִ�.
	// Ÿ���� �ڿ� �ִٸ� dotproduct() ���� ������ �ǹǷ� �̸� �̿��ؼ� �����Ҽ� �ִ�.
	float max_angle = (pCharacter->GetCharacterType()==CHAR_HERO)? 0.f : -2.f;
	float attack_len = pCharacter->GetAttackLength();
	CCharacter *ptarget = NULL;

	CharMap::iterator it = m_ModelList.begin();
	while( m_ModelList.end() != it )
	{
		SObject obj = it++->second;
		if( type == obj.pchar->GetCharacterType() ) continue;

		Vector3 p1 = obj.pchar->GetPos();
		float l = (float)sqrt( p0.LengthRoughly(p1) );
		// ���� ���� ���� �ִ��� �˻�
		if( attack_len > l )
		{
			Vector3 dir = p1 - p0;
			dir.Normalize();
			float angle = ( dir.DotProduct( pCharacter->GetDirection() ) );

			// �������� ����� ���� ��ġ�� �������� Ÿ���� �ȴ�.
			if( max_angle < angle )
			{
				ptarget = obj.pchar;
				max_angle = angle;
			}
		}
	}

	if( !ptarget ) return NULL;

	*pReval = ptarget->GetPos() - p0;
	pReval->y = 0;
	pReval->Normalize();
	
	return ptarget;
}


//-----------------------------------------------------------------------------//
// �������� �ε�
//-----------------------------------------------------------------------------//
BOOL CControl::LoadTerrain( char *pFileName )
{
//	if( !m_pTerrain->LoadTerrain(pFileName) )
//		return FALSE;
//	if( !m_pTerrain->SetTerrain() )
//		return FALSE;
	return TRUE;
}


//-----------------------------------------------------------------------------//
// ���� �ΰ����ɿ� ���õ� ó��
//		ĳ���Ͱ� �̵��� �� ���������� ���� ��ġ�� ����ϴٸ� �ߺ����� �ʴ� ��ġ�� 
//		�ٽ� �̵��Ѵ�.
//-----------------------------------------------------------------------------//
void CControl::AIGroupProc( int nDelta )
{
	m_ElapseTime += nDelta;
	if( 300 > m_ElapseTime ) return;
	m_ElapseTime = 0;

	// ĳ���� �̵��浹 �׽�Ʈ��ȣ�� 3���̴�.
	int collisioncnt = m_pCollision->CollisionGroupTest( 1, 3 );
	if( 0 < collisioncnt )
	{
		for( int i=0; i < collisioncnt; ++i )
		{
			CCharacter *pc0 = (CCharacter*)m_pCollision->m_ChkTable[ i][ 0];
			CCharacter *pc1 = (CCharacter*)m_pCollision->m_ChkTable[ i][ 1];

			if( CHAR_HERO == pc0->GetCharacterType() || CHAR_HERO == pc1->GetCharacterType() ) continue;

			// pc1, pa1 : ������ ������Ʈ
			Vector3 p0;
			Vector3 p1 = pc1->GetPos();
			if( pc1->GetDestPos().EqualEpsilon(3.f, p1) )
			{
				// exchange
				CCharacter *ptmp1 = pc1;
				pc1 = pc0;
				pc0 = ptmp1;
			}

			p0 = pc0->GetPos();
			p1 = pc1->GetPos();
			Vector3 vt = p1 - p0;
			vt.Normalize();

			Vector3 v = pc0->GetDirection() + vt;
			v.Normalize();
			v *= 20.f;

			if( pc1->GetDestPos().EqualEpsilon(3.f, p1) )
				pc1->Move( p0 + v );
		}
	}
}
