/*
 Copyright (c) 2001 - 2002
 Author: Konstantin Boukreev 
 E-mail: konstantin@mail.primorye.ru 
 Created: 25.12.2001 19:41:07
 Version: 1.0.2

 Permission to use, copy, modify, distribute and sell this software
 and its documentation for any purpose is hereby granted without fee,
 provided that the above copyright notice appear in all copies and
 that both that copyright notice and this permission notice appear
 in supporting documentation.  Konstantin Boukreev makes no representations
 about the suitability of this software for any purpose.
 It is provided "as is" without express or implied warranty.

*/


#include "stdafx.h"
#include <windows.h>
#include "dbgsymengine.h"

#include <crtdbg.h>
#include <malloc.h> 
#include <tlhelp32.h>

#pragma comment (lib, "dbghelp")

#ifdef VERIFY
#undef VERIFY
#endif // VERIFY

#ifdef _DEBUG
#define VERIFY(m_posX) _ASSERTE(m_posX)
#else
#define VERIFY(m_posX) (m_posX)
#endif //_DEBUG

#define WORK_AROUND_SRCLINE_BUG

#ifdef _DEBUG
//#if 1
// #define SYM_ENGINE_TRACE_SPIN_COUNT
#endif //_DEBUG

///////////////////////////////////////////////////////////////////////

bool IsNT()
{	
	#if 1
	OSVERSIONINFO vi = { sizeof(vi)};	
	::GetVersionEx(&vi);
	return vi.dwPlatformId == VER_PLATFORM_WIN32_NT;
	#else
	return false;
	#endif
}

HANDLE SymGetProcessHandle()
{
	if (IsNT())
		return GetCurrentProcess();
	else
		return (HANDLE)GetCurrentProcessId();
}

BOOL __stdcall My_ReadProcessMemory (HANDLE, LPCVOID lpBaseAddress, LPVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead)
{
    return ReadProcessMemory(GetCurrentProcess(), lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
}

///////////////////////////////////////////////////////////////////////

dbgsymengine::dbgsymengine (unsigned address) 
	: m_address(address), m_ok(false), m_pframe(0)
{
}

dbgsymengine::~dbgsymengine()
{
//	if (m_ok) guard::instance().clear();
	delete m_pframe;
}

unsigned dbgsymengine::module(char * buf, unsigned len)
{
	if (!len || !buf || IsBadWritePtr(buf, len))
		return 0;

	if (!check())
		return 0;

	HANDLE hProc = SymGetProcessHandle();
	HMODULE hMod = (HMODULE)SymGetModuleBase (hProc, m_address);
	if (!hMod) return 0;
	return get_module_basename(hMod, buf, len);	
}

unsigned dbgsymengine::symbol(char * buf, unsigned len, unsigned * pdisplacement)
{
	if (!len || !buf || 
		IsBadWritePtr(buf, len) ||
		(pdisplacement && IsBadWritePtr(pdisplacement, sizeof(unsigned))))
		return 0;

	if (!check())
		return 0;

	BYTE symbol [ 512 ] ;
	PIMAGEHLP_SYMBOL pSym = (PIMAGEHLP_SYMBOL)&symbol;
	memset(pSym, 0, sizeof(symbol)) ;
	pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL) ;
    pSym->MaxNameLength = sizeof(symbol) - sizeof(IMAGEHLP_SYMBOL);

	HANDLE hProc = SymGetProcessHandle();
	DWORD displacement = 0;
	int r = SymGetSymFromAddr(hProc, m_address, &displacement, pSym);
	if (!r) return 0;
	if (pdisplacement) 
		*pdisplacement = displacement;

	r = _snprintf_s(buf, sizeof(buf), len, "%s()", pSym->Name);
    
	r = r == -1 ? len - 1 : r;
	buf[r] = 0;	
	return r;
}

unsigned dbgsymengine::fileline (char * buf, unsigned len, unsigned * pline, unsigned * pdisplacement)
{
	if (!len || !buf || 
		IsBadWritePtr(buf, len) || 
		(pline && IsBadWritePtr(pline, sizeof(unsigned))) || 
		(pdisplacement && IsBadWritePtr(pdisplacement, sizeof(unsigned))))
		return 0;

	if (!check())
		return 0;

	IMAGEHLP_LINE img_line;
	memset(&img_line, 0, sizeof(IMAGEHLP_LINE));
	img_line.SizeOfStruct = sizeof(IMAGEHLP_LINE);

	HANDLE hProc = SymGetProcessHandle();
	unsigned displacement = 0;
	if (!get_line_from_addr(hProc, m_address, &displacement, &img_line))
		return 0;
	if (pdisplacement) 
		*pdisplacement = displacement;
	if (pline) 
		*pline = img_line.LineNumber;
	lstrcpynA(buf, img_line.FileName, len);
	return lstrlenA(buf);
}

bool dbgsymengine::stack_first (CONTEXT* pctx)
{
	if (!pctx || IsBadReadPtr(pctx, sizeof(CONTEXT))) 
		return false;

	if (!check())
		return false;

	if (!m_pframe) 
	{
		m_pframe = new STACKFRAME;
		if (!m_pframe) return false;
	}
				
	memset(m_pframe, 0, sizeof(STACKFRAME));

    #ifdef _X86_
    m_pframe->AddrPC.Offset       = pctx->Eip;
    m_pframe->AddrPC.Mode         = AddrModeFlat;
    m_pframe->AddrStack.Offset    = pctx->Esp;
    m_pframe->AddrStack.Mode      = AddrModeFlat;
    m_pframe->AddrFrame.Offset    = pctx->Ebp;
    m_pframe->AddrFrame.Mode      = AddrModeFlat;
    #else
    m_pframe->AddrPC.Offset       = (DWORD)pctx->Fir;
    m_pframe->AddrPC.Mode         = AddrModeFlat;
    m_pframe->AddrReturn.Offset   = (DWORD)pctx->IntRa;
    m_pframe->AddrReturn.Mode     = AddrModeFlat;
    m_pframe->AddrStack.Offset    = (DWORD)pctx->IntSp;
    m_pframe->AddrStack.Mode      = AddrModeFlat;
    m_pframe->AddrFrame.Offset    = (DWORD)pctx->IntFp;
    m_pframe->AddrFrame.Mode      = AddrModeFlat;
    #endif

	m_pctx = pctx;
	return stack_next();
}

bool dbgsymengine::stack_next  ()
{
	if (!m_pframe || !m_pctx) 
	{		
		_ASSERTE(0);
		return false;
	}
		
	if (!m_ok)
	{
		_ASSERTE(0);
		return false;
	}

	SetLastError(0);
	HANDLE hProc = SymGetProcessHandle();
	BOOL r = StackWalk (IMAGE_FILE_MACHINE_I386,
				hProc, 
				GetCurrentThread(), 
				m_pframe, 
				m_pctx,
                (PREAD_PROCESS_MEMORY_ROUTINE)My_ReadProcessMemory,
				SymFunctionTableAccess,
                SymGetModuleBase,
				0);

	if (!r || 
		!m_pframe->AddrFrame.Offset)
	{		
		return false;
	}		

	// "Debugging Applications" John Robbins
	// Before I get too carried away and start calculating
	// everything, I need to double-check that the address returned
	// by StackWalk really exists. I've seen cases in which
	// StackWalk returns TRUE but the address doesn't belong to
	// a module in the process.
	DWORD dwModBase = SymGetModuleBase (hProc, m_pframe->AddrPC.Offset);
	if (!dwModBase) 
	{	
		_ASSERTE(0);
		return false;
	}

	address(m_pframe->AddrPC.Offset);
	return true;
}

bool dbgsymengine::get_line_from_addr (HANDLE hProc, unsigned addr, unsigned * pdisplacement, IMAGEHLP_LINE * pLine)
{	 
	#ifdef WORK_AROUND_SRCLINE_BUG

	// "Debugging Applications" John Robbins
    // The problem is that the symbol engine finds only those source
    // line addresses (after the first lookup) that fall exactly on
    // a zero displacement. I'll walk backward 100 bytes to
    // find the line and return the proper displacement.
    DWORD displacement = 0 ;
    while (!SymGetLineFromAddr (hProc, addr - displacement, (DWORD*)pdisplacement, pLine))
    {        
        if (100 == ++displacement)
            return false;        
    }

	// "Debugging Applications" John Robbins
    // I found the line, and the source line information is correct, so
    // change the displacement if I had to search backward to find the source line.
    if (displacement)    
        *pdisplacement = displacement;    
    return true;

	#else 
    return 0 != SymGetLineFromAddr (hProc, addr, (DWORD *) pdisplacement, pLine);
	#endif
}

unsigned dbgsymengine::get_module_basename (HMODULE hMod, char * buf, unsigned len)
{
	char filename[MAX_PATH];
	DWORD r = GetModuleFileNameA(hMod, filename, MAX_PATH);
	if (!r) return 0;
	
	char * p = 0;

	// Find the last '\' mark.
	int i = r - 1;
	for (; i >= 0; i--)
	{	
		if (filename[i] == '\\') 
		{
			p = &filename[i + 1]; 
			break;
		} 
	}

	if (!p)
	{
		i = 0;
		p = filename;
	}
	
	
	len = (len - 1 < r - i - 1) ? len - 1 : r - i - 1;
//	len = min(len - 1, r - i - 1);
	memcpy(buf, p, len);
	buf[len] = 0;
	return len;
}

bool dbgsymengine::check()
{	
	if (!m_ok) 
		m_ok = guard::instance().init(); 
	return m_ok; 
}

dbgsymengine::guard::guard() 
	: m_ref(0) 
{}

dbgsymengine::guard::~guard() 
{ 
	clear(); 
}

bool dbgsymengine::guard::init()
{	
	if (!m_ref) 
	{
		m_ref = -1;
		
		HANDLE hProc = SymGetProcessHandle();
		DWORD  dwPid = GetCurrentProcessId();

		// initializes
		SymSetOptions (SymGetOptions()|SYMOPT_DEFERRED_LOADS|SYMOPT_LOAD_LINES);
	//	SymSetOptions (SYMOPT_UNDNAME|SYMOPT_LOAD_LINES);
		if (::SymInitialize(hProc, 0, TRUE))
		{			
			// enumerate modules
			if (IsNT())		
			{
				typedef BOOL (WINAPI *ENUMPROCESSMODULES)(HANDLE, HMODULE*, DWORD, LPDWORD);

				HINSTANCE hInst = LoadLibrary( "psapi.dll" );
				if (hInst)
				{				
					ENUMPROCESSMODULES fnEnumProcessModules = 
						(ENUMPROCESSMODULES)GetProcAddress(hInst, "EnumProcessModules");
					DWORD cbNeeded = 0;
					if (fnEnumProcessModules &&
						fnEnumProcessModules(GetCurrentProcess(), 0, 0, &cbNeeded) &&
						cbNeeded)
					{	
						HMODULE * pmod = (HMODULE *)alloca(cbNeeded);
						DWORD cb = cbNeeded;
						if (fnEnumProcessModules(GetCurrentProcess(), pmod, cb, &cbNeeded))
						{
							m_ref = 0;
							for (unsigned i = 0; i < cb / sizeof (HMODULE); ++i)
							{								
								if (!load_module(hProc, pmod[i]))
								{
								//	m_ref = -1;
								//	break;
									_ASSERTE(0);
								}									
							}							
						}
					}
					else
					{
						_ASSERTE(0);
					}
					VERIFY(FreeLibrary(hInst));
				}
				else
				{
					_ASSERTE(0);
				}
			}
			else
			{
				typedef HANDLE (WINAPI *CREATESNAPSHOT)(DWORD, DWORD);
				typedef BOOL (WINAPI *MODULEWALK)(HANDLE, LPMODULEENTRY32);

				HMODULE hMod = GetModuleHandle("kernel32");
				CREATESNAPSHOT fnCreateToolhelp32Snapshot = (CREATESNAPSHOT)GetProcAddress(hMod, "CreateToolhelp32Snapshot");
				MODULEWALK fnModule32First = (MODULEWALK)GetProcAddress(hMod, "Module32First");
				MODULEWALK fnModule32Next  = (MODULEWALK)GetProcAddress(hMod, "Module32Next");

				if (fnCreateToolhelp32Snapshot && 
					fnModule32First && 
					fnModule32Next)
				{				
					HANDLE hModSnap = fnCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
					if (hModSnap)
					{
						MODULEENTRY32 me32 = {0};						 
						me32.dwSize = sizeof(MODULEENTRY32);
						if (fnModule32First(hModSnap, &me32))
						{
							m_ref = 0;
							do
							{
								if (!load_module(hProc, me32.hModule))
								{
								//	m_ref = -1;
								//	break;
								}																	
							}
							while(fnModule32Next(hModSnap, &me32));
						}
						VERIFY(CloseHandle(hModSnap));						
					}
				}
			}

			if (m_ref == -1)
			{				
				VERIFY(SymCleanup(SymGetProcessHandle()));
			}
		}
		else
		{
			_ASSERTE(0);
		}
	}
	if (m_ref == -1)
		return false;
	if (0 == m_ref) 
		++m_ref; // lock it once
//	++m_ref;
	return true;
}

void dbgsymengine::guard::clear()
{
	if (m_ref ==  0) return;
	if (m_ref == -1) return;	
	if (--m_ref == 0)
	{	 
		VERIFY(SymCleanup(SymGetProcessHandle()));
	}
}

bool dbgsymengine::guard::load_module(HANDLE hProcess, HMODULE hMod)
{	
	char filename[MAX_PATH];
	if (!GetModuleFileNameA(hMod, filename, MAX_PATH))
		return false;
	
	HANDLE hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE) return false;

	// "Debugging Applications" John Robbins	
    // For whatever reason, SymLoadModule can return zero, but it still loads the modules. Sheez.
	SetLastError(ERROR_SUCCESS);
    if (!SymLoadModule(hProcess, hFile, filename, 0, (DWORD)hMod, 0) && 
		ERROR_SUCCESS != GetLastError())
	{
		return false;
	}
	
	return true;
}

bool dbgsymengine::stack_trace(std::ostream& os, CONTEXT * pctx, unsigned skip, const char * fmt)
{
	if (!fmt) return false;	
	dbgsymengine sym(0);
	return stack_trace(os, sym, pctx, skip, fmt);	
}

/////////////////////////////////////////////
// prints a current thread's stack

struct current_context : CONTEXT
{
	HANDLE	thread;	 
	volatile int signal;
};

bool dbgsymengine::stack_trace(std::ostream& os, dbgsymengine& sym, 
		CONTEXT * pctx, unsigned skip, const char * fmt)
{
	if (!sym.stack_first(pctx)) 
		return false;

	char buf [512] = {0};
	char fbuf[512] = {0};
	char sbuf[512] = {0};

	os << std::dec;
	
	do
	{
		if (!skip)
		{			
			unsigned ln = 0;
			unsigned ld = 0;
			unsigned sd = 0;
			char *   pf	= 0;
			char *   ps = 0;

			for (char * p = (char *)fmt; *p; ++p)
			{				
				if (*p == '%')
				{
					++p; // skips '%'
					char c = *p;
					switch (c)
					{
					case 'm':
						os << (sym.module(buf, sizeof(buf)) ? buf : "?.?");
						break;
					case 'f':
						if (!pf)
							pf = (sym.fileline(fbuf, sizeof(fbuf), &ln, &ld)) ? fbuf : " ";
						os << pf;
						break;
					case 'l':
						if (!pf)
							pf = (sym.fileline(fbuf, sizeof(fbuf), &ln, &ld)) ? fbuf : " ";
						if (*(p + 1) == 'd') { os << ld; ++p; }
						else os << ln;							
						break;
					case 's':
						if (!ps)
							ps = sym.symbol(sbuf, sizeof(sbuf), &sd) ? sbuf : "?()";
						if (*(p + 1) == 'd') { os << sd; ++p; }
						else os << ps;
						break;
					case 'a':
						sprintf_s( buf, sizeof(buf), "0x%x", sym.address() );
						os << buf;
						break;
					case '%':
						os << '%';
						break;
					default:
						os << '%' << c;	// prints unknown format's argument
						break;
					}
				}
				else
				{
					os << *p;
				}
			}
		}
		else
		{
			--skip;
		}
	}
	while (os.good() && sym.stack_next());

	return true;
}
