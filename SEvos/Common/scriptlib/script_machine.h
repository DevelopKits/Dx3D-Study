//-----------------------------------------------------------------------------//
// 2006-08-15  programer: jaejung ��(���բ�)��
// 
// 2008-01-05 ������ ������� ���׷��̵� ��
//			  CMachine, CProcessor �߰���
//-----------------------------------------------------------------------------//

#if !defined(__SCRIPT_MACHINE_H__)
#define __SCRIPT_MACHINE_H__


namespace ns_script
{
	class CProcessor;

	// ����ó�� ����ӽ��̴�.
	// CProcessor�� �����ؼ� gm������ �����Ѵ�. 
	// CProcessor������ �ٽ� ��ũ��Ʈ�� ����� �� �ֱ� ������ Stack�� �׾Ƽ�
	// CProcessor�� �����Ų��. (�Լ�ȣ��� ���� ����̴�.)
	// �ϳ� �̻��� Stack�� �����ϸ鼭 �������� Processor�� ����ɼ� �ֵ��� �ߴ�.
	class CMachine
	{
	public:
		CMachine();
		virtual ~CMachine();
	protected:
		typedef std::stack<CProcessor*>	ProcStack;
		typedef std::list<ProcStack*>	ProcList;
		ProcList m_ProcList;
	public:
		BOOL ExecuteScript( char *pFileName, void callback (int,ns_script::CProcessor*), char *pArgument );
		int Run();
		void Clear();
	protected:
		BOOL ExecuteScriptChild( ProcStack *pStack, char *pFileName, void callback (int,ns_script::CProcessor*), char *pArgument );
	};


	// gm������ �о Instruction�� ó���Ѵ�.
	class CProcessor
	{
	public:
		CProcessor();
		virtual ~CProcessor();

		enum { IADDR_SIZE = 2048, DADDR_SIZE = 1024, NO_REGS = 8, LINESIZE=128, WORDSIZE=20 };
		enum STEPRESULT { srOKAY, srINPUT, srEXESCRIPT, srHALT, srIMEM_ERR, srDMEM_ERR, srZERODIVIDE, };
		enum OPCLASS
		{
			opclRR,		// reg operand r,s,t
			opclRM,		// reg r, mem d+s
			opclRA		// reg r, int d+s
		};

		typedef struct
		{
			int iop;
			int iarg1;
			int iarg2;
			int iarg3;

		} INSTRUCTION;

		// CCodeGenŬ������ ������
		enum Register
		{
			REG_AC = 0,		// accumulator
			REG_AC1,		// 2nd accumulator
			REG_AC2,		// 3nd accumulator
			REG_BP = 4,		// base pointer
			REG_GP,			// global pointer
							// points to bottom of memory for (global) variable storage
			REG_SP,			// stack pointer
			REG_PC			// programe counter
		};

		typedef void CALLFUNC (int,CProcessor*);

		CMachine *m_pMachine;
		BOOL m_bRun;						// Processor�� �������̶�� TRUE
		int m_nNumberOfExecuteInst;			// Run() �Լ����� ����� instruction����
		INSTRUCTION m_iMem[ IADDR_SIZE];	// Instruction �޸�
		INSTRUCTION m_CurInst;				// ���� Instruction
		float m_dMem[ DADDR_SIZE];			// Memory / stack
		float m_Reg[ NO_REGS];				// Register
		int m_StepCnt;						// ����� Instruction��
		CALLFUNC *m_pCallFunc;				// CallBack �Լ�
		int m_TraceFlag;					// Debug��
		int m_iCountFlag;					// Debug��

		// 
		char m_pgmName[ 20];	// filename
		char m_pArgument[ 64];	// ���� ���ڰ�
		FILE *m_pGm;			// code file pointer
		char m_Line[ LINESIZE]; // �о���� ���� ��Ʈ��
		int m_lineLen;			// �о���̴� ������ ����
		int m_inCol;			// �о���̴� �Ѷ����� index ���̴�.
		int m_Num;				// getnum() �Լ��� ��������� �����
		char m_Word[ WORDSIZE];	// getword() �Լ��� ��������� �����
		char m_Ch;				// getch() �Լ��� ��������� �����
		int m_Done;

	public:
		BOOL ExecuteScript( CMachine *pMachine, char *pFileName, void callback (int,ns_script::CProcessor*), char *pArgument );
		int Run();
		void* GetArgumentClass( int idx );
		float GetArgumentFloat( int idx );
		char* GetArgumentStr( int idx );
		void SetReturnValue( DWORD value );
		void SetReturnValue( int value );
		void SetReturnValue( float value );
		void SetReturnValue( void *value );
		void SetNumberOfExecuteInst( int num ) { m_nNumberOfExecuteInst = num; }
		void Clear();

	protected:
		BOOL ReadInstructions( char *pFileName );
		BOOL ReadInstructions_Binary( char *pFileName );
		int doCommand();
		STEPRESULT stepTM();
		STEPRESULT stepAcu( INSTRUCTION *pcurrentinst );
		void getCh();
		int nonBlank();
		int getNum();
		int getWord();
		int skipCh( char c );
		int opClass( int c );
		int error( char *msg, int lineNo, int instNo );
		int CallFunction( int funcid );
	};

};

#endif // __SCRIPT_MACHINE_H__
