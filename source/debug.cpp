/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2021 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include <crisscross/universal_include.h>
#include <crisscross/debug.h>
#include <crisscross/filewriter.h>
#include <crisscross/nasty_cast.h>

#include <cstdio>
#include <cstring>

#if defined (TARGET_OS_LINUX) || defined (TARGET_OS_MACOSX) || defined (TARGET_OS_FREEBSD) || defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD)
#if !defined(TARGET_COMPILER_CLANG)
#include <cxxabi.h>
#else
namespace __cxxabiv1
{
	extern "C"
	{
		char*
		__cxa_demangle(const char* __mangled_name, char* __output_buffer,
					   size_t* __length, int* __status);
	}
}
namespace abi = __cxxabiv1;
#endif
#if defined(TARGET_OS_LINUX) && !defined(TARGET_COMPILER_CYGWIN)
#include <execinfo.h>
#endif
#endif

using namespace CrissCross::IO;

#ifdef TARGET_OS_MACOSX
#include <dlfcn.h>
typedef int (*backtrace_t)(void * *, int);
typedef char **(*backtrace_symbols_t)(void * const *, int);
#endif

#ifndef ENABLE_SYMBOL_ENGINE
#ifdef TARGET_COMPILER_VC
#pragma warning (disable: 4311)
#endif
#else

#include <windows.h>
#include <dbghelp.h>

extern "C" {
#if TARGET_CPU_BITS == 32
typedef BOOL(__stdcall *STACKWALK32)( DWORD, HANDLE, HANDLE, LPSTACKFRAME, PVOID, PREAD_PROCESS_MEMORY_ROUTINE, PFUNCTION_TABLE_ACCESS_ROUTINE, PGET_MODULE_BASE_ROUTINE, PTRANSLATE_ADDRESS_ROUTINE);
typedef BOOL(__stdcall *SYMGETLINEFROMADDR32)(HANDLE, DWORD, PDWORD, PIMAGEHLP_LINE);
typedef BOOL(__stdcall *SYMGETMODULEINFO32)(HANDLE, DWORD, PIMAGEHLP_MODULE);
typedef BOOL(__stdcall *SYMGETSYMFROMADDR32)(HANDLE, DWORD, PDWORD, PIMAGEHLP_SYMBOL);
#else
typedef BOOL(__stdcall *STACKWALK64)(DWORD, HANDLE, HANDLE, LPSTACKFRAME64, PVOID, PREAD_PROCESS_MEMORY_ROUTINE64, PFUNCTION_TABLE_ACCESS_ROUTINE64, PGET_MODULE_BASE_ROUTINE64, PTRANSLATE_ADDRESS_ROUTINE64);
typedef BOOL(__stdcall *SYMGETLINEFROMADDR64)(HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64);
typedef BOOL(__stdcall *SYMGETMODULEINFO64)(HANDLE, DWORD64, PIMAGEHLP_MODULE64);
typedef BOOL(__stdcall *SYMGETSYMFROMADDR64)(HANDLE, DWORD64, PDWORD64, PIMAGEHLP_SYMBOL64);
#endif
typedef BOOL(__stdcall *SYMINITIALIZE)(HANDLE, LPCSTR, BOOL);
typedef BOOL(__stdcall *SYMCLEANUP)(HANDLE);
typedef DWORD(__stdcall *SYMGETOPTIONS)(VOID);
typedef DWORD(__stdcall *SYMSETOPTIONS)(DWORD);

static HMODULE hDbgHelp = nullptr;
#if TARGET_CPU_BITS == 32
static STACKWALK32 pStackWalk = nullptr;
static PFUNCTION_TABLE_ACCESS_ROUTINE pSymFunctionTableAccess = nullptr;
static SYMGETLINEFROMADDR32 pSymGetLineFromAddr = nullptr;
static PGET_MODULE_BASE_ROUTINE pSymGetModuleBase = nullptr;
static SYMGETMODULEINFO32 pSymGetModuleInfo = nullptr;
static SYMGETSYMFROMADDR32 pSymGetSymFromAddr = nullptr;
#else
static STACKWALK64 pStackWalk = nullptr;
static PFUNCTION_TABLE_ACCESS_ROUTINE64 pSymFunctionTableAccess = nullptr;
static SYMGETLINEFROMADDR64 pSymGetLineFromAddr = nullptr;
static PGET_MODULE_BASE_ROUTINE64 pSymGetModuleBase = nullptr;
static SYMGETMODULEINFO64 pSymGetModuleInfo = nullptr;
static SYMGETSYMFROMADDR64 pSymGetSymFromAddr = nullptr;
#endif
static SYMINITIALIZE pSymInitialize = nullptr;
static SYMCLEANUP pSymCleanup = nullptr;
static SYMSETOPTIONS pSymSetOptions = nullptr;
static SYMGETOPTIONS pSymGetOptions = nullptr;
}

class SymbolEngine
{
public:
	SymbolEngine(SymbolEngine const &) = delete;
	SymbolEngine & operator=(SymbolEngine const &) = delete;

	static SymbolEngine & instance();
#if TARGET_CPU_BITS == 32
	std::string addressToString(DWORD address);
#else
	std::string addressToString(DWORD64 address);
#endif
	void StackTrace(PCONTEXT _pContext, CoreIOWriter * _outputBuffer);
	HANDLE hProcess;

public:
	~SymbolEngine();
private:
	SymbolEngine();
	bool m_initialized;
};

SymbolEngine & SymbolEngine::instance()
{
	static SymbolEngine theEngine;

	return theEngine;
}

SymbolEngine::SymbolEngine()
{
	DWORD dwOpts;

	if (!hDbgHelp) {
		hDbgHelp = LoadLibraryA("dbghelp.dll");
		if (!hDbgHelp) goto init_fail;
		#if TARGET_CPU_BITS == 32
		pStackWalk = (STACKWALK32)GetProcAddress(hDbgHelp, "StackWalk");
		pSymFunctionTableAccess = (PFUNCTION_TABLE_ACCESS_ROUTINE)GetProcAddress(hDbgHelp, "SymFunctionTableAccess");
		pSymGetLineFromAddr = (SYMGETLINEFROMADDR32)GetProcAddress(hDbgHelp, "SymGetLineFromAddr");
		pSymGetModuleBase = (PGET_MODULE_BASE_ROUTINE)GetProcAddress(hDbgHelp, "SymGetModuleBase");
		pSymGetModuleInfo = (SYMGETMODULEINFO32)GetProcAddress(hDbgHelp, "SymGetModuleInfo");
		pSymGetSymFromAddr = (SYMGETSYMFROMADDR32)GetProcAddress(hDbgHelp, "SymGetSymFromAddr");
		#else
		pStackWalk = (STACKWALK64)GetProcAddress(hDbgHelp, "StackWalk64");
		pSymFunctionTableAccess = (PFUNCTION_TABLE_ACCESS_ROUTINE64)GetProcAddress(hDbgHelp, "SymFunctionTableAccess64");
		pSymGetLineFromAddr = (SYMGETLINEFROMADDR64)GetProcAddress(hDbgHelp, "SymGetLineFromAddr64");
		pSymGetModuleBase = (PGET_MODULE_BASE_ROUTINE64)GetProcAddress(hDbgHelp, "SymGetModuleBase64");
		pSymGetModuleInfo = (SYMGETMODULEINFO64)GetProcAddress(hDbgHelp, "SymGetModuleInfo64");
		pSymGetSymFromAddr = (SYMGETSYMFROMADDR64)GetProcAddress(hDbgHelp, "SymGetSymFromAddr64");
		#endif
		pSymGetOptions = (SYMGETOPTIONS)GetProcAddress(hDbgHelp, "SymGetOptions");
		pSymSetOptions = (SYMSETOPTIONS)GetProcAddress(hDbgHelp, "SymSetOptions");
		pSymInitialize = (SYMINITIALIZE)GetProcAddress(hDbgHelp, "SymInitialize");
		pSymCleanup = (SYMCLEANUP)GetProcAddress(hDbgHelp, "SymCleanup");
	}

	if (!pStackWalk ||
		!pSymFunctionTableAccess ||
		!pSymGetLineFromAddr ||
		!pSymGetModuleBase ||
		!pSymGetModuleInfo ||
		!pSymGetSymFromAddr ||
		!pSymGetOptions ||
		!pSymSetOptions ||
		!pSymInitialize ||
		!pSymCleanup)
	{
		goto init_fail;
	}

	hProcess = GetCurrentProcess();

	dwOpts = pSymGetOptions();

	dwOpts |= SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS | SYMOPT_AUTO_PUBLICS;
	dwOpts &= ~(SYMOPT_DEBUG | SYMOPT_PUBLICS_ONLY | SYMOPT_FAIL_CRITICAL_ERRORS);

	pSymSetOptions(dwOpts);

	pSymInitialize(hProcess, 0, true);

	m_initialized = true;
	return;
init_fail:
	m_initialized = false;
	return;
}

SymbolEngine::~SymbolEngine()
{
	if (m_initialized) {
		pSymCleanup(hProcess);

		pStackWalk = nullptr;
		pSymFunctionTableAccess = nullptr;
		pSymGetLineFromAddr = nullptr;
		pSymGetModuleBase = nullptr;
		pSymGetModuleInfo = nullptr;
		pSymGetSymFromAddr = nullptr;
		pSymGetOptions = nullptr;
		pSymSetOptions = nullptr;
		pSymInitialize = nullptr;
		pSymCleanup = nullptr;
		FreeLibrary(hDbgHelp);
	}
}

#if TARGET_CPU_BITS == 32
std::string SymbolEngine::addressToString(DWORD address)
#elif TARGET_CPU_BITS == 64
std::string SymbolEngine::addressToString(DWORD64 address)
#endif
{
	std::string str = "";

	if (!m_initialized)
		return str;

	static char addr_string[20];
#if TARGET_CPU_BITS == 32
	const char *format = "0x%08x";
#elif TARGET_CPU_BITS == 64
	const char *format = "0x%016x";
#endif

	/* First the raw address */
	sprintf(addr_string, format, address);

	str += addr_string;

	/* Then any name for the symbol */
	struct tagSymInfo
	{
		IMAGEHLP_SYMBOL symInfo;
		char nameBuffer[4 * 256];
	} SymInfo = { { sizeof(IMAGEHLP_SYMBOL) } };

	IMAGEHLP_SYMBOL *
	pSym = &SymInfo.symInfo;

	pSym->MaxNameLength =
	        sizeof(SymInfo) - offsetof(tagSymInfo, symInfo.Name);

#if TARGET_CPU_BITS == 32
	DWORD dwDisplacement;
#elif TARGET_CPU_BITS == 64
	DWORD64 dwDisplacement;
#endif
	IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };

	if (pSymGetSymFromAddr(GetCurrentProcess(), address, &dwDisplacement, pSym)) {
		str += "  ";
		str += pSym->Name;
	}

	/* Finally any file/line number */
	if (pSymGetLineFromAddr(GetCurrentProcess(), address, (PDWORD)&dwDisplacement, &lineInfo)) {
		const char *pDelim = strrchr(lineInfo.FileName, '\\');
		char lineDetails[64];
		snprintf(lineDetails, sizeof(lineDetails), "at %s(%u)", (pDelim ? pDelim + 1 : lineInfo.FileName), lineInfo.LineNumber);
		str += " ";
		str += lineDetails;
	}

	return str;
}

void SymbolEngine::StackTrace(PCONTEXT _pContext, CoreIOWriter * _outputBuffer)
{
	if (!m_initialized)
		return;

#if TARGET_CPU_BITS == 32
	_outputBuffer->WriteLine("  Frame         Address    Code");
#else
	_outputBuffer->WriteLine("  Frame                Address             Code");
#endif

#if TARGET_CPU_BITS == 32
	STACKFRAME stackFrame = { 0 };
#else
	STACKFRAME64 stackFrame = { 0 };
#endif

#if defined(TARGET_CPU_X86)
	stackFrame.AddrPC.Offset = _pContext->Eip;
	stackFrame.AddrStack.Offset = _pContext->Esp;
	stackFrame.AddrFrame.Offset = _pContext->Ebp;
#elif defined(TARGET_CPU_X64)
	stackFrame.AddrPC.Offset = _pContext->Rip;
	stackFrame.AddrStack.Offset = _pContext->Rsp;
	stackFrame.AddrFrame.Offset = _pContext->Rbp;
#elif defined(TARGET_CPU_ARM) && TARGET_CPU_BITS == 32
	stackFrame.AddrPC.Offset = _pContext->Pc;
	stackFrame.AddrStack.Offset = _pContext->Sp;
	stackFrame.AddrFrame.Offset = _pContext->R11;
#elif defined(TARGET_CPU_ARM) && TARGET_CPU_BITS == 64
	stackFrame.AddrPC.Offset = _pContext->Pc;
	stackFrame.AddrStack.Offset = _pContext->Sp;
	stackFrame.AddrFrame.Offset = _pContext->Fp;
#endif

	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Mode = AddrModeFlat;
	stackFrame.AddrStack.Mode = AddrModeFlat;

#ifdef TARGET_CPU_X86
	DWORD machine = IMAGE_FILE_MACHINE_I386;
#elif defined(TARGET_CPU_X64)
	DWORD machine = IMAGE_FILE_MACHINE_AMD64;
#elif defined(TARGET_CPU_ARM) && TARGET_CPU_BITS == 32
	DWORD machine = IMAGE_FILE_MACHINE_ARM;
#elif defined(TARGET_CPU_ARM) && TARGET_CPU_BITS == 64
	DWORD machine = IMAGE_FILE_MACHINE_ARM64;
#endif

#if TARGET_CPU_BITS == 32
	const char *format = "  0x%08x   %s";
#elif TARGET_CPU_BITS == 64
	const char *format = "  0x%016x   %s";
#endif

	while (pStackWalk(machine, hProcess, GetCurrentThread(),        /* this value doesn't matter much if previous one is a real handle */
	                   &stackFrame,
	                   _pContext,
	                   nullptr, pSymFunctionTableAccess, pSymGetModuleBase,
	                   nullptr)) {
		_outputBuffer->WriteLine(format, stackFrame.AddrFrame.Offset,
		                         addressToString(stackFrame.AddrPC.
		                                         Offset).c_str());
	}
}
#endif
void CrissCross::Debug::PrintStackTrace(CrissCross::IO::CoreIOWriter * _outputBuffer)
{
#if !defined (TARGET_OS_NETBSD) && !defined (TARGET_OS_FREEBSD) && !defined (TARGET_OS_OPENBSD) \
 && !defined(TARGET_COMPILER_MINGW) && !defined(TARGET_COMPILER_CYGWIN) && !defined(TARGET_OS_HAIKU)
#ifdef ENABLE_SYMBOL_ENGINE

	CONTEXT context = { CONTEXT_ALL };
	BOOL ret = ::GetThreadContext(GetCurrentThread(), &context);
	CoreAssert(ret != FALSE);
	RtlCaptureContext(&context);
	SymbolEngine::instance().StackTrace(&context, _outputBuffer);

#elif defined (ENABLE_BACKTRACE)
#if defined (TARGET_OS_MACOSX)
	backtrace_t backtrace;
	backtrace_symbols_t backtrace_symbols;
	backtrace = nasty_cast<backtrace_t, void*>(dlsym(RTLD_DEFAULT, "backtrace"));
	backtrace_symbols = nasty_cast<backtrace_symbols_t, void*>(dlsym(RTLD_DEFAULT, "backtrace_symbols"));
#endif

	void *array[256];
	int size;
	char * *strings;
	int i;

	memset(array, 0, sizeof(void *) * 256);

	/* use -rdynamic flag when compiling */
	size = backtrace(array, 256);
	strings = backtrace_symbols(array, size);

	_outputBuffer->WriteLine("Obtained %d stack frames.", size);

	std::string bt = "";

	for (i = 0; i < size; i++) {
#if defined (TARGET_OS_LINUX)
		int status;
		/* extract the identifier from strings[i].  It's inside of parens. */
		char *identifier = ::strchr(strings[i], '(');
		char *identifier_end = ::strchr(strings[i], '+');
		char *address = ::strchr(strings[i], '[');
		bt += address;
		if (identifier != 0 && identifier_end != 0 && identifier < identifier_end) {
			bt += " ";
			*identifier_end = '\0';
			char * realname = abi::__cxa_demangle(identifier + 1, 0, 0, &status);
			if (realname != nullptr) {
				bt += realname;
			} else {
				bt += (identifier + 1);
			}
			char *off_end = ::strchr(identifier_end + 1, ')');
			if (off_end) {
				*off_end = 0;
				bt += " + ";
				bt += (identifier_end + 1);
			}
			free(realname);
		}
#elif defined (TARGET_OS_MACOSX)
		char *addr = ::strstr(strings[i], "0x");
		char *mangled = ::strchr(addr, ' ') + 1;
		char *offset = ::strchr(addr, '+');
		char *postmangle = ::strchr(mangled, ' ');
		if (mangled) *(mangled - 1) = 0;
		bt += addr;
		int status;
		bt += ": ";
		if (addr && mangled) {
			if (postmangle)
				*postmangle = '\0';
			char *realname = abi::__cxa_demangle(mangled, 0, 0, &status);
			if (realname) {
				bt += realname;
			} else {
				bt += mangled;
			}
			bt += " ";
			bt += offset;
			free(realname);
		}
#endif
		bt += "\n";
	}

	_outputBuffer->WriteLine("%s\n", bt.c_str());

	free(strings);

#else /* defined(ENABLE_SYMBOL_ENGINE) || defined(ENABLE_BACKTRACE) */
	_outputBuffer->WriteLine("FAIL: backtrace() function not available.");
#endif

#else /* !defined (TARGET_OS_NETBSD) && !defined (TARGET_OS_FREEBSD) && !defined (TARGET_OS_OPENBSD) */
	_outputBuffer->WriteLine("Stack traces are not implemented for this platform.");
#endif
}
#ifndef USE_FAST_ASSERT
void Assert(bool _condition, const char *_testcase, const char *_file, int _line)
{
	if (!_condition) {
		char buffer[10240];
		sprintf(buffer, "Assertion failed : '%s'\nFile: %s\nLine: %d\n\n",
		        _testcase, _file, _line);
		fprintf(stderr, "%s", buffer);
		fprintf(stderr, "=== STACK TRACE ===\n");
		CoreIOWriter *stderror = new CoreIOWriter(stderr, false, CC_LN_LF);
		PrintStackTrace(stderror);
		delete stderror;
		#ifndef _DEBUG
		abort();
		#else
		_ASSERT(_condition);
		#endif
	}
}
#endif
