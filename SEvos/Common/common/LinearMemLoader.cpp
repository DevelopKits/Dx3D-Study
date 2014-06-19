
#include "stdafx.h"
#include "linearmemloader.h"
#include <sys/stat.h>


typedef struct _tagSBaseType
{
	char name[ 32];
	char size;
} SBaseType;
static SBaseType g_BaseType[] = 
{
	{ "int", sizeof(int) }, 
	{ "float", sizeof(float) },
	{ "char", sizeof(char) },
	{ "short", sizeof(short) },
	{ "size", sizeof(int) },
	{ "dummy", sizeof(int) }
};
const int g_BaseTypeSize = sizeof(g_BaseType) / sizeof(SBaseType);

SBaseType* GetBaseType( char *pTypeName )
{
	for( int i=0; i < g_BaseTypeSize; ++i )
	{
		if( !strcmp(g_BaseType[ i].name, pTypeName) )
			return &g_BaseType[ i];
	}
	return NULL;
}


CLinearMemLoader::CLinearMemLoader()
{
	CreateDefaultToken();
}

CLinearMemLoader::CLinearMemLoader( char *szTokenFileName )
{
	CreateDefaultToken();
	if( szTokenFileName )
		LoadTokenFile( szTokenFileName );
}


CLinearMemLoader::~CLinearMemLoader()
{
	Clear();
}

void CLinearMemLoader::Clear()
{
	TokenItor i = m_STokenMap.begin();
	while( m_STokenMap.end() != i )
	{
		SType type = i++->second;
		if( type.pMember )
			delete type.pMember;
	}
	m_STokenMap.clear();
}


//-----------------------------------------------------------------------------//
// �⺻���ΰ� �̸� ����� ���´�.
//-----------------------------------------------------------------------------//
void CLinearMemLoader::CreateDefaultToken()
{
	for( int i=0; i < g_BaseTypeSize; ++i )
		AddToken( g_BaseType[ i].name, g_BaseType[ i].size );
}


//-----------------------------------------------------------------------------//
// Token file �ε�
// opt: 0 �ߺ��� ����Ÿ�� �߻��ϸ� ������ �߻��Ѵ�.
//		1 �ߺ��� ����Ÿ�� �߻��ϸ� ���� �����.
//-----------------------------------------------------------------------------//
BOOL CLinearMemLoader::LoadTokenFile( char *szTokenFileName, int opt )
{
	SDftDataParser parser;
	SDftDataList *pRoot = parser.OpenScriptFile( szTokenFileName );
	if( !pRoot ) return NULL;

	SDftDataList *pNode = pRoot->pChild;
	LoadTokenFileRec( pRoot->pChild, NULL, opt );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Token�� �����Ѵ�.
// opt: 0 �ߺ��� ����Ÿ�� �߻��ϸ� ������ �߻��Ѵ�.
//		1 �ߺ��� ����Ÿ�� �߻��ϸ� ���� �����.
//-----------------------------------------------------------------------------//
void CLinearMemLoader::LoadTokenFileRec( SDftDataList *pNode, MemberList *pTokenList, int opt )
{
	if( !pNode ) return;

	TokenItor itor = m_STokenMap.find( pNode->t->pStr[ 0] );

	if( itor != m_STokenMap.end() )
	{
		// ���� ����Ÿ�� �ߺ��Ǿ� ����Ǿ� �ִٸ� opt���� ���� �帧�� �����ȴ�.
		if( 0 == opt )
		{
			// error
			return;
		}
		else if( 1 == opt )
		{
			SType type = itor->second;
			type.pMember->clear();
			LoadTokenFile_Member( pNode->pChild, type.pMember );
			itor->second.size = GetStructSize( type.pMember );
		}

/*
		SBaseType *p = GetBaseType( pNode->t->pStr[ 0] );
		if( p )
		{
			if( pTokenList )
				pTokenList->push_back( SToken(p->name, p->size, TYPE_DATA) );
		}
		else
		{
			// custome structure �ϰ��
			SType type = itor->second;
			if( pTokenList )
				pTokenList->push_back( SToken(pNode->t->pStr[ 0], type.size, TYPE_STRUCT) );
		}
/**/
	}
	else
	{
/*
		// �迭
		if( strchr(pNode->t->pStr[ 0], '[') )
		{
			char *p1 = strchr( pNode->t->pStr[ 0], '[' );
			char *p2 = strchr( pNode->t->pStr[ 0], ']' );
			char szNum[ 16];
			ZeroMemory( szNum, sizeof(szNum) );
			strncpy( szNum, p1+1, (p2-p1)-1 );
			int nSize = atoi( szNum );
			char szToken[ 64];
			ZeroMemory( szToken, sizeof(szToken) );
			strncpy( szToken, pNode->t->pStr[ 0], (p1 - pNode->t->pStr[ 0]) );

			TokenItor ArrayItor = m_STokenMap.find( szToken );
			if( ArrayItor == m_STokenMap.end() )
				return;

			nSize *= ArrayItor->second.size; // Array Total size
			if( pTokenList )
				pTokenList->push_back( SToken(szToken, nSize, TYPE_ARRAY) );
		}
		// ������
		else if( strchr(pNode->t->pStr[ 0], '*') )
		{
			char *p1 = strchr( pNode->t->pStr[ 0], '*' );
			char szToken[ 64];
			ZeroMemory( szToken, sizeof(szToken) );
			strncpy( szToken, pNode->t->pStr[ 0], (p1 - pNode->t->pStr[ 0]) );

			if( pTokenList )
				pTokenList->push_back( SToken(szToken, sizeof(void*), TYPE_POINTER) );
		}
/**/
		// ���ο� ����ü
//		else
		{
			list<SToken> *pNewList = new list<SToken>;
			//LoadTokenFileRec( pNode->pChild, pNewList, opt );
			LoadTokenFile_Member( pNode->pChild, pNewList );
			SType type( pNewList, GetStructSize(pNewList), IsPointer(pNewList) );
			m_STokenMap.insert( TokenMap::value_type(pNode->t->pStr[ 0], type) );
		}
	}

	LoadTokenFileRec( pNode->pNext, pTokenList, opt );

	return;
}


//-----------------------------------------------------------------------------//
// Custome struct�� ��� ��ū�� pTokenList�� �����Ѵ�.
//-----------------------------------------------------------------------------//
void CLinearMemLoader::LoadTokenFile_Member( SDftDataList *pNode, MemberList *pTokenList )
{
	SDftDataList *ptok = pNode;

	while( ptok )
	{
		TokenItor itor = m_STokenMap.find( ptok->t->pStr[ 0] );
		if( itor == m_STokenMap.end() )
		{
			// �迭
			if( strchr(ptok->t->pStr[ 0], '[') )
			{
				char *p1 = strchr( ptok->t->pStr[ 0], '[' );
				char *p2 = strchr( ptok->t->pStr[ 0], ']' );
				char szNum[ 16];
				ZeroMemory( szNum, sizeof(szNum) );
				strncpy_s( szNum, sizeof(szNum), p1+1, (p2-p1)-1 );
				int nSize = atoi( szNum );
				char szToken[ 64];
				ZeroMemory( szToken, sizeof(szToken) );
				strncpy_s( szToken, sizeof(szToken), ptok->t->pStr[ 0], (p1 - ptok->t->pStr[ 0]) );

				TokenItor ArrayItor = m_STokenMap.find( szToken );
				if( ArrayItor == m_STokenMap.end() )
					break;	// error

				nSize *= ArrayItor->second.size; // Array Total size
				pTokenList->push_back( SToken(szToken, nSize, TYPE_ARRAY) );
			}
			// ������
			else if( strchr(ptok->t->pStr[ 0], '*') )
			{
				char *p1 = strchr( ptok->t->pStr[ 0], '*' );
				char szToken[ 64];
				ZeroMemory( szToken, sizeof(szToken) );
				strncpy_s( szToken, sizeof(szToken), ptok->t->pStr[ 0], (p1 - ptok->t->pStr[ 0]) );
				pTokenList->push_back( SToken(szToken, sizeof(void*), TYPE_POINTER) );
			}
			else
			{
				// error
				break;
			}
		}
		else
		{
			SBaseType *p = GetBaseType( ptok->t->pStr[ 0] );
			if( p )
			{
				pTokenList->push_back( SToken(p->name, p->size, TYPE_DATA) );
			}
			else
			{
				// custome structure �ϰ��
				SType type = itor->second;
				pTokenList->push_back( SToken(ptok->t->pStr[ 0], type.size, TYPE_STRUCT) );
			}
		}

		ptok = ptok->pNext;
	}
}


//-----------------------------------------------------------------------------//
// ����ü pStruct ������ szTokenName Ÿ������ szFileName ���Ͽ� ����.
// szFileName : Write�� �����̸�
// pStruct: ������ �����ϰ��ִ� �޸�
// szTokenName : ����ü Ÿ��
//-----------------------------------------------------------------------------//
BOOL CLinearMemLoader::WriteBin( char *szFileName, void *pStruct, char *szTokenName )
{
	if( !pStruct ) return FALSE;

	FILE *fp;
	fopen_s( &fp, szFileName, "wb" );
	if( !fp ) return FALSE;

	WriteBin( fp, pStruct, szTokenName );
	fclose( fp );

	return TRUE;
}
BOOL CLinearMemLoader::WriteScript( char *szFileName, void *pStruct, char *szTokenName )
{
	if( !pStruct ) return FALSE;

	FILE *fp;
	fopen_s( &fp, szFileName, "w" );
	if( !fp ) return FALSE;
	fprintf( fp, "GPJ\n" );

	WriteScript( fp, pStruct, szTokenName, 0 );
	fclose( fp );

	return TRUE;
}

//-----------------------------------------------------------------------------//
// ����ü pStruct ������ szTokenName Ÿ������ ���������� fp�� ����.
// fp : Dest���� ������
// pStruct: ������ �����ϰ��ִ� �޸�
// szTokenName : ����ü Ÿ��
//-----------------------------------------------------------------------------//
BOOL CLinearMemLoader::WriteBin( FILE *fp, void *pStruct, char *szTokenName )
{
	queue<SWsp> WspQueue;
	WspQueue.push( SWsp(szTokenName, TYPE_DATA, 0, 0, (BYTE*)pStruct) );
	int nFileStartPos = ftell( fp );

	while( !WspQueue.empty() )
	{
		SWsp WspInfo = WspQueue.front(); WspQueue.pop();

		TokenItor titor = m_STokenMap.find( WspInfo.strToken );
		if( m_STokenMap.end() == titor ) break; // error !!
		SType type = titor->second;

		// file�� ����
		switch( WspInfo.eType )
		{
		case TYPE_ARRAY:
		case TYPE_DATA:
		case TYPE_STRUCT:
			{
				fwrite( WspInfo.pStruct, type.size, 1, fp );

				// struct���� pointer �� filepos ����
				CollectPointerRec( ftell(fp) - type.size, (BYTE*)WspInfo.pStruct, type.pMember, &WspQueue );
			}
			break;

		case TYPE_POINTER:
			{
				// ���� filePos�� �����Ѵ�.
				DWORD dwCurPos = ftell( fp );
				DWORD dwPointerOffset = dwCurPos - nFileStartPos;
				fseek( fp, WspInfo.nFilePos, SEEK_SET ); // pointer���� Update�ɰ����� �̵�
				fwrite( &dwPointerOffset, sizeof(DWORD), 1, fp );
				fseek( fp, dwCurPos, SEEK_SET ); // �ٽ� ���ƿ�
				fwrite( WspInfo.pStruct, type.size, WspInfo.nPointerSize, fp );

				// struct���� pointer �� filepos ����
				dwCurPos = ftell( fp ) - (type.size * WspInfo.nPointerSize);
				for( int i=0; i < WspInfo.nPointerSize; ++i )
					CollectPointerRec( dwCurPos + (type.size*i), WspInfo.pStruct+(type.size*i), type.pMember, &WspQueue );
			}
			break;
		}
	}

	return TRUE;
}
//-----------------------------------------------------------------------------//
// ����ü������ pStruct�� szTokenName���˿� �°� ��ũ��Ʈ�� �����Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CLinearMemLoader::WriteScript( FILE *fp, void *pStruct, char *szTokenName, int tab )
{
	TokenItor titor = m_STokenMap.find( szTokenName );
	if( m_STokenMap.end() == titor ) return FALSE; // error !!
	SType type = titor->second;

	WriteTab( fp, tab );
	fprintf( fp, "%s ", szTokenName );

	BYTE *psrc = (BYTE*)pStruct;
	BOOL bchild = FALSE;

	MemberItor it = type.pMember->begin();
	while( type.pMember->end() != it )
	{
		SToken tok = *it++;
		switch( tok.eType )
		{
		case TYPE_NULL:
			break;

		case TYPE_DATA:
		case TYPE_ARRAY:
			{
				WriteTextData( fp, psrc, &tok );
				fprintf( fp, " " );

				psrc += tok.nSize;
			}
			break;

		case TYPE_STRUCT:
			{
				if( !bchild )
				{
					fprintf( fp, "\n" );
					WriteTab( fp, tab );
					fprintf( fp, "{" );
					fprintf( fp, "\n" );
					bchild = TRUE;
				}

				WriteScript( fp, psrc, (char*)tok.strToken.c_str(), tab+1 );

				psrc += tok.nSize;
			}
			break;

		case TYPE_POINTER:
			{
				if( !bchild )
				{
					fprintf( fp, "\n" );
					WriteTab( fp, tab );
					fprintf( fp, "{" );
					fprintf( fp, "\n" );
					bchild = TRUE;
				}

				// ** pointer type�� ���������� ���� size���� ����Ǿ��ִ�.
				int *psize = (int*)(psrc - 4); // pointersize �� ��´�.
				BYTE* pPointerAdress = (BYTE*)*(DWORD*)psrc;

				TokenItor titor = m_STokenMap.find( tok.strToken );
				if( m_STokenMap.end() == titor ) break; // error !!
				SType type = titor->second;
				for( int i=0; i < *psize; ++i )				
					WriteScript( fp, (pPointerAdress+(type.size*i)), (char*)tok.strToken.c_str(), tab+1 );

				psrc += tok.nSize;
			}
			break;
		}
	}

	if( bchild )
	{
		WriteTab( fp, tab );
		fprintf( fp, "}" );
	}

	fprintf( fp, "\n" );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// binary file�ε� szFileName ������ szTokenName Ÿ������ �о�帰��.
// �о�帰 ����Ÿ�� BYTE* ���ϵȴ�. (�޸����Ŵ� �����������)
//-----------------------------------------------------------------------------//
BYTE* CLinearMemLoader::ReadBin( char *szFileName, char *szTokenName )
{
	FILE *fp;
	fopen_s( &fp, szFileName, "rb" );
	if( !fp ) return NULL;

	struct _stat buf;
	_stat( szFileName, &buf );
	int filesize = buf.st_size;	// 64 bit 

	BYTE *pRead = new BYTE[ filesize];
	fread( pRead, 1, filesize, fp );
	fclose( fp );

	if( !ReadBin(pRead, szTokenName) )
	{
		delete[] pRead;
		pRead = NULL;
	}

	return pRead;
}


//-----------------------------------------------------------------------------//
// pReadMem �� ����� ����Ÿ�� szTokenName Ÿ������ �о�帰��.
// pReadMem : Write() �Լ��� ������ file ����
// nOffset : file�߰����� CLinearMemLoader Ŭ������ ����Ҷ� �ʿ��Ѱ�.. �ظ��ϸ� ��������
// szTokenName : pReadMem form
// **�о�帰 byte���� �����ҷ��� ������ �ӵ�����ȭ�� ������ ����.**
//-----------------------------------------------------------------------------//
int CLinearMemLoader::ReadBin( BYTE *pReadMem, char *szTokenName )
{
	TokenItor itor = m_STokenMap.find( szTokenName );
	if( m_STokenMap.end() == itor ) return 0;

	// pointer offset�� ����
	ReadRec( (DWORD)pReadMem, pReadMem, itor->second.pMember );

	return 1;
}


//-----------------------------------------------------------------------------//
// Member���� size�� ���Ѵ�.
//-----------------------------------------------------------------------------//
int CLinearMemLoader::GetStructSize( MemberList *pList )
{
	int nSize = 0;
	MemberItor itor = pList->begin();
	while( itor != pList->end() )
	{
		nSize += itor->nSize;
		++itor;
	}
	return nSize;
}


//-----------------------------------------------------------------------------//
// Member�� pointer�� �ִ��� �˻��Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CLinearMemLoader::IsPointer( MemberList *pList )
{
	MemberItor itor = pList->begin();
	while( itor != pList->end() )
	{
		if( TYPE_POINTER == itor->eType )
			return TRUE;
		else if( TYPE_STRUCT == itor->eType )
		{
			TokenItor titor = m_STokenMap.find( itor->strToken );
			if( m_STokenMap.end() == titor ) return FALSE; // error !!
			if( titor->second.pointer )
				return TRUE;
		}
		++itor;
	}
	return FALSE;
}


//-----------------------------------------------------------------------------//
// token �߰�
// szTokenName: token name
// nSize : token�� byte��
// szParentTokenName : parent token name (�̰�� parent�� child�� �߰��ȴ�.)
//-----------------------------------------------------------------------------//
BOOL CLinearMemLoader::AddToken( char *szTokenName, int nSize, char *szParentTokenName ) // szParentTokenName = NULL
{
	if( szParentTokenName )
	{
		TokenItor titor = m_STokenMap.find( szParentTokenName );
		if( m_STokenMap.end() == titor ) return FALSE; // error!!
		titor->second.pMember->push_back( SToken(szTokenName, nSize, TYPE_DATA) );
	}
	else
	{
		list<SToken> *pNewList = new list<SToken>;
		pNewList->push_back( SToken(szTokenName, nSize, TYPE_DATA) );
		m_STokenMap.insert( TokenMap::value_type(szTokenName, SType(pNewList,nSize,FALSE)) );
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// pointer & filepos ã��
// dwCurFilePos : pStruct ����Ÿ�� ����� filepointer
// pStruct : File�� ������ Data Pointer
// pMList : pStruct �� �������Ʈ
//-----------------------------------------------------------------------------//
void CLinearMemLoader::CollectPointerRec( DWORD dwCurFilePos, BYTE *pStruct, MemberList *pMList, queue<SWsp> *pWspList )
{
	DWORD dwFilePos = dwCurFilePos;
	MemberItor mitor = pMList->begin();
	while( mitor != pMList->end() )
	{
		SToken t = *mitor;

		if( TYPE_POINTER == mitor->eType )
		{
			// ** pointer type�� ���������� ���� size���� ����Ǿ� �־���Ѵ�.
			int *pSize = (int*)(pStruct - 4); // pointersize �� ��´�.
			if( 0 < *pSize )
			{
				DWORD dwAddress = *(DWORD*)pStruct;
				pWspList->push( SWsp(mitor->strToken, TYPE_POINTER, dwFilePos, *pSize, (BYTE*)dwAddress ) );
			}
		}
		else if( TYPE_ARRAY == mitor->eType )
		{
			TokenItor titor = m_STokenMap.find( mitor->strToken );
			if( m_STokenMap.end() == titor ) break; // error!!

			// member������ pointer�� ��������
			SType type = titor->second;
			if( type.pointer )
			{
				int nArraySize = mitor->nSize / type.size;
				for( int i=0; i < nArraySize; ++i )
					CollectPointerRec( dwFilePos + (type.size*i), (pStruct + (type.size*i)), type.pMember, pWspList );
			}
		}
		else if( TYPE_STRUCT == mitor->eType )
		{
			TokenItor titor = m_STokenMap.find( mitor->strToken );
			if( m_STokenMap.end() == titor ) break; // error!!

			// member������ pointer�� ��������
			SType type = titor->second;
			if( type.pointer )
				CollectPointerRec( dwFilePos, pStruct, type.pMember, pWspList );
		}

		dwFilePos += mitor->nSize;
		pStruct += mitor->nSize;
		++mitor;
	}
}


//-----------------------------------------------------------------------------//
// pointer offset�� ����
// member������ �ϳ����˻��ؼ� type�� pointer�ϰ�� dwOffset����ŭ �����Ѵ�.
//-----------------------------------------------------------------------------//
void CLinearMemLoader::ReadRec( DWORD dwOffset, BYTE *pStruct, MemberList *pMList )
{
	MemberItor mitor = pMList->begin();
	while( mitor != pMList->end() )
	{
		SToken st = *mitor;

		if( TYPE_POINTER == mitor->eType )
		{
			// size���� 0 ���� Ŭ���� ������
			int *pSize = (int*)(pStruct - 4); // pointersize �� ��´�.
 			if( 0 < *pSize )
			{
				*(DWORD*)pStruct += dwOffset; // Poiner�� ����
				BYTE* pPointerAdress = (BYTE*)*(DWORD*)pStruct;

				TokenItor titor = m_STokenMap.find( mitor->strToken );
				if( m_STokenMap.end() == titor ) break; // error !!
				SType type = titor->second;
				if( type.pointer )
				{
					for( int i=0; i < *pSize; ++i )
						ReadRec( dwOffset, (pPointerAdress+(type.size*i)), type.pMember );
				}
			}
		}
		else if( TYPE_ARRAY == mitor->eType )
		{
			TokenItor titor = m_STokenMap.find( mitor->strToken );
			if( m_STokenMap.end() == titor ) break; // error !!
			SType type = titor->second;
			if( type.pointer )
			{
				int nArraySize = mitor->nSize / type.size;
				for( int i=0; i < nArraySize; ++i )
					ReadRec( dwOffset, pStruct+(type.size*i), type.pMember );
			}
		}
		else if( TYPE_STRUCT == mitor->eType )
		{
			TokenItor titor = m_STokenMap.find( mitor->strToken );
			if( m_STokenMap.end() == titor ) break; // error !!
			SType type = titor->second;
			if( type.pointer )
				ReadRec( dwOffset, pStruct, type.pMember );
		}

		pStruct += mitor->nSize;
		++mitor;
	}
}


//-----------------------------------------------------------------------------//
// szFileName ������ szTokenName Ÿ������ ��¡�ؼ� �޸𸮿� �����Ѵ�.
// szFileName : ��ũ��Ʈ ���� ���
// szTokenName : ��ȯ�� ����
// szFileName ��ũ��Ʈ ������ ���� ��ūƮ���� �м��ϸ鼭 ���� �޸𸮷� ��ȯ
// ��Ų��.
//-----------------------------------------------------------------------------//
BYTE* CLinearMemLoader::ReadScript( char *szFileName, char *szTokenName )
{
	SDftDataParser parser;
	SDftDataList *pRoot = parser.OpenScriptFile( szFileName );
	if( !pRoot ) return NULL;
	SDftDataList *pToken = CParser<SDftData>::FindToken( pRoot->pChild, SDftData(szTokenName) );
	if( !pToken ) return NULL;

	TokenItor titor = m_STokenMap.find( szTokenName );
	if( m_STokenMap.end() == titor ) return NULL;

	SType type = titor->second;
	BYTE *pStruct = new BYTE[ type.size];
	ZeroMemory( pStruct, type.size );

	ReadScriptRec( pStruct, pToken, type.pMember );

	return pStruct;
}


//-----------------------------------------------------------------------------//
// �ڵ尡 ���� ������.
// Script�� ��¡�Ҷ� Script driven ������� ��¡���� MemberList driven������� ��¡�Ѵ�. (�Ǽ�)
//-----------------------------------------------------------------------------//
void CLinearMemLoader::ReadScriptRec( BYTE *pStruct, SDftDataList *pNode, MemberList *pMList )
{
	if( !pNode || !pStruct || !pMList ) return;

	// Line ��¡
	int idx=1;
	MemberItor mitor = pMList->begin();
	while( pNode->t->pStr[ idx] && (pMList->end() != mitor) )
	{
		SToken t = *mitor;
		switch( t.eType )
		{
		case TYPE_DATA:
		case TYPE_ARRAY: // string
			idx = ParseData( pStruct, &t, pNode->t->pStr[ idx], pNode, idx ); break;
			break;

		case TYPE_POINTER:
			{
				// ����Ÿ ������ ��´�.
				int count = 0;
				while( pNode->t->pStr[ idx + count] )
					++count;

				if( 0 < count )
				{
					TokenItor titor = m_STokenMap.find( t.strToken );
					if( m_STokenMap.end() == titor ) break; // error !!
					SType type = titor->second;

					BYTE *pSubMem = new BYTE[ count * type.size];
					ZeroMemory( pSubMem, count * type.size );

					for( int i=0; i < count; ++i )
						ParseData( pSubMem+(i*type.size), &t, pNode->t->pStr[ idx+i] );

					// memory�ּҰ� ����
					*(DWORD*)pStruct = (DWORD)pSubMem;
					// pointer size �� ����
					*(int*)(pStruct-4) = count;
				}

				idx += count;
			}
			break;

		default:
			++idx;
			break;
		}

		pStruct += t.nSize;
		++mitor;
	}

	// Child Line ��¡
	pNode = pNode->pChild;
	while( pNode && (pMList->end() != mitor) )
	{
		switch( mitor->eType )
		{
		// �Ǽ�����...
		// TYPE_DATA�ϰ�� (size, structure) 2���� ��찡 �����Ѵ�.
		case TYPE_DATA:
		case TYPE_STRUCT:
			{
				// ����Ÿ���� TYPE_POINTER�ϰ�� ���� TYPE�� size�� ����Ű�Ƿ� �����ϰ� �Ѿ��.
				// �׷��� �ʴٸ� Structure�ϰ���̹Ƿ� ��¡�Ѵ�.
				SToken t = *mitor;
				MemberItor temp = mitor;
				++temp;

				// size type �ϰ��
				if( (pMList->end() != temp) && TYPE_POINTER == temp->eType )
				{
					// �ƹ��� ����
					pNode = pNode->pNext;
				}
				// structure �ϰ��
				else
				{
					TokenItor titor = m_STokenMap.find( mitor->strToken );
					if( m_STokenMap.end() == titor ) break; // error !!
					SType type = titor->second;
					ReadScriptRec( pStruct, pNode, type.pMember );
					pNode = pNode->pNext;
				}
			}
			break;

		case TYPE_ARRAY:
			{
				SToken t = *mitor;
				TokenItor titor = m_STokenMap.find( mitor->strToken );
				if( m_STokenMap.end() == titor ) break; // error !!
				SType type = titor->second;
				int nArraySize = mitor->nSize / type.size;
				for( int i=0; (i < nArraySize) && pNode; ++i )
				{
					ReadScriptRec( pStruct + (i*type.size), pNode, type.pMember );
					pNode = pNode->pNext;
				}
			}
			break;

		case TYPE_POINTER:
			{
				SToken t = *mitor;
				TokenItor titor = m_STokenMap.find( mitor->strToken );
				if( m_STokenMap.end() == titor ) break; // error !!

				SType type = titor->second;
				int nCount = CParser<SDftData>::GetNodeCount( pNode, SDftData((char*)mitor->strToken.c_str()) );
				if( 0 < nCount )
				{
					BYTE *pSubMem = new BYTE[ nCount * type.size];
					ZeroMemory( pSubMem, nCount * type.size );

					for( int i=0; (i < nCount) && pNode; ++i )
					{
						ReadScriptRec( pSubMem + (i*type.size), pNode, type.pMember );
						pNode = pNode->pNext;
					}

					// memory�ּҰ� ����
					*(DWORD*)pStruct = (DWORD)pSubMem;
					// pointer size �� ����
					*(int*)(pStruct-4) = nCount;
				}
			}
			break;
		}

		pStruct += mitor->nSize;
		++mitor;
	}
}


//-----------------------------------------------------------------------------//
// token parse
//-----------------------------------------------------------------------------//
int CLinearMemLoader::ParseData( BYTE *pStruct, SToken *pToken, char *szToken, SDftDataList *pNode, int idx )
{
	if( pToken->strToken == "int" )
	{
		if( TYPE_ARRAY == pToken->eType )
		{
			int size = pToken->nSize;
			while( 0 < size )
			{
				if( !pNode->t->pStr[ idx] )	*(int*)pStruct = 0;
				else						*(int*)pStruct = atoi( pNode->t->pStr[ idx] );
				++idx;
				size -= sizeof(int);
				pStruct += sizeof(int);
			}
			--idx;
		}
		else
		{
			*(int*)pStruct = atoi( szToken );
		}
	}
	else if( pToken->strToken == "float" )
	{
		*(float*)pStruct = (float)atof( szToken );
	}
	else if( pToken->strToken == "short" )
	{
		*(short*)pStruct = (short)atoi( szToken );
	}
	else if( pToken->strToken == "char" )
	{
		if( TYPE_ARRAY == pToken->eType )
		{
			strcpy_s( (char*)pStruct, strlen(szToken)+1, szToken );
		}
		else
		{
			*(char*)pStruct = *szToken;
		}
	}
	else if( pToken->strToken == "size" )
	{
		// script�� �о�帱�� size���� �ڵ����� �����ȴ�.
	}
	else if( pToken->strToken == "dummy" )
	{
		// �ƹ��ϵ� ����.
	}

	return ++idx;
}


//-----------------------------------------------------------------------------//
// ���������� fp�� ��ūŸ�Կ� �°� �ؽ�Ʈ���·� �����Ѵ�.
//-----------------------------------------------------------------------------//
void CLinearMemLoader::WriteTextData( FILE *fp, BYTE *pSrc, SToken *pTok )
{
	if( pTok->strToken == "int" )
	{
		if( TYPE_ARRAY == pTok->eType )
		{
			int size = pTok->nSize;
			while( 0 < size )
			{
				fprintf( fp, "%d ", *(int*)pSrc );
				size -= sizeof(int);
				pSrc += sizeof(int);
			}
		}
		else
		{
			fprintf( fp, "%d", *(int*)pSrc );
		}
	}
	else if( pTok->strToken == "float" )
	{
		fprintf( fp, "%f", *(float*)pSrc );
	}
	else if( pTok->strToken == "short" )
	{
		fprintf( fp, "%d", *(short*)pSrc );
	}
	else if( pTok->strToken == "char" )
	{
		if( TYPE_ARRAY == pTok->eType )
		{
			fprintf( fp, "\"%s\"", (char*)pSrc );
		}
		else
		{
			fprintf( fp, "%c", *(char*)pSrc );
		}
	}
	else if( pTok->strToken == "size" )
	{
		fprintf( fp, "%d", *(int*)pSrc );
	}
	else if( pTok->strToken == "dummy" )
	{
		// �ƹ��ϵ� ����.
	}

}


//-----------------------------------------------------------------------------//
// Tab ������ŭ �̵�
//-----------------------------------------------------------------------------//
void CLinearMemLoader::WriteTab( FILE *fp, int tab )
{
	for( int t=0; t < tab; ++t )
		fprintf( fp, "\t" );
}
