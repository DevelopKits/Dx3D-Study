//-----------------------------------------------------------------------------//
// 2001-11-12  programer: jaejung ��(���բ�)��
// Sound �����ϴ� Ŭ����
// IJukeBox�� �迭�� ������ wave, midi, mp3�� �����Ѵ�.
// m_pTrackArray �迭�� MAX_TRACK��ŭ Sound File�� ������ �ִ�.
//
// 2007-12-21
//	- code ����
//-----------------------------------------------------------------------------//

#if !defined(__SOUND_H__)
#define __SOUND_H__


//#define SOUND_VERSION			"1.02"	// 2002-06-20 // direct show �߰�
#define SOUND_VERSION			"1.03"	// 2007-12-21 // code ����

class IJukeBox;
class CSound
{
	enum
	{
		MAX_TRACK = 256,
		MAX_RECORD = 5,
	};
	typedef struct tagSound_Record
	{
		BOOL bUsed;
		BOOL play;			// ����Ѵٸ� TRUE ����ٸ� FALSE
		BOOL loop;			// Loop
		int sound_id;		// ��½�ų Sound Index
		int sound_delay;	// Delay �ð�

	} SOUND_RECORD, *LPSOUND_RECORD;

public:
	CSound();
	virtual ~CSound();

	BOOL	Init( HWND hwnd );						// CSound �ʱ�ȭ
	BOOL	Load( int iID, char *szFileName );		// Load Sound file
	BOOL	Load_Adpcm( int iID, char *szFileName );// Load adpcm wave file
	void	Proc( int iElaps );						// Sound Processor
	BOOL	Play( int iID, BOOL bLoop = FALSE );	// Play Sound
	BOOL	Stop( int iID );						// Stop Sound
	void	Volume( int iID, int iVol );			// volume Setting
	void	SetSoundOn( int iID, BOOL bOn );		// Sound On,Off
	void	AllStop();								// Stop All Sound
	void	Add_Record( int iSound_ID, int iSound_Delay, BOOL bPlay, BOOL bLoop );		// Record ����
	void	Clear_Record();								// Record �ʱ�ȭ

protected:
	int				m_iPlayID;						// ���� play ���� sound index
	int				m_iRecord_Count;				// Record ����
	IJukeBox		*m_pTrackArray[ MAX_TRACK];		// Sound ������ �迭
	SOUND_RECORD	m_Record[ MAX_RECORD];			// Record

};

// ���� Instance
extern CSound	g_Sound;

#endif // __SOUND_H__
