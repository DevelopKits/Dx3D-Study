//-----------------------------------------------------------------------------//
// 2008-02-13  programer: jaejung ��(���բ�)��
// 
//-----------------------------------------------------------------------------//

#if !defined(__CONTROL_H__)
#define __CONTROL_H__


// ���ӿ��� ���Ǵ� �޼����� �����Ѵ�.
enum MSG_TYPE
{
	MSG_CONTROL,	// Control���� ó���� �޼���, lparam=Msg Type
	MSG_KEYDOWN,	// lparam = key, wparam = Ű�� �����ð�
	MSG_SETPOS,		// lparam = vector3* (pointer�� ���ڷ� �ϱ⶧���� ���������� �Ҵ��ؼ��� �ȵȴ�.)
	MSG_MOVPOS,		// lparam = vector3* (pointer�� ���ڷ� �ϱ⶧���� ���������� �Ҵ��ؼ��� �ȵȴ�.)
					// wparam = 1 = �ȱ�
					//			2 = �ٱ�

};

enum CHARACTER_TYPE
{
	CHAR_HERO, 
	CHAR_ENEMY,
};

typedef struct _tagMsg
{
	MSG_TYPE type;
	char *id;			// �������� ���̵�, NULL=hero, name=character
	int lparam;
	int wparam;

	_tagMsg() {}
	_tagMsg( MSG_TYPE t, char *i, int l, int w ):type(t), id(i), lparam(l), wparam(w) {}

} SMsg;


// CControl Class
class CCharacter;
class CAIObject;
class CollisionList;
class CTerrain;
class CControl
{
public:
	CControl();
	virtual ~CControl();

protected:
	// ���ӳ����� ��µǴ� ĳ����, ��ü�� ǥ���ϱ����� ����ü
	typedef struct _tagSObject
	{
		CCharacter *pchar;
		CAIObject *pai;
		_tagSObject() {}
		_tagSObject( CCharacter *pc, CAIObject *pa ):pchar(pc), pai(pa) {}
	} SObject;

	typedef std::list< SObject > CharList;
	typedef std::map< std::string, SObject > CharMap;
	typedef std::queue< SMsg > MsgQ;
	CharMap m_ModelList;	// ȭ�鿡 ��µǴ� ������Ʈ�� �����Ѵ�.
	CharList m_DeadList;	// ���ŵ� ������Ʈ�� �����Ѵ�.
	CTerrain *m_pTerrain;	// ���� Ŭ����
	CCharacter *m_pHero;	// ���ΰ� Ŭ����

	MsgQ m_MsgQ;
	CollisionList *m_pCollision;
	int m_ElapseTime;

public:
	CCharacter* AddCharacter( CHARACTER_TYPE Type, char *pName, char *pModelFile, 
				  		      char *pAniFile, char *pWeaponFile, char *pComboFile, char *pAIFile=NULL );
	CCharacter* GetCharacter( char *pCharacterName );
	CAIObject* GetAI( char *pCharacterName );
	CTerrain* GetTerrain() const { return m_pTerrain; }
	BOOL LoadTerrain( char *pFileName );
	void Command( SMsg &Msg );
	void Animate( int nDelta );
	void Render();
	void Clear();

	CCharacter* GetAttackTarget( CCharacter *pCharacter, Vector3 *pReval );

protected:
	void MsgProc();

	// ���� �ΰ����ɿ� ���õ� ó��
	void AIGroupProc( int nDelta );

};
extern CControl g_Control;

#endif // __CONTROL_H__
