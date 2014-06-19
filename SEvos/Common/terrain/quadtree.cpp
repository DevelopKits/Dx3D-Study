
#include "stdafx.h"
#include "quadtree.h"
#include "../common/utility.h"

#define _USE_INDEX16


//-----------------------------------------------------------------------------//
// ����,�ʺ� ���� ���� ����
//-----------------------------------------------------------------------------//
CQuadTree::CQuadTree( int cx, int cy )
{
	int		i;
	m_pParent = NULL;
	m_nCenter = 0;
	for( i = 0 ; i < 4 ; i++ )
	{
		m_pChild[i] = NULL;
		m_pNeighbor[i] = NULL;
	}
	m_nCorner[ CORNER_TL] = 0;
	m_nCorner[ CORNER_TR] = cx - 1;
	m_nCorner[ CORNER_BL] = cx * ( cy - 1 );
	m_nCorner[ CORNER_BR] = cx * cy - 1;

	m_nCenter			  = ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + 
						  	  m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;
}

CQuadTree::CQuadTree( CQuadTree* pParent )
{
	int		i;
	m_pParent = pParent;
	m_nCenter = 0;
	for( i = 0 ; i < 4 ; i++ )
	{
		m_pChild[i] = NULL;
		m_pNeighbor[i] = NULL;
		m_nCorner[i] = 0;
	}
}

CQuadTree::~CQuadTree()
{
	Destroy();
}


//-----------------------------------------------------------------------------//
/// �޸𸮿��� ����Ʈ���� �����Ѵ�.
//-----------------------------------------------------------------------------//
void CQuadTree::Destroy()
{
	for( int i = 0 ; i < 4 ; i++ ) 
		SAFE_DELETE( m_pChild[i] );
}


//-----------------------------------------------------------------------------//
/// 4���� �ڳʰ��� �����Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CQuadTree::SetCorners( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR )
{
	m_nCorner[CORNER_TL] = nCornerTL;
	m_nCorner[CORNER_TR] = nCornerTR;
	m_nCorner[CORNER_BL] = nCornerBL;
	m_nCorner[CORNER_BR] = nCornerBR;
	m_nCenter			 = ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + 
							 m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;
	return TRUE;
}


//-----------------------------------------------------------------------------//
/// �ڽ� ��带 �߰��Ѵ�.
//-----------------------------------------------------------------------------//
CQuadTree* CQuadTree::AddChild( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR )
{
	CQuadTree*	pChild;

	pChild = new CQuadTree( this );
	pChild->SetCorners( nCornerTL, nCornerTR, nCornerBL, nCornerBR );

	return pChild;
}


//-----------------------------------------------------------------------------//
/// Quadtree�� 4���� �ڽ� Ʈ���� �κк���(subdivide)�Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CQuadTree::SubDivide()
{
	int		nTopEdgeCenter;
	int		nBottomEdgeCenter;
	int		nLeftEdgeCenter;
	int		nRightEdgeCenter;
	int		nCentralPoint;

	// ��ܺ� ���
	nTopEdgeCenter		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] ) / 2;
	// �ϴܺ� ��� 
	nBottomEdgeCenter	= ( m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 2;
	// ������ ���
	nLeftEdgeCenter		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL] ) / 2;
	// ������ ���
	nRightEdgeCenter	= ( m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR] ) / 2;
	// �Ѱ��
	nCentralPoint		= ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] + 
							m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 4;

	// ���̻� ������ �Ұ����Ѱ�? �׷��ٸ� SubDivide() ����
	if( abs(m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= 1 )
	{
		return FALSE;
	}

	// 4���� �ڽĳ�� �߰�
	m_pChild[CORNER_TL] = AddChild( m_nCorner[CORNER_TL], nTopEdgeCenter, nLeftEdgeCenter, nCentralPoint );
	m_pChild[CORNER_TR] = AddChild( nTopEdgeCenter, m_nCorner[CORNER_TR], nCentralPoint, nRightEdgeCenter );
	m_pChild[CORNER_BL] = AddChild( nLeftEdgeCenter, nCentralPoint, m_nCorner[CORNER_BL], nBottomEdgeCenter );
	m_pChild[CORNER_BR] = AddChild( nCentralPoint, nRightEdgeCenter, nBottomEdgeCenter, m_nCorner[CORNER_BR] );

	return TRUE;
}


//-----------------------------------------------------------------------------//
/// ����� �������� �ε����� �����Ѵ�.
//-----------------------------------------------------------------------------//
int	CQuadTree::GenTriIndex( int nTris, LPVOID pIndex, SVtxNormTex* pHeightMap, float fLODRatio )
{
	// �ø��� ����� �׳� ����
	if( m_bCulled )
	{
		m_bCulled = FALSE;
		return nTris;
	}

#ifdef _USE_INDEX16
	LPWORD p = ((LPWORD)pIndex) + nTris * 3;
#else
	LPDWORD p = ((LPDWORD)pIndex) + nTris * 3;
#endif

	if( IsVisible(pHeightMap, fLODRatio) )
	{
		// ���� ������ ����� �κк���(subdivide)�� �Ұ����ϹǷ� �׳� ����ϰ� �����Ѵ�.
		if( abs(m_nCorner[CORNER_TR]-m_nCorner[CORNER_TL]) <= 1 )
		{
			// ������� �ﰢ��
			*p++ = m_nCorner[0];
			*p++ = m_nCorner[1];
			*p++ = m_nCorner[2];
			nTris++;
			// �����ϴ� �ﰢ��
			*p++ = m_nCorner[2];
			*p++ = m_nCorner[1];
			*p++ = m_nCorner[3];
			nTris++;
			return nTris;
		}

		BOOL b[4] = {TRUE, TRUE, TRUE, TRUE };
		if( m_pNeighbor[EDGE_UP] ) b[EDGE_UP] = m_pNeighbor[EDGE_UP]->IsVisible( pHeightMap, fLODRatio );
		if( m_pNeighbor[EDGE_DN] ) b[EDGE_DN] = m_pNeighbor[EDGE_DN]->IsVisible( pHeightMap, fLODRatio );
		if( m_pNeighbor[EDGE_LT] ) b[EDGE_LT] = m_pNeighbor[EDGE_LT]->IsVisible( pHeightMap, fLODRatio );
		if( m_pNeighbor[EDGE_RT] ) b[EDGE_RT] = m_pNeighbor[EDGE_RT]->IsVisible( pHeightMap, fLODRatio );

		// �̿������� ��δ� ��°����ϴٸ� ������� �̿������� ���� LOD�̹Ƿ� 
		// �κк����� �ʿ����.
		if( b[EDGE_UP] && b[EDGE_DN] && b[EDGE_LT] && b[EDGE_RT] )
		{
			*p++ = m_nCorner[0];
			*p++ = m_nCorner[1];
			*p++ = m_nCorner[2];
			nTris++;
			*p++ = m_nCorner[2];
			*p++ = m_nCorner[1];
			*p++ = m_nCorner[3];
			nTris++;
			return nTris;
		}
/**/
		int n;
		if( !b[EDGE_UP] ) // ��� �κк����� �ʿ��Ѱ�?
		{
			n = ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_TR] ) / 2;
			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_TL]; *p++ = n; nTris++;
			*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_TR]; nTris++;
		}
		else	// ��� �κк����� �ʿ���� ���
		{ *p++ = m_nCenter; *p++ = m_nCorner[CORNER_TL]; *p++ = m_nCorner[CORNER_TR]; nTris++; }

		if( !b[EDGE_DN] ) // �ϴ� �κк����� �ʿ��Ѱ�?
		{
			n = ( m_nCorner[CORNER_BL] + m_nCorner[CORNER_BR] ) / 2;
			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_BR]; *p++ = n; nTris++;
			*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_BL]; nTris++;
		}
		else	// �ϴ� �κк����� �ʿ���� ���
		{ *p++ = m_nCenter; *p++ = m_nCorner[CORNER_BR]; *p++ = m_nCorner[CORNER_BL]; nTris++; }

		if( !b[EDGE_LT] ) // ���� �κк����� �ʿ��Ѱ�?
		{
			n = ( m_nCorner[CORNER_TL] + m_nCorner[CORNER_BL] ) / 2;
			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_BL]; *p++ = n; nTris++;
			*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_TL]; nTris++;
		}
		else	// ���� �κк����� �ʿ���� ���
		{ *p++ = m_nCenter; *p++ = m_nCorner[CORNER_BL]; *p++ = m_nCorner[CORNER_TL]; nTris++; }

		if( !b[EDGE_RT] ) // ���� �κк����� �ʿ��Ѱ�?
		{
			n = ( m_nCorner[CORNER_TR] + m_nCorner[CORNER_BR] ) / 2;
			*p++ = m_nCenter; *p++ = m_nCorner[CORNER_TR]; *p++ = n; nTris++;
			*p++ = m_nCenter; *p++ = n; *p++ = m_nCorner[CORNER_BR]; nTris++;
		}
		else	// ���� �κк����� �ʿ���� ���
		{ *p++ = m_nCenter; *p++ = m_nCorner[CORNER_TR]; *p++ = m_nCorner[CORNER_BR]; nTris++; }
/**/
		
		return nTris;	// �� ��� �Ʒ��� �ڽĳ��� Ž���� �ʿ�����Ƿ� ����!
	}

	if( m_pChild[CORNER_TL] ) nTris = m_pChild[CORNER_TL]->GenTriIndex( nTris, pIndex, pHeightMap, fLODRatio );
	if( m_pChild[CORNER_TR] ) nTris = m_pChild[CORNER_TR]->GenTriIndex( nTris, pIndex, pHeightMap, fLODRatio );
	if( m_pChild[CORNER_BL] ) nTris = m_pChild[CORNER_BL]->GenTriIndex( nTris, pIndex, pHeightMap, fLODRatio );
	if( m_pChild[CORNER_BR] ) nTris = m_pChild[CORNER_BR]->GenTriIndex( nTris, pIndex, pHeightMap, fLODRatio );

	return nTris;
}


//-----------------------------------------------------------------------------//
// QuadTree�� �����Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CQuadTree::Build( SVtxNormTex* pHeightMap )
{
	BuildQuadTree( pHeightMap );
	BuildNeighborNode( this, pHeightMap, abs(m_nCorner[CORNER_TR]-m_nCorner[CORNER_TL])+1 );

	return TRUE;
}


//-----------------------------------------------------------------------------//
//	�ﰢ���� �ε����� �����, ����� �ﰢ���� ������ ��ȯ�Ѵ�.
//-----------------------------------------------------------------------------//
int	CQuadTree::GenerateIndex( LPVOID pIndex, SVtxNormTex* pHeightMap, CFrustum* pFrustum, float fLODRatio )
{
	FrustumCull( pHeightMap, pFrustum );
	return GenTriIndex( 0, pIndex, pHeightMap, fLODRatio );
}


//-----------------------------------------------------------------------------//
// pHeightMap�� pFrustum�ȿ� ���ԵǴ��� �˻��Ѵ�.
//-----------------------------------------------------------------------------//
CQuadTree::QuadLocation CQuadTree::IsInFrustum( SVtxNormTex *pHeightMap, CFrustum* pFrustum )
{
	BOOL	b[4];
	BOOL	bInSphere;

	// ��豸�ȿ� �ִ°�?
//	if( m_fRadius == 0.0f ) g_pLog->Log( "Index:[%d], Radius:[%f]",m_nCenter, m_fRadius );
	bInSphere = pFrustum->IsInSphere( &pHeightMap[ m_nCenter].v, m_fRadius );
	if( !bInSphere ) return FRUSTUM_OUT;	// ��豸 �ȿ� ������ �������� �������� �׽�Ʈ ����

	// ����Ʈ���� 4���� ��� �������� �׽�Ʈ
	b[0] = pFrustum->IsIn( &pHeightMap[ m_nCorner[0]].v );
	b[1] = pFrustum->IsIn( &pHeightMap[ m_nCorner[1]].v );
	b[2] = pFrustum->IsIn( &pHeightMap[ m_nCorner[2]].v );
	b[3] = pFrustum->IsIn( &pHeightMap[ m_nCorner[3]].v );

	// 4����� �������� �ȿ� ����
	if( (b[0] + b[1] + b[2] + b[3]) == 4 ) return FRUSTUM_COMPLETELY_IN;

	// �Ϻκ��� �������ҿ� �ִ� ���
	return FRUSTUM_PARTIALLY_IN;
}


//-----------------------------------------------------------------------------//
// IsInFrustum()�Լ��� ����� ���� �������� �ø� ����
//-----------------------------------------------------------------------------//
void CQuadTree::FrustumCull( SVtxNormTex *pHeightMap, CFrustum* pFrustum )
{
	QuadLocation ret;

	ret = IsInFrustum( pHeightMap, pFrustum );
	switch( ret )
	{
		case FRUSTUM_COMPLETELY_IN :	// �������ҿ� ��������, ������� �˻� �ʿ����
			m_bCulled = FALSE;
			return;
		case FRUSTUM_PARTIALLY_IN :		// �������ҿ� �Ϻ�����, ������� �˻� �ʿ���
			m_bCulled = FALSE;
			break;
		case FRUSTUM_OUT :				// �������ҿ��� �������, ������� �˻� �ʿ����
			m_bCulled = TRUE;
			return;
	}
	if( m_pChild[0] ) m_pChild[0]->FrustumCull( pHeightMap, pFrustum );
	if( m_pChild[1] ) m_pChild[1]->FrustumCull( pHeightMap, pFrustum );
	if( m_pChild[2] ) m_pChild[2]->FrustumCull( pHeightMap, pFrustum );
	if( m_pChild[3] ) m_pChild[3]->FrustumCull( pHeightMap, pFrustum );
}


BOOL CQuadTree::IsVisible( SVtxNormTex* pHeightMap, float fLODRatio ) 
{ 
	return ( abs(m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= GetLODLevel(pHeightMap, fLODRatio) );
}


//-----------------------------------------------------------------------------//
// ī�޶�� ���� ������ �Ÿ����� �������� LOD���� ���Ѵ�.
//-----------------------------------------------------------------------------//
int	CQuadTree::GetLODLevel( SVtxNormTex* pHeightMap, float fLODRatio )
{
	Vector3 v = pHeightMap[ m_nCenter].v - *(Vector3*)&g_Camera.GetEye();
	float d = v.Length();
	return max( (int)(d*fLODRatio), 1 );
}


//-----------------------------------------------------------------------------//
// �̿���带 �����.(�ﰢ�� ������ ������)
//-----------------------------------------------------------------------------//
void CQuadTree::BuildNeighborNode( CQuadTree* pRoot, SVtxNormTex* pHeightMap, int cx )
{
	for( int i=0 ; i<4 ; i++ )
	{
		int conertl = m_nCorner[0];
		int conertr = m_nCorner[1];
		int conerbl = m_nCorner[2];
		int conerbr = m_nCorner[3];
		// �̿������ 4�� �ڳʰ��� ��´�.
		int n = GetNodeIndex( i, cx, conertl, conertr, conerbl, conerbr );
		// �ڳʰ����� �̿������ �����͸� ���´�.
		if( n >= 0 ) m_pNeighbor[i] = pRoot->FindNode( pHeightMap, conertl, conertr, conerbl, conerbr );
	}

	// �ڽĳ��� ���ȣ��
	if( m_pChild[0] )
	{
		m_pChild[0]->BuildNeighborNode( pRoot, pHeightMap, cx );
		m_pChild[1]->BuildNeighborNode( pRoot, pHeightMap, cx );
		m_pChild[2]->BuildNeighborNode( pRoot, pHeightMap, cx );
		m_pChild[3]->BuildNeighborNode( pRoot, pHeightMap, cx );
	}
}


//-----------------------------------------------------------------------------//
// ����Ʈ���� �����.(Build()�Լ����� �Ҹ���)
//-----------------------------------------------------------------------------//
BOOL CQuadTree::BuildQuadTree( SVtxNormTex* pHeightMap )
{
	if( SubDivide() )
	{
		// ������ܰ�, ���� �ϴ��� �Ÿ��� ���Ѵ�.
		Vector3 v = pHeightMap[ m_nCorner[CORNER_TL]].v - 
					pHeightMap[ m_nCorner[CORNER_BR]].v;
		// v�� �Ÿ����� �� ��带 ���δ� ��豸�� �����̹Ƿ�, 
		// 2�� ������ �������� ���Ѵ�.
		m_fRadius	  = v.Length() / 2.0f;

		m_pChild[CORNER_TL]->BuildQuadTree( pHeightMap );
		m_pChild[CORNER_TR]->BuildQuadTree( pHeightMap );
		m_pChild[CORNER_BL]->BuildQuadTree( pHeightMap );
		m_pChild[CORNER_BR]->BuildQuadTree( pHeightMap );
	}
	return TRUE;
}


//-----------------------------------------------------------------------------//
// 4�� ����(���,�ϴ�,����,����)�� �̿���� �ε����� ���Ѵ�.
// ��� �ε������� �����Ѵ�.
//-----------------------------------------------------------------------------//
int	CQuadTree::GetNodeIndex( int ed, int cx, int& tl, int& tr, int& bl, int& br )
{
	int	n, gap;
	int topleft = tl;
	int topright = tr;
	int bottomleft = bl;
	int bottomright = br;
	gap	= abs(topright-topleft);	// ���� ����� �¿�����

	switch( ed )
	{
		case EDGE_UP:	// ���� ���� �̿������ �ε���
			tl = topleft - cx * gap;
			tr = topright - cx * gap;
			bl = topleft;
			br = topright;
			break;
		case EDGE_DN:	// �Ʒ� ���� �̿������ �ε���
			tl = bottomleft;
			tr = bottomright;
			bl = bottomleft + cx * gap;
			br = bottomright + cx * gap;
			break;
		case EDGE_LT:	// ���� ���� �̿������ �ε���
			tl = topleft + gap;
			tr = topleft;
			bl = bottomleft + gap;
			br = bottomleft;
			break;
		case EDGE_RT:	// ���� ���� �̿������ �ε���
			tl = topright;
			tr = topright - gap;
			bl = bottomright;
			br = bottomright - gap;
			break;
	}

	n = ( tl + tr + bl + br ) / 4;	// ��� �ε���
	if( !IS_IN_RANGE( n, 0, cx * cx - 1 ) ) return -1;

	return n;
}	


//-----------------------------------------------------------------------------//
// ����Ʈ���� �˻��ؼ� 4�� �ڳʰ��� ��ġ�ϴ� ��带 ã�´�.
// _0,_1,_2,_3: CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR
//-----------------------------------------------------------------------------//
CQuadTree* CQuadTree::FindNode( SVtxNormTex* pHeightMap, int tl, int tr, int bl, int br )
{
	CQuadTree*	p = NULL;
	// ��ġ�ϴ� ����� ��尪�� ����
	if( (m_nCorner[0] == tl) && (m_nCorner[1] == tr) && (m_nCorner[2] == bl) && (m_nCorner[3] == br) )
		return this;

	// �ڽ� ��尡 �ִ°�?
	if( m_pChild[0] )
	{
		RECT rc;
		POINT pt;
		int n = ( tl + tr + bl + br ) / 4;

		// ���� �ʻ󿡼��� ��ġ
		pt.x = (int)pHeightMap[n].v.x;
		pt.y = (int)pHeightMap[n].v.z;

		// 4���� �ڳʰ��� �������� �ڽĳ���� �� ���������� ��´�.
		SetRect( &rc, (int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].v.x, 
					  (int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_TL]].v.z, 
					  (int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].v.x, 
					  (int)pHeightMap[m_pChild[0]->m_nCorner[CORNER_BR]].v.z );
		NormalizeRect( &rc );
		// pt���� ���������ȿ� �ִٸ� �ڽĳ��� ����.
		if( ::PtInRect( &rc, pt ) )
			return m_pChild[0]->FindNode( pHeightMap, tl, tr, bl, br );

		SetRect( &rc, (int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_TL]].v.x, 
					  (int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_TL]].v.z, 
					  (int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_BR]].v.x, 
					  (int)pHeightMap[m_pChild[1]->m_nCorner[CORNER_BR]].v.z );
		NormalizeRect( &rc );
		if( ::PtInRect( &rc, pt ) )
			return m_pChild[1]->FindNode( pHeightMap, tl, tr, bl, br );

		SetRect( &rc, (int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_TL]].v.x, 
					  (int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_TL]].v.z, 
					  (int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_BR]].v.x, 
					  (int)pHeightMap[m_pChild[2]->m_nCorner[CORNER_BR]].v.z );
		NormalizeRect( &rc );
		if( ::PtInRect( &rc, pt ) )
			return m_pChild[2]->FindNode( pHeightMap, tl, tr, bl, br );

		SetRect( &rc, (int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_TL]].v.x, 
					  (int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_TL]].v.z, 
					  (int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_BR]].v.x, 
					  (int)pHeightMap[m_pChild[3]->m_nCorner[CORNER_BR]].v.z );
		NormalizeRect( &rc );
		if( ::PtInRect( &rc, pt ) )
			return m_pChild[3]->FindNode( pHeightMap, tl, tr, bl, br );
	}

	return NULL;
}
