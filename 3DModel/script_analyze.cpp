
#include "global.h"
#include "script_global.h"
#include "script_analyze.h"

using namespace std;
using namespace ns_script;


//-----------------------------------------------------------------------------//
// ParseTree �м�
// pFileName : �ҽ� ���ϸ� (������� ������ �ʿ���)
//-----------------------------------------------------------------------------//
BOOL CAnalyze::Build( char *pFileName, SParseTree *pParseTree, CSymTable *pSymTab, 
					  CTypeTable *pTypeTab, CStringTable *pStrTab )
{
	m_pStrTab = pStrTab;
	strcpy( m_FileName, pFileName );

	BuildSymTable( NULL, pParseTree, pSymTab, pTypeTab );
	TypeCheck( NULL, pParseTree, pSymTab, pTypeTab );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// ParseTree�� ��ȸ�ϸ鼭 �ɺ����̺�, Ÿ�����̺��� �����Ѵ�
// pParent : ����Ʈ���� �θ�Ʈ���� ���Ѵ�. Root�ϰ�� NULL�� �ȴ�.
// pTree : ���� �м��� Ʈ���� ���Ѵ�.
// ������ ����Ÿ�� �Ҵ�ɶ� Ÿ���� �������� ������ Build�ÿ��� ����ó���� ���Ǿ���.
//-----------------------------------------------------------------------------//
BOOL CAnalyze::BuildSymTable( SParseTree *pParent, SParseTree *pTree, CSymTable *pSymTab, CTypeTable *pTypeTab )
{
	if( !pTree ) return FALSE;
	pTree->pSym = pSymTab;

	switch( pTree->nodekind )
	{
	case Stmt:
		{
			switch( pTree->kind )
			{
			case ProgramK:
				BuildSymTable( pTree, pTree->child[ 0], pSymTab, pTypeTab );
				BuildSymTable( pTree, pTree->child[ 1], pSymTab, pTypeTab );
				BuildSymTable( pTree, pTree->child[ 2], pSymTab, pTypeTab );
				BuildSymTable( pTree, pTree->child[ 3], pSymTab, pTypeTab );
				BuildSymTable( pTree, pTree->child[ 4], pSymTab, pTypeTab );
				break;

			case IncludeK:
				break;

			// Ŭ��������� ���� TypeTable�� �߰��ȴ�.
			case ClassK:
				pTypeTab->NewType( STypeData(pTree->attr.name,pTree->attr.name,0,DT_CLASS) );
				BuildSymTable( pTree, pTree->child[ 0], pSymTab, pTypeTab );
				BuildSymTable( pTree, pTree->child[ 1], pSymTab, pTypeTab );
				break;
			// Ŭ���� ����� Ȥ�� import�� ���������.
			case VarDecK:
				{
					BOOL _class = (ClassK==pParent->kind);

					SParseTree *p1 = pTree->child[ 0];
					SParseTree *p2 = pTree->child[ 1];
					SParseTree *p3 = pTree->child[ 2];

					if( _class )	// Ŭ���� �������
					{
						if( !pTypeTab->AddType(pParent->attr.name, STypeData(p2->attr.name,
											   p1->attr.name,p3->attr.fnum,DT_DATA)) )
							BuildError( pTree, "Ŭ������ ��������� ���谡 �߸��Ǿ����ϴ�.\n", 1 );
					}
					else // import ����
					{
						pSymTab->AddSymbol( p2->attr.name, TRUE, p3->attr.fnum );
						pSymTab->SetType( p2->attr.name, p1->attr.name );
					}

					BuildSymTable( pParent, pTree->child[ 0], pSymTab, pTypeTab );
					BuildSymTable( pParent, pTree->child[ 1], pSymTab, pTypeTab );
				}
				break;
			// Ŭ���� ����� Ȥ�� import�� ���������.
			case FuncDecK:
				{
					SParseTree *p1 = pTree->child[ 0];
					SParseTree *p2 = pTree->child[ 1];
					SParseTree *p3 = pTree->child[ 3];

					// �Լ��� ���ڰ����� ��´�.
					int argcnt = GetFunctionArgumentCount( pTree );
					if( !pTypeTab->AddType(pParent->attr.name, STypeData(p2->attr.name, 
										   p1->attr.name,p3->attr.fnum,DT_FUNC,argcnt)) )
					{
						BuildError( pTree, "Ŭ������ ����Լ��� ���谡 �߸��Ǿ����ϴ�.\n", 1 );
					}
					BuildSymTable( pTree, pTree->child[ 0], pSymTab, pTypeTab );
					BuildSymTable( pTree, pTree->child[ 1], pSymTab, pTypeTab );
					BuildSymTable( pTree, pTree->child[ 2], pSymTab, pTypeTab );
					BuildSymTable( pTree, pTree->child[ 3], pSymTab, pTypeTab );
				}
				break;

			// ����,�������� ������ ����.
			case ComposeK:
				BuildSymTable( pTree, pTree->child[ 0], pSymTab, pTypeTab );
				break;

			// ��ũ��Ʈ�� �Ҵ�� Ÿ���� �����ǰ�, �ɺ����̺� �߰��ȴ�.
			case AssignK:
				pSymTab->AddSymbol( pTree->child[ 0]->attr.name );
				BuildSymTable( pParent, pTree->child[ 0], pSymTab, pTypeTab );
				BuildSymTable( pParent, pTree->child[ 1], pSymTab, pTypeTab );
				break;

			// If, While, Event�� ParentNode�� ComposeK�� ����Ű�� �ؾ��Ѵ�.
			case IfK:
				BuildSymTable( pParent, pTree->child[ 0], pSymTab, pTypeTab );
				BuildSymTable( pParent, pTree->child[ 1], pSymTab, pTypeTab );
				BuildSymTable( pParent, pTree->child[ 2], pSymTab, pTypeTab );
				break;

			case WhileK:
				BuildSymTable( pParent, pTree->child[ 0], pSymTab, pTypeTab );
				BuildSymTable( pParent, pTree->child[ 1], pSymTab, pTypeTab );
				break;

			case EventK:
				BuildSymTable( pParent, pTree->child[ 0], pSymTab, pTypeTab );
				BuildSymTable( pParent, pTree->child[ 1], pSymTab, pTypeTab );
				break;

			case ScriptWaitK:
			case ScriptEndK:
			case ScriptExitK:
				break;
			}
		}
		break;

	case Exp:
		{
			switch( pTree->kind )
			{
			case CallK:
				BuildSymTable( pTree, pTree->child[ 0], pSymTab, pTypeTab );
				BuildSymTable( pTree, pTree->child[ 1], pSymTab, pTypeTab );
				break;

			case OpK:
			case ConditionOpK:
				BuildSymTable( pTree, pTree->child[ 0], pSymTab, pTypeTab );
				BuildSymTable( pTree, pTree->child[ 1], pSymTab, pTypeTab );
				break;

			case ConstStrK:
				m_pStrTab->AddString( pTree->attr.name );
				break;

			case TypeK:
			case IdK:
			case FuncK:
			case ParamK:
			case ConstIntK:
			case ConstFloatK:
				break;
			}
		}
		break;
	}

	BuildSymTable( pParent, pTree->sibling, pSymTab, pTypeTab );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// ���� �Ҵ�, �Լ����� Ÿ���� üũ�Ѵ�.
//-----------------------------------------------------------------------------//
BOOL CAnalyze::TypeCheck( SParseTree *pParent, SParseTree *pTree, CSymTable *pSymTab, CTypeTable *pTypeTab )
{
	if( !pTree ) return TRUE;
	for( int i=0; i < MAXCHILD; ++i )
	{
		if( pTree->child[ i] )
			TypeCheck( pTree, pTree->child[ i], pTree->child[ i]->pSym, pTypeTab );
	}

	switch( pTree->nodekind )
	{
	case Stmt:
		{
			switch( pTree->kind )
			{
			case ProgramK:
			case ClassK:
			case VarDecK:
//			case VarK:
			case FuncDecK:
			case IncludeK:
			case ComposeK:
				break;

			// ��ũ��Ʈ�� �Ҵ�� Ÿ���� �����ǰ�, �ɺ����̺� �߰��ȴ�.
			case AssignK:
				{
					// ������ ���𸸵ǰ� ���� �Ҵ����� �ʾҴٸ� �Ѿ
					if( !pTree->child[ 1] )
						break;

					char *type = pSymTab->GetType( pTree->child[ 0]->attr.name );
					// ������ ó�� ���� �Ҵ��ϸ� Ÿ���� �����Ѵ�.
					if( !type )
					{
						pSymTab->SetType( pTree->child[ 0]->attr.name, pTree->child[ 1]->type );
					}
					else
					{
						// Ÿ���� ������ ���¶�� ������ Ÿ���� ���Ѵ�.
						if( strcmp(type, pTree->child[ 1]->type) )
						{
							// float -> int
							// int -> float �� ����Ѵ�.
							if( !strcmp(type, "float") || !strcmp(type, "int") &&
								!strcmp(pTree->child[ 1]->type, "float") || !strcmp(pTree->child[ 1]->type, "int") )
								break;

							TypeError( pTree, "�ٸ� Ÿ���� ���� �Ҵ��Ͽ����ϴ�.\n" );
						}
					}
				}
				break;

			case IfK:
			case WhileK:
			case EventK:
				if( !pTree->child[ 0]->type || strcmp("bool", pTree->child[ 0]->type) )
					TypeError( pTree, "���ǹ����� and, or ���길 �����մϴ�.\n" );
				break;
			}
		}
		break;

	case Exp:
		{
			switch( pTree->kind )
			{
			case CallK:
				pTree->type = GetSymbolType( pTree->child[ 0]->attr.name, pSymTab, pTypeTab, pTree );
				if( !pTree->type )
					TypeError( pTree->child[ 0], "�������� �ʴ� �Լ��Դϴ�.\n", 1 );
				break;

			case TypeK:
				if( IsSymbol(pTree->attr.name, pSymTab, pTypeTab, pTree) )
				{
					pTree->type = pTree->attr.name;
				}
				else
				{
					TypeError( pTree, "�������� �ʴ� Type�Դϴ�.\n", 1 );
					pTree->type = "void"; // ����Ʈ void
				}
				break;

			case IdK:
				{
					if( IsSymbol(pTree->attr.name, pSymTab, pTypeTab, pTree) )
					{
						pTree->type = GetSymbolType( pTree->attr.name, pSymTab, pTypeTab, pTree );
					}
					else
					{
						TypeError( pTree, "�������� �ʴ� ID�Դϴ�.\n", 1 );
						pTree->type = "void"; // ����Ʈ void
					}
				}
				break;

			case OpK:
				{
					// ��������� int, float���� �����ϴ�.

					pTree->type = "void"; // default
					if( strcmp("int",pTree->child[ 0]->type) && strcmp("float",pTree->child[ 0]->type) )
						TypeError( pTree, "�߸��� ������ �Ͽ����ϴ�.\n" );
					else if( strcmp(pTree->child[ 0]->type, pTree->child[ 1]->type) )
						TypeError( pTree, "�������� Ÿ���� �����Ͽ����ϴ�.\n" );
					else if( EQ == pTree->attr.op || LT == pTree->attr.op || RT == pTree->attr.op )
						pTree->type = "bool";
					else
					{
						pTree->type = pTree->child[ 0]->type;
					}
				}
				break;

			case ConditionOpK:	pTree->type = "bool";	break;
			case ConstIntK:		pTree->type = "int";	break;
			case ConstFloatK:	pTree->type = "float";	break;
			case ConstStrK:		pTree->type = "string";	break;
			case VarNameK:		break;
			}
		}
		break;
	}

	TypeCheck( pParent, pTree->sibling, pSymTab, pTypeTab );

	return TRUE;
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
char* CAnalyze::GetSymbolType( char *pSymbolName, ns_script::CSymTable *pSymTab, ns_script::CTypeTable *pTypeTab, 
							   ns_script::SParseTree *pTree )
{
	char *reval = NULL;
	char *type = pSymTab->GetType( pSymbolName );
	reval = type;

	char *pdot = strchr( pSymbolName, '.' );
	if( !pdot )
	{
		// �������� or �����Լ�
		char *globalfunc = pTypeTab->GetType( pSymbolName );
		if( type && globalfunc )
			TypeError( pTree, "Type�� ID�� ������ ID�� �����մϴ�.\n" );
		reval = (type)? type : globalfunc;
	}
	return reval;
}
// GetSymbolType �Լ��� ��������� �ɺ��� Ÿ�Կ� ������� �ɺ��� ��ϵǾ��ִ����� �˻��Ѵ�.
BOOL CAnalyze::IsSymbol( char *pSymbolName, ns_script::CSymTable *pSymTab, ns_script::CTypeTable *pTypeTab, 
						 ns_script::SParseTree *pTree )
{
	BOOL reval = FALSE;
	BOOL bsym = pSymTab->IsSymbol( pSymbolName );
	reval = bsym;

	char *pdot = strchr( pSymbolName, '.' );
	if( !pdot )
	{
		// �������� or �����Լ�
		BOOL bglobalfunc = pTypeTab->IsType( pSymbolName );
		if( bsym && bglobalfunc )
			TypeError( pTree, "Type�� ID�� ������ ID�� �����մϴ�.\n" );
		reval = bsym || bglobalfunc;
	}
	return reval;
}


//-----------------------------------------------------------------------------//
// �Լ�����Tree���� �Լ����ڰ����� ����ؼ� �����Ѵ�.
//	func_dec -> 'fn' type id '(' param_list ')' ':' num
//-----------------------------------------------------------------------------//
int CAnalyze::GetFunctionArgumentCount( ns_script::SParseTree *pFuncTree )
{
	int reval = 0;
	SParseTree *p = pFuncTree->child[ 2];
	while( p )
	{
		reval++;
		p = p->sibling;
	}
	return reval;
}


void CAnalyze::BuildError( SParseTree *t, char *message, int type )
{
	if( 1 == type ) // link err
		g_Dbg.Console( "build error [%s], %s", m_FileName, message );
	else if( 0 == type ) // syntax err
		g_Dbg.Console( "build error [%s] at line (%d): token %s %s", m_FileName, t->lineno, t->attr.name, message );
	m_bBuildError = TRUE;
}

void CAnalyze::TypeError( SParseTree *t, char *message, int type )
{
	if( 0 == type )
		g_Dbg.Console( "type error [%s] at line (%d): %s", m_FileName, t->lineno, message );
	else if( 1 == type ) // syntax err
		g_Dbg.Console( "type error [%s] at line (%d): token [%s] %s", m_FileName, t->lineno, t->attr.name, message );
	m_bTypeError = TRUE;
}
