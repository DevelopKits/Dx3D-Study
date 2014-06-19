//-----------------------------------------------------------------------------//
// 2008-01-21  programer: jaejung ��(���բ�)��
// 
// CCollisionList Ŭ������ �浹�׽�Ʈ�� �ϰ������� ó���Ѵ�.
// CCollisionList Ŭ������ �浹�׽�Ʈ�� ������Ʈ�� 2���� �׷����� ������, �� �׷���
// Ʈ�� �ڷᱸ�� ���´�. �� �θ𳢸� �浹�׽�Ʈ�� �����ϸ� �ڽ����� ������ �浹�׽�Ʈ
// �ϰ�, ��������� �浹�׽�Ʈ�� �� ���� ������ �浹�׽�Ʈ�� �����ϸ� �浹�Ǿ��ٰ� �˷��ش�.
// �� ������Ʈ�� nTestNum�� ������ �ٸ� �׷쳢�� �浹�׽�Ʈ�� ���� nTestNum������ �浹�׽�Ʈ�Ѵ�.
// �浹�׽�Ʈ�� ������Ʈ�� ICollisionable �������̽��� ��ӹ޾ƾ� �Ѵ�.
//-----------------------------------------------------------------------------//

#if !defined(__COLLISION_H__)
#define __COLLISION_H__

#pragma warning (disable: 4786)
#include <list>


///////////////////////////////////////////////////////////////////////////////////////
// interface ICollisionable
// �浹�׽�Ʈ�� ������Ʈ�� ICollisionable �������̽��� ��ӹ޾� �����ؾ� �Ѵ�.
struct ICollisionable
{
	// Collision Test���� ����
	virtual BOOL IsTest( int TestNum )=0;
	virtual int GetObjId()=0;

	//-----------------------------------------------------------------------------//
	// ���� ��������� �����Ѵ�.
	// ���� ���� �ٸ� ���� �ڽ����� ����Ǿ� �ִٸ� �θ��� ����� �����ؼ� ���ϵȴ�.
	//-----------------------------------------------------------------------------//
	virtual Matrix44* GetWorldTM()=0;

	// �浹�� ����
	virtual Sphere* GetSphere()=0;
	// �ּ� �浹�� ���� (�̵��� ����)
	virtual Sphere* GetMinimumSphere()=0;

	//-----------------------------------------------------------------------------//
	// �浹�׽�Ʈ �ϱ����� �浹�ڽ�, �浹���� ��ǥ�� ����ġ�� �ű�� ���ؼ� 
	// ��������� ���Ѵ�. Bone�� ��� ���������� ��������� ������ �ִ�.
	// UpdateCollisionBox() �Լ��� �浹�׽�Ʈ�ϱ� ������ ȣ��Ǹ� 
	// �̴� �� �������� �浹�ڽ� ��ġ�� ������Ʈ ������ϴ� ������带 ���̱����ؼ���.
	//-----------------------------------------------------------------------------//
	virtual void UpdateCollisionBox()=0;

	//-----------------------------------------------------------------------------//
	// ���� ���� �浹�ڽ��� ����Ʈ�� �������� �����Ѵ�. 
	// �浹�ڽ��� Mesh, Bone�� BoneNode���� ������ �ִ�.
	//-----------------------------------------------------------------------------//
	virtual void GetCollisionBox( std::list<Box*> *pList )=0;

	//-----------------------------------------------------------------------------//
	// �浹���� �浹�� ����Ų ��ü���� ȣ���
	// TestNum: �׽�Ʈ ��ȣ
	// pObj : �浹�� ��ü ������
	//-----------------------------------------------------------------------------//
	virtual void Collision( int TestNum, ICollisionable *pObj )=0;
};


///////////////////////////////////////////////////////////////////////////////////////
// CollisionList

#include "3dnode.h"

// �浹�׽�Ʈ�� ������Ʈ�� �ϰ������� ó���Ѵ�.
class CollisionList
{
public:
	CollisionList();
	virtual ~CollisionList();
	enum { MAX_COLLISION_TABLE = 64 };
	ICollisionable* m_ChkTable[ MAX_COLLISION_TABLE][ 2];		// [table size][ src/dst]

protected:
	// �浹�׽�Ʈ�� ������Ʈ�� �����ϴ� Ʈ��
	typedef struct _tagObjTree : C3dNode
	{
		int testnum;			// �׽�Ʈ��ȣ (0���� ������, ���� �׽�Ʈ��ȣ���� ���Ѵ�.)
		ICollisionable *pobj;
		Sphere *psphere;
		Sphere *pminsphere;		// ĳ���� �̵��浹üũ�� ���ؼ� �������
		std::list< Box* > boxlist;
		_tagObjTree( int id ) : C3dNode(id) {}

	} SObjTree;
	SObjTree *m_pGrp1;
	SObjTree *m_pGrp2;

public:
	void AddObj( int nGroup, ICollisionable *pObj, int nTestNum=0 );			// �浹�׽�Ʈ�� ������Ʈ �߰�
	void AddObj( ICollisionable *pParent, ICollisionable *pObj, int nTestNum=0 ); // �浹�׽�Ʈ�� ������Ʈ �߰�
	void DelObj( ICollisionable *pObj );						// ����Ʈ���� ������Ʈ �߰�
	void UpdateCollisionBox();									// �浹�ڽ��� ��ǥ�� �����Ѵ�.
	int CollisionTest( int nTestNum );							// �浹�׽�Ʈ
	int CollisionGroupTest( int nGroup, int nTestNum );			// �׷쳻 �浹�׽�Ʈ

protected:
	void AddObj( SObjTree *pParent, ICollisionable *pObj, int nTestNum );
	BOOL CollisionTest_SrcRec( SObjTree *pSrc, SObjTree *pTarget, int nTestNum );
	BOOL CollisionTest_TargetRec( SObjTree *pSrc, SObjTree *pTarget, int nTestNum );
	BOOL CollisionTest_Obj( SObjTree *pSrc, SObjTree *pTarget, int nTestNum );
	int _CollisionGroupTest( SObjTree *pGrp, int nTestNum );
};

#endif // __COLLISION_H__
