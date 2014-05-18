//-----------------------------------------------------------------------------//
// 2008-03-25  programer: jaejung ��(���բ�)��
// 
// UserInterface���� ���� ������ Ŭ������.
//-----------------------------------------------------------------------------//

#if !defined(__SCENE_H__)
#define __SCENE_H__

// User Interface Type
enum UITYPE
{
	UI_BUTTON,
	UI_IMAGE,
};

enum UICMD_TYPE
{
	UICMD_SETIMAGEINDEX,	// CUIImage::SetImageIndex
};


// UI �޼���
enum UIMSG_TYPE
{
	// Scene���� ó���ȴ�.
	UIMSG_COMMAND,		// ��Ʈ�ѿ� ���õ� �޼���
						// wparam: UICMD_TYPE
						// lparam: argument

	UIMSG_SWAPPED,		// ���ҵ� ������ �޼����� ������.


	// Control���� ó��
	UIMSG_LCLICK,		// ���콺�� Ŭ�������� Control���� �޼����� ������.
	UIMSG_LCLICKUP,		// Ŭ���� ���¿��� Up�Ǿ����� �޼����� ������.
	UIMSG_SETVISIBLE,	// Visible ���� ����
						// wparam: visible��

	// Image���� ó��
	UIMSG_SETINDEX,
						// wparam: ImageIndex


};


// ��Ŭ�������� ���Ǵ� �޼��� ����
typedef struct _tagSUIMsg
{
	int classid;
	int msg;
	int wparam;
	int lparam;
	_tagSUIMsg() {}
	_tagSUIMsg( int c, int m, int w, int l ):classid(c), msg(m), wparam(w), lparam(l) {}

} SUIMsg;


// �� Ŭ����
class CUIControl;
class CScene
{
public:
	CScene( int Id );
	virtual ~CScene();

protected:
	int m_Id;
	std::queue< SUIMsg > m_MsgQ;
	std::list< CScene* > m_ChildScene;
	std::list< CUIControl* > m_Control;
	CScene *m_pCurScene;
	CScene *m_pParent;

	CUIControl *m_pFocusCtrl;

public:
	BOOL Create( char *pUIFileName, CScene *pParent=NULL );
	BOOL Create( SScene *pScene, CScene *pParent=NULL );
	virtual void Render();
	virtual void Animate( int nDelta );
	virtual void MessageProc( MSG *pMsg );
	virtual void KeyDown( int Key );
	virtual void ButtonDown( int ButtonId );
	virtual void ButtonUp( int ButtonId );
	virtual void AddControl( SControl *pCtrl );
	virtual void Destroy();

	int GetID() const { return m_Id; }
	void SendMessage( SUIMsg *pMsg );
	void PostMessage( SUIMsg *pMsg );
	void SwapScene( int CurSceneId, int NextSceneId );
	CScene* FindClass( int Id );
	CUIControl* FindUIControl( int Id );

protected:
	BOOL UIMsgProc();
	virtual BOOL DefaultMsgProc( SUIMsg *pMsg );
	BOOL AddChildScene( CScene *pScene );
	void RegisterScene( CScene *pScene );

};

// ��ũ��
// �ܺο��� ��Ŭ������ �����Ҽ��ְ� �ϱ����� ��ũ��
#define SCENE_BEGIN	static CScene* g_SceneTable[] = {
#define SCENE_CLASS( name, id )	{ new name(id) }, 
#define SCENE_END	};

#endif // __SCENE_H__
