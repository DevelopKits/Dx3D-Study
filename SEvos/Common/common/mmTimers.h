//-----------------------------------------------------------------------------//
// 2006-01-10  programer: jaejung ��(���բ�)��
//
// 2002-02-10 ó�� �ۼ���
// 2006-01-10 �ڵ� ���� & ������ id�� timer�� ȣ���Ҽ��ְ� ������
//-----------------------------------------------------------------------------//

/*
  This simple class encapsulates the multimedia timers. 
  To use the class include mmTimers.cpp and mmTimers.h in your project. 
  Link with winmm.lib. To use a timer derive a class from CMMTimer 
  and override member timerProc. timerProc will be called when the timer goes off. 
  Instantiate a variable of the new class. 
  The parameter to the constructor is the timer resolution in ms. 
  To start a timer call startTimer. 
  The first parameter specifies the period of the timer in ms. 
  The second parameter specifies whether the timer is a one shot or periodic timer. 
  To stop a periodic timer call stopTimer. 
*/  


#ifndef	___multimedia_timers___
#define	___multimedia_timers___

#pragma comment(lib, "winmm.lib")
#include <mmsystem.h>

#pragma warning(disable: 4786)
#include <map> 


typedef void (CALLBACK *LPMMTIMECALLBACK)( UINT uTimerID, DWORD dwParam );


//-----------------------------------------------------------------------------//
// CMMTimers
//-----------------------------------------------------------------------------//
class CMMTimer
{
public:
	CMMTimer();
	CMMTimer( LPMMTIMECALLBACK lpTimeProc, DWORD m_dwParam );
	virtual ~CMMTimer();

protected:
	UINT m_nTimerRes;

public: // ����ȭ ������ ĸ��ȭ�� ����
	std::map<int,int> m_TimerMap;
	LPMMTIMECALLBACK m_lpTimeProc;
	DWORD m_dwParam;

public:
	void Create( LPMMTIMECALLBACK lpTimeProc, DWORD dwParam );
	BOOL SetTimer( int nTimerId, UINT uElapse, BOOL bOneShot=FALSE );
	BOOL KillTimer( int nTimerId );
	UINT GetTimerRes() { return m_nTimerRes; }
	void StopAllTimer();

protected:

};

#endif
