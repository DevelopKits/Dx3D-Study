//-----------------------------------------------------------------------------//
// 2006-08-15 programer: jaejung ��(���բ�)��
// 
// 2008-01-03 ������ ������� ���׷��̵� ��
// ParseTree�� �м��ؼ� �ɺ��� �����ϰ�, Ÿ���� üũ�Ѵ�.
// Ÿ���� �����ο� ��ũ��Ʈ ���� anlyzer��. 
//-----------------------------------------------------------------------------//

#if !defined(__SCRIPT_ANALYZE_H__)
#define __SCRIPT_ANALYZE_H__

namespace ns_script
{
	class CAnalyze
	{
	public:
		CAnalyze(): m_bBuildError(FALSE), m_bTypeError(FALSE) {}
		virtual ~CAnalyze() {}

	protected:
		BOOL m_bBuildError;
		BOOL m_bTypeError;
		CStringTable *m_pStrTab;
		char m_FileName[ MAX_PATH];

	public:
		BOOL Build( char *pFileName, SParseTree *pParseTree, CSymTable *pSymTab, 
			        CTypeTable *pTypeTab, CStringTable *pStrTab );
		BOOL IsError() { return m_bBuildError || m_bTypeError; }

	protected:
		BOOL BuildSymTable( SParseTree *pParent, SParseTree *pTree, CSymTable *pSymTab, CTypeTable *pTypeTab );
		BOOL TypeCheck( SParseTree *pParent, SParseTree *pTree, CSymTable *pSymTab, CTypeTable *pTypeTab );
		char* GetSymbolType( char *pSymbolName, CSymTable *pSymTab, CTypeTable *pTypeTab, SParseTree *pTree );
		BOOL IsSymbol( char *pSymbolName, CSymTable *pSymTab, CTypeTable *pTypeTab, SParseTree *pTree );
		int GetFunctionArgumentCount( SParseTree *pFuncTree );
		void BuildError( SParseTree *t, char *message, int type=0 );
		void TypeError( SParseTree *t, char *message, int type=0 );
	};
}

#endif // __SCRIPT_ANALYZE_H__
