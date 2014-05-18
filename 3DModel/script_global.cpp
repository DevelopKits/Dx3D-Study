
#include "global.h"
#include "script_global.h"
#include "script_parser.h"
#include <algorithm>

using namespace std;
using namespace ns_script;

stack<string> ns_script::g_Include;



///////////////////////////////////////////////////////////////////////////////////
// CPreCompiler


//-----------------------------------------------------------------------------//
// define���� ������ ���� �����Ѵ�. precompiler�� ����ϴ�.
// ���� �����ϸ� FALSE�� �����Ѵ�.
// pID : ���� Ű���� �ȴ�.
// type: ��ūŸ��
// pStr: ����� ��ū����Ÿ
//-----------------------------------------------------------------------------//
BOOL CPreCompiler::InsertDefine( char *pID, Tokentype type, char *pStr )
{
	map<string,STokDat>::iterator it = m_DefMap.find( pID );
	if( m_DefMap.end() != it ) return FALSE; // ���� Ű���� ����Ÿ�� �̹� ������ ����!!

	STokDat t;
	t.str = pStr;
	t.tok = type;
	m_DefMap.insert( map<string,STokDat>::value_type(pID, t) );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// ����� define������ ã�´�.
// ����Ÿ�� ���ٸ� FALSE�� �����Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CPreCompiler::FindDefine( char *pID, Tokentype *pType, string *pStr )
{
	map<string,STokDat>::iterator it = m_DefMap.find( pID );
	if( m_DefMap.end() == it ) return FALSE;

	*pStr = it->second.str;
	*pType = it->second.tok;
	return TRUE;
}


//-----------------------------------------------------------------------------//
// ��ó�� �۾�
//-----------------------------------------------------------------------------//
void CPreCompiler::PreCompile( SParseTree *pTree )
{
	if( !pTree ) return;
	if( Exp == pTree->nodekind )
	{
		if( IdK == pTree->kind )
		{
			Tokentype t;
			string str;
			if( FindDefine( pTree->attr.name, &t, &str) )
			{
				if( NUM == t )
				{
					pTree->kind = ConstIntK;
					pTree->attr.num = atoi( str.c_str() );
				}
				else if( STRING == t )
				{
					pTree->kind = ConstStrK;
					strcpy( pTree->attr.name, str.c_str() );
				}
			}
		}
	}
	for( int i=0; i < MAXCHILD; ++i )
	{
		if( pTree->child[ i] )
			PreCompile( pTree->child[ i] );
	}
	PreCompile( pTree->sibling);
}


 
///////////////////////////////////////////////////////////////////////////////////
// CTypeTable

char *g_DefautType[] = { "void", "bool", "int", "float", "string" };
const int g_DefautTypeSize = sizeof(g_DefautType) / sizeof(char*);

CTypeTable::CTypeTable( CTypeTable *parent ) 
{
	m_pPt = parent;

	// �⺻���ΰ� �̸� �����Ѵ�.
	for( int i=0; i < g_DefautTypeSize; ++i )
		NewType( STypeData(g_DefautType[i], g_DefautType[i],0,DT_DATA) );

}

CTypeTable::~CTypeTable() 
{
	map< string, list<STypeData>* >::iterator itor = m_TypeTable.begin();
	while( m_TypeTable.end() != itor )
	{
		if( itor->second )
			delete itor->second;
		++itor;
	}
	m_TypeTable.clear();
}


//-----------------------------------------------------------------------------//
// NewType()�Լ��� Ÿ���� �����Ѵ�.
// map�� ����Ǵ� ������ type�� ������� Ȥ�� ����Լ��� �̸��� Ÿ���̴�.
// data,class,function ��� map�� ����Ǹ� ��� �ϳ��� STypeData����ü�� 
// map�� list�� ����ȴ�. 
// list�� ���� ù��° ������ �� ����ŸŸ���� type,state ���� ������ ������ 
// name ���� NULL�� ����ȴ�. �ֳ��ϸ� name�� �̹� map�� key�� ���Ǳ� �����̸�
// �������,�Լ��� �� ������ ����Ÿ�� �˻��ϱ����� name���� NULL�� ����ȴ�.
//-----------------------------------------------------------------------------//
BOOL CTypeTable::NewType( STypeData Type )
{
	if( IsType(Type.name) ) return FALSE;
	list<STypeData> *plist = new list<STypeData>;

	char key[ 64];
	strcpy( key, Type.name );

	// name���� NULL�� �����ǰ�, �Ŀ� type�� �˻��Ҷ� �������,�Լ��� �ƴ� data
	// �� �˻��ϱ� ���� �����ȴ�.
//	Type.name = NULL;
	Type.name[ 0] = NULL;
	plist->push_back( Type );
	m_TypeTable.insert( map< string, list<STypeData>* >::value_type(key,plist) );

	return TRUE;
}

// szTypeName Ÿ�Կ� ��������� �Լ��� �߰��Ѵ�.
// szTypeName�� Ű���ȴ�.
BOOL CTypeTable::AddType( char *pTypeName, STypeData Member )
{
	// pTypeName�� NULL�ϰ�� ��������, �Լ��� �ȴ�.
	if( !pTypeName )
	{
		NewType( Member );
	}
	else
	{
		if( !IsType(pTypeName) ) return FALSE;
		m_TypeTable[ pTypeName]->push_back( Member );
	}
	return TRUE;
}


// szTypeName�� Ÿ���� ������� Ȥ�� �Լ��� Ÿ���� ��´�.
char* CTypeTable::GetType( char *pTypeName, char *pMemberName ) // pMemberName=NULL
{
	if( !pTypeName ) return NULL;
	if( !IsType(pTypeName) ) return NULL;
	list<STypeData> *plist = m_TypeTable[ pTypeName];
	list<STypeData>::iterator itor = find( plist->begin(), plist->end(), STypeData(pMemberName,NULL,0,DT_DATA) );
	if( plist->end() == itor ) return NULL;
	if( NULL == itor->type[ 0] ) return NULL;
	return itor->type;
}

BOOL CTypeTable::IsType( char *pTypeName, char *pMemberName ) // pMemberName=NULL
{
	if( _IsType(pTypeName, pMemberName) )
		return TRUE;

	if( !m_pPt ) return FALSE;
	return m_pPt->IsType( pTypeName, pMemberName );
}


BOOL CTypeTable::_IsType( char *szTypeName, char *pMemberName )
{
	if( !szTypeName ) return FALSE;
	map< string, list<STypeData>* >::iterator itor = m_TypeTable.find( szTypeName );
	if( m_TypeTable.end() == itor ) return FALSE;
	list<STypeData> *plist = itor->second;
	if( !pMemberName ) return TRUE;
	list<STypeData>::iterator itor2 = find( plist->begin(), plist->end(), STypeData(pMemberName,NULL,0,DT_DATA) );
	if( plist->end() == itor2 ) return FALSE;
	return TRUE;
}


//-----------------------------------------------------------------------------//
// ��Ÿ�Գ��� ��������޸� �����°��� ����Ѵ�.
//-----------------------------------------------------------------------------//
void CTypeTable::CalcMemOffset()
{
	map< string, list<STypeData>* >::iterator i = m_TypeTable.begin();
	while( m_TypeTable.end() != i )
	{
		list<STypeData> *plist = i->second;
		list<STypeData>::iterator k = plist->begin();

		int memoffset = 0;
		while( plist->end() != k )
		{
			k->offset = memoffset;
			k->size = 1;			// �޸� ũ��� 4byte������ �Ϲ������� 1�� ������.
			// �Լ��� �޸�ũ�⿡ ������ ��ġ�� ������, 
			// Ŭ������ Ŭ������ ���� �������� �ʴ´�.
//			if( DT_FUNC != k->state && DT_CLASS != k->state )
			if( DT_FUNC != k->state )
				++memoffset;
			++k;
		}
		// list�� ù��°���� �޸�ũ�⸦ �����Ѵ�.(list�� ù��°���� ��ü�� ���Ѵ�.)
//		plist->front().offset = memoffset;
		plist->front().size = memoffset;

		++i;
	}
}


//-----------------------------------------------------------------------------//
// CalcMemOffset() �Լ� ȣ���Ŀ��� �� �Լ��� �����Ҽ� �ִ�.
// �ش� Ÿ���� �޸�ũ�⸦ �����Ѵ�. (DWORD����)
//-----------------------------------------------------------------------------//
int CTypeTable::GetTypeMemSize( char *pTypeName )
{
	if( !pTypeName ) return 0;
	if( !IsType(pTypeName) ) return 0;

	// �Լ��� ������ ũ�⸦ ���Ѵ�.
	list<STypeData> *plist = m_TypeTable[ pTypeName];
//	return plist->front().offset;
	return plist->front().size;
}


//-----------------------------------------------------------------------------//
// CalcMemOffset() �Լ� ȣ���Ŀ��� �� �Լ��� �����Ҽ� �ִ�.
// �ش� Ÿ���� �޸𸮿������� �����Ѵ�.
//-----------------------------------------------------------------------------//
int CTypeTable::GetTypeMemOffset( char *pTypeName, char *pMemberName ) // pMemberName=NULL
{
	if( !pTypeName ) return 0;
	if( !IsType(pTypeName) ) return 0;
	list<STypeData> *plist = m_TypeTable[ pTypeName];
	list<STypeData>::iterator itor = find( plist->begin(), plist->end(), STypeData(pMemberName,NULL,0,DT_DATA) );
	if( plist->end() == itor ) return 0;
	return itor->offset;
}


int CTypeTable::GetTypeId( char *pTypeName, char *pMemberName )
{
	if( !pTypeName ) return 0;
	if( !IsType(pTypeName) ) return 0;
	list<STypeData> *plist = m_TypeTable[ pTypeName];
	list<STypeData>::iterator itor = find( plist->begin(), plist->end(), STypeData(pMemberName,NULL,0,DT_DATA) );
	if( plist->end() == itor ) return 0;
	return itor->id;
}


list<STypeData>* CTypeTable::GetMemberList( char *pTypeName )
{
	if( !pTypeName ) return 0;
	if( !IsType(pTypeName) ) return 0;
	return m_TypeTable[ pTypeName];
}


///////////////////////////////////////////////////////////////////////////////////
// CSymTable

//-----------------------------------------------------------------------------//
// �ɺ� ����
// �ɺ� ������ Ÿ�԰� ����Ÿ�� �����Ǿ����� ���� �����̴�.
// import : ���ӳ��� ����Ÿ�� ��ȣ�ۿ��ϴ� �����̸� true
// id : import=true�϶� Ȱ��ȭ �Ǹ� ���ӳ�����Ÿ�� ���������� �ʿ���
//-----------------------------------------------------------------------------//
BOOL CSymTable::AddSymbol( char *pSymbolName, BOOL import, int id ) // import=FALSE, id=0
{
	if( IsSymbol(pSymbolName) ) return FALSE;
	if( strchr(pSymbolName,'.') ) return FALSE;	// Ŭ���� ��������� �ɺ��� �߰������ʴ´�.

	SSymData sd;
	strcpy( sd.name, pSymbolName );
	sd.type[ 0] = NULL;
//	sd.type = NULL;
	sd.offset = 0;
	sd.import = import;
	sd.id = id;
	sd.load = FALSE;

	m_SymTab.insert( map<string,SSymData>::value_type(pSymbolName,sd) );
	return TRUE;
}


// �ɺ��� �ش��ϴ� Ÿ�� ����
BOOL CSymTable::SetType( char *pSymbolName, char *pTypeName )
{
	SSymData *p = _FindData( pSymbolName );
	if( !p ) return FALSE;
	if( NULL != p->type[ 0] ) return FALSE; // Ÿ���� �̹� �����Ǿ��ٸ� �ٽ� �����Ҽ� ����.
//	p->type = pTypeName;
	if( pTypeName )	strcpy( p->type, pTypeName );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// ���������� �ɺ����̺� ������ ��´�.
// ���ٸ� �θ� �ɺ����̺��� �˻��ϰ� �̸� �ݺ��Ѵ�.
// pSymbolName : ������
// return value: �ش� ������ �ɺ������� �����Ѵ�.
//-----------------------------------------------------------------------------//
CSymTable::SSymData* CSymTable::_FindData( char *pSymbolName )
{
	map<string,SSymData>::iterator itor = m_SymTab.find( pSymbolName );
	if( m_SymTab.end() == itor )
	{
		// ���ٸ� �θ����̺��� �˻��Ѵ�.
		if( m_pPt )
			return m_pPt->_FindData( pSymbolName );	
	}
	else
	{
		return &itor->second;
	}	
	return NULL;
}


//-----------------------------------------------------------------------------//
// pSymbolName �� Ÿ���� ��� �Լ���.
// pSymbolName�� Ŭ���� ���� Ȥ�� �Լ�, Ȥ�� ���� ����, ���� �Լ� �ϼ� �ִ�.
// Ŭ���� �������,�Լ��� �������� ���ؼ��� pSymbolName���� '.'�� �ִ��� Ȯ���ؾ��ϰ�
// ���ٸ� �������� Ȥ�� �Լ��� �ȴ�.
// ���� '.'�� �ִٸ� ó�� token������ �����̸��� ���� SymbolTable�� ���ؼ�
// Ÿ���� ��´�. ���Ŀ��� ��������� Ÿ���� �����ؼ� ���� Ÿ���� ����.
// �������� Ŭ������ Ŭ������ �������� �����Ƿ� ����Լ��δ� �������� �ʾҴ�.
//-----------------------------------------------------------------------------//
char* CSymTable::GetType( char *pSymbolName )
{
	char *reval = NULL;
	char *pdot = strchr( pSymbolName, '.' );
	if( pdot )
	{
		// Ŭ���� ������� or ����Լ�
		char var[ 64];
		strncpy( var, pSymbolName, pdot-pSymbolName );
		var[ pdot-pSymbolName] = NULL;
		SSymData *p = _FindData( var );
		if( !p ) return NULL;
		reval = m_pTypeTab->GetType( p->type, pdot+1 );
	}
	else
	{
		// ��������
		SSymData *p = _FindData( pSymbolName );
		if( p )
		{
			if( NULL != p->type[ 0] )
				reval = p->type;
		}
	}
	return reval;
}


// SetData() �Լ��� �ɺ��� Type�� �����Ǿ� ������츸 ����� �۵��Ѵ�.
BOOL CSymTable::SetData( char *pSymbolName, void *data )
{
	SSymData *p = _FindData( pSymbolName );
	if( !p ) return FALSE;
	if( !p->type ) return FALSE;

	if( !strcmp(p->type,"int") )
	{
		p->data.num = *(int*)data;
	}
	else if( !strcmp(p->type,"float") )
	{
		p->data.fnum = *(float*)data;
	}
	else if( !strcmp(p->type,"string") )
	{
		p->data.name = (char*)data;
	}
	else // class
	{
		p->data._class = data;
	}	
	return TRUE;
}

int CSymTable::GetIntData( char *pSymbolName )
{
	SSymData *p = _FindData( pSymbolName );
	if( !p ) return 0;
	if( !p->type ) return 0;
	return p->data.num;
}

float CSymTable::GetFloatData( char *pSymbolName )
{
	SSymData *p = _FindData( pSymbolName );
	if( !p ) return 0.f;
	if( !p->type ) return 0.f;
	return p->data.fnum;
}

char* CSymTable::GetStrData( char *pSymbolName )
{
	SSymData *p = _FindData( pSymbolName );
	if( !p ) return NULL;
	if( !p->type ) return NULL;
	return p->data.name;
}

void* CSymTable::GetClassData( char *pSymbolName )
{
	SSymData *p = _FindData( pSymbolName );
	if( !p ) return NULL;
	if( !p->type ) return NULL;
	return p->data._class;
}


//-----------------------------------------------------------------------------//
// GetType() �Լ��� ����� ������ ������.
//-----------------------------------------------------------------------------//
BOOL CSymTable::IsSymbol( char *pSymbolName )
{
	BOOL reval = FALSE;

	char *pdot = strchr( pSymbolName, '.' );
	if( pdot )
	{
		// Ŭ���� ������� or ����Լ�
		char var[ 64];
		strncpy( var, pSymbolName, pdot-pSymbolName );
		var[ pdot-pSymbolName] = NULL;
		SSymData *p = _FindData( var );
		if( !p ) return FALSE;
		reval = m_pTypeTab->IsType( p->type, pdot+1 );
	}
	else
	{
		// ��������
		reval = _IsSymbolRec( pSymbolName );
	}

	return reval;
}


//-----------------------------------------------------------------------------//
// ����Ʈ�� Ŭ������� TRUE�� �����Ѵ�.
// ���� ��� Ŭ������ ����Ʈ Ŭ������.
//-----------------------------------------------------------------------------//
BOOL CSymTable::IsImportClass( char *pSymbolName )
{
	SSymData *p = NULL;

	char *pdot = strchr( pSymbolName, '.' );
	if( pdot )
	{
		// Ŭ���� ����
		char var[ 64];
		strncpy( var, pSymbolName, pdot-pSymbolName );
		var[ pdot-pSymbolName] = NULL;
		p = _FindData( var );
	}
	else
	{
		p = _FindData( pSymbolName );
	}
	if( !p ) return FALSE;

	// ����ƮŸ���� �ƴ϶�� Ŭ���� Ÿ���̴�.
	for( int i=0; i < g_DefautTypeSize; ++i )
	{
		if( !strcmp(g_DefautType[ i], p->type) )
			break;
	}

	return (i >= g_DefautTypeSize);
}


//-----------------------------------------------------------------------------//
// import�� ������ ���ø����̼����� ���� �����͸� �Ҵ� �޾Ҵٸ� load������ 
// TRUE�� �����Ѵ�.
//-----------------------------------------------------------------------------//
void CSymTable::Loaded( char *pSymbolName )
{
	// Ŭ���� ����������� ������������ �����Ѵ�.
	char *pdot = strchr( pSymbolName, '.' );
	if( pdot )
	{
		// Ŭ���� �������
		char var[ 64];
		strncpy( var, pSymbolName, pdot-pSymbolName );
		var[ pdot-pSymbolName] = NULL;
		SSymData *p = _FindData( var );
		if( p ) 
			p->load = TRUE;
	}
	else
	{
		SSymData *p = _FindData( pSymbolName );
		if( p )
		{
			if( p->import ) 
				p->load = TRUE;
		}
	}
}


//-----------------------------------------------------------------------------//
// ���� map�� �ɺ��� �ִ��� �˻��Ѵ�. ���� Ŭ�������� ���ٸ�
// �θ�Ŭ�������� �˻��Ѵ� 
//-----------------------------------------------------------------------------//
BOOL CSymTable::_IsSymbolRec( char *szSymbolName )
{
	if( _IsSymbol(szSymbolName) )
		return TRUE;
	if( !m_pPt ) return FALSE;
	return m_pPt->IsSymbol( szSymbolName );
}

BOOL CSymTable::_IsSymbol( char *szSymbolName )
{
	map<string,SSymData>::iterator itor = m_SymTab.find( szSymbolName );
	if( m_SymTab.end() == itor )
		return FALSE;
	return TRUE;
}


//-----------------------------------------------------------------------------//
// mempos ���� ����Ѵ�.
//-----------------------------------------------------------------------------//
void CSymTable::CalcMemOffset()
{
	if( !m_pTypeTab ) return;
	int mem = 0;
	map<string,SSymData>::iterator itor = m_SymTab.begin();
	while( m_SymTab.end() != itor )
	{
		SSymData dat = itor->second;
		if( !dat.type )
		{
			++itor;
			continue;
		}
		itor->second.offset = mem;
		mem += m_pTypeTab->GetTypeMemSize( dat.type );

		++itor;
	}
	m_nTotalMemSize = mem;
}


//-----------------------------------------------------------------------------//
// CalcMemOffset() �Լ��� ȣ����Ŀ��� �� �Լ��� �����Ѵ�.
// mempos���� ������ ����Ǵ� �޸� offset���̴�. (DWORD����)
// mempos���� ���û� ������ ����ɶ� ����� ��ġ�� ����Ű�� DWORD�����̴�. 
// ������ �Ʒ��� �ڶ���, ���� �ڶ����� ���� ����ȴ�. ���� ������ �Ʒ��� 
// �ڶ󳪴°����� �Ѵ�. (stack�� �����ּҿ��� �����ּҷ� push���� ���Ѵ�.) 
//-----------------------------------------------------------------------------//
int CSymTable::GetMemOffset( char *pSymbolName ) 
{
	int reval = 0;

	char *pdot = strchr( pSymbolName, '.' );
	if( pdot )
	{
		// Ŭ���� �������
		char var[ 64];
		strncpy( var, pSymbolName, pdot-pSymbolName );
		var[ pdot-pSymbolName] = NULL;
		SSymData *p = _FindData( var );
		if( !p ) return 0;

		int offset = m_pTypeTab->GetTypeMemOffset( p->type, pdot+1 );
		reval = p->offset + offset;
	}
	else
	{
		// ��������
		SSymData *p = _FindData( pSymbolName );
		reval = p->offset;
	}

	return reval;
}


//-----------------------------------------------------------------------------//
// pSymbolName �������� Ŭ�������� �����Ҷ� (ex "enemy.state")
// Ŭ������ �������� �����Ѵ�.
//-----------------------------------------------------------------------------//
int CSymTable::GetClassMemOffset( char *pSymbolName )
{
	int reval = 0;
	char *pdot = strchr( pSymbolName, '.' );
	if( pdot )
	{
		// Ŭ���� �������
		char var[ 64];
		strncpy( var, pSymbolName, pdot-pSymbolName );
		var[ pdot-pSymbolName] = NULL;
		SSymData *p = _FindData( var );
		if( !p ) return 0;
		reval = p->offset;
	}
	else
	{
		// ��������
		SSymData *p = _FindData( pSymbolName );
		reval = p->offset;
	}
	return reval;
}


//-----------------------------------------------------------------------------//
// �ɺ��� import�� �����̰ų�, Ŭ�����ϰ�� Ÿ���� ID�� �����Ѵ�. 
//-----------------------------------------------------------------------------//
int CSymTable::GetSymbolId( char *pSymbolName )
{
	int reval = 0;

	// Ŭ���� ����������� ������������ �����Ѵ�.
	char *pdot = strchr( pSymbolName, '.' );
	if( pdot )
	{
		// Ŭ���� �������
		char var[ 64];
		strncpy( var, pSymbolName, pdot-pSymbolName );
		var[ pdot-pSymbolName] = NULL;
		SSymData *p = _FindData( var );
		if( !p ) return 0;
		reval = m_pTypeTab->GetTypeId( p->type, pdot+1 );
	}
	else
	{
		SSymData *p = _FindData( pSymbolName );
		if( !p )
		{
			// ���� �Լ��� ���
			reval = m_pTypeTab->GetTypeId( pSymbolName );
		}
		else
		{
			if( p->import ) 
				reval = p->id;
		}
	}
	return reval;
}


//-----------------------------------------------------------------------------//
// �ɺ��� import�� �����̰ų�, Ŭ�����ϰ�� ���ø����̼����� ���� �����͸� �ε�
// �Ǿ����� ���θ� �����Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CSymTable::IsLoaded( char *pSymbolName )
{
	BOOL reval = FALSE;

	// Ŭ���� ����������� ������������ �����Ѵ�.
	char *pdot = strchr( pSymbolName, '.' );
	if( pdot )
	{
		// Ŭ���� �������
		char var[ 64];
		strncpy( var, pSymbolName, pdot-pSymbolName );
		var[ pdot-pSymbolName] = NULL;
		SSymData *p = _FindData( var );
		if( !p ) return 0;
		reval = p->load;
	}
	else
	{
		SSymData *p = _FindData( pSymbolName );
		if( p )
		{
			if( p->import ) 
				reval = p->load;
		}
	}
	return reval;
}


//-----------------------------------------------------------------------------//
// ����Ʈ�� �������� (Ŭ������ �ƴ�) �� ����Ʈ�� �����ؼ� �����Ѵ�.
// plst : ������ ����Ǿ ���ϵȴ�.
//-----------------------------------------------------------------------------//
void CSymTable::GetGlobalImportData( list< string > *plst )
{
	map<string,SSymData>::iterator itor = m_SymTab.begin();
	while( m_SymTab.end() != itor )
	{
		SSymData dat = itor->second;
		if( dat.import && dat.id )
			plst->push_back( dat.name );
		++itor;
	}
}


///////////////////////////////////////////////////////////////////////////////////
// CStringTable


void CStringTable::AddString( char *str )
{
	if( !str ) return;

	m_StrList.push_back( SStrInfo(str,m_nMaxStrSize) );
	int size = strlen( str ) + 1; // NULL���ڱ��� ����
	// 4byte ������ �ٲ۴�.
	size = (size + 3) & ~3;
	m_nMaxStrSize += size / 4;
}


int CStringTable::GetOffset( char *str )
{
	list<SStrInfo>::iterator itor = find( m_StrList.begin(), m_StrList.end(), SStrInfo(str,0) );
	if( m_StrList.end() == itor ) return 0;
	return itor->offset;
}


//-----------------------------------------------------------------------------//
// StringTable�� �����Ѵ�.
//-----------------------------------------------------------------------------//
void CStringTable::WriteStringTable( FILE *fp )
{
	// �� ��Ʈ�����̺� ũ�⸦ �������� ���̺��� �����Ѵ�.
	fwrite( &m_nMaxStrSize, 1, sizeof(int), fp );

	int c = NULL;
	list<SStrInfo>::iterator i = m_StrList.begin();
	while( m_StrList.end() != i )
	{
		SStrInfo info = *i++;
		fwrite( info.str, sizeof(char), strlen(info.str)+1, fp );
//		fputc( '\0', fp );
		// string�� ������ 4byte������ ������ ������ �������� �����Ѵ�.
		int len = (strlen( info.str ) + 1);
		int dword = (len + 3) & ~3;
		for( int k=0; k < dword-len; ++k )
			fwrite( &c, sizeof(char), 1, fp );
//			fputc( '\0', fp );
	}
}



///////////////////////////////////////////////////////////////////////////////////
// Global Function

ns_script::SParseTree* ns_script::NewStmt( Kind k, ns_script::CScanner *pS )
{
	SParseTree *t = new SParseTree;
	t->nodekind = Stmt;
	t->kind = k;
	t->lineno = pS->GetLineNo();
	t->attr.name = 0;
	t->type = NULL;
	ZeroMemory( t->child, sizeof(t->child) );
	t->sibling = NULL;

	return t;
}


ns_script::SParseTree* ns_script::NewExp( Kind k, ns_script::CScanner *pS )
{
	SParseTree *t = new SParseTree;
	t->nodekind = Exp;
	t->kind = k;
	t->lineno = pS->GetLineNo();
	t->attr.name = 0;
	t->type = NULL;
	ZeroMemory( t->child, sizeof(t->child) );
	t->sibling = NULL;

	return t;
}


void ns_script::PrintToken( ns_script::Tokentype token, char *szTokenString )
{
	switch( token )
	{
	case ID:
	case STRING:	g_Dbg.Console( "string = %s\n", szTokenString ); break;

	case ASSIGN:	g_Dbg.Console( "=" ); break;
	case TIMES:		g_Dbg.Console( "*" ); break;
	case LPAREN:	g_Dbg.Console( "(" ); break;
	case RPAREN:	g_Dbg.Console( ")" ); break;
	case LBRACE:	g_Dbg.Console( "{" ); break;
	case RBRACE:	g_Dbg.Console( "}" ); break;
	case COMMA:		g_Dbg.Console( "," ); break;
	case NUM:		g_Dbg.Console( "number" ); break;

	default: 		g_Dbg.Console( "UnKnown token %d, %s\n", token, szTokenString ); break;
	}
}


//-----------------------------------------------------------------------------//
// ParseTree ����
//-----------------------------------------------------------------------------//
void ns_script::DeleteParseTree( SParseTree *pTree )
{
	if( !pTree ) return;
	for( int i=0; i < MAXCHILD; ++i )
		DeleteParseTree( pTree->child[ i] );
	DeleteParseTree( pTree->sibling );
	
	switch( pTree->kind )
	{
	case ConstStrK:
	case IdK:
	case VarNameK:
	case FuncK:
	case ParamK:
	case TypeK:
		delete[] pTree->attr.name;
		break;
	}
	delete pTree;
}

