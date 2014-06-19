
#include "global.h"
#include "sound.h"
#include "sound_track.h"
//#include "dbg.h"

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "strmiids.lib")



// ���� Instance
CSound	g_Sound;

//------------------------
// ������
//------------------------
CSound::CSound() : m_iPlayID( -1 ), m_iRecord_Count( 0 )
{
	memset( m_pTrackArray, 0, sizeof(IJukeBox*)*MAX_TRACK );
	Clear_Record();
}


//------------------------
// �Ҹ���
//------------------------
CSound::~CSound()
{
	AllStop();	
	// delete sound data
	//for( int i=0; i < MAX_TRACK; ++i )
	//{
	//	if( m_pTrackArray[ i] )
	//		delete m_pTrackArray[ i];
	//}

	// DirectSound, DirectMusic ����
	//DSound_Shutdown();
	//DMusic_Shutdown();
}


//-------------------------------------------
// CSound �ʱ�ȭ
//-------------------------------------------
BOOL CSound::Init( HWND hwnd )
{
	//if( !DSound_Init( hwnd ) )
	//	return FALSE;

	//if( !DMusic_Init( hwnd ) )
	//	return FALSE;

	return TRUE;
}


//-------------------------------------------
// Load Sound file
// Ȯ���� ���� Load ��, ���� Ȯ���ڰ� �����ʴٸ� ����
// �Լ��� �����Ѵٸ� index�� �����Ѵ�.
//--------------------------------------------
BOOL CSound::Load( int iID, char *szFileName )
{
	//if( MAX_TRACK <= (unsigned)iID ) return FALSE;
	//if( m_pTrackArray[ iID] ) return FALSE; // �̹� �����Ǿ��ٸ� ����

	//// Ȯ���� ����
	//char szExpend[ 4];
	//int len = strlen( szFileName );
	//strcpy( szExpend, &szFileName[ len-3] );

	//// Load
	//if( !strcmp( szExpend, "mid" ) ) // midi
	//	m_pTrackArray[ iID] = new CTrack_Mid();
	//else if( !strcmp( szExpend, "wav" ) ) // wave
	//	m_pTrackArray[ iID] = new CTrack_Wav();
	//else
	//{
	//	return FALSE; // �������� �ʴ� Ȯ����
	//}

	//// load sound file
	//if( !m_pTrackArray[ iID]->Load(szFileName) )
	//	return FALSE;

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Load adpcm wave file 
//-----------------------------------------------------------------------------//
BOOL CSound::Load_Adpcm( int iID, char *szFileName )
{
	//if( MAX_TRACK <= (unsigned)iID ) return FALSE;
	//if( m_pTrackArray[ iID] ) return FALSE; // �̹� �����Ǿ��ٸ� ����

	//// adpcm wave
	//m_pTrackArray[ iID] = new CTrack_Adpcm();

	//// load sound file
	//if( !m_pTrackArray[ iID]->Load( szFileName ) )
	//	return FALSE;

	return TRUE;
}


//-----------------------------
// Play Sound
//-----------------------------
BOOL CSound::Play( int iID, BOOL bLoop ) // bLoop = FALSE
{
//	if( (unsigned)iID >= MAX_TRACK ) return FALSE;
//
//	if( m_pTrackArray[ iID] )
//	{
////		if( -1 != m_iPlayID )
////			Stop( m_iPlayID );
//
//		// play sound
//		m_pTrackArray[ iID]->Play( bLoop );
//		m_iPlayID = iID;
//		return TRUE;
//	}

	return FALSE;
}


//------------------------------
// Stop Sound
//------------------------------
BOOL CSound::Stop( int iID )
{
	//if( (unsigned)iID >= MAX_TRACK ) return FALSE;

	//if( m_pTrackArray[ iID] )
	//{
	//	// play sound
	//	m_pTrackArray[ iID]->Stop();
	//	m_iPlayID = -1;
	//	return TRUE;
	//}

	return FALSE;
}


//-------------------------------
// Stop All Sound
//-------------------------------
void CSound::AllStop()
{
	//for( int i=0; i < MAX_TRACK; ++i )
	//{
	//	if( m_pTrackArray[ i] )
	//		m_pTrackArray[ i]->Stop();
	//}
}


//------------------------
// volume Setting
//------------------------
void CSound::Volume( int iID, int iVol )
{
	//if( (unsigned)iID >= MAX_TRACK ) return;
	//if( m_pTrackArray[ iID] )
	//	m_pTrackArray[ iID]->Volume( iVol );
}


//------------------------
// Record ����
//------------------------
void CSound::Add_Record( int iSound_ID, int iSound_Delay, BOOL bPlay, BOOL bLoop  )
{
	//if( MAX_RECORD <= (unsigned)m_iRecord_Count ) return;

	//int find = -1;
	//for( int i=0; i < MAX_RECORD; ++i )
	//{
	//	if( !m_Record[ i].bUsed )
	//	{
	//		find = i;
	//		break;
	//	}
	//}
	//if( 0 > find ) return;

	//// ����
	//m_Record[ find].bUsed = TRUE;
	//m_Record[ find].play = bPlay;
	//m_Record[ find].loop = bLoop;
	//m_Record[ find].sound_id = iSound_ID;
	//m_Record[ find].sound_delay = iSound_Delay;

	//// ���ڵ� ��������
	//++m_iRecord_Count;
}


//----------------------
// Record �ʱ�ȭ
//----------------------
void CSound::Clear_Record()
{
	m_iRecord_Count = 0;
	memset( m_Record, 0, sizeof(m_Record) );
}


//-------------------------
// Sound Processor
//-------------------------
void CSound::Proc( int iElaps )
{
	//for( int i=0; i < MAX_TRACK; ++i )
	//{
	//	if( m_pTrackArray[ i] )
	//		m_pTrackArray[ i]->Proc();
	//}
	//if( 0 >= m_iRecord_Count )
	//	return;

	//for( i=0; i < MAX_RECORD; ++i )
	//{
	//	if( m_Record[ i].bUsed )
	//	{
	//		// �����ð��� ������ ���带 Play �ϰų� Stop �Ѵ�.
	//		m_Record[ i].sound_delay -= iElaps;
	//		if( 0 >= m_Record[ i].sound_delay )
	//		{
	//			if( m_Record[ i].play )
	//				Play( m_Record[ i].sound_id, m_Record[ i].loop );
	//			else
	//				Stop( m_Record[ i].sound_id );

	//			m_Record[ i].bUsed = FALSE;
	//			--m_iRecord_Count;
	//		}
	//	}
	//}
}


//-----------------------------------------------------------------------------//
// Sound On,Off
//-----------------------------------------------------------------------------//
void CSound::SetSoundOn( int iID, BOOL bOn )
{
	//if( (unsigned)iID >= MAX_TRACK ) return;
	//Volume( iID, (bOn)? 100 : 0 );
	//if( m_pTrackArray[ iID] )
	//	m_pTrackArray[ iID]->SoundOn( bOn );
}

