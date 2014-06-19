//-----------------------------------------------------------------------------//
// 2005-10-23  programer: jaejung ��(���բ�)��
//
// 2005-12-05 write, read �Լ� ����߰�
// ���̳ʸ� �޸� (structure memory)�� TokenData�� �°� binary, text file�� �����Ѵ�. (write)
// write�Լ��� �ۼ��� ���̳ʸ������� structure memory�� ��ȯ�Ѵ�. (read)
//
// 2008-02-05 
//		script ����Ÿ �ε�&���� ��� �߰�
//
// 2009-01-03
//		int �迭 �ε�&���� ��� �߰�, Clear�Լ� �߰�
//-----------------------------------------------------------------------------//

#if !defined(__LINEARMEMLOADER_H__)
#define __LINEARMEMLOADER_H__


#pragma warning (disable: 4786)
#include <windows.h>
#include "parser.h"
#include <string>
#include <list>
#include <queue>
#include <map>
using namespace std;

//-----------------------------------------------------------------------------//
// CLinearMemLoader
//-----------------------------------------------------------------------------//
class CLinearMemLoader
{
public:
	CLinearMemLoader();
	CLinearMemLoader( char *szTokenFileName );
	virtual ~CLinearMemLoader();

protected:
	enum VALUETYPE { TYPE_NULL, TYPE_DATA, TYPE_POINTER, TYPE_ARRAY, TYPE_STRUCT };
	typedef struct _tagSToken
	{
		_tagSToken():nSize(0) {}
		_tagSToken( string str, int size, VALUETYPE type ):nSize(size), strToken(str), eType(type) {}
		VALUETYPE eType;
		string strToken; // token
		int nSize; // (type_data, type_pointer(4) = typesize) (type_array = typesize * arraysize)
	} SToken;
	typedef struct _tagSType
	{
		list<SToken> *pMember;	// ��� ���� Ÿ�� ����Ʈ
		int size;				// Ÿ�� ������ (����Ʈ����)
		BOOL pointer;			// ����� �����Ͱ� �����ϸ� TRUE �׷����ʴٸ� FALSE
		_tagSType() {}
		_tagSType( list<SToken> *p, int s, BOOL pt ) : pMember(p), size(s),pointer(pt) {}
	} SType;
	typedef struct _tagSWsp	// Write Struct Pointer
	{
		_tagSWsp() {}
		_tagSWsp( string tok, VALUETYPE type, int fp, int size, BYTE *p ) : strToken(tok), eType(type), nFilePos(fp), nPointerSize(size), pStruct(p) {}
		string strToken;
		VALUETYPE eType;
		int nFilePos;		// Update�ؾߵ� file pointer position
		int nPointerSize;	// (TYPE_POINTER = pointersize) (�׿� = 0)
		BYTE *pStruct;
	} SWsp;
	typedef list<SToken> MemberList;
	typedef MemberList::iterator MemberItor;
	typedef map<string, SType > TokenMap;
	typedef TokenMap::iterator TokenItor;
	TokenMap m_STokenMap;

public:
	BOOL WriteBin( char *szFileName, void *pStruct, char *szTokenName );
	BOOL WriteBin( FILE *fp, void *pStruct, char *szTokenName );
	BOOL WriteScript( char *szFileName, void *pStruct, char *szTokenName );
	BOOL WriteScript( FILE *fp, void *pStruct, char *szTokenName, int tab );
	BYTE* ReadBin( char *szFileName, char *szTokenName );
	int ReadBin( BYTE *pReadMem, char *szTokenName );
	BYTE* ReadScript( char *szFileName, char *szTokenName );
	BOOL LoadTokenFile( char *szTokenFileName, int opt=0 );
	BOOL AddToken( char *szTokenName, int nSize, char *szParentTokenName=NULL );
	void Clear();

protected:
	void CreateDefaultToken();
	void LoadTokenFileRec( SDftDataList *pNode, MemberList *pTokenList, int opt );
	void LoadTokenFile_Member( SDftDataList *pNode, MemberList *pTokenList );

	void ReadRec( DWORD dwOffset, BYTE *pStruct, MemberList *pMList );
	void ReadScriptRec( BYTE *pStruct, SDftDataList *pNode, MemberList *pMList );
	void CollectPointerRec( DWORD dwCurFilePos, BYTE *pStruct, MemberList *pMList, queue<SWsp> *pWspList );
	int ParseData( BYTE *pStruct, SToken *pToken, char *szToken,SDftDataList *pNode=NULL, int idx=0 );
	int GetStructSize( MemberList *pList );
	BOOL IsPointer( MemberList *pList );
	void WriteTextData( FILE *fp, BYTE *pSrc, SToken *pTok );
	void WriteTab( FILE *fp, int tab );

};

#endif // __LINEARMEMLOADER_H__
