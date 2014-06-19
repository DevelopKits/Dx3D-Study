
#include "stdafx.h"
//#include "global.h"
#include "collision.h"

using namespace std;


///////////////////////////////////////////////////////////////////////////////////////
// Collision
int g_CompareId;

int FindCollision( ICollisionable *pObj )
{
	if( pObj->GetObjId() == g_CompareId )
		return 1;
	return 0;
}


CollisionList::CollisionList()
{
	m_pGrp1 = new SObjTree( 0 );
	m_pGrp2 = new SObjTree( 0 );

}

CollisionList::~CollisionList()
{
	ReleaseTree( m_pGrp1 );
	ReleaseTree( m_pGrp2 );

}


//-----------------------------------------------------------------------------//
// nGroup: 0, 1 �� �����Ѵ�.
// �浹�׽�Ʈ�� �ٸ� �׷쳢���� �����ϴ�.
// pObj : �浹�׽�Ʈ�� ������Ʈ
// nTestNum : �浹�׽�Ʈ ��ȣ�̸�, �ٸ� �׷쿡�� ���� �浹�׽�Ʈ��ȣ ���� �浹�׽�Ʈ�Ѵ�.
//-----------------------------------------------------------------------------//
void CollisionList::AddObj( int nGroup, ICollisionable *pObj, int nTestNum )
{
	SObjTree *grp;
	if( 0 == nGroup ) grp = m_pGrp1;
	else if( 1 == nGroup ) grp = m_pGrp2;
	else return;
	AddObj( grp, pObj, nTestNum );
}


//-----------------------------------------------------------------------------//
// �浹�׽�Ʈ�� ������Ʈ �߰�
// pParent : �θ� �浹 ������Ʈ
// pParent ������Ʈ�� �浹���� �ڽ����� pObj ������Ʈ�� �浹�׽�Ʈ �˻�ȴ�.
//-----------------------------------------------------------------------------//
void CollisionList::AddObj( ICollisionable *pParent, ICollisionable *pObj, int nTestNum )
{
	// �α׷쿡�� �θ� ��带 ã�´�.
	SObjTree *parent;
	parent = (SObjTree*)FindTree( m_pGrp1, pParent->GetObjId() );
	if( !parent ) parent = (SObjTree*)FindTree( m_pGrp2, pParent->GetObjId() );
	if( !parent ) return;
	AddObj( parent, pObj, nTestNum );
}


//-----------------------------------------------------------------------------//
// �浹�׽�Ʈ�� ������Ʈ �߰�
// nTestNum �� 3�� ������Ʈ�� �̵��Ҷ� �浹�� üũ�ϴ� ��ȣ�̴�.
//-----------------------------------------------------------------------------//
void CollisionList::AddObj( SObjTree *pParent, ICollisionable *pObj, int nTestNum )
{
	SObjTree *pnew = new SObjTree( pObj->GetObjId() );
	pnew->testnum = nTestNum;
	pnew->pobj = pObj;
	pnew->psphere = pObj->GetSphere();
	pnew->pminsphere = pObj->GetMinimumSphere();

	if( !pnew->psphere )
		pObj->GetCollisionBox( &pnew->boxlist );

	InsertChildTree( pParent, pnew );
}


//-----------------------------------------------------------------------------//
// ����Ʈ���� ������Ʈ �߰�
//-----------------------------------------------------------------------------//
void CollisionList::DelObj( ICollisionable *pObj )
{
	SObjTree *p1 = (SObjTree*)FindTree( m_pGrp1, pObj->GetObjId() );
	SObjTree *p2 = (SObjTree*)FindTree( m_pGrp2, pObj->GetObjId() );
	DelTree( m_pGrp1, pObj->GetObjId() );
	DelTree( m_pGrp2, pObj->GetObjId() );
	ReleaseTree( p1 );
	ReleaseTree( p2 );
}


//-----------------------------------------------------------------------------//
// �浹�׽�Ʈ�ϱ� ���� CollisionBox ��ǥ�� ������Ʈ�Ѵ�.
// ���� �̵��Ǿ��ٸ� CollisionTest() �Լ� ȣ�� ���� �� �Լ��� ���� ȣ���ؾ� �Ѵ�.
//-----------------------------------------------------------------------------//
void CollisionList::UpdateCollisionBox()
{
	SObjTree *pGrp1 = (SObjTree*)m_pGrp1->m_pChild;
	SObjTree *pGrp2 = (SObjTree*)m_pGrp2->m_pChild;
	while( pGrp1 )
	{
		pGrp1->pobj->UpdateCollisionBox();
		pGrp1 = (SObjTree*)pGrp1->m_pNext;
	}
	while( pGrp2 )
	{
		pGrp2->pobj->UpdateCollisionBox();
		pGrp2 = (SObjTree*)pGrp2->m_pNext;
	}
}


//-----------------------------------------------------------------------------//
// nTestNum ������ ������ CollisionBox�鸸 �浹�׽�Ʈ�Ѵ�.
// �����׷쳢���� �浹�׽�Ʈ ���� �ʴ´�.
// �浹�� ������Ʈ ������ �����Ѵ�.
//-----------------------------------------------------------------------------//
int CollisionList::CollisionTest( int nTestNum )
{
	SObjTree *pGrp1 = (SObjTree*)m_pGrp1->m_pChild;
	int cnt = 0;
	while( pGrp1 )
	{
		if( !pGrp1->pobj->IsTest(nTestNum) )
		{
			pGrp1 = (SObjTree*)pGrp1->m_pNext;
			continue;
		}

		SObjTree *pGrp2 = (SObjTree*)m_pGrp2->m_pChild;
		while( pGrp2 )
		{
			if( !pGrp2->pobj->IsTest(nTestNum) )
			{
				pGrp2 = (SObjTree*)pGrp2->m_pNext;
				continue;
			}

			// Root�׷� �浹 �׽�Ʈ
			if( CollisionTest_Obj(pGrp1, pGrp2, nTestNum) )
			{
				// �ڽ��� ���ٸ� �浹�׽�Ʈ ��
				// nTestNum�� 3�̶�� ��Ʈ�� �浹�׽�Ʈ �Ѵ�.
				if( 3 == nTestNum || (!pGrp1->m_pChild && !pGrp2->m_pChild) )
				{
					m_ChkTable[ cnt][ 0] = pGrp1->pobj;
					m_ChkTable[ cnt][ 1] = pGrp2->pobj;
					++cnt;
				}
				else
				{
					// �ڽ��� �ִٸ� �ڽı��� �浹�׽�Ʈ �����ؾ� ���������� �浹�� ���°� �ȴ�.
					SObjTree *p1 = (SObjTree*)((pGrp1->m_pChild)? pGrp1->m_pChild : pGrp1);
					SObjTree *p2 = (SObjTree*)((pGrp2->m_pChild)? pGrp2->m_pChild : pGrp2);
					if( CollisionTest_SrcRec(p1, p2, nTestNum) )
					{
						m_ChkTable[ cnt][ 0] = pGrp1->pobj;
						m_ChkTable[ cnt][ 1] = pGrp2->pobj;
						++cnt;
					}
				}
			}
			pGrp2 = (SObjTree*)pGrp2->m_pNext;
		}
		pGrp1 = (SObjTree*)pGrp1->m_pNext;
	}
	return cnt;
}


//-----------------------------------------------------------------------------//
// �׷쳻 �浹�׽�Ʈ
// nGroup : �׽�Ʈ�� �׷�
// nTestNum :�浹�׽�Ʈ ��ȣ
//-----------------------------------------------------------------------------//
int CollisionList::CollisionGroupTest( int nGroup, int nTestNum )
{
	SObjTree *grp;
	if( 0 == nGroup ) grp = m_pGrp1;
	else if( 1 == nGroup ) grp = m_pGrp2;
	else return 0;
	return _CollisionGroupTest( grp, nTestNum );
}
int CollisionList::_CollisionGroupTest( SObjTree *pGrp, int nTestNum )
{
	SObjTree *pGrp1 = (SObjTree*)pGrp->m_pChild;
	
	int cnt = 0;
	while( pGrp1 )
	{
		if( !pGrp1->pobj->IsTest(nTestNum) )
		{
			pGrp1 = (SObjTree*)pGrp1->m_pNext;
			continue;
		}

		SObjTree *pGrp2 = (SObjTree*)pGrp1->m_pNext;
		while( pGrp2 )
		{
			if( !pGrp2->pobj->IsTest(nTestNum) )
			{
				pGrp2 = (SObjTree*)pGrp2->m_pNext;
				continue;
			}

			// Root�׷� �浹 �׽�Ʈ
			if( CollisionTest_Obj(pGrp1, pGrp2, nTestNum) )
			{
				// �ڽ��� ���ٸ� �浹�׽�Ʈ ��
				// nTestNum�� 3�̶�� ��Ʈ�� �浹�׽�Ʈ �Ѵ�.
				if( 3 == nTestNum || (!pGrp1->m_pChild && !pGrp2->m_pChild) )
				{
					m_ChkTable[ cnt][ 0] = pGrp1->pobj;
					m_ChkTable[ cnt][ 1] = pGrp2->pobj;
					++cnt;
				}
				else
				{
					// �ڽ��� �ִٸ� �ڽı��� �浹�׽�Ʈ �����ؾ� ���������� �浹�� ���°� �ȴ�.
					SObjTree *p1 = (SObjTree*)((pGrp1->m_pChild)? pGrp1->m_pChild : pGrp1);
					SObjTree *p2 = (SObjTree*)((pGrp2->m_pChild)? pGrp2->m_pChild : pGrp2);
					if( CollisionTest_SrcRec(p1, p2, nTestNum) )
					{
						m_ChkTable[ cnt][ 0] = pGrp1->pobj;
						m_ChkTable[ cnt][ 1] = pGrp2->pobj;
						++cnt;
					}
				}
			}
			pGrp2 = (SObjTree*)pGrp2->m_pNext;
		}
		pGrp1 = (SObjTree*)pGrp1->m_pNext;
	}
	return cnt;
}


//-----------------------------------------------------------------------------//
// �ΰ��� SObjTree �׷��� �浹�׽�Ʈ �Ѵ�.
// SObjTree�� Ʈ���ڷ� ������ �Ǿ��ֱ� ������ Ʈ���� ��ȸ�ϱ� ���ؼ��� 
// �ΰ��� State�� �ʿ��ϴ�. (CollisionTest_SrcRec, CollisionTest_TargetRec)
//-----------------------------------------------------------------------------//
BOOL CollisionList::CollisionTest_SrcRec( SObjTree *pSrc, SObjTree *pTarget, int nTestNum )
{
	if( 0 == nTestNum ) return FALSE; // error
	if( !pSrc || !pTarget ) return FALSE;

	if( CollisionTest_TargetRec(pSrc, pTarget, nTestNum) ) return TRUE;
	return CollisionTest_SrcRec( (SObjTree*)pSrc->m_pNext, pTarget, nTestNum );
}
BOOL CollisionList::CollisionTest_TargetRec( SObjTree *pSrc, SObjTree *pTarget, int nTestNum )
{
	if( 0 == nTestNum ) return FALSE; // error
	if( !pSrc || !pTarget ) return FALSE;

	if( CollisionTest_Obj(pSrc, pTarget, nTestNum) )
	{
		if( !pSrc->m_pChild && !pTarget->m_pChild ) return TRUE;
		// �ڽ��� �������� �ڽı��� �浹�׽�Ʈ �Ѵ�.
		SObjTree *p1 = (SObjTree*)((pSrc->m_pChild)? pSrc->m_pChild : pSrc);
		SObjTree *p2 = (SObjTree*)((pTarget->m_pChild)? pTarget->m_pChild : pTarget);
		return CollisionTest_SrcRec( p1, p2, nTestNum );
	}
	return CollisionTest_TargetRec( (SObjTree*)pSrc, (SObjTree*)pTarget->m_pNext, nTestNum );
}


//-----------------------------------------------------------------------------//
// pSrc, pTarget ��尡 ���� nTestNum �׽�Ʈ��ȣ�� ���������� �浹�׽�Ʈ �Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CollisionList::CollisionTest_Obj( SObjTree *pSrc, SObjTree *pTarget, int nTestNum )
{
//	if( 0 == nTestNum ) return FALSE; // error
	if( !pSrc || !pTarget ) return FALSE;

	// Src TestNum���� 0�϶��� Target�� TestNum������� �浹�׽�Ʈ�� �Ѵ�.
	// Src TestNum���� �����Ǿ� �������� ���ڷ� �Ѿ�� nTestNum���� Target�� TestNum�� �������� �浹�׽�Ʈ�Ѵ�.
	if( ((0 != pSrc->testnum) && (nTestNum != pSrc->testnum)) ||
		(((0 != pSrc->testnum) && (0 != pTarget->testnum)) && (pSrc->testnum != pTarget->testnum)) )
		return FALSE;

	// Sphere vs Sphere �浹�׽�Ʈ
	if( 3 == nTestNum )
	{
		if( pSrc->pminsphere && pTarget->pminsphere )
			return pSrc->pminsphere->Collision( pTarget->pminsphere );
	}
	else
	{
		if( pSrc->psphere && pTarget->psphere )
			return pSrc->psphere->Collision( pTarget->psphere );
	}

	// �ٸ�������Ʈ���� �浹�׽�Ʈ �Ұ�
	if( pSrc->psphere || pTarget->psphere )
		return FALSE;

	// Box vs Box �浹�׽�Ʈ
	if( pSrc->boxlist.size() <= 0 || pTarget->boxlist.size() <= 0 )
		return FALSE;

	// �浹 üũ
	list<Box*>::iterator it = pSrc->boxlist.begin();
	while( pSrc->boxlist.end() != it )
	{
		list<Box*>::iterator kt = pTarget->boxlist.begin();
		while( pTarget->boxlist.end() != kt )
		{
			if( (*it)->Collision(*kt) )
			{
				return TRUE;
			}
			++kt;
		}
		++it;
	}

	return FALSE;
}

