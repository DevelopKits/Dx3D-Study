//-----------------------------------------------------------------------------//
// 2008-02-12  programer: jaejung ��(���բ�)��
// 
//	Model Ŭ������ ������, ����, ����, ���ʵ� �ϳ��̻��� ���� �����Ѵ�.
//	Ű���ۿ� ���� ĳ���� ������, �浹ó���� ���ؼ� CollisionList�� ��ü��
//  ����ϰ�, Control�� ���� �޼����� �޾Ƽ� �⺻���� ����� ó���ϴ� 
//  ����� �Ѵ�.
//-----------------------------------------------------------------------------//

#if !defined(__CHARACTER_H__)
#define __CHARACTER_H__


#include "../model/collision.h"


// CCharacter Class
class CModel;
class CLifeBar;
class CCharacter : public ICollisionable
{
public:
	CCharacter();
	virtual ~CCharacter();

protected:
	enum { BODY=0, WEAPON, MAX_MODEL, };

	STATE m_State;
	CHARACTER_TYPE m_Group;
	int m_ObjId;
	char m_Name[ 32];
	CModel *m_pModel[ MAX_MODEL];
	Sphere m_Sphere;
	Sphere m_MinSphere;
	CLifeBar *m_pLifeBar;
	Matrix44 *m_pWorldTM;	// m_pModel[ BODY] �� ������� �����͸� ����Ų��.
	CTerrain *m_pTerrain;

	float m_AttackLength;	// �����Ҽ� �ִ� �ִ���� (���⿡ ���� �޶�����.)
	int m_Life;				// ����ġ 0�̵Ǹ� �״´�.
	int m_Level;			// ����

	// �޺� ���� ����
	SCombo *m_pCombo[ MAX_COMBO];
	int m_ComboCount;
	SActInfo *m_pCurAct;
	int m_KeyDownTime;
	int m_PrevKey;			// ���� ����Ű������ �����Ѵ�.
	char m_ComboTrace[ 64];	// ������

	// �̵� ���� ����
	float m_Velocity;		// �̵� �ӵ� (millisecond ����)
	Vector3 m_Dir;			// �̵� ����
	Vector3 m_DestPos;		// �̵� ������ ��ġ

	// ���� ���� ����
	float m_LaunchVelocity;	// ���� ���ӵ�
	int m_LaunchIncTime;
	float m_OffsetY;

	// ���� ��Ŀ��
	CCharacter *m_pAttackFocus;
	int m_nFocusTime;

public:
	void SetName( char *pName ) { if( pName ) { strcpy_s(m_Name, sizeof(m_Name), pName); } }
	char* GetName() const { return (char*)m_Name; }
	BOOL LoadBody( char *pFileName );
	BOOL LoadWeapon( char *pFileName );
	BOOL LoadCombo( char *pFileName );
	BOOL LoadAni( char *pFileName );
	void SetAnimation( int nAniIdx, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick );
	void SetAnimation( char *pAniName, int nAniTime, BOOL bContinue, BOOL bLoop, BOOL bQuick );
	void SetCollision( CHARACTER_TYPE Group, CollisionList *pCollision );
	void SetTerrain( CTerrain *pTerrain );
	void Render();
	void Animate( int nDelta );
	void Command( SMsg Msg );
	void KeyProc( int Key, int KeyTime );
	void Action( int Key, int KeyTime, int ElapseTime, SActInfo *pAct );

	void Move( const Vector3& DestPos, BOOL bDash=FALSE );
	BOOL Attack( const Vector3& vDir, int Key );
	void AttackSuccess();
	void Damage( ATTACK_TYPE AttackType, int Damage, Vector3 *pDir );

	BOOL IsDead() { return 0 >= m_Life; }
	const int& GetHP() const { return m_Life; }
	void SetHP( int hp ) { m_Life = hp; }
	const int& GetLevel() const { return m_Level; }
	void SetLevel( int level ) { m_Level = level; }

	STATE GetState() const { return m_State; }
	CHARACTER_TYPE GetCharacterType() const { return m_Group; }
	void SetPos( Vector3 Pos );
	Vector3 GetPos() const; 
	void SetDirection( const Vector3& dir );		// dir: ���� ��
	void SetDirection( const Quaternion& q );
	const Vector3& GetDirection() const { return m_Dir; }
	const float& GetAttackLength() const { return m_AttackLength; }	
	const Vector3& GetDestPos() const { return m_DestPos; }
	
	void SetWorldTM( Matrix44 *pTM );
	void MutliplyWorldTM( Matrix44 *pTM );
//	void MutliplyBoneTM( int nBoneId, Matrix44 *pTM );
	void Clear();

protected:
	SActInfo* FindComboAnimation( SActInfo *pCurAct, int Key, int PrevKey, int ElapseTime );
	SCombo* FindCombo( STATE State, int Key, int PrevKey, int ElapseTime );
	BOOL IsCorrectAction( SActInfo *pCurAct, int Key, int PrevKey, int ElapseTime );
	void PrintComboTree( SCombo *pCombo[ MAX_COMBO], int ComboSize );
	void PrintComboActTree( SActInfo *pAct, int Tab );

	BOOL IsAct( SActInfo *pAct, int key );
	void ClearCombo();

public:
	// ICollisionable �������̽� ����
	virtual BOOL IsTest( int TestNum );
	virtual int GetObjId() { return m_ObjId; }
	virtual Matrix44* GetWorldTM();
	virtual Sphere* GetSphere() { return &m_Sphere; }
	virtual Sphere* GetMinimumSphere() { return &m_MinSphere; }
	virtual void UpdateCollisionBox();
	virtual void GetCollisionBox( std::list<Box*> *pList );
	virtual void Collision( int TestNum, ICollisionable *pObj );

};

#endif // __CHARACTER_H__
