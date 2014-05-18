//-----------------------------------------------------------------------------//
// 2005-10-04  programer: jaejung ��(���բ�)��
// 
//-----------------------------------------------------------------------------//

#if !defined(__MODEL_H__)
#define __MODEL_H__


#include "collision.h"


//-----------------------------------------------------------------------------//
// CModel
//-----------------------------------------------------------------------------//
class CBone_;
class CBoneNode;
class CMesh;
class CModel : public ICollisionable
{
public:
	CModel();
	virtual ~CModel();

protected:
	ANI_TYPE m_eType;
	int m_ObjId;
	int m_nMeshCount;
	CMesh **m_pMesh;
	CBone_ *m_pBone;
	SAniLoader *m_pAniLoader;
	Vector3 m_Dir;					// ���� ����
	float m_fSize;					// ��ũ�� (0-1 ��)
	Matrix44 m_matWorld;
	Matrix44 m_matTmpWorld;
	Matrix44 m_matUpdate;			// ���ϸ��̼� Matrix�� �ӽ� �����ϱ� ���� ����
	Matrix44 *m_pLinkPTM;			// �ٸ� �𵨰� ����Ǿ����� �����Ǹ�, �θ� TM�̴�.
	Matrix44 *m_pLinkSTM;			// �ٸ� �𵨰� ����Ǿ����� �����Ǹ�, �θ��� ���� TM�̴�.
	BOOL m_bUpdateMatrix;			// ���ϸ��̼��� ������ WorldMatrix�� ������Ʈ�ϱ� ���� �÷���
	BOOL m_bQuickAni;

public:
	BOOL LoadModel( char *szFileName );
	BOOL LoadAni( char *szFileName, int nAniIdx=0, BOOL bLoop=FALSE );
	BOOL LoadDynamicModel( SBMMLoader *pLoader, BOOL bCollision=FALSE );
	BOOL LoadDynamicAni( SAniLoader *pAniLoader, int nAniIdx, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick );
	BOOL LoadDynamicAni( SAniLoader *pAniLoader, char *pAniName, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick );
	void SetAnimation( int nAniIdx, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick );
	void SetAnimation( char *pAniName, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick );
	void SetLink( CModel *pModel, CBoneNode *pBoneNode );
	CBoneNode* GetBoneNode( char *pBoneName );
	int  GetCurrentFrame();
	BOOL Animate( int nDelta );
	void Render();

	Matrix44* GetWoldTM() { return &m_matWorld; }
	void SetWorldTM( Matrix44 *pTM );
	void SetSize( const float size );
	void SetDirection( const Vector3& dir );		// rot: ���� ��
	void SetDirection( const Quaternion& q );
	const Vector3& GetDirection() const { return m_Dir; }
	void SetPos( const Vector3 &pos );
	Vector3 GetPos();
	void MovePos( const Vector3 &pos );
	void MutliplyWorldTM( Matrix44 *pTM );
	void MutliplyBoneTM( int nBoneId, Matrix44 *pTM );
	void Clear();

	// ICollisionable �������̽� ����
	virtual BOOL IsTest( int TestNum ) { return TRUE; }
	virtual int GetObjId();
	virtual Matrix44* GetWorldTM();
	virtual Sphere* GetSphere() { return NULL; }
	virtual Sphere* GetMinimumSphere() { return NULL; }
	virtual void UpdateCollisionBox();
	virtual void GetCollisionBox( std::list<Box*> *pList );
	virtual void Collision( int TestNum, ICollisionable *pDst ) {}
};

#endif // __MODEL_H__
