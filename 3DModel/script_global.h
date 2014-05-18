//-----------------------------------------------------------------------------//
// 2006-08-15 programer: jaejung ��(���բ�)��
// 
// 2008-01-01 ������ ���� �����Ϸ� �ڵ带 �����ؼ� �������
//-----------------------------------------------------------------------------//

#if !defined(__SCRIPT_GLOBAL_H__)
#define __SCRIPT_GLOBAL_H__

#pragma warning (disable: 4786)
#include <list>
#include <map>
#include <deque>
#include <string>
#include <stack>

namespace ns_script
{
	enum { MAXCHILD=5, MAX_TABLE=16, MAX_MULTICOMPILE=64 };

	enum Tokentype
	{
		_ERROR, ENDFILE, ID, NUM, FNUM, STRING, ASSIGN, LPAREN, RPAREN, LBRACE, RBRACE, COMMA, COLON,
		PLUS, MINUS, TIMES, DIV, REMAINDER, 
		LT/* < */, RT/* > */, NEQ/* != */, EQ/* == */, OR/* || */, AND/* && */,
	
		INCLUDE, EVENT, IF, ELSE, WHILE, CLASS, FUNCTION, IMPORT, DEFINE, SCRWAIT, SCREND, SCREXIT
	};

	enum NodeKind { Stmt, Exp };
	enum Kind
	{
		ProgramK, ClassK, VarDecK, VarNameK, FuncDecK, AssignK, IfK, WhileK, EventK, ComposeK, IncludeK, 
		ScriptWaitK, ScriptEndK, ScriptExitK, // statement
		TypeK, ConditionOpK, OpK, IdK, FuncK, ParamK, ConstIntK, ConstFloatK, ConstStrK, CallK, // exp
	};


	class CScanner;

	// Type Table
	enum DataType
	{ DT_DATA, DT_CLASS, DT_FUNC };

	// TypeData
	typedef struct _tagSTypeData
	{
		char name[ 64];		// �����̸�
		char type[ 64];		// Ÿ���̸�	(NULL�϶��� Ÿ���� �������� �ʾ�������)
		int id;			// ���̵� (���ӳ� ����Ÿ�� ������ ���ؼ� �ʿ��Ѱ� unique)
		int offset;		// �޸𸮿����� (���û� ����� ��ġ)
		int size;		// �޸� ������
		DataType state;	// ������(int,float,string)���� Ȥ�� Ŭ����, �Լ������� ��Ÿ����.
		int argcnt;		// state=DT_FUNC �ΰ�� ���ڰ����� ��Ÿ����. (�� ��ũ���ʹ� �Լ��� ������ �˻��Ѵ�.)

		_tagSTypeData() {}
		_tagSTypeData( char *n, char *t, int i, DataType d, int cnt=0 ):
			id(i), state(d), argcnt(cnt) 
			{  name[ 0] = NULL, type[ 0] = NULL;
				if( n ) strcpy_s( name, n );
				if( t ) strcpy_s(type,t); 
			}
		BOOL operator == ( const _tagSTypeData& rhs )
			{ if(!name[0] && !rhs.name[0] ) return TRUE; if(!name[0]||!rhs.name[0]) return FALSE; return !strcmp(name,rhs.name); }
	} STypeData;


	// TypeTable
	// ����Ÿ��, ����Ÿ���� �������� �ʾҴ�.
	// ���� TypeTable�� �ϳ��� �����ϴ°ɷ� �����Ѵ�.
	class CTypeTable
	{
	public:
		CTypeTable( CTypeTable *parent );
		virtual ~CTypeTable();
	protected:
		CTypeTable *m_pPt;	// parent type table
		std::map< std::string, std::list<STypeData>* > m_TypeTable;
	public:
		BOOL NewType( STypeData Type );
		BOOL AddType( char *pTypeName, STypeData Member );
		char* GetType( char *pTypeName, char *pMemberName=NULL );
		BOOL IsType( char *pTypeName, char *pMemberName=NULL );
		void CalcMemOffset();
		int GetTypeMemSize( char *pTypeName );
		int GetTypeMemOffset( char *pTypeName, char *pMemberName=NULL );
		int GetTypeId( char *pTypeName, char *pMemberName=NULL );
		std::list<STypeData>* GetMemberList( char *pTypeName );
	protected:
		BOOL _IsType( char *pTypeName, char *pMemberName ); // ���� table������ �˻�
	};

	// Symbol Table
	// �����̸�, ����Ÿ, ���ι�ȣ�� �����Ѵ�.
	// ��������,���������� �����Ҽ� �ִ� ����� �����Ǿ��ִ�.
	class CSymTable
	{
	public:
		CSymTable( CTypeTable *typetab, CSymTable *parent ) { m_nTotalMemSize=0; m_pTypeTab=typetab; m_pPt = parent; }
		virtual ~CSymTable() {}
	protected:
		typedef struct _tagSymData
		{
			BOOL import;		// ���ӳ� ����Ÿ���� import�� �����̸� true
			BOOL load;			// ���ø����̼����� ���� �����͸� �ε��ߴٸ� TRUE
			char type[ 64];		// ����Ÿ��
			char name[ 64];		// �����̸�
			int offset;		// �޸𸮿����� (���û� ����� ��ġ)
			int id;			// ���̵� (���ӳ� ����Ÿ�� ������ ���ؼ� �ʿ��Ѱ� unique)
			union
			{
				int num;
				float fnum;
				char *name;
				void *_class;
			} data;
		} SSymData;

		CTypeTable *m_pTypeTab;	// current type table
		CSymTable *m_pPt;		// parent symbol table
		std::map<std::string,SSymData> m_SymTab;
		int m_nTotalMemSize;

	public:
		BOOL AddSymbol( char *pSymbolName, BOOL import=FALSE, int id=0 );
		BOOL SetType( char *pSymbolName, char *pTypeName );
		char* GetType( char *pSymbolName );
		BOOL IsSymbol( char *pSymbolName );
		BOOL IsImportClass( char *pSymbolName );
		void Loaded( char *pSymbolName );

		void CalcMemOffset();
		int GetMemOffset( char *pSymbolName );
		int GetClassMemOffset( char *pSymbolName );
		int GetSymbolId( char *pSymbolName );
		BOOL IsLoaded( char *pSymbolName );
		int GetTotalMemSize() { return m_nTotalMemSize; }

		// data ����
		BOOL SetData( char *pSymbolName, void *data );
		int GetIntData( char *pSymbolName );
		float GetFloatData( char *pSymbolName );
		char* GetStrData( char *pSymbolName );
		void* GetClassData( char *pSymbolName );
		void GetGlobalImportData( std::list< std::string > *plst );

	protected:
		BOOL _IsSymbolRec( char *szSymbolName );
		BOOL _IsSymbol( char *szSymbolName ); // ���� table������ �˻�
		SSymData* _FindData( char *pSymbolName );
		char* _GetType( char *pTypeName, char *pMemberName );

	};



	// String Table
	// ��ũ��Ʈ���� ���Ǵ� ConstString�� ��������Ѵ�.
	class CStringTable
	{
	public:
		CStringTable() : m_nMaxStrSize(0) {}
		virtual ~CStringTable() {}

	protected:
		typedef struct _tagStrInfo
		{
			char str[ 128];
			int offset;	// StringTable���� string�� ��ġ (4byte����)

			_tagStrInfo() {}
			_tagStrInfo( char *c, int o ):offset(o) { strcpy_s(str,c); }
			BOOL operator== ( const _tagStrInfo& rhs )
				{ if( !str ) return FALSE; return !strcmp(str,rhs.str); }
		} SStrInfo;

		int m_nMaxStrSize;	// 4����Ʈ����
		std::list< SStrInfo > m_StrList;

	public:
		void AddString( char *str );
		int GetOffset( char *str );
		void WriteStringTable( FILE *fp );

	protected:

	};


	// import���� ���� �Լ�
	enum SystemFunc
	{
		// system �Լ�
		LoadVariable = 1,
		StoreVariable,
		LoadPointer,
		// global �Լ�
		DbgPrint = 1000,
		DbgPrintStr,		
		ExeScript,
		_GetArgument,
	};

	// ParseTree ����
	typedef struct _tagParseTree
	{
		NodeKind nodekind;
		Kind kind;
		union
		{
			int num;
			float fnum;
			char *name;
			void *_class;
			Tokentype op;
		} attr;

		int lineno;
		char *type;
		CSymTable *pSym;
		_tagParseTree *child[ MAXCHILD];
		_tagParseTree *sibling;

	} SParseTree;


	// PreCompiler
	// ��ó�� �۾��� �̸�, define���� ���ǵ� ��Ʈ���� ���������� �����Ѵ�.
	// CParser�� ���� ��ū�� ��¡���� �� Ŭ������ ����ȴ�.
	class CPreCompiler
	{
	public:
		CPreCompiler() {}
		virtual ~CPreCompiler() {}

	protected:
		typedef struct _tagSTokDat
		{
			std::string str;
			Tokentype tok;
		} STokDat;
		std::map< std::string, STokDat > m_DefMap;
	public:
		void PreCompile( SParseTree *pTree );
		BOOL InsertDefine( char *pID, Tokentype type, char *pStr );
		BOOL FindDefine( char *pID, Tokentype *pType, std::string *pStr );
	};


	extern std::stack<std::string> g_Include;

	// Parse Node �����Լ�
	SParseTree* NewStmt( Kind k, CScanner *pS );
	SParseTree* NewExp( Kind k, CScanner *pS );
	void PrintToken( Tokentype token, char *szTokenString );
	void DeleteParseTree( SParseTree *pTree );

}

#endif // __SCRIPT_GLOBAL_H__
