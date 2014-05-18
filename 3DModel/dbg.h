//-----------------------------------------------------------------------------//
// 2003-12-12  programer: jaejung (^_^;;)
// 
//-----------------------------------------------------------------------------//

#ifndef __DBG_H__
#define __DBG_H__

#include <windows.h>
#include <imagehlp.h>

// �ߺ���밡��
enum
{
	DBGLIB_NONE,
	DBGLIB_CONSOL	=0x01,	// �ܼ�â ���
	DBGLIB_OUTPUT	=0x02,	// Outputâ ���
	DBGLIB_LOG		=0x04, 	// txt���� ���
	DBGLIB_MESSAGE	=0x08,	// MessageBox ���
};


//-----------------------------------------------------------------------------//
// ClassName: IDbgDrawAgent
// ����������� ����� Draw Engine Agent �̴�.
// ���α׷��Ӵ� �� Ŭ����(IDbgDrawAgent)�� ��ӹ޾Ƽ� �����ؾ� �Ѵ�.
//-----------------------------------------------------------------------------//
class IDbgDrawAgent
{
public:
	int m_X, m_Y, m_nHeight;
	IDbgDrawAgent() : m_X(0), m_Y(0), m_nHeight(0) {}
	virtual void Create( int x, int y, int nHeight )
		{ m_X =x, m_Y = y, m_nHeight = nHeight; }
	virtual void DrawBegin()=0;
	virtual void DrawEnd()=0;
	virtual void Draw( int x, int y, BOOL bFocus, char *szMsg )=0;
};

#pragma warning (disable: 4786)
#include <map>
#include <vector>
#include "dbgstream.h"
#include <assert.h>


//-----------------------------------------------------------------------------//
// ClassName: CDbg
// 2001-11-03 programer: jaejung
//  - tracking, log, print ���
// 2003-11-09 programer: jaejung
//  - outputstream, stackwalk
//-----------------------------------------------------------------------------//
class CDbg
{
public:
	CDbg();
	virtual ~CDbg();
protected:
	// debug base class
	class CDbgBase
	{
	public:
		CDbgBase( char *szTitle ) { strcpy(m_szTitle,szTitle); }
		char m_szTitle[ 64], m_szDrawBuf[ 128];
		virtual char* ConvertString()=0;
		virtual void operator--()=0;
		virtual void operator++()=0;
	};
	// debug int
	class CDbgInt : public CDbgBase
	{
	public:
		CDbgInt( char *szTitle, int *pVal, int nModify ):
		  CDbgBase(szTitle), m_pVal(pVal), m_nModify(nModify) { ConvertString(); }
		int	*m_pVal;
		int m_nModify;
		char* ConvertString() { sprintf(m_szDrawBuf,"%s : %d",m_szTitle,*m_pVal); return m_szDrawBuf; }
		void operator--() { *m_pVal -= m_nModify; ConvertString(); }
		void operator++() { *m_pVal += m_nModify; ConvertString(); }
	};
	// debug float
	class CDbgFloat : public CDbgBase
	{
	public:
		CDbgFloat( char *szTitle, float *pVal, float fModify ):
		  CDbgBase(szTitle), m_pVal(pVal), m_fModify(fModify) { ConvertString(); }
		float *m_pVal;
		float m_fModify;
		char* ConvertString() { sprintf(m_szDrawBuf,"%s : %f",m_szTitle,*m_pVal); return m_szDrawBuf; }
		void operator--() { *m_pVal -= m_fModify; ConvertString(); }
		void operator++() { *m_pVal += m_fModify; ConvertString(); }
	};
	// debug double
	class CDbgDouble : public CDbgBase
	{
	public:
		CDbgDouble( char *szTitle, double *pVal, double dModify ):
		  CDbgBase(szTitle), m_pVal(pVal), m_dModify(dModify) { ConvertString(); }
		double *m_pVal;
		double m_dModify;
		char* ConvertString() { sprintf(m_szDrawBuf,"%s : %f",m_szTitle,*m_pVal); return m_szDrawBuf; }
		void operator--() { *m_pVal -= m_dModify; ConvertString(); }
		void operator++() { *m_pVal += m_dModify; ConvertString(); }
	};

	enum { DBG_MAXTRACKINGVALUE = 128 };
	doutputostream	m_OutputOstream;
	dconsolostream	m_ConsolOstream;
	dmessageostream m_MessageOstream;
	typedef std::map<int,dfileostream*> LogFileMap;
	typedef LogFileMap::iterator LogFileItor;
	typedef std::vector<CDbgBase*> TrackValueVector;
	int				m_nOutputType;
	int				m_nLogID;
	int				m_nDbgDrawCursor;
	LogFileMap		m_LogFileMap;
	TrackValueVector m_TrackingValue;
	IDbgDrawAgent	*m_pDbgDrawAgent;
public:
	BOOL Create( int nOutputType, int nLogID=-1, IDbgDrawAgent *pDbgDrawAgent=NULL ); // Ŭ���� ����
	void Print( char *szMsg, ... );									// OutputType�� ���� ����Ѵ�.
	void Console( char *szMsg, ... );								// Consol ���
	void Log( int nLogID, char *szMsg, ... );						// File ���
	void Output( char *szMsg, ... );								// Outputâ ���
	void MessageBox( char *szMsg, ... );							// �޼����ڽ� ���
	void TrackingValue( char *szTitle, int *pValue, int nModify, int nLogID=-1 );	// ���� ����
	void TrackingValue( char *szTitle, float *pValue, float fModify, int nLogID=-1 );
	void TrackingValue( char *szTitle, double *pValue, double dModify, int nLogID=-1 );
	void StackWalk( CONTEXT *ContextRecord, int nOutputType, int nLogID=-1 );	// ����Ž��
	void StackWalk( CONTEXT *ContextRecord ) { StackWalk(ContextRecord,m_nOutputType,m_nLogID); }	// ����Ž��
	void PCInfo( int nOutputType, int nLogID=-1 );					// PC���� ���
	void PCInfo() { PCInfo(m_nOutputType, m_nLogID); }				// PC���� ���
	void SetOutputType( int nOutputType, int nLogID=-1 );			// OutputType ����
	BOOL CreateLogFile( int nLogID, char *szFileName );				// LogFile ����
	BOOL SetDrawAgent( IDbgDrawAgent *pDbgDrawAgent );				// ��� interface ����
	void KeyProc( int nKey );										// Keyboard ó��
	void Draw();													// Trackingvalue ���
	void ClearTrackingValue();										// Trackingvalue �ʱ�ȭ
protected:
	dfileostream* FindLogFile( int nLogID );
	void DbgPrint( int nOutputType, int nLogID, char *szMsg );
};
extern CDbg g_Dbg; // ���� instance
#endif // __DBG_H__
