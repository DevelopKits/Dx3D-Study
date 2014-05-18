
#include "global.h"
#include "script_global.h"
#include "script_parser.h"
#include "script_analyze.h"
#include "script_codegen.h"
#include "script_machine.h"
#include "script.h"

using namespace std;
using namespace ns_script;

CScript::CScript()
{
	m_pMachine = new CMachine;
	m_IncTime = 0;

}
CScript::~CScript() 
{
	if( m_pMachine ) delete m_pMachine;
}


//-----------------------------------------------------------------------------//
// ��ũ��Ʈ�� �������ϰ� �����Ѵ�.
// pFileName : ���� �� ��ũ��Ʈ ���ϸ�
// callback : �ݹ��Լ�
// pArgument : �������ڰ� (��Ʈ��)
//-----------------------------------------------------------------------------//
BOOL CScript::Execute( char *pFileName, void callback (int,ns_script::CProcessor*), char *pArgument ) // pArgument=NULL
{
	// TypeTable ����
	// TypeTable�� ���� �ϳ��� �����Ѵ�.
	CTypeTable *ptype = new CTypeTable( NULL );
	CPreCompiler *ppre = new CPreCompiler();

	BOOL result = Compile( pFileName, callback, ptype, ppre );
	delete ptype;
	delete ppre;
	if( !result ) return FALSE;

	// Simulation
	g_Dbg.Console( "Execute...\n" );
	g_Dbg.Console( "%s\n", pFileName );
	m_pMachine->ExecuteScript( pFileName, callback, pArgument );

	return TRUE;
}


//-----------------------------------------------------------------------------//
// �ҽ������� ������ �ϰ� ���ӿ��� ���� �����ڵ带 �����Ѵ�.
// ������ �����̸��� �ҽ����ϰ� ���� Ȯ���ڴ� .gm �̴�. (GameMachine ����)
// szFileName : �����ϵ� �ҽ����� �̸�
//-----------------------------------------------------------------------------//
BOOL CScript::Compile( char *pFileName, void callback (int,ns_script::CProcessor*), 
					  ns_script::CTypeTable *pType, ns_script::CPreCompiler *pPreC ) // pType=NULL, pPreC=NULL
{
	g_Dbg.Console( "Compiling...\n" );
	g_Dbg.Console( "%s\n", pFileName );

	// Parse
	CParser parser;
	ns_script::SParseTree *pt = parser.Parse( pFileName, pPreC );
	if( parser.IsError() ) return FALSE;
	if( !pt ) return FALSE;

	// include�� ������ compile�Ѵ�.
	while( !g_Include.empty() )
	{
		string filename = g_Include.top();
						  g_Include.pop();
		Compile( (char*)filename.c_str(), callback, pType, pPreC );
	}

	// PreCompiler
	pPreC->PreCompile( pt );

	CSymTable *psym = new CSymTable( pType, NULL );

	// Analyze
	CAnalyze analyze;
	CStringTable strtable;
	pt->pSym = psym;
	analyze.Build( pFileName, pt, psym, pType, &strtable );
	if( analyze.IsError() ) return FALSE;

	pType->CalcMemOffset();
	psym->CalcMemOffset();

	// Code Generate
	CCodeGen cgen;
	cgen.CodeGenerate( pt, pType, &strtable, pFileName );

	DeleteParseTree( pt );
	delete psym;

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Instruction ����
// nDelta : �����Ӱ� ����ð� (millisecond ����)
//-----------------------------------------------------------------------------//
int CScript::Run( int nDelta )
{
	m_IncTime += nDelta;
	if( m_IncTime < 33 ) return 0;	// 30 frame

	m_IncTime = 0;
	return m_pMachine->Run();
}
