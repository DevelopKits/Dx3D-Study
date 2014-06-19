
#include "stdafx.h"
#include "../Common/evos/evosmap.h"
#include "movingobject.h"

#include "SimpleEvos.h"
#include "dbg.h"
#include "Graph/PathEdge.h"
#include "pathexplorer.h"


CPathExplorer::CPathExplorer(CUnit *powner) : m_pOwner(powner)
{
	m_CurrentEdge = m_PathList.begin();
}


CPathExplorer::~CPathExplorer() 
{
	Clear();

}

Vector2D CPathExplorer::GetNextNodePos()
{
	if (m_CurrentEdge == m_PathList.end()) return Vector2D(-1,-1);
	return m_CurrentEdge->Destination();
}


Vector2D CPathExplorer::GetPrevNodePos()
{
	if (m_CurrentEdge == m_PathList.end()) return Vector2D(-1,-1);
	return m_CurrentEdge->Source();
}


//------------------------------------------------------------------------
// �������� �����ߴٸ� TRUE�� �����Ѵ�.
// [2011/1/17 jjuiddong]
//------------------------------------------------------------------------
BOOL CPathExplorer::IsArrive()
{
	return m_CurrentEdge == m_PathList.end();
}


//------------------------------------------------------------------------
// CurrentNode�� �����ߴٸ� true�� �����Ѵ�.
// [2011/1/18 jjuiddong]
//------------------------------------------------------------------------
BOOL CPathExplorer::IsNextNodeArrival(const Vector2D &curPos)
{
	if (IsArrive()) return TRUE;
	return (10.f > m_CurrentEdge->Destination().DistanceSq(curPos));
}


//------------------------------------------------------------------------
// ���� ���� ��ǥ���� �ٲ۴�.
// [2011/1/17 jjuiddong]
//------------------------------------------------------------------------
BOOL CPathExplorer::GetNext()
{
	if (m_CurrentEdge == m_PathList.end()) 
	{
		return FALSE;
	}

	++m_CurrentEdge;

	if (m_CurrentEdge == m_PathList.end()) 
	{
		return FALSE;
	}

	return TRUE;
}


//------------------------------------------------------------------------
// ������忡 �����ϴµ� �ɸ��� �ð��� �����Ѵ�.
// [2011/1/17 jjuiddong]
//------------------------------------------------------------------------
double CPathExplorer::GetNextNodeArrivalTime( CMovingObject *pobj )
{
	Vector2D nextPos = GetNextNodePos();

	Vector2D dist = nextPos - pobj->GetPosition();
	double t = dist.Length() / (double)pobj->GetVelocity();

	return t;
}


//------------------------------------------------------------------------
// Ŭ���� �ʱ�ȭ
// [2011/1/17 jjuiddong]
//------------------------------------------------------------------------
void CPathExplorer::Clear()
{
	m_PathList.clear();
	m_CurrentEdge = m_PathList.begin();
}


//------------------------------------------------------------------------
// ��ã�� ù��° ���� currentPos�� �̵��Ѵ�.
// [2011/1/17 jjuiddong]
//------------------------------------------------------------------------
void CPathExplorer::Init(const Vector2D &begin, const Vector2D &end)
{
	CalculateEndPosition(end);

	SetStartPos(begin);

	const Vector2D front = GetBeginNodePos();
	const Vector2D back = GetEndNodePos();
	m_PathList.push_front(PathEdge(begin, front, 0));
	m_PathList.push_back(PathEdge(back, m_DestPos, 0));

	SmoothPath();
	m_CurrentEdge = m_PathList.begin();
}



//------------------------------------------------------------------------
// ������ �Լ�
// [2011/1/19 jjuiddong]
//------------------------------------------------------------------------
void CPathExplorer::PrintPath()
{
/*
	g_Dbg.Print("----path list----\n");
	PathItor itor = m_PathList.begin();
	while (m_PathList.end() != itor)
	{
		int nodeIdx = *itor++;
		g_Dbg.Print("%d ", nodeIdx);
	}
	g_Dbg.Print("\n");
/**/
}


//------------------------------------------------------------------------
// ���� ����� ��ġ�� �����Ѵ�.
// [2011/1/19 jjuiddong]
//------------------------------------------------------------------------
Vector2D CPathExplorer::GetBeginNodePos()
{
	if (m_PathList.empty()) return Vector2D(-1,-1);

	const PathEdge &beginEdge = m_PathList.front();
	return beginEdge.Source();
}

//------------------------------------------------------------------------
// ������ ����� ��ġ�� �����Ѵ�.
// [2011/1/19 jjuiddong]
//------------------------------------------------------------------------
Vector2D CPathExplorer::GetEndNodePos()
{
	if (m_PathList.empty()) return Vector2D(-1,-1);

	const PathEdge &endEdge = m_PathList.back();
	return endEdge.Destination();
}


//------------------------------------------------------------------------
// ��θ� ȭ�鿡 ����Ѵ�.
// pencolor : CGdi::enum
// [2011/1/21 jjuiddong]
//------------------------------------------------------------------------
void CPathExplorer::RenderPath(int penColor)
{
	PathList &path = GetPathList();
	PathList::iterator itor = path.begin();

	gdi->SetPenColor(penColor);
	CEvosMap *pmap = GetGame()->GetMap();
	while (itor != path.end())
	{
		Vector2D dest = itor->Destination();
		Vector2D src = itor->Source();
		gdi->Line(src.x, src.y, dest.x, dest.y);
		++itor;
	}
}


//------------------------------------------------------------------------
// ����� ���������� destPos�� �� �� �ִٸ� �������� destPos�� �����Ѵ�. 
// destPos�� �� �����ٸ� destPos���� ���� ����� ��尡 destPos�� �ȴ�.
// [2011/2/5 jjuiddong]
//------------------------------------------------------------------------
void CPathExplorer::CalculateEndPosition(const Vector2D &destPos)
{
	// ������ ��忡�� ���������� �� ���ִ��� ������ �Ǵ��ؼ�
	// �������� �缳�� �Ѵ�.
	CEvosMap *pmap = GetGame()->GetMap();
	Vector2D endPos = GetEndNodePos();
	if (pmap->IsAvailableGoThrough(endPos, destPos))
	{
		m_DestPos = destPos;
	}
	else
	{
		m_DestPos = endPos;
	}
}


//------------------------------------------------------------------------
// 
// [2011/2/6 jjuiddong]
//------------------------------------------------------------------------
void CPathExplorer::SmoothPath()
{
	if (m_PathList.empty()) return;

	CEvosMap *pmap = GetGame()->GetMap();
	PathItor i = m_PathList.begin();
	while (i != m_PathList.end())
	{
		PathItor k = i;
		++k;
		if (k == m_PathList.end())
		{
			break;
		}

		Vector2D pos1 = i->Source();
		while (k != m_PathList.end())
		{
			Vector2D pos2 = k->Destination();
			if (pmap->IsAvailableGoThrough(pos1, pos2))
			{
				k->SetSource(i->Source());
				k = m_PathList.erase(i);
				i = k;
				++k;
			}
			else
			{
				i = k;
				break;
			}
		}
	}

}
