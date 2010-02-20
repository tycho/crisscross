/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2010 Steven Noonan.
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
#include <cxxabi.h>
#if defined(TARGET_OS_LINUX)
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

#pragma warning (disable: 4312)
#pragma comment( lib, "dbghelp" )

class SymbolEngine
{
	public:
		static SymbolEngine & instance();
#if TARGET_CPU_BITS == 32
		std::string addressToString(DWORD address);
#else
		std::string addressToString(DWORD64 address);
#endif
		void StackTrace(PCONTEXT _pContext, CoreIOWriter * _outputBuffer);
	private:
		SymbolEngine(SymbolEngine const &);
		SymbolEngine & operator=(SymbolEngine const &);
		SymbolEngine();
		HANDLE hProcess;
	public:
		~SymbolEngine();
	private:
};

SymbolEngine & SymbolEngine::instance()
{
	static SymbolEngine
	theEngine;

	return theEngine;
}

SymbolEngine::SymbolEngine()
{
	hProcess = GetCurrentProcess();

	DWORD dwOpts = SymGetOptions();

	dwOpts |= SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS;
	SymSetOptions(dwOpts);

	::SymInitialize(hProcess, 0, true);
}

SymbolEngine::~SymbolEngine()
{
	::SymCleanup(hProcess);
}

#if TARGET_CPU_BITS == 32
std::string SymbolEngine::addressToString(DWORD address)
#elif TARGET_CPU_BITS == 64
std::string SymbolEngine::addressToString(DWORD64 address)
#endif
{
	char buffer[4096];
#if TARGET_CPU_BITS == 32
	const char *format = "0x%08x";
#elif TARGET_CPU_BITS == 64
	const char *format = "0x%016x";
#endif

	/* First the raw address */
	sprintf(buffer, format, address);

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
	DWORD
#elif TARGET_CPU_BITS == 64
	DWORD64
#endif
	dwDisplacement;

	if (SymGetSymFromAddr(GetCurrentProcess(), address, &dwDisplacement, pSym)) {
		strcat(buffer, " ");
		strcat(buffer, pSym->Name);
		/*if ( dwDisplacement != 0 )
		 * oss << "+0x" << std::hex << dwDisplacement << std::dec; */
	}

	/* Finally any file/line number */
	IMAGEHLP_LINE
	lineInfo = { sizeof(IMAGEHLP_LINE) };
	if (SymGetLineFromAddr(GetCurrentProcess(), (DWORD)address, (PDWORD)&dwDisplacement, &lineInfo)) {
		const char *pDelim = strrchr(lineInfo.FileName, '\\');
		char temp[1024];
		sprintf(temp, " at %s(%u)", (pDelim ? pDelim + 1 : lineInfo.FileName), lineInfo.LineNumber);
		strcat(buffer, temp);
	}

	return std::string(buffer);
}

void SymbolEngine::StackTrace(PCONTEXT _pContext, CoreIOWriter * _outputBuffer)
{
#if TARGET_CPU_BITS == 32
	_outputBuffer->WriteLine("  Frame       Address   Code");
#else
	_outputBuffer->WriteLine("  Frame              Address            Code");
#endif

	STACKFRAME stackFrame = { 0 };

	stackFrame.AddrPC.Offset =
#if TARGET_CPU_BITS == 32
		_pContext->Eip;
#else
		_pContext->Rip;
#endif
	stackFrame.AddrPC.Mode = AddrModeFlat;

	stackFrame.AddrFrame.Offset =
#if TARGET_CPU_BITS == 32
		_pContext->Ebp;
#else
		_pContext->Rbp;
#endif
	stackFrame.AddrFrame.Mode = AddrModeFlat;

	stackFrame.AddrStack.Offset =
#if TARGET_CPU_BITS == 32
		_pContext->Esp;
#else
		_pContext->Rsp;
#endif
	stackFrame.AddrStack.Mode = AddrModeFlat;

#ifdef TARGET_CPU_X86
	DWORD machine = IMAGE_FILE_MACHINE_I386;
	const char *format = " 0x%08x %s";
#elif defined(TARGET_CPU_X64)
	DWORD machine = IMAGE_FILE_MACHINE_AMD64;
	const char *format = " 0x%016x %s";
#endif

	while (::StackWalk(machine, hProcess, GetCurrentThread(),        /* this value doesn't matter much if previous one is a real handle */
	                   &stackFrame,
	                   _pContext,
	                   NULL, ::SymFunctionTableAccess, ::SymGetModuleBase,
	                   NULL)) {
		_outputBuffer->WriteLine(format, stackFrame.AddrFrame.Offset,
		                         addressToString(stackFrame.AddrPC.
		                                         Offset).c_str());
	}
}
#endif
void CrissCross::Debug::PrintStackTrace(CrissCross::IO::CoreIOWriter * _outputBuffer)
{
#if !defined (TARGET_OS_NETBSD) && !defined (TARGET_OS_FREEBSD) && !defined (TARGET_OS_OPENBSD) \
 && !defined(TARGET_COMPILER_MINGW) && !defined(TARGET_COMPILER_CYGWIN)
#ifdef ENABLE_SYMBOL_ENGINE

	CONTEXT context = { CONTEXT_ALL };
	BOOL ret = ::GetThreadContext(GetCurrentThread(), &context);
	CoreAssert(ret != FALSE);
#ifdef TARGET_CPU_X64
	RtlCaptureContext(&context);
#elif defined(TARGET_CPU_X86)
	_asm call $ + 5;
	_asm pop eax;
	_asm mov context.Eip, eax;
	_asm mov eax, esp;
	_asm mov context.Esp, eax;
	_asm mov context.Ebp, ebp;
#endif
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
		if (identifier != 0 && identifier_end != 0 && identifier < identifier_end) {
			bt += address;
			bt += " ";
			*identifier_end = '\0';
			char * realname = abi::__cxa_demangle(identifier + 1, 0, 0, &status);
			if (realname != NULL) {
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
