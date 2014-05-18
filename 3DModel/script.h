//-----------------------------------------------------------------------------//
// 2008-01-03  programer: jaejung ��(���բ�)��
// 
// 2008-02-27
//		define ��� �߰�
//-----------------------------------------------------------------------------//

#if !defined(__SCRIPT_H__)
#define __SCRIPT_H__

#include "script_global.h"

namespace ns_script
{
	class CMachine;
	class CProcessor;

	class CScript
	{
	public:
		CScript();
		virtual ~CScript();
	protected:
		CMachine *m_pMachine;
		int m_IncTime;			// ���� �����Ӱ������� ��ũ��Ʈ�� �����ϱ� �ϱ����� �ʿ���
	public:
		BOOL Execute( char *pFileName, void func (int,ns_script::CProcessor*), char *pArgument=NULL );
		int Run( int nDelta );
	protected:
		BOOL Compile( char *pFileName, void func (int,ns_script::CProcessor*),  
			ns_script::CTypeTable *pType=NULL, ns_script::CPreCompiler *pPreC=NULL );
	};
}

#endif // __SCRIPT_H__
