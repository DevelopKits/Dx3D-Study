
#ifndef  __PATHFINDMANAGER_H__
#define __PATHFINDMANAGER_H__

#include <list>

// ��θ� �˻��ؼ� ����� �������ִ� ���� Ŭ������.
// �̱��� ����
class CEvosMap;
class CUnit;
class CPathExplorer;
class CPathPalnner;
class CPathFindManager
{
public:
	CPathFindManager(int iNumSearchCyclesPerUpdate) : m_iNumSearchCyclesPerUpdate(iNumSearchCyclesPerUpdate) {}
	virtual ~CPathFindManager() {}
	typedef std::list<CPathPalnner*> PlannerList;
	typedef PlannerList::iterator PlannerItor;

protected:
	PlannerList m_PlannerList;
	int m_iNumSearchCyclesPerUpdate;

public:
	void Update(float timeDelta);
	void Register(CUnit *punit, const Vector2D &dest);
	BOOL SearchMap( CEvosMap *pmap, CUnit *pobject, const Vector2D &dest, 
		CPathExplorer *ppathdata );
	BOOL SearchMapAstar( CEvosMap *pmap, CUnit *pobject, const Vector2D &dest, 
		CPathExplorer *ppathdata );

protected:
	BOOL IsExist(CUnit *punit);
	CPathPalnner* GetPathPlanner(CUnit *punit);
	void DeletePathPlanner(CUnit *punit);

};

#endif
