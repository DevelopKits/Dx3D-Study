
#include "global.h"
#include "script_global.h"
#include "script_codegen.h"

using namespace std;
using namespace ns_script;



//-----------------------------------------------------------------------------//
// �����ڵ带 �����Ѵ�.
// pSourceFileName: ���� �����̸�, ȭ���ڸ� gm���� �ٲ۴�.
//-----------------------------------------------------------------------------//
BOOL CCodeGen::CodeGenerate( SParseTree *pTree, CTypeTable *pTypeTab, CStringTable *pStrTab, char *pSourceFileName )
{
	char filename[ MAX_PATH];
	strcpy( filename, pSourceFileName );
	strcpy( &filename[ strlen(filename)-3], "gm" );

	m_Fp = fopen( filename, "w" );
	if( !m_Fp ) return FALSE;

//	m_bComment = FALSE;
	m_pStrTab = pStrTab;

	// String Table ����
	int symsize = pTree->pSym->GetTotalMemSize();
	fwrite( &symsize, 1, sizeof(int), m_Fp ); // symbol tableũ�⸦ �����Ѵ�.
	pStrTab->WriteStringTable( m_Fp );	// string table ���� 

	char s[ MAX_PATH];
	strcpy( s, "File: " );
	strcat( s, pSourceFileName );
	EmitComment( "GameMachine compilation to GM code" );
	EmitComment( s );
	// generate standard prelud
	EmitComment( "Standard prelud:" );
	EmitRM( "LD", sp, 0, ac, "load maxaddress from location 0" );
	EmitRM( "LD", bp, 0, ac, "load maxaddress from location 0" );
	EmitRM( "ST", ac, 0, ac, "clear location 0" );
	EmitComment( "End of standard prelude." );

	// ����Ʈ�� ���������� �ε��Ѵ�.
	GenLoadGlobalVarPointer( pTree->pSym );

	CodeGen( pTree, pTypeTab );

	// event�� ���õ� �������κ��� ó���Ѵ�.
	// 1. scriptwait ��ɾ�κп� goto eventlist �ڵ��߰�
	// 2. event list loop �ڵ��߰�
	if( 0 > m_EventListLoc ) // event list�� ���ٸ� halt�� goto
		m_EventListLoc = EmitSkip( 0 );

	list<int>::iterator i = m_ScrWaitLoc.begin();
	while( m_ScrWaitLoc.end() != i )
	{
		int loc = *i++;
		EmitBackup( loc );
		EmitRM( "LDC", pc, m_EventListLoc, 0, "jmp to eventlist" );
	}
	EmitRestore();
	EmitRM( "LDC", pc, m_EventListLoc, 0, "jmp to eventlist" );

	EmitComment( "End of execution." );
	EmitRO( "HALT", 0, 0, 0, " " );

	// binary ����
	WriteBinary( pTree, pStrTab, pSourceFileName );

	fclose( m_Fp );
	return TRUE;
}


//-----------------------------------------------------------------------------//
// �ڵ����
//-----------------------------------------------------------------------------//
BOOL CCodeGen::CodeGen( SParseTree *pTree, CTypeTable *pTypeTab )
{
	if( pTree )
	{
		switch( pTree->nodekind )
		{
		case Stmt:
			GenStmt( pTree, pTypeTab );
			break;
		case Exp:
			GenExp( pTree, pTypeTab );
			break;
		default:
			break;
		}
		CodeGen( pTree->sibling, pTypeTab );
	}
	return TRUE;
}


//-----------------------------------------------------------------------------//
// Statement �ڵ� ���� 
//-----------------------------------------------------------------------------//
BOOL CCodeGen::GenStmt( SParseTree *pTree, CTypeTable *pTypeTab )
{
	CSymTable *psym = pTree->pSym;

	switch( pTree->kind )
	{
	case ProgramK:
		CodeGen( pTree->child[ 0], pTypeTab );
		CodeGen( pTree->child[ 1], pTypeTab );
		CodeGen( pTree->child[ 2], pTypeTab );
		CodeGen( pTree->child[ 3], pTypeTab );
		CodeGen( pTree->child[ 4], pTypeTab );
		break;

	case ComposeK:
		CodeGen( pTree->child[ 0], pTypeTab );
		break;

	case AssignK:
		{
			int id = psym->GetSymbolId( pTree->child[ 0]->attr.name );

			CodeGen( pTree->child[ 1], pTypeTab );

			if( id )
			{
				// import data
				GenImportData( pTree, pTypeTab );
			}
			else
			{
				int offset = psym->GetMemOffset( pTree->child[ 0]->attr.name );
				EmitRM( "ST", ac, offset, gp, "assign: store value" );

				// Ŭ�����ϰ�� ����Ʈ�� ��������� ��� �ε��Ѵ�.
				if( psym->IsImportClass(pTree->child[ 0]->attr.name) )
					GenLoadClassPointer( pTree->child[ 0]->attr.name, psym, pTypeTab );
			}
		}
		break;

	case IfK:
		{
			EmitComment( "-> if" );
			SParseTree *p1 = pTree->child[ 0];
			SParseTree *p2 = pTree->child[ 1];
			SParseTree *p3 = pTree->child[ 2];
			// generate code for test expression
			CodeGen( p1, pTypeTab );
			int savedLoc1 = EmitSkip( 1 );
			EmitComment( "if: jump to else belongs here" );
			// recurse on then part
			CodeGen( p2, pTypeTab );
			int savedLoc2 = EmitSkip( 1 );
			EmitComment( "if: jump to end belongs here" );
			int currentLoc = EmitSkip( 0 );
			EmitBackup( savedLoc1 );
			EmitRM_Abs( "JEQ", ac, currentLoc, "if: jmp to else" );
			EmitRestore();
			// recurse on else part
			CodeGen( p3, pTypeTab );
			currentLoc = EmitSkip( 0 );
			EmitBackup( savedLoc2 );
			EmitRM_Abs( "LDA", pc, currentLoc, "jmp to end" );
			EmitRestore();
			EmitComment( "<- if" );
		}
		break;

	case WhileK:
		{
			EmitComment( "-> while" );
			int savedLoc1 = EmitSkip( 0 ); // head of while
			CodeGen( pTree->child[ 0], pTypeTab );
			int savedLoc2 = EmitSkip( 1 ); // exp == 0 �̸� while���� �����.
			CodeGen( pTree->child[ 1], pTypeTab );
			EmitComment( "jump to head of while" );
			EmitRM( "LDC", pc, savedLoc1, 0, "jump to head of while" );
			int currentLoc = EmitSkip( 0 );
			EmitBackup( savedLoc2 );
			EmitRM( "JEQ", ac, currentLoc, 0, "while: jmp to outside while" );
			EmitRestore();
		}
		break;

	case EventK:
		{
			// EventList�� ���۵Ǵ� location�� �����Ѵ�.
			if( -1 == m_EventListLoc )
				m_EventListLoc = EmitSkip( 0 );

			EmitComment( "-> event" );
			SParseTree *p1 = pTree->child[ 0];
			SParseTree *p2 = pTree->child[ 1];
			
			// generate code for test expression
			CodeGen( p1, pTypeTab );
			int savedLoc1 = EmitSkip( 1 );
			EmitComment( "event: jump to end belongs here" );
			// recurse on then part
			CodeGen( p2, pTypeTab );
			int currentLoc = EmitSkip( 0 );
			EmitBackup( savedLoc1 );
			EmitRM_Abs( "JEQ", ac, currentLoc, "event: jmp to end" );
			EmitRestore();
			EmitComment( "<- event" );
		}
		break;

	case VarDecK:
	case ClassK:
	case FuncDecK:
	case IncludeK:
		// �ƹ��ϵ� ����
		break;

	case ScriptWaitK:
		// ScriptWait ����� ȣ��� ��ġ�� �����Ѵ�.
		// EventList �ڵ�����κп��� goto���� �ۼ��ȴ�.
		EmitComment( "Script Wait. Goto Event List" );
		m_ScrWaitLoc.push_back( EmitSkip(1) );
		break;

	case ScriptEndK:
		EmitComment( "Script End." );
		EmitRO( "HALT", 1, 0, 0, " " );
		break;

	case ScriptExitK:
		EmitComment( "Script Exit." );
		EmitRO( "HALT", 2, 0, 0, " " );
		break;

	default:
		g_Dbg.Console( "�������� �߸��Ǿ����ϴ� GenStmt() \n" );
		break;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Exp �ڵ����
//-----------------------------------------------------------------------------//
BOOL CCodeGen::GenExp( SParseTree *pTree, CTypeTable *pTypeTab )
{
	CSymTable *psym = pTree->pSym;

	switch( pTree->kind )
	{
	case ConditionOpK:
	case OpK:
		{
			CodeGen( pTree->child[ 0], pTypeTab );
			EmitRO( "PUSH", ac, 0, 0, "op: push left" );
			CodeGen( pTree->child[ 1], pTypeTab );
			EmitRO( "POP", ac1, 0, 0, "op: load left" );

			switch( pTree->attr.op )
			{
			case PLUS:		EmitRO( "ADD", ac, ac1, ac, "op +" );	break;
			case MINUS:		EmitRO( "SUB", ac, ac1, ac, "op -" );	break;
			case TIMES:		EmitRO( "MUL", ac, ac1, ac, "op *" );	break;
			case DIV:		EmitRO( "DIV", ac, ac1, ac, "op /" );	break;
			case REMAINDER:	EmitRO( "REM", ac, ac1, ac, "op %" );	break;
			case OR:		EmitRO( "OR", ac, ac1, ac, "op ||" );	break;
			case AND:		EmitRO( "AND", ac, ac1, ac, "op &&" );	break;

			case LT:	// ac > ac1
				EmitRO( "SUB", ac, ac, ac1, "op >" );
				EmitRM( "JLT", ac, 2, pc, "br if true" );
				EmitRM( "LDC", ac, 0, ac, "false case" );
				EmitRM( "LDA", pc, 1, pc, "unconditional jmp" );
				EmitRM( "LDC", ac, 1, ac, "true case" );
				break;

			case RT:	// ac < ac1
				EmitRO( "SUB", ac, ac1, ac, "op <" );
				EmitRM( "JLT", ac, 2, pc, "br if true" );
				EmitRM( "LDC", ac, 0, ac, "false case" );
				EmitRM( "LDA", pc, 1, pc, "unconditional jmp" );
				EmitRM( "LDC", ac, 1, ac, "true case" );
				break;

			case EQ:	// ==
				EmitRO( "SUB", ac, ac1, ac, "op ==" );
				EmitRM( "JEQ", ac, 2, pc, "br if true" );
				EmitRM( "LDC", ac, 0, ac, "false case" );
				EmitRM( "LDA", pc, 1, pc, "unconditional jmp" );
				EmitRM( "LDC", ac, 1, ac, "true case" );
				break;

			case NEQ:	// !=
				EmitRO( "SUB", ac, ac1, ac, "op ==" );
				EmitRM( "JNE", ac, 2, pc, "br if false" );
				EmitRM( "LDC", ac, 0, ac, "false case" );
				EmitRM( "LDA", pc, 1, pc, "unconditional jmp" );
				EmitRM( "LDC", ac, 1, ac, "true case" );
				break;
			}
		}
		break;

	case ConstIntK:	
		EmitComment( "-> Const Int" );
		EmitRM( "LDC", ac, pTree->attr.num, 0, "load const" );
		break;

	case ConstFloatK:
		EmitComment( "-> Const Float" );
		EmitRM( "LDC", ac, pTree->attr.fnum, 0, "load const" );
		break;

	case ConstStrK:
		{
			// ��Ʈ�����̺��� �������� ������ ����Ǳ⶧���� string offset���� 
			// �ɺ����̺��� ���� ��������.
			EmitComment( "-> Const String" );
			int offset = m_pStrTab->GetOffset( pTree->attr.name ) + psym->GetTotalMemSize();
			EmitRM( "LDC", ac, offset, 0, pTree->attr.name );
		}
		break;

	case IdK:
		{
			// import������� ���ӿ��� data�� �޾ƿ��� ó���� �ʿ��ϴ�.
			// �׳� �����ϰ�� stack�� ����� ����Ÿ�� ������ �ȴ�.

			EmitComment( "-> Idk" );

			int id = psym->GetSymbolId( pTree->attr.name );
			if( 0 == id )
			{
				int offset = psym->GetMemOffset( pTree->attr.name );
				EmitRM( "LD", ac, offset, gp, "load id value" );
			}
			else
			{
				// import data
				GenImportData( pTree, pTypeTab );
			}

			EmitComment( "<- Idk" );
		}
		break;

	case CallK:
		GenCall( pTree, pTypeTab );
		break;

	default:
		g_Dbg.Console( "�������� �߸��Ǿ����ϴ� GenExp() \n" );
		break;
	}

	return TRUE;
}


//-----------------------------------------------------------------------------//
// Function Call Code ����
// import ������ �ϰ������� ó���ϱ����ؼ� ������� �Լ���.
// import������ ó�� �Ҵ�� �� ���ø�ĳ�̼����� ���� �����͸� ���ͼ�, ��������
// ���ø����̼ǿ� �������� �ʰ� �ٷ� ����Ÿ�� �Ҵ��ϰų�, �������Ҽ��ְ� �ȴ�.
//
// stack ����
//		top		return space
//		 |		--------------
//		 |		stack pointer
//		 |		--------------
//	   bottom   argument
// �Լ�ȣ���� ������ return value�� ac �������Ϳ� ����ȴ�.
//-----------------------------------------------------------------------------//
void CCodeGen::GenCall( SParseTree *pTree, CTypeTable *pTypeTab )
{
	int id;
	char *comment;
	CSymTable *psym = pTree->pSym;
	
	switch( pTree->kind )
	{
	case CallK:
		{
			EmitRM( "LDA", ac2, 0, sp, "Load current sp" );			// sp�� �����Ѵ�.

			SParseTree *p1 = pTree->child[ 0];
			SParseTree *p2 = pTree->child[ 1];

			//----------------------------------------------------
			// ���� �������� �ִ� ���ں��� push �Ѵ�.
			stack< SParseTree* > que;
			while( p2 )
			{
				que.push( p2 );
				p2 = p2->sibling;
			}
			while( !que.empty() )
			{
				SParseTree *p = que.top(); que.pop();
				GenExp( p, pTypeTab );
				EmitRO( "PUSH", ac, 0, 0, "push argument" );
			}
			//----------------------------------------------------

			// class member�Լ� ȣ���̶�� ���ڿ� class pointer�� �ִ´�. (stack top�� this �����Ͱ� ��)
			if( strchr(p1->attr.name, '.') )
			{
				int offset = psym->GetClassMemOffset( p1->attr.name );
				EmitRM( "LD", ac, offset, gp, "load this pointer" );
				EmitRO( "PUSH", ac, 0, 0, "push this pointer" );
			}

			id = psym->GetSymbolId( p1->attr.name );
			comment = p1->attr.name;

			EmitRO( "PUSH", ac2, 0, 0, "push sp" );					// sp�� push�Ѵ�.
			EmitRO( "PUSH", ac, 0, 0, "push return_value space" );	// ���Ϲ��� ������ ����� ������ �����.

			EmitCall( id, comment );

			EmitRO( "POP", ac, 0, 0, "pop return_value" );			// ���ϰ� ac�� �Ҵ�
			EmitRO( "POP", ac1, 0, 0, "pop sp" );					// �Լ��� ȣ��Ǳ����� stack pointer�� ������.
			EmitRM( "LDA", sp, 0, ac1, "restore sp" );				// sp�� �����Ѵ�.
		}
		break;

	case IdK:
		g_Dbg.Console( "CodeGen Err,  GenCall.\n" );
		break;

	case AssignK:
		g_Dbg.Console( "CodeGen Err,  GenCall.\n" );
		break;
	}
}


//-----------------------------------------------------------------------------//
// ����Ʈ�� Ŭ������ ��������� ���ø����̼����� ���� �����͸� ���´�.
// �Լ��ݺκ��� GenCall() �Լ��� �����ϴ�.
// pSymbolName: ����Ʈ�� Ŭ������ ������
//-----------------------------------------------------------------------------//
void CCodeGen::GenLoadClassPointer( char *pSymbolName, CSymTable *pSym, CTypeTable *pTypeTab )
{
	if( pSym->IsLoaded(pSymbolName) ) return;

	char *ptype = pSym->GetType( pSymbolName );
	list<STypeData> *pmember = pTypeTab->GetMemberList( ptype );
	if( pmember->size() <= 1 ) return;

	EmitComment( "-> Load Import Class" );
	char buf[ 128];

	int class_offset = pSym->GetClassMemOffset( pSymbolName );
	list<STypeData>::iterator it = pmember->begin();
	while( pmember->end() != it )
	{
		STypeData dat = *it++;
		if( DT_FUNC == dat.state || DT_CLASS == dat.state ) continue;

		sprintf( buf, "Load Pointer id: %s", dat.name );
		EmitComment( buf );

		EmitRM( "LDA", ac2, 0, sp, "Load current sp" );		// sp�� �����Ѵ�.

		// �����͸� ������ؼ� ���ڷ� ID�� �ʿ��ϴ�.
		EmitRM( "LDC", ac, dat.id, 0, "load id" );
		EmitRO( "PUSH", ac, 0, 0, "push argument" );	// ���ڸ� push �Ѵ�.

		// ���ڿ� class pointer�� �ִ´�. (stack top�� this �����Ͱ� ��)
		EmitRM( "LD", ac, class_offset, gp, "load this pointer" );
		EmitRO( "PUSH", ac, 0, 0, "push this pointer" );
		EmitRO( "PUSH", ac2, 0, 0, "push sp" );					// sp�� push�Ѵ�.
		EmitRO( "PUSH", ac, 0, 0, "push return_value space" );	// ���Ϲ��� ������ ����� ������ �����.

		EmitCall( LoadPointer, "LoadPointer" );

		EmitRO( "POP", ac, 0, 0, "pop return_value" );			// ���ϰ� ac�� �Ҵ�
		EmitRO( "POP", ac1, 0, 0, "pop sp" );					// �Լ��� ȣ��Ǳ����� stack pointer�� ������.
		EmitRM( "LDA", sp, 0, ac1, "restore sp" );				// sp�� �����Ѵ�.

		// ���ϵ� �����͸� �����Ѵ�.
		EmitRM( "ST", ac, class_offset + dat.offset, gp, "assign: store pointer" );		
	}

	pSym->Loaded( pSymbolName );
}


//-----------------------------------------------------------------------------//
// ����Ʈ�� �۷ι� ������ ���ø����̼� ���κ��� �����͸� ���´�.
// GenClassLoadPointer() �Լ��� Ŭ���� ��������� ����Ʈ������ �� �Լ��� �۷ι� ������
// ó���ϴµ��� �ٸ���. 
//-----------------------------------------------------------------------------//
void CCodeGen::GenLoadGlobalVarPointer( CSymTable *pSym )
{
	list<string> lst;
	pSym->GetGlobalImportData( &lst );

	EmitComment( "-> Load Import Global variable" );
	char buf[ 128];

	list<string>::iterator it = lst.begin();
	while( lst.end() != it )
	{
		int id = pSym->GetSymbolId( (char*)it->c_str() );
		int offset = pSym->GetMemOffset( (char*)it->c_str() );

		sprintf( buf, "Load Pointer id: %s", it->c_str() );
		EmitComment( buf );

		EmitRM( "LDA", ac2, 0, sp, "Load current sp" );		// sp�� �����Ѵ�.

		// �����͸� ������ؼ� ���ڷ� ID�� �ʿ��ϴ�.
		EmitRM( "LDC", ac, id, 0, "load id" );
		EmitRO( "PUSH", ac, 0, 0, "push argument" );	// ���ڸ� push �Ѵ�.

		EmitRO( "PUSH", ac2, 0, 0, "push sp" );					// sp�� push�Ѵ�.
		EmitRO( "PUSH", ac, 0, 0, "push return_value space" );	// ���Ϲ��� ������ ����� ������ �����.

		EmitCall( LoadPointer, "LoadPointer" );

		EmitRO( "POP", ac, 0, 0, "pop return_value" );			// ���ϰ� ac�� �Ҵ�
		EmitRO( "POP", ac1, 0, 0, "pop sp" );					// �Լ��� ȣ��Ǳ����� stack pointer�� ������.
		EmitRM( "LDA", sp, 0, ac1, "restore sp" );				// sp�� �����Ѵ�.

		// ���ϵ� �����͸� �����Ѵ�.
		EmitRM( "ST", ac, offset, gp, "assign: store pointer" );		

		++it;
	}
}


//-----------------------------------------------------------------------------//
// ����Ʈ�� ����Ÿ�� �����ϱ����� �ڵ带 �����Ѵ�.
// ����Ʈ�� ������ ���ø����̼��� �����͸� ������ �ִ�. �׷��� �ӽ��� �ڽ���
// �޸𸮿��� �˻��ϴ°� �ƴ϶� �ܺθ޸𸮸� �����ؾ� �ϱ⶧���� ������ ����Ÿ��
// ������ ����� �޶�����. 
// �ܺθ޸𸮿��� ����Ÿ�� ��ų� �����ϱ� ���ؼ��� opILDF,opILD, opISTF, opIST
// ��ɾ ����ؾ� �Ѵ�. ���� int, float���� �����Ѵ�. 
//-----------------------------------------------------------------------------//
void CCodeGen::GenImportData( SParseTree *pTree, CTypeTable *pTypeTab )
{
	CSymTable *psym = pTree->pSym;
	
	switch( pTree->kind )
	{
	case IdK:
		{
			char *type = psym->GetType( pTree->attr.name );
			int offset = psym->GetMemOffset( pTree->attr.name );
			if( !strcmp("float", type) )
			{
				EmitRM( "ILDF", ac, offset, gp, "load id import value" );
			}
			else if( !strcmp("int", type) )
			{
				EmitRM( "ILD", ac, offset, gp, "load id import value" );
			}
			else
			{
				// err, �ý��ۻ����δ� ���⿡ �����Ҽ� ����.
				g_Dbg.Console( "CodeGen Err,  GenImportData.\n" );
			}
		}
		break;

	case AssignK:
		{
			char *type = psym->GetType( pTree->child[ 0]->attr.name );
			int offset = psym->GetMemOffset( pTree->child[ 0]->attr.name );
			if( !strcmp("float", type) )
			{
				EmitRM( "ISTF", ac, offset, gp, "assign: store import value" );
			}
			else if( !strcmp("int", type) )
			{
				EmitRM( "IST", ac, offset, gp, "assign: store import value" );
			}
			else
			{
				// error
				g_Dbg.Console( "CodeGen Err,  classŸ���� �Ҵ���� �ʽ��ϴ�.\n" );
			}
		}
		break;
	}
}


OPCODE FindOp( char *command )
{
	// op code ã��
	OPCODE op = opHALT;
	int n = op;
	while( (op < opRALim) && (strncmp(g_opCodeTab[ op], command, 4) != 0) )
	{
		++n;
		op = (OPCODE)n;
	}
	if( op >= opRALim )
		return opHALT;
	return op;	
}


//-----------------------------------------------------------------------------//
// code�� ���̳ʸ��� �����Ѵ�.
// pc (program counter) ������ ����ȴ�.
//-----------------------------------------------------------------------------//
BOOL CCodeGen::WriteBinary( SParseTree *pTree, CStringTable *pStrTab, char *pSourceFileName )
{
	m_InstList.sort(); // loc �������� ����

	// Ȯ���ڸ� bgm���� �ٲ۴�.
	char filename[ MAX_PATH];
	strcpy( filename, pSourceFileName );
	strcpy( &filename[ strlen(filename)-3], "bgm" );

	FILE *fp = fopen( filename, "wb" );
	if( !fp ) return FALSE;

	// String Table ����
	int symsize = pTree->pSym->GetTotalMemSize();
	fwrite( &symsize, 1, sizeof(int), fp ); // symbol tableũ�⸦ �����Ѵ�.
	pStrTab->WriteStringTable( fp );		// string table ���� 

	// instruction �� ����
	int instcnt = m_InstList.size();
	fwrite( &instcnt, 1, sizeof(int), fp );

	list<SInst>::iterator i = m_InstList.begin();
	while( m_InstList.end() != i )
	{
		OPCODE op = FindOp( i->op );
		fwrite( &op, 1, sizeof(int), fp );
		fwrite( &i->r, 1, sizeof(int), fp );
		fwrite( &i->s, 1, sizeof(int), fp );
		fwrite( &i->t, 1, sizeof(int), fp );
		++i;
	}
	fclose( fp );

	return TRUE;
}


//--------------------------------------------------------
// procedure emitRO emits a register-only
// TM instruction
// op = the opcode
// r = target register
// s = 1st source register
// t = 2nd source register
// c = a comment to be printed if m_bComment is TRUE
//--------------------------------------------------------
void CCodeGen::EmitRO( char *op, int r, int s, int t, char *c )
{
	fprintf( m_Fp, "%3d:  %5s  %d,%d,%d ", m_nEmitLoc++, op, r, s, t );
	if( m_bComment ) fprintf( m_Fp, "\t%s", c );
	fprintf( m_Fp, "\n" );
	if( m_nHighEmitLoc < m_nEmitLoc ) m_nHighEmitLoc = m_nEmitLoc;

	m_InstList.push_back( SInst(m_nEmitLoc-1, op, r, s, t, c) );
}


//--------------------------------------------------------
// procedure emitRM emits a register-to-memory
// TM instruction
// op = the opcode
// r = target register
// d = the offset
// s = the base register
// c = a comment to be printed if m_bComment is TRUE
//--------------------------------------------------------
void CCodeGen::EmitRM( char *op, int r, int d, int s, char *c )
{
	fprintf( m_Fp, "%3d:  %5s  %d,%d(%d) ", m_nEmitLoc++, op, r, d, s );
	if( m_bComment ) fprintf( m_Fp, "\t%s", c );
	fprintf( m_Fp, "\n" );
	if( m_nHighEmitLoc < m_nEmitLoc ) m_nHighEmitLoc = m_nEmitLoc;

	m_InstList.push_back( SInst(m_nEmitLoc-1, op, r, d, s, c) );
}


//-----------------------------------------------------------------------------//
// �Լ� Call��ɾ �����Ѵ�. 
//-----------------------------------------------------------------------------//
void CCodeGen::EmitCall( int id, char *c )
{
	fprintf( m_Fp, "%3d:   CALL  %d,0,0 ", m_nEmitLoc++, id );
	if( m_bComment ) fprintf( m_Fp, "\t%s", c );
	fprintf( m_Fp, "\n" );
	if( m_nHighEmitLoc < m_nEmitLoc ) m_nHighEmitLoc = m_nEmitLoc;

	m_InstList.push_back( SInst(m_nEmitLoc-1, "CALL", id, 0, 0, c) );
}


int CCodeGen::EmitSkip( int howMany )
{
	int i = m_nEmitLoc;
	m_nEmitLoc += howMany;
	if( m_nHighEmitLoc < m_nEmitLoc ) m_nHighEmitLoc = m_nEmitLoc;
	return i;
}


void CCodeGen::EmitBackup( int loc )
{
	if( loc > m_nHighEmitLoc ) EmitComment( "BUG in emitBackup" );
	m_nEmitLoc = loc;
}


void CCodeGen::EmitRestore()
{
	m_nEmitLoc = m_nHighEmitLoc;
}


//--------------------------------------------------------
// procedure emitRM_Abs converts an absolute reference
// to a pc-relative reference when emitting a
// register-to-memory TM instruction
// op = the opcode
// r = target register
// a = the absolute location in memory
// c = a comment to be printed if m_bComment is TRUE
//--------------------------------------------------------
void CCodeGen::EmitRM_Abs( char *op, int r, int a, char *c )
{
	int d = a-(m_nEmitLoc+1);
	fprintf( m_Fp, "%3d:  %5s  %d,%d(%d) ", m_nEmitLoc, op, r, d, pc );
	++m_nEmitLoc;
	if( m_bComment ) fprintf( m_Fp, "\t%s", c );
	fprintf( m_Fp, "\n" );
	if( m_nHighEmitLoc < m_nEmitLoc ) m_nHighEmitLoc = m_nEmitLoc;

	m_InstList.push_back( SInst(m_nEmitLoc-1, op, r, d, pc, c) );
}


void CCodeGen::EmitComment( char *c )
{
	if( m_bComment ) fprintf( m_Fp, "* %s\n", c );
}


