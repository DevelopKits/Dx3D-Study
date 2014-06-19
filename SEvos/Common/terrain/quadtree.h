//-----------------------------------------------------------------------------//
// 2009-08-23  programer: jaejung ��(���բ�)��
// 
//-----------------------------------------------------------------------------//

#if !defined(__QUADTREE_H__)
#define __QUADTREE_H__

#include "terrain.h"

class CFrustum;
class CQuadTree
{
public:
	CQuadTree( int cx, int cy );
	CQuadTree( CQuadTree *parent );
	virtual ~CQuadTree();

	enum CONER_TYPE { CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR };
	enum { EDGE_UP, EDGE_DN, EDGE_LT, EDGE_RT }; // �̿���� ó���� �����
	enum QuadLocation { FRUSTUM_OUT = 0,			/// �������ҿ��� �������
					   FRUSTUM_PARTIALLY_IN = 1,	/// �������ҿ� �κ�����
					   FRUSTUM_COMPLETELY_IN = 2,	/// �������ҿ� ��������
					   FRUSTUM_UNKNOWN = -1 };		/// �𸣰���(^^;)

protected:
	CQuadTree*	m_pChild[ 4];		/// QuadTree�� 4���� �ڽĳ��
	CQuadTree*	m_pParent;			/// Triangle Crack(Popping)�� �������ؼ� ����Ѵ�.
	CQuadTree*	m_pNeighbor[4];		/// Triangle Crack(Popping)�� �������ؼ� ����Ѵ�.
	int			m_nCenter;			/// QuadTree�� ������ ù��° ��
	int			m_nCorner[ 4];		/// QuadTree�� ������ �ι�° ��
									///    TopLeft(TL)      TopRight(TR)
									///              0------1
									///              |      |
									///              |      |
									///              2------3
									/// BottomLeft(BL)      BottomRight(BR)
	BOOL		m_bCulled;			/// �������ҿ��� �ø��� ����ΰ�?
	float		m_fRadius;			/// ��带 ���δ� ��豸(bounding sphere)�� ������

public:
	BOOL Build( SVtxNormTex *pHeightMap );
	int GenerateIndex( LPVOID pIB, SVtxNormTex* pHeightMap, CFrustum* pFrustum, float fLODRatio );

protected:
	CQuadTree* AddChild( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR );	
	BOOL SetCorners( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR );
	BOOL SubDivide();
	BOOL IsVisible( SVtxNormTex* pHeightMap, float fLODRatio );
	int	GenTriIndex( int nTriangles, LPVOID pIndex, SVtxNormTex* pHeightMap, float fLODRatio );
	void Destroy();
	QuadLocation IsInFrustum( SVtxNormTex *pHeightMap, CFrustum* pFrustum );
	void FrustumCull( SVtxNormTex *pHeightMap, CFrustum* pFrustum );
	int	GetLODLevel( SVtxNormTex* pHeightMap, float fLODRatio );
	void GetCorner( int& _0, int& _1, int& _2, int& _3 ) // �ڳ� �ε��� ���� ���´�.
		{ _0 = m_nCorner[0]; _1 = m_nCorner[1]; _2 = m_nCorner[2]; _3 = m_nCorner[3]; }

	BOOL BuildQuadTree( SVtxNormTex* pHeightMap );
	void BuildNeighborNode( CQuadTree* pRoot, SVtxNormTex* pHeightMap, int cx );
	CQuadTree* FindNode( SVtxNormTex* pHeightMap, int _0, int _1, int _2, int _3 );
	int GetNodeIndex( int ed, int cx, int& _0, int& _1, int& _2, int& _3 );

};

#endif // __QUADTREE_H__
