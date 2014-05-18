
#include "global.h"
#include "animesh.h"
#include "fileloader.h"


CAniMesh::CAniMesh()
{

}

CAniMesh::CAniMesh( SMeshLoader *pLoader, Matrix44 *pPalette, BOOL bCollision ) 
: CMesh(pLoader,bCollision)
{
	LoadPhysiq( pLoader, pPalette );

}

CAniMesh::~CAniMesh() 
{
	// Skinning Animation�� ��ü���� VertexBuffer�� �ʿ��ϸ� 
	// �Ҹ�� ���ؽ��� �����Ѵ�.
	SAFE_RELEASE( m_pVtxBuff );

}


//-----------------------------------------------------------------------------//
// Physiq, Palette Load
//-----------------------------------------------------------------------------//
BOOL CAniMesh::LoadPhysiq( SMeshLoader *pLoader, Matrix44 *pPalette )
{
	m_pPhysiq = &pLoader->physiq;
	m_pPalette = pPalette;

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Palette������ �����ؼ� m_pVtxBuff�� ���ϸ��̼ǵ� ������ �����Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CAniMesh::ApplyPalette()
{
	if( !m_pVtxBuff ) return FALSE;
	if( 0 >= m_pPhysiq->size ) return FALSE;

	BYTE *pV;
	m_pVtxBuff->Lock( 0, 0, (void**)&pV, 0 );

	for( int i=0; i < m_pPhysiq->size; ++i )
	{
		int idx = m_pPhysiq->p[ i].vtx;
		int offset = idx * m_VtxInfo.nStride;
		Vector3 *pVtx = (Vector3*)(pV + offset);
		*pVtx = Vector3(0,0,0);

		for( int k=0; k < m_pPhysiq->p[ i].size; ++k )
		{
			SWeight *w = &m_pPhysiq->p[ i].w[ k];
			Vector3 v = *(Vector3*)&m_VtxInfo.pV[ offset] * m_pPalette[ w->bone];
			*pVtx += v * w->weight;
		}
	}
	m_pVtxBuff->Unlock();

	return TRUE;
}


//-----------------------------------------------------------------------------//
// ���
//-----------------------------------------------------------------------------//
void CAniMesh::Render()
{
	ApplyPalette();
	CMesh::Render();
}
