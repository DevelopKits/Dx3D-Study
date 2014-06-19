
#include "stdafx.h"
#include "model.h"
#include "3dnode.h"
#include "bone.h"
#include "mesh.h"
#include "animesh.h"
#include "keyanimesh.h"

using namespace std;

//-----------------------------------------------------------------------------//
// ������
//-----------------------------------------------------------------------------//
CModel::CModel() : 
m_pBmmLoader(NULL),
m_pMesh(NULL), 
m_pBone(NULL), 
m_eType(ANI_RIGID), 
m_nMeshCount(0), m_pAniLoader(NULL), m_fSize(1.0f),
m_IsRenderCollisionBox(TRUE),
m_pLinkPTM(NULL), m_pLinkSTM(NULL), m_bUpdateMatrix(FALSE)
{
	m_bQuickAni = FALSE;
	m_matWorld.SetIdentity();
	m_ObjId = d3d::CreateObjectId();
}


//-----------------------------------------------------------------------------//
// �Ҹ���
//-----------------------------------------------------------------------------//
CModel::~CModel() 
{
	Clear();
}


//-----------------------------------------------------------------------------//
// ������ �ε��Ѵ�.
// szFileName : bmm file (script, binary)
// meterial, mesh, bone, physiq ���� ��� �ε��Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CModel::LoadModel( char *szFileName )
{
	if (!g_pDevice) return FALSE;
	Clear();

	SBMMLoader* pLoader = CFileLoader::LoadModel( szFileName );
	if (!pLoader) return FALSE;

	if (!LoadDynamicModel(pLoader,TRUE))
		return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------//
// �������� Model�� �����Ѵ�.
// bCollision=TRUE : �浹�ڽ��� �����Ѵ�.
//					 CAniMesh�� �浹�ڽ��� BoneŬ�������� �����Ǳ� ������ CMesh������ 
//					 �浹�ڽ��� �������� �ʴ´�.
//					 Sphere�浹�� Skinning �𵨿��� �����Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CModel::LoadDynamicModel( SBMMLoader *pLoader, BOOL bCollision ) // bCollision = FALSE
{
	if (!pLoader) return FALSE;

	m_pBmmLoader = pLoader;
	m_nMeshCount = pLoader->m.size;
	m_eType = (ANI_TYPE)pLoader->type;
	m_IsCollision = bCollision;

	// Skinning ���ϸ��̼��̸� Bone�� �����Ѵ�.
	if (ANI_SKIN == m_eType)
	{
		m_pBone = new CBone_();
		m_pBone->Load( m_ObjId, &pLoader->b, bCollision, &pLoader->m );
	}

	//////////////////////////////////////////////////////////////////////////////
	// Mesh Array ����
	m_pMesh = new CMesh*[ pLoader->m.size];
	for( int i=0; i < pLoader->m.size; ++i )
	{
		CMesh *pMesh = NULL;
		SMeshLoader *p = &pLoader->m.pMesh[ i];
		switch( pLoader->type )
		{
		case ANI_RIGID: pMesh = new CMesh( p, bCollision ); break;
		case ANI_KEYANI: pMesh = new CKeyAniMesh( p, bCollision ); break;
		case ANI_SKIN:
			pMesh = new CAniMesh( p, m_pBone->GetPalette(), FALSE ); break;
		}
		pMesh->SetRenderCollisionBox(m_IsRenderCollisionBox);
		m_pMesh[ i] = pMesh;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Animation file Load
// szFileName : gpg, binary file ��� ����
// nAniIdx : ���ϸ��̼ǵ� Index
//-----------------------------------------------------------------------------//
BOOL CModel::LoadAni( char *szFileName, int nAniIdx, BOOL bLoop ) // nAniIdx = 0, bLoop=FALSE
{
	SAniLoader *pAniLoader = CFileLoader::LoadAnimation( szFileName );
	m_pAniLoader = pAniLoader;
	return LoadDynamicAni( pAniLoader, nAniIdx, 0, 0, bLoop, 0 );
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CModel::SetAnimation( int nAniIdx, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick )
{
	LoadDynamicAni( m_pAniLoader, nAniIdx, nAniTime, bContinue, bLoop, bQuick );
}
void CModel::SetAnimation( char *pAniName, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick )
{
	LoadDynamicAni( m_pAniLoader, pAniName, nAniTime, bContinue, bLoop, bQuick );
}


//-----------------------------------------------------------------------------//
// Model�� Animation�� ����Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CModel::LoadDynamicAni( SAniLoader *pAniLoader, int nAniIdx, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick )
{
	if( !pAniLoader ) return FALSE;
	if( nAniIdx >= pAniLoader->size ) return FALSE;
	SKeyGroupLoader *key = &pAniLoader->pAniGroup[ nAniIdx];

	if( ANI_SKIN == m_eType )
	{
		m_bQuickAni = bQuick;
		if (m_pBone)
			m_pBone->SetAnimation( key, nAniTime, bContinue, bLoop, bQuick );
	}
	else if( ANI_KEYANI == m_eType )
	{
		for( int i=0; i < key->size; ++i )
		{
			if( m_nMeshCount <= key->id ) return FALSE;
			m_pMesh[ key->id]->LoadAni( &key->pKey[ i] );
		}
	}
	return TRUE;
}
BOOL CModel::LoadDynamicAni( SAniLoader *pAniLoader, char *pAniName, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick )
{
	if( !pAniLoader ) return FALSE;
	SKeyGroupLoader *key = NULL;
	for( int k=0; k < pAniLoader->size; ++k )
	{
		if( !strcmp( pAniName, pAniLoader->pAniGroup[ k].name) )
		{
			key = &pAniLoader->pAniGroup[ k];
			break;
		}
	}
	if( !key ) return FALSE;

	if( ANI_SKIN == m_eType )
	{
		m_bQuickAni = bQuick;
		m_pBone->SetAnimation( key, nAniTime, bContinue, bLoop, bQuick );
	}
	else if( ANI_KEYANI == m_eType )
	{
		for( int i=0; i < key->size; ++i )
		{
			if( m_nMeshCount <= key->id ) return FALSE;
			m_pMesh[ key->id]->LoadAni( &key->pKey[ i] );
		}
	}
	return TRUE;
}


//-----------------------------------------------------------------------------//
// ������� �ٸ����� ���� ���� ��Ų��.
// ���� ���� ��ű� ���̸� �ٸ� ���� �ڽ����� �������� �ϹǷ� 
// �ٸ� �� ������� TM������ ���� ���� �̵��ؾ� �Ѵ�. �� ����� �����ϱ�
// ���ؼ� SetLink()�� ����� ����.
//-----------------------------------------------------------------------------//
void CModel::SetLink( CModel *pModel, CBoneNode *pBoneNode )
{
	if( !pBoneNode )
	{
		m_pLinkPTM = NULL;
		m_pLinkSTM = NULL;
		return;
	}

	m_pLinkPTM = &pModel->m_matWorld;
	m_pLinkSTM = pBoneNode->GetAccTM();
}


//-----------------------------------------------------------------------------//
// pBoneName�� ����带 �����Ѵ�.
//-----------------------------------------------------------------------------//
CBoneNode* CModel::GetBoneNode( char *pBoneName )
{
	if( !m_pBone ) return NULL;
	return m_pBone->FindBoneNode( pBoneName );
}


//-----------------------------------------------------------------------------//
// ���� ���ϸ��̼� �������� �����Ѵ�.
//-----------------------------------------------------------------------------//
int  CModel::GetCurrentFrame()
{
	int reval = 0;
	if( ANI_KEYANI == m_eType )
	{

	}
	else if( ANI_SKIN == m_eType )
	{
		reval = m_pBone->GetCurrentFrame();
	}
	return reval;
}


//-----------------------------------------------------------------------------//
// Animation
//-----------------------------------------------------------------------------//
BOOL CModel::Update( int nDelta )
{
	if( m_bUpdateMatrix )
	{
		m_bUpdateMatrix = FALSE;
		m_matWorld = m_matUpdate;
		SetWorldTM( &m_matUpdate );
	}

	BOOL reval = TRUE;
	if( ANI_KEYANI == m_eType )
	{
		for( int i=0; i < m_nMeshCount; ++i )
			m_pMesh[ i]->Animate( nDelta );
	}
	else if( ANI_SKIN == m_eType )
	{
		// ���ϸ��̼��� ����Ǹ� Bone�� AniTM�� WorldTM�� �����Ѵ�.
		if (m_pBone)
		{
			reval = m_pBone->Animate( nDelta );
			if( !reval && m_bQuickAni )
			{
				m_bUpdateMatrix = TRUE;
				m_matUpdate = *m_pBone->GetAniTM() * m_matWorld; 
			}
		}
	}

	return reval;
}


//-----------------------------------------------------------------------------//
// ���
//-----------------------------------------------------------------------------//
void CModel::Render()
{
	g_pDevice->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&m_matWorld );
	if( m_pLinkPTM ) g_pDevice->MultiplyTransform( D3DTS_WORLD, (D3DXMATRIX*)m_pLinkPTM );
	if( m_pLinkSTM ) g_pDevice->MultiplyTransform( D3DTS_WORLD, (D3DXMATRIX*)m_pLinkSTM );

	g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	for (int i=0; i < m_nMeshCount; ++i)
		m_pMesh[ i]->Render();

	if (m_IsRenderCollisionBox && m_pBone) 
		m_pBone->Render();
}


//------------------------------------------------------------------------
// 
// [2011/3/16 jjuiddong]
//------------------------------------------------------------------------
void CModel::SetIsRenderCollisionBox(BOOL isRender) 
{ 
	m_IsRenderCollisionBox = isRender;
	for (int i=0; i < m_nMeshCount; ++i)
		m_pMesh[ i]->SetRenderCollisionBox(isRender);
}


//------------------------------------------------------------------------
// 
// [2011/3/15 jjuiddong]
//------------------------------------------------------------------------
void CModel::RenderDepth()
{
	Render();
}


//-----------------------------------------------------------------------------//
// SetWorldTM
//-----------------------------------------------------------------------------//
void CModel::SetWorldTM( Matrix44 *pTM ) 
{
	if( &m_matWorld != pTM )
		m_matWorld = *pTM; 
	if( m_pBone ) m_pBone->SetTM( pTM );
}

void CModel::SetSize( const float size )
{
	m_fSize = size;
	Vector3 pos( m_matWorld._41, m_matWorld._42, m_matWorld._43 );
	m_matWorld = m_matWorld * size;
	m_matWorld._41 = pos.x;
	m_matWorld._42 = pos.y;
	m_matWorld._43 = pos.z;
	m_matWorld._44 = 1.f;
	if( m_pBone ) m_pBone->SetTM( &m_matWorld );
}


//-----------------------------------------------------------------------------//
// dir : Normalize �� ���⺤��
//-----------------------------------------------------------------------------//
void CModel::SetDirection( const Vector3& dir )
{
	if( m_Dir == dir ) return;
	Quaternion q( Vector3(0,0,-1), dir );
	SetDirection( q );
}


void CModel::SetDirection( const Quaternion& q )
{
	Vector3 dir = q.GetDirection();
	if( m_Dir == dir ) return;

	Vector3 pos( m_matWorld._41, m_matWorld._42, m_matWorld._43 );

	m_matWorld.SetIdentity();
	m_matWorld._11 *= m_fSize;
	m_matWorld._22 *= m_fSize;
	m_matWorld._33 *= m_fSize;
	m_matWorld *= q.GetMatrix();

	m_matWorld._41 = pos.x;
	m_matWorld._42 = pos.y;
	m_matWorld._43 = pos.z;
	m_Dir = dir;

	if( m_pBone ) m_pBone->SetTM( &m_matWorld );
}

void CModel::SetPos( const Vector3 &pos )
{
	m_matWorld._41 = pos.x;
	m_matWorld._42 = pos.y;
	m_matWorld._43 = pos.z;
	if( m_pBone ) m_pBone->SetTM( &m_matWorld );
}

Vector3 CModel::GetPos()
{
	return Vector3( m_matWorld._41, m_matWorld._42, m_matWorld._43 ); 
}

void CModel::MovePos( const Vector3 &pos )
{
	m_matWorld._41 += pos.x;
	m_matWorld._42 += pos.y;
	m_matWorld._43 += pos.z;
	if( m_pBone ) m_pBone->SetTM( &m_matWorld );
}


//-----------------------------------------------------------------------------//
// MutliplyWorldTM
//-----------------------------------------------------------------------------//
void CModel::MutliplyWorldTM( Matrix44 *pTM ) 
{
	m_matWorld *= *pTM;
	if( m_pBone ) m_pBone->MutliplyTM( pTM );
}


//-----------------------------------------------------------------------------//
// 
//-----------------------------------------------------------------------------//
void CModel::MutliplyBoneTM( int nBoneId, Matrix44 *pTM )
{
	// ���� �������� ����
}


//-----------------------------------------------------------------------------//
// �������� ������ �޸𸮿� ����Ÿ�� ��� �����ϰ� �ʱ�ȭ�Ѵ�.
//-----------------------------------------------------------------------------//
void CModel::Clear()
{
	for( int i=0; i < m_nMeshCount; ++i )
	{
		if (m_pMesh[i]) 
			delete m_pMesh[i];
	}

	SAFE_ADELETE(m_pMesh);
	m_nMeshCount = 0;
	m_fSize = 1.0f;
	SAFE_DELETE( m_pBone );
}

int CModel::GetObjId()
{
	return GetId();
}

int CModel::GetId()
{
	return m_ObjId;
}


//-----------------------------------------------------------------------------//
// ���� ��������� �����Ѵ�.
// ���� ���� �ٸ� ���� �ڽ����� ����Ǿ� �ִٸ� �θ��� ����� �����ؼ� ���ϵȴ�.
//-----------------------------------------------------------------------------//
Matrix44* CModel::GetWorldTM()
{
	if( !m_pLinkPTM && !m_pLinkSTM ) return &m_matWorld;

	// �ٸ� �𵨿� ��ũ�Ǿ������
	m_matTmpWorld.SetIdentity();
	if( m_pLinkSTM ) m_matTmpWorld *= *m_pLinkSTM;
	if( m_pLinkPTM ) m_matTmpWorld *= *m_pLinkPTM;
	m_matTmpWorld *= m_matWorld;
	return &m_matTmpWorld;
}


//-----------------------------------------------------------------------------//
// �浹�׽�Ʈ �ϱ����� �浹�ڽ�, �浹���� ��ǥ�� ����ġ�� �ű�� ���ؼ� 
// ��������� ���Ѵ�. Bone�� ��� ���������� ��������� ������ �ִ�.
// UpdateCollisionBox() �Լ��� �浹�׽�Ʈ�ϱ� ������ ȣ��Ǹ� 
// �̴� �� �������� �浹�ڽ� ��ġ�� ������Ʈ ������ϴ� ������带 ���̱����ؼ���.
//-----------------------------------------------------------------------------//
void CModel::UpdateCollisionBox()
{
	switch( m_eType )
	{
	case ANI_RIGID:
	case ANI_KEYANI:
		{
			Matrix44 *pworld = GetWorldTM();
			for( int i=0; i < m_nMeshCount; ++i )
				m_pMesh[ i]->UpdateCollisionBox( pworld );
		}
		break;

	case ANI_SKIN:
		m_pBone->UpdateCollisionBox();
		break;
	}
}


//-----------------------------------------------------------------------------//
// ���� ���� �浹�ڽ��� ����Ʈ�� �������� �����Ѵ�. 
// �浹�ڽ��� Mesh, Bone�� BoneNode���� ������ �ִ�.
//-----------------------------------------------------------------------------//
void CModel::GetCollisionBox( list<Box*> *pList )
{
	switch( m_eType )
	{
	case ANI_RIGID:
	case ANI_KEYANI:
		{
			for( int i=0; i < m_nMeshCount; ++i )
			{
				Box *pbox = m_pMesh[ i]->GetCollisionBox();
				if( pbox ) pList->push_back( pbox );
			}
		}
		break;

	case ANI_SKIN:
		m_pBone->GetCollisionBox( pList );
		break;
	}
}


//------------------------------------------------------------------------
// 
// [2011/2/24 jjuiddong]
//------------------------------------------------------------------------
CModel* CModel::Clone()
{
	CModel *pClone = new CModel();
	if (!pClone->LoadDynamicModel(m_pBmmLoader, m_IsCollision))
	{
		delete pClone;
		return NULL;
	}

	pClone->m_fSize = m_fSize;
	pClone->m_Dir = m_Dir;
	pClone->m_matWorld = m_matWorld;
	pClone->m_matTmpWorld = m_matTmpWorld;
	pClone->m_matUpdate = m_matUpdate;
	pClone->m_bUpdateMatrix = m_bUpdateMatrix;
	pClone->m_bQuickAni = m_bQuickAni;
	pClone->m_IsCollision = m_IsCollision;

	return pClone;
}


//------------------------------------------------------------------------
// 
// [2011/2/26 jjuiddong]
//------------------------------------------------------------------------
BOOL CModel::Pick(const Vector2 &pickPos)
{
	BOOL isPick = FALSE;
	switch( m_eType )
	{
	case ANI_RIGID:
	case ANI_KEYANI:
		{
			for( int i=0; i < m_nMeshCount; ++i )
			{
				if (m_pMesh[ i]->Pick(m_matWorld, pickPos))
				{
					isPick = TRUE;
					break;
				}
			}
		}
		break;

	case ANI_SKIN:
		isPick = m_pBone->Pick(pickPos);
		break;
	}

	return isPick;
}

