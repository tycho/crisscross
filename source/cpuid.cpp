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

/*
 * Last updated 1/9/2008
 * using Intel CPUID documentation (AP-485) updated 12/2007
 */

#include <crisscross/universal_include.h>
#include <crisscross/string_utils.h>

#include <cmath>
#include <cstdio>
#include <cstring>

#define ENABLE_CPUID

/* Doesn't work on non-x86, and Cygwin doesn't have the functionality for cpu_set_t. */
#if !(defined (TARGET_CPU_X86) || defined (TARGET_CPU_X64)) || defined (TARGET_COMPILER_CYGWIN) || defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD)
#undef ENABLE_CPUID
#endif

/* FIXME: This is disabled due to inline asm complications. */
#if defined (TARGET_OS_WINDOWS) && !defined (TARGET_CPU_X86)
#undef ENABLE_CPUID
#endif

#ifdef ENABLE_CPUID

#ifdef TARGET_OS_WINDOWS
#include <windows.h>
#endif

//#define USE_CHUD_FOR_CPUID

#if defined(TARGET_OS_MACOSX)
#if defined(USE_CHUD_FOR_CPUID)
extern "C" {
int chudProcessorCount();
int utilBindThreadToCPU(int n);
int utilUnbindThreadFromCPU();
}
#else
#include <sys/types.h>
#include <sys/sysctl.h>
namespace {
	int getProcessorCount();
}
#endif
#endif

#ifdef TARGET_OS_FREEBSD
#include <pthread_np.h>
#include <sys/param.h>
#include <sys/cpuset.h>
#define CPUSET_T cpuset_t
#elif defined(TARGET_OS_LINUX)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <pthread.h>
#define CPUSET_T cpu_set_t
#endif

/* The following definition enables some rather suspicious cache descriptors */
/* from sandpile.org which haven't been verified with Intel's docs. */
/* #    define ENABLE_SANDPILE */

#include <crisscross/cpuid.h>
#include <crisscross/core_io.h>

using namespace CrissCross::System;

#if defined(TARGET_OS_MACOSX) && !defined(USE_CHUD_FOR_CPUID)
namespace {
	int getProcessorCount()
	{
		int     count;
		size_t  size = sizeof(count);

		if (sysctlbyname("hw.ncpu",&count,&size,NULL,0))
			return 1;

		return count;
	}
}
#endif

typedef enum
{
	REG_NULL,
	REG_EAX,
	REG_EBX,
	REG_ECX,
	REG_EDX
} Register;

typedef struct
{
	cc_uint32_t m_level;
	cc_uint8_t  m_register;
	cc_uint32_t m_bitmask;
	cc_uint32_t m_vendor;
	const char *m_name;
} CPUFeature;

static CPUFeature features [] = {
//  Standard (0000_0001h)
	{ 0x00000001, REG_EDX, 0x00000001, VENDOR_INTEL | VENDOR_AMD, "FPU"},
	{ 0x00000001, REG_EDX, 0x00000002, VENDOR_INTEL | VENDOR_AMD, "VME"},
	{ 0x00000001, REG_EDX, 0x00000004, VENDOR_INTEL | VENDOR_AMD, "DE"},
	{ 0x00000001, REG_EDX, 0x00000008, VENDOR_INTEL | VENDOR_AMD, "PSE"},
	{ 0x00000001, REG_EDX, 0x00000010, VENDOR_INTEL | VENDOR_AMD, "TSC"},
	{ 0x00000001, REG_EDX, 0x00000020, VENDOR_INTEL | VENDOR_AMD, "MSR"},
	{ 0x00000001, REG_EDX, 0x00000040, VENDOR_INTEL | VENDOR_AMD, "PAE"},
	{ 0x00000001, REG_EDX, 0x00000080, VENDOR_INTEL | VENDOR_AMD, "MCE"},
	{ 0x00000001, REG_EDX, 0x00000100, VENDOR_INTEL | VENDOR_AMD, "CX8"},
	{ 0x00000001, REG_EDX, 0x00000200, VENDOR_INTEL | VENDOR_AMD, "APIC"},
//	{ 0x00000001, REG_EDX, 0x00000400, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
	{ 0x00000001, REG_EDX, 0x00000800, VENDOR_INTEL | VENDOR_AMD, "SEP"},
	{ 0x00000001, REG_EDX, 0x00001000, VENDOR_INTEL | VENDOR_AMD, "MTRR"},
	{ 0x00000001, REG_EDX, 0x00002000, VENDOR_INTEL | VENDOR_AMD, "PGE"},
	{ 0x00000001, REG_EDX, 0x00004000, VENDOR_INTEL | VENDOR_AMD, "MCA"},
	{ 0x00000001, REG_EDX, 0x00008000, VENDOR_INTEL | VENDOR_AMD, "CMOV"},
	{ 0x00000001, REG_EDX, 0x00010000, VENDOR_INTEL | VENDOR_AMD, "PAT"},
	{ 0x00000001, REG_EDX, 0x00020000, VENDOR_INTEL | VENDOR_AMD, "PSE-36"},
	{ 0x00000001, REG_EDX, 0x00040000, VENDOR_INTEL             , "PSN"},
	{ 0x00000001, REG_EDX, 0x00080000, VENDOR_INTEL | VENDOR_AMD, "CLFSH"},
//	{ 0x00000001, REG_EDX, 0x00100000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
	{ 0x00000001, REG_EDX, 0x00200000, VENDOR_INTEL             , "DS"},
	{ 0x00000001, REG_EDX, 0x00400000, VENDOR_INTEL             , "ACPI"},
	{ 0x00000001, REG_EDX, 0x00800000, VENDOR_INTEL | VENDOR_AMD, "MMX"},
	{ 0x00000001, REG_EDX, 0x01000000, VENDOR_INTEL | VENDOR_AMD, "FXSR"},
	{ 0x00000001, REG_EDX, 0x02000000, VENDOR_INTEL | VENDOR_AMD, "SSE"},
	{ 0x00000001, REG_EDX, 0x04000000, VENDOR_INTEL | VENDOR_AMD, "SSE2"},
	{ 0x00000001, REG_EDX, 0x08000000, VENDOR_INTEL             , "SS"},
	{ 0x00000001, REG_EDX, 0x10000000, VENDOR_INTEL | VENDOR_AMD, "HTT"},
	{ 0x00000001, REG_EDX, 0x20000000, VENDOR_INTEL             , "TM"},
//	{ 0x00000001, REG_EDX, 0x40000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
	{ 0x00000001, REG_EDX, 0x80000000, VENDOR_INTEL             , "PBE"},

	{ 0x00000001, REG_ECX, 0x00000001, VENDOR_INTEL | VENDOR_AMD, "SSE3"},
	{ 0x00000001, REG_ECX, 0x00000002, VENDOR_INTEL             , "PCLMULDQ"},
	{ 0x00000001, REG_ECX, 0x00000004, VENDOR_INTEL             , "DTES64"},
	{ 0x00000001, REG_ECX, 0x00000008, VENDOR_INTEL | VENDOR_AMD, "MONITOR"},
	{ 0x00000001, REG_ECX, 0x00000010, VENDOR_INTEL             , "DS-CPL"},
	{ 0x00000001, REG_ECX, 0x00000020, VENDOR_INTEL             , "VMX"},
	{ 0x00000001, REG_ECX, 0x00000040, VENDOR_INTEL             , "SMX"},
	{ 0x00000001, REG_ECX, 0x00000080, VENDOR_INTEL             , "EST"},
	{ 0x00000001, REG_ECX, 0x00000100, VENDOR_INTEL             , "TM2"},
	{ 0x00000001, REG_ECX, 0x00000200, VENDOR_INTEL | VENDOR_AMD, "SSSE3"},
	{ 0x00000001, REG_ECX, 0x00000400, VENDOR_INTEL             , "CNXT-ID"},
//	{ 0x00000001, REG_ECX, 0x00000800, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x00000001, REG_ECX, 0x00001000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
	{ 0x00000001, REG_ECX, 0x00002000, VENDOR_INTEL | VENDOR_AMD, "CX16"},
	{ 0x00000001, REG_ECX, 0x00004000, VENDOR_INTEL             , "xTPR"},
	{ 0x00000001, REG_ECX, 0x00008000, VENDOR_INTEL             , "PDCM"},
//	{ 0x00000001, REG_ECX, 0x00010000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x00000001, REG_ECX, 0x00020000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
	{ 0x00000001, REG_ECX, 0x00040000, VENDOR_INTEL             , "DCA"},
	{ 0x00000001, REG_ECX, 0x00080000, VENDOR_INTEL | VENDOR_AMD, "SSE4.1"},
	{ 0x00000001, REG_ECX, 0x00100000, VENDOR_INTEL             , "SSE4.2"},
	{ 0x00000001, REG_ECX, 0x00200000, VENDOR_INTEL             , "x2APIC"},
	{ 0x00000001, REG_ECX, 0x00400000, VENDOR_INTEL             , "MOVBE"},
	{ 0x00000001, REG_ECX, 0x00800000, VENDOR_INTEL | VENDOR_AMD, "POPCNT"},
//	{ 0x00000001, REG_ECX, 0x01000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
	{ 0x00000001, REG_ECX, 0x02000000, VENDOR_INTEL             , "AES"},
	{ 0x00000001, REG_ECX, 0x04000000, VENDOR_INTEL             , "XSAVE"},
	{ 0x00000001, REG_ECX, 0x08000000, VENDOR_INTEL             , "OSXSAVE"},
//	{ 0x00000001, REG_ECX, 0x10000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x00000001, REG_ECX, 0x20000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x00000001, REG_ECX, 0x40000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x00000001, REG_ECX, 0x80000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved

//  Extended (8000_0001h)
//	{ 0x80000001, REG_EDX, 0x00000001, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00000002, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00000004, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00000008, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00000010, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00000020, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00000040, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00000080, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00000100, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00000200, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00000400, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
	{ 0x80000001, REG_EDX, 0x00000800, VENDOR_INTEL | VENDOR_AMD, "SYSCALL"},
//	{ 0x80000001, REG_EDX, 0x00001000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00002000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00004000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00008000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00010000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00020000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00040000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x00080000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
	{ 0x80000001, REG_EDX, 0x00100000, VENDOR_INTEL             , "XD"},
	{ 0x80000001, REG_EDX, 0x00100000,                VENDOR_AMD, "NX"},
//	{ 0x80000001, REG_EDX, 0x00200000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
	{ 0x80000001, REG_EDX, 0x00400000,                VENDOR_AMD, "MMXEXT"},
//	{ 0x80000001, REG_EDX, 0x00800000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_EDX, 0x01000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
	{ 0x80000001, REG_EDX, 0x02000000,                VENDOR_AMD, "FFXSR"},
//	{ 0x80000001, REG_EDX, 0x04000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
	{ 0x80000001, REG_EDX, 0x08000000,                VENDOR_AMD, "RDTSCP"},
//	{ 0x80000001, REG_EDX, 0x10000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
	{ 0x80000001, REG_EDX, 0x20000000, VENDOR_INTEL             , "EM64T"},
	{ 0x80000001, REG_EDX, 0x20000000,                VENDOR_AMD, "LM"},
	{ 0x80000001, REG_EDX, 0x40000000,                VENDOR_AMD, "3DNOWEXT"},
	{ 0x80000001, REG_EDX, 0x80000000,                VENDOR_AMD, "3DNOW"},

	{ 0x80000001, REG_ECX, 0x00000001, VENDOR_INTEL | VENDOR_AMD, "LAHF"},
	{ 0x80000001, REG_ECX, 0x00000002,                VENDOR_AMD, "CL"},
	{ 0x80000001, REG_ECX, 0x00000004,                VENDOR_AMD, "SVM"},
	{ 0x80000001, REG_ECX, 0x00000008,                VENDOR_AMD, "EAS"},
	{ 0x80000001, REG_ECX, 0x00000010,                VENDOR_AMD, "AMC8"},
	{ 0x80000001, REG_ECX, 0x00000020,                VENDOR_AMD, "ABM"},
	{ 0x80000001, REG_ECX, 0x00000040,                VENDOR_AMD, "SSE4A"},
	{ 0x80000001, REG_ECX, 0x00000080,                VENDOR_AMD, "MAS"},
	{ 0x80000001, REG_ECX, 0x00000100,                VENDOR_AMD, "3DNP"},
	{ 0x80000001, REG_ECX, 0x00000200,                VENDOR_AMD, "OSVW"},
	{ 0x80000001, REG_ECX, 0x00000400,                VENDOR_AMD, "IBS"},
	{ 0x80000001, REG_ECX, 0x00000800,                VENDOR_AMD, "SSE5"},
	{ 0x80000001, REG_ECX, 0x00001000,                VENDOR_AMD, "SKINIT"},
	{ 0x80000001, REG_ECX, 0x00002000,                VENDOR_AMD, "WDT"},
//	{ 0x80000001, REG_ECX, 0x00004000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x00008000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x00010000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x00020000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x00040000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x00080000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x00100000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x00200000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x00400000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x00800000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x01000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x02000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x04000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x08000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x10000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x20000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x40000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved
//	{ 0x80000001, REG_ECX, 0x80000000, VENDOR_INTEL | VENDOR_AMD, ""},    // Reserved

	{ 0, REG_NULL, 0, 0, NULL},
};

namespace CrissCross
{
	namespace System
	{
		struct Registers
		{
			unsigned int eax;
			unsigned int ebx;
			unsigned int ecx;
			unsigned int edx;
		};

		struct Registers *Std;
		unsigned int StdMax;
		struct Registers *Ext;
		unsigned int ExtMax;

		/* If the current processor supports the CPUID instruction, execute
		 * one, with REQUEST in %eax, and set *EAX, *EBX, *ECX, and *EDX to
		 * the values the 'cpuid' stored in those registers.  Return true if
		 * the current processor supports CPUID, false otherwise.  */
		namespace
		{
			struct GoThreadProc_Params
			{
				CPUID *cpuid_class;
				int processor;
			};

#ifdef TARGET_OS_WINDOWS
			static DWORD CALLBACK s_GoThreadProc(LPVOID lpParameter)
			{
				GoThreadProc_Params *params = (GoThreadProc_Params *)lpParameter;
				return params->cpuid_class->GoThread(params->processor);
			}
#endif

			bool call_cpuid(unsigned int request, unsigned int *_eax, unsigned int *_ebx, unsigned int *_ecx, unsigned int *_edx)
			{
#ifndef TARGET_CPU_X64
				unsigned int pre_change, post_change;
				const unsigned int id_flag = 0x200000;
#endif

				/* This is pretty much the standard way to detect whether the CPUID
				 *     instruction is supported: try to change the ID bit in the EFLAGS
				 *     register.  If we can change it, then the CPUID instruction is
				 *     implemented.  */
#ifndef TARGET_CPU_X64
#if defined (TARGET_COMPILER_GCC)
				asm ("pushfl\n\t"          /* Save %eflags to restore later.  */
					 "pushfl\n\t"          /* Push second copy, for manipulation.  */
					 "popl %1\n\t"         /* Pop it into post_change.  */
					 "movl %1,%0\n\t"      /* Save copy in pre_change.   */
					 "xorl %2,%1\n\t"      /* Tweak bit in post_change.  */
					 "pushl %1\n\t"                /* Push tweaked copy... */
					 "popfl\n\t"           /* ... and pop it into %eflags.  */
					 "pushfl\n\t"          /* Did it change?  Push new %eflags... */
					 "popl %1\n\t"         /* ... and pop it into post_change.  */
					 "popfl"                     /* Restore original value.  */
					 : "=&r" (pre_change), "=&r" (post_change)
					 : "ir" (id_flag));
#else
				__asm {
					mov edx, id_flag;
					pushfd;                         /* Save %eflags to restore later.  */
					pushfd;                         /* Push second copy, for manipulation.  */
					pop ebx;                       /* Pop it into post_change.  */
					mov eax, ebx;          /* Save copy in pre_change.   */
					xor ebx, edx;           /* Tweak bit in post_change.  */
					push ebx;                       /* Push tweaked copy... */
					popfd;                          /* ... and pop it into eflags.  */
					pushfd;                         /* Did it change?  Push new %eflags... */
					pop ebx;                       /* ... and pop it into post_change.  */
					popfd;                          /* Restore original value.  */
					mov pre_change, eax;
					mov post_change, ebx;
				}
#endif
#endif
	
				/* If the bit changed, then we support the CPUID instruction.  */
#ifndef TARGET_CPU_X64
				if ((pre_change ^ post_change) & id_flag) {
#endif
#if defined (TARGET_COMPILER_GCC)
				asm volatile ("mov %%ebx, %%esi\n\t"     /* Save %ebx.  */
							  "xorl %%ecx, %%ecx\n\t"
							  "cpuid\n\t"
							  "xchgl %%ebx, %%esi"           /* Restore %ebx.  */
						  : "=a" (*_eax), "=S" (*_ebx), "=c" (*_ecx), "=d" (*_edx)
						  : "0" (request)
						  : "memory");
#else
				__asm {
					push esi;
					push edi;
					xor ecx, ecx;
					mov eax, request;
					cpuid;
					mov edi, [_eax];
					mov esi, [_ebx];
					mov[edi], eax;
					mov[esi], ebx;
					mov edi, [_ecx];
					mov esi, [_edx];
					mov[edi], ecx;
					mov[esi], edx;
					pop edi;
					pop esi;
				}
#endif
				return true;
#ifndef TARGET_CPU_X64
			} else
				return false;
#endif
			}
		}

		X86Processor::X86Processor()
		: m_vendor(VENDOR_UNKNOWN),
		  m_name(NULL),
		  m_logical(0),
		  m_cores(0),
		  m_family(0),
		  m_model(0),
		  m_stepping(0),
		  m_brandID(0),
		  m_apicID(0)
		{
		}

		X86Processor::X86Processor(X86Processor const &_copy)
		: m_vendor(_copy.m_vendor),
		  m_name(cc_newstr(_copy.m_name)),
		  m_logical(_copy.m_logical),
		  m_cores(_copy.m_cores),
		  m_family(_copy.m_family),
		  m_model(_copy.m_model),
		  m_stepping(_copy.m_stepping),
		  m_brandID(_copy.m_brandID),
		  m_apicID(_copy.m_apicID)
		{
			CrissCross::Data::DArray<const char *> *features = _copy.m_features.ConvertIndexToDArray();
			for (size_t i = 0; i < features->size(); i++) {
				if (!features->valid(i)) continue;
				m_features.insert(features->get(i), NULL);
			}
			delete features;

			m_caches.setSize(_copy.m_caches.size());
			for (size_t i = 0; i < _copy.m_caches.size(); i++)
			{
				if (!_copy.m_caches.valid(i)) continue;
				m_caches.insert(cc_newstr(_copy.m_caches.get(i)));
			}
		}

		X86Processor::~X86Processor()
		{
		}

		void X86Processor::Print(CrissCross::IO::CoreIOWriter *_writer) const
		{
			/* Print the Manufacturer tag if it was read properly. */
			switch(Manufacturer()) {
			case VENDOR_INTEL:
				_writer->WriteLine("CPU[%u] Manufacturer: %s", m_index, "GenuineIntel");
				break;
			case VENDOR_AMD:
				_writer->WriteLine("CPU[%u] Manufacturer: %s", m_index, "AuthenticAMD");
				break;
			default:
				/* Don't print anything. */
				break;
			}

			/* Print the Name tag if it was read properly. */
			if (Name() && strlen(Name()) > 1)
				_writer->WriteLine("CPU[%u] Name: %s", m_index, Name());

			if (Family() > 0 && Model() > 0 && Stepping() > 0)
				_writer->WriteLine("CPU[%u] Family %2d Model %2d Stepping %2d", m_index,
					Family(), Model(), Stepping());

			/* Print out the CPU cache info. */
			const CrissCross::System::caches_t *caches = Caches();
			if (caches->size() > 0) {
				_writer->WriteLine("CPU[%u] Caches:", m_index);
				for (size_t j = 0; j < caches->size(); j++) {
					if (caches->valid(j))
						_writer->Write("  %s\n", caches->get(j));
				}

				_writer->WriteLine();
			}

			/* Print out CPU features (MMX, SSE, and so on). */
			if (Features()->size() > 0) {
				_writer->Write("CPU[%u] Features: ", m_index);

				CrissCross::Data::DArray<const char *> *featureIDs =
						Features()->ConvertIndexToDArray();

				for (size_t i = 0; i < featureIDs->size(); i++) {
					if (featureIDs->valid(i))
						_writer->Write("%s ", featureIDs->get(i));
				}

				delete featureIDs;

				_writer->WriteLine();
			}

			_writer->WriteLine();
		}

		CPUVendor X86Processor::Manufacturer() const
		{
			return m_vendor;
		}

		const char *X86Processor::Name() const
		{
			return m_name;
		}

		char X86Processor::Family() const
		{
			return m_family;
		}

		char X86Processor::Model() const
		{
			return m_model;
		}

		char X86Processor::Stepping() const
		{
			return m_stepping;
		}

		char X86Processor::BrandID() const
		{
			return m_brandID;
		}

		char X86Processor::APICID() const
		{
			return m_apicID;
		}

		const features_t *X86Processor::Features() const
		{
			return &m_features;
		}

		const caches_t *X86Processor::Caches() const
		{
			return &m_caches;
		}

		CPUID::CPUID()
		{
			unsigned int i = 0;

			Std = new Registers[32];
			CoreAssert(Std);
			Ext = new Registers[32];
			CoreAssert(Ext);

			memset(Std, 0, sizeof(Registers) * 32);
			memset(Ext, 0, sizeof(Registers) * 32);

			StdMax = 0;
			ExtMax = 0;

			/* Basic initializations complete */

			call_cpuid(0, &Std[0].eax, &Std[0].ebx, &Std[0].ecx, &Std[0].edx);
			StdMax = Std[0].eax;

			if (StdMax > 31) {
#ifdef CPUID_DEBUG
				g_console->WriteLine("CPUID.0.EAX has an invalid value: %d",
				                     StdMax);
#endif
				StdMax = 31;
			} else {
#ifdef CPUID_DEBUG
				g_console->WriteLine("Standard CPUID maximum input is %d.",
				                     StdMax);
#endif
			}

			for (i = 1; i <= StdMax; i++) {
				call_cpuid(i, &Std[i].eax, &Std[i].ebx, &Std[i].ecx, &Std[i].edx);
			}

			call_cpuid(0x80000000, &Ext[0].eax, &Ext[0].ebx, &Ext[0].ecx, &Ext[0].edx);

			if (Ext[0].eax < 0x80000004) {
				return;
			} else {
				ExtMax = Ext[0].eax - 0x80000000;
			}

			for (i = 0; i <= ExtMax; i++) {
				call_cpuid(0x80000000 + i, &Ext[i].eax, &Ext[i].ebx, &Ext[i].ecx, &Ext[i].edx);
			}
		}

		CPUID::~CPUID()
		{
			/* Time to deallocate all the memory we allocated. */
			size_t i = 0, j = 0;

			delete [] Std;
			delete [] Ext;
			for (i = 0; i < proc.size(); i++) {
				if (!proc.valid(i)) continue;

				for (j = 0; j < proc[i]->m_caches.size(); j++) {
					if (proc[i]->m_caches.valid(j))
						delete [] proc[i]->m_caches.get(j);
				}

				delete [] (char *)proc[i]->m_name;

				delete proc[i];
				proc.remove(i);
			}
		}

		int CPUID::VirtualCount()
		{
			CoreAssert(this != NULL);

			int count = 0;

			for (size_t i = 0; i < proc.size(); i++) {
				if (proc.valid(i))
					count++;
			}

			return count;
		}

		int CPUID::CoresPerPackage()
		{
			CoreAssert(this != NULL);
			return proc[0]->m_cores;
		}

		int CPUID::LogicalPerPackage()
		{
			CoreAssert(this != NULL);
			return proc[0]->m_logical;
		}

		long int CPUID::GoThread(int processor)
		{
			CoreAssert(this != NULL);

			if (processor < 0) {
				return 1;
			}

			DetectManufacturer(processor);
			DetectProcessorName(processor);
			DetectFeatures(processor);
			DetectCacheInfo(processor);
			DetectFMS(processor);
			DetectBrandID(processor);
			DetectCount(processor);
			DetectAPIC(processor);
			return 0;
		}

		void CPUID::Go()
		{
			CoreAssert(this != NULL);
#ifdef TARGET_OS_WINDOWS
			DWORD dThread = NULL;
			SYSTEM_INFO siSystem;
			int iCount = 0;
			struct GoThreadProc_Params params;

			params.cpuid_class = this;
			GetSystemInfo(&siSystem);

			iCount = siSystem.dwNumberOfProcessors;

			for (params.processor = 0; params.processor < iCount;
			     params.processor++) {
				proc.insert(new X86Processor(), params.processor);
				HANDLE hThread =
				        CreateThread(NULL, 0, ( LPTHREAD_START_ROUTINE )s_GoThreadProc,
				                     &params, CREATE_SUSPENDED, &dThread);
				SetThreadAffinityMask(hThread,
				                      ( DWORD )pow(( double )2,
				                                   ( double )params.
				                                   processor));
				Sleep(0);                    /* Wait for affinity switch. */
				SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
				ResumeThread(hThread);
				WaitForSingleObject(hThread, INFINITE);
				proc[params.processor]->m_index = params.processor;
			}

#elif defined (TARGET_OS_LINUX) || defined(TARGET_OS_FREEBSD)
			int NUM_PROCS = sysconf(_SC_NPROCESSORS_CONF), i;
			CPUSET_T mask, originalmask;
			pthread_t pth = pthread_self();

			CPU_ZERO(&originalmask);
			pthread_getaffinity_np(pth, sizeof(originalmask), &originalmask);
			for (i = 0; i < NUM_PROCS; i++) {
				proc.insert(new X86Processor(), i);
				CPU_ZERO(&mask);
				CPU_SET(( int )pow(2, i), &mask);
				pthread_setaffinity_np(pth, sizeof(mask), &mask);
				GoThread(i);
				proc[i]->m_index = i;
			}

			pthread_setaffinity_np(pth, sizeof(originalmask), &originalmask);
#elif defined (TARGET_OS_MACOSX)
#if defined(USE_CHUD_FOR_CPUID)
			int NUM_PROCS = chudProcessorCount();
			for (int i = 0; i < NUM_PROCS; i++) {
				proc.insert(new X86Processor(), i);
				utilUnbindThreadFromCPU();
				utilBindThreadToCPU(i);
				GoThread(i);
				procs[i]->m_index = i;
			}
			utilUnbindThreadFromCPU();
#else
			int NUM_PROCS = getProcessorCount();
			proc.insert(new X86Processor(), 0);
			GoThread(0);
			proc[0]->m_index = 0;
			for (int i = 1; i < NUM_PROCS; i++) {
				proc.insert(new X86Processor(*proc[0]), i);
				proc[i]->m_index = i;
			}
#endif
#endif
		}

		void CPUID::DetectManufacturer(int processor)
		{
			CoreAssert(this != NULL);
			char manufacturer[13];
			char *_man = manufacturer;

			memcpy(_man, &Std[0].ebx, 4);
			_man += 4;
			memcpy(_man, &Std[0].edx, 4);
			_man += 4;
			memcpy(_man, &Std[0].ecx, 4);
			_man += 4;
			*_man = '\x0';
			
			proc[processor]->m_vendor = VENDOR_UNKNOWN;
			if (strcmp(manufacturer, "GenuineIntel") == 0)
				proc[processor]->m_vendor = VENDOR_INTEL;
			else if (strcmp(manufacturer, "AuthenticAMD") == 0)
				proc[processor]->m_vendor = VENDOR_AMD;
		}

		char *squeeze(char *str)
		{
			int r; /* next character to be read */
			int w; /* next character to be written */

			r=w=0;
			while (str[r])
			{
				if (isspace(str[r]) || iscntrl(str[r]))
				{
					if (w > 0 && !isspace(str[w-1]))
						str[w++] = ' ';
				}
				else
					str[w++] = str[r];
				r++;
			}
			str[w] = 0;
			return str;
		}

		void CPUID::DetectProcessorName(int processor)
		{
			CoreAssert(this != NULL);

			char *processorname = new char[(4 * 12) + 1];
			char *_proc = &processorname[0];

			memcpy(_proc, &Ext[2].eax, 4);
			_proc += 4;
			memcpy(_proc, &Ext[2].ebx, 4);
			_proc += 4;
			memcpy(_proc, &Ext[2].ecx, 4);
			_proc += 4;
			memcpy(_proc, &Ext[2].edx, 4);
			_proc += 4;
			memcpy(_proc, &Ext[3].eax, 4);
			_proc += 4;
			memcpy(_proc, &Ext[3].ebx, 4);
			_proc += 4;
			memcpy(_proc, &Ext[3].ecx, 4);
			_proc += 4;
			memcpy(_proc, &Ext[3].edx, 4);
			_proc += 4;
			memcpy(_proc, &Ext[4].eax, 4);
			_proc += 4;
			memcpy(_proc, &Ext[4].ebx, 4);
			_proc += 4;
			memcpy(_proc, &Ext[4].ecx, 4);
			_proc += 4;
			memcpy(_proc, &Ext[4].edx, 4);
			_proc += 4;
			*_proc = '\x0';
			proc[processor]->m_name = squeeze(processorname);
		}

		void CPUID::DetectCacheInfo(int processor)
		{
			CoreAssert(this != NULL);

			if (proc[processor]->m_vendor != VENDOR_UNKNOWN) {
				if (proc[processor]->m_vendor == VENDOR_INTEL ) {
					int ntlb = 255, i;

					for (i = 0; i < ntlb; i++) {
						ntlb = Std[2].eax & 0xff;

						AddIntelCacheData(processor, Std[2].eax >> 8);
						AddIntelCacheData(processor, Std[2].eax >> 16);
						AddIntelCacheData(processor, Std[2].eax >> 24);

						if ((Std[2].ebx & 0x80000000) == 0) {
							AddIntelCacheData(processor, Std[2].ebx);
							AddIntelCacheData(processor, Std[2].ebx >> 8);
							AddIntelCacheData(processor, Std[2].ebx >> 16);
							AddIntelCacheData(processor, Std[2].ebx >> 24);
						}

						if ((Std[2].ecx & 0x80000000) == 0) {
							AddIntelCacheData(processor, Std[2].ecx);
							AddIntelCacheData(processor, Std[2].ecx >> 8);
							AddIntelCacheData(processor, Std[2].ecx >> 16);
							AddIntelCacheData(processor, Std[2].ecx >> 24);
						}

						if ((Std[2].edx & 0x80000000) == 0) {
							AddIntelCacheData(processor, Std[2].edx);
							AddIntelCacheData(processor, Std[2].edx >> 8);
							AddIntelCacheData(processor, Std[2].edx >> 16);
							AddIntelCacheData(processor, Std[2].edx >> 24);
						}
					}
				} else if (proc[processor]->m_vendor == VENDOR_AMD) {
					DecodeAMDCacheIdentifiers(processor);
				}
			}

			CrissCross::Data::QuickSort<char *> sorter;
			proc[processor]->m_caches.sort(sorter);
		}

		const char *CPUID::CreateCacheDescription(cacheType _type, const char *_pages, unsigned int _size, unsigned int _assoc, unsigned int _entries, unsigned int _linesize, bool _sectored)
		{
			CoreAssert(this != NULL);

			static char description[512];
			char assoc[64], prefix[64], size[32], sectored[32], linesz[32], entries[32];

			/* No associativity? Invalid cache entry. Abort, abort! */
			if (_assoc == 0)
				return NULL;

			memset(prefix, 0, sizeof(prefix));
			/* Get the prefix worked out. */
			switch (_type)
			{
			case CACHE_TYPE_TRACE:
				sprintf(prefix, "Trace cache: ");
				break;
			case CACHE_TYPE_L0DATA_TLB:
				sprintf(prefix, "L0 Data TLB: %s pages, ", _pages);
				break;
			case CACHE_TYPE_L1DATA_TLB:
				sprintf(prefix, "L1 Data TLB: %s pages, ", _pages);
				break;
			case CACHE_TYPE_L2DATA_TLB:
				sprintf(prefix, "L2 Data TLB: %s pages, ", _pages);
				break;
			case CACHE_TYPE_L0CODE_TLB:
				sprintf(prefix, "L0 Code TLB: %s pages, ", _pages);
				break;
			case CACHE_TYPE_L1CODE_TLB:
				sprintf(prefix, "L1 Code TLB: %s pages, ", _pages);
				break;
			case CACHE_TYPE_L2CODE_TLB:
				sprintf(prefix, "L2 Code TLB: %s pages, ", _pages);
				break;
			case CACHE_TYPE_DATA_TLB:
				sprintf(prefix, "Data TLB: %s pages, ", _pages);
				break;
			case CACHE_TYPE_CODE_TLB:
				sprintf(prefix, "Code TLB: %s pages, ", _pages);
				break;
			case CACHE_TYPE_L1CODE:
				sprintf(prefix, "1st-level code cache: ");
				break;
			case CACHE_TYPE_L1DATA:
				sprintf(prefix, "1st-level data cache: ");
				break;
			case CACHE_TYPE_L2:
				sprintf(prefix, "2nd-level cache: ");
				break;
			case CACHE_TYPE_L3:
				sprintf(prefix, "3rd-level cache: ");
				break;
			}

			/* Figure out the appropriate size descriptor. */
			memset(size, 0, sizeof(size));
			switch (_type)
			{
			case CACHE_TYPE_L0DATA_TLB:
			case CACHE_TYPE_L1DATA_TLB:
			case CACHE_TYPE_L2DATA_TLB:
			case CACHE_TYPE_L0CODE_TLB:
			case CACHE_TYPE_L1CODE_TLB:
			case CACHE_TYPE_L2CODE_TLB:
			case CACHE_TYPE_DATA_TLB:
			case CACHE_TYPE_CODE_TLB:
				break;
			case CACHE_TYPE_TRACE:
				sprintf(size, "%dK-uops, ", _size);
				break;
			default:
				if (_size >= 1024)
					sprintf(size, "%dMB, ", _size / 1024);
				else
					sprintf(size, "%dKB, ", _size);
			}

			/* Get the associativity part set up */
			switch (_assoc)
			{
			case 0x01: sprintf(assoc, "direct mapped"); break;
			case 0xFF: sprintf(assoc, "fully associative"); break;
			default: sprintf(assoc, "%d-way set associative", _assoc); break;
			}

			/* If this is a TLB, we have entries */
			if (_entries)
				sprintf(entries, ", %d entries", _entries);
			else
				memset(entries, 0, sizeof(entries));

			/* Is it a sectored cache? */
			sprintf(sectored, "%s", _sectored ? ", sectored cache" : "");

			/* If there's a line size, we append it. */
			if (_linesize)
				sprintf(linesz, ", %d byte line size", _linesize);
			else
				memset(linesz, 0, sizeof(linesz));

			sprintf(description, "%s%s%s%s%s%s\n", prefix, size, assoc, entries, sectored, linesz);
			return description;
		}

		void CPUID::DecodeAMDCacheIdentifiers(int processor)
		{
			CoreAssert(this != NULL);

			/* L1 Cache Information */
			unsigned int L1DTlb2and4MAssoc, L1DTlb2and4MSize, L1ITlb2and4MAssoc, L1ITlb2and4MSize;
			unsigned int L1DTlb4KAssoc, L1DTlb4KSize, L1ITlb4KAssoc, L1ITlb4KSize;
			unsigned int L1DcSize, L1DcAssoc, L1DcLinesPerTag, L1DcLineSize;
			unsigned int L1IcSize, L1IcAssoc, L1IcLinesPerTag, L1IcLineSize;


			/* */
			/* L1 Data 2MB/4MB TLB */

			L1DTlb2and4MAssoc = (Ext[5].eax & 0xFF000000) >> 24;
			L1DTlb2and4MSize = (Ext[5].eax & 0x00FF0000) >> 16;
			AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_DATA_TLB, "2MB or 4MB", 0, L1DTlb2and4MAssoc, L1DTlb2and4MSize, 0, false));


			/* */
			/* L1 Code 2MB/4MB TLB */

			L1ITlb2and4MAssoc = (Ext[5].eax & 0x0000FF00) >> 8;
			L1ITlb2and4MSize = (Ext[5].eax & 0x000000FF);
			AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_CODE_TLB, "2MB or 4MB", 0, L1ITlb2and4MAssoc, L1ITlb2and4MSize, 0, false));


			/* */
			/* L1 Data 4KB TLB */

			L1DTlb4KAssoc = (Ext[5].ebx & 0xFF000000) >> 24;
			L1DTlb4KSize = (Ext[5].ebx & 0x00FF0000) >> 16;
			AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_DATA_TLB, "4KB", 0, L1DTlb4KAssoc, L1DTlb4KSize, 0, false));


			/* */
			/* L1 Code 4KB TLB */

			L1ITlb4KAssoc = (Ext[5].ebx & 0x0000FF00) >> 8;
			L1ITlb4KSize = (Ext[5].ebx & 0x000000FF);
			AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_CODE_TLB, "4KB", 0, L1ITlb4KAssoc, L1ITlb4KSize, 0, false));


			/* */
			/* L1 Data Cache */

			L1DcSize = (Ext[5].ecx & 0xFF000000) >> 24;
			L1DcAssoc = (Ext[5].ecx & 0x00FF0000) >> 16;
			L1DcLinesPerTag = (Ext[5].ecx & 0x0000FF00) >> 8;
			L1DcLineSize = (Ext[5].ecx & 0x000000FF);
			AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1DATA, NULL, L1DcSize, L1DcAssoc, 0, L1DcLineSize, false));


			/* */
			/* L1 Code Cache */

			L1IcSize = (Ext[5].edx & 0xFF000000) >> 24;
			L1IcAssoc = (Ext[5].edx & 0x00FF0000) >> 16;
			L1IcLinesPerTag = (Ext[5].edx & 0x0000FF00) >> 8;
			L1IcLineSize = (Ext[5].edx & 0x000000FF);
			AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1CODE, NULL, L1IcSize, L1IcAssoc, 0, L1IcLineSize, false));



			/* L2 Cache Information */
			unsigned int L2DTlb2and4MAssoc, L2DTlb2and4MSize, L2ITlb2and4MAssoc, L2ITlb2and4MSize;
			unsigned int L2DTlb4KAssoc, L2DTlb4KSize, L2ITlb4KAssoc, L2ITlb4KSize;
			unsigned int L2Size, L2Assoc, L2LinesPerTag, L2LineSize;

			/* */
			/* L2 Data 2MB/4MB TLB */

			L2DTlb2and4MAssoc = (Ext[6].eax & 0xF0000000) >> 28;
			L2DTlb2and4MSize = (Ext[6].eax & 0x0FFF0000) >> 16;
			AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2DATA_TLB, "2MB or 4MB", 0, L2DTlb2and4MAssoc, L2DTlb2and4MSize, 0, false));


			/* */
			/* L2 Code 2MB/4MB TLB */

			L2ITlb2and4MAssoc = (Ext[6].eax & 0x0000F000) >> 12;
			L2ITlb2and4MSize = (Ext[6].eax & 0x00000FFF);
			AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2CODE_TLB, "2MB or 4MB", 0, L2ITlb2and4MAssoc, L2ITlb2and4MSize, 0, false));


			/* */
			/* L2 Data 4KB TLB */

			L2DTlb4KAssoc = (Ext[6].ebx & 0xF0000000) >> 28;
			L2DTlb4KSize = (Ext[6].ebx & 0x0FFF0000) >> 16;
			AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2DATA_TLB, "4KB", 0, L2DTlb4KAssoc, L2DTlb4KSize, 0, false));


			/* */
			/* L2 Code 4KB TLB */

			L2ITlb4KAssoc = (Ext[6].ebx & 0x0000F000) >> 12;
			L2ITlb4KSize = (Ext[6].ebx & 0x00000FFF);
			AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2CODE_TLB, "4KB", 0, L2ITlb4KAssoc, L2ITlb4KSize, 0, false));


			/* */
			/* L2 Cache */

			L2Size = (Ext[6].ecx & 0xFFFF0000) >> 16;
			L2Assoc = (Ext[6].ecx & 0x0000F000) >> 12;
			L2LinesPerTag = (Ext[6].ecx & 0x00000F00) >> 8;
			L2LineSize = (Ext[6].ecx & 0x000000FF);
			AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, L2Size, L2Assoc, 0, L2LineSize, false));



			/* L3 Cache Information */
			unsigned int L3Size, L3Assoc, L3LinesPerTag, L3LineSize;

			/* */
			/* L3 Cache */

			L3Size = (Ext[6].edx & 0xFFFC0000) >> 18;
			L3Assoc = (Ext[6].edx & 0x0000F000) >> 12;
			L3LinesPerTag = (Ext[6].edx & 0x00000F00) >> 8;
			L3LineSize = (Ext[6].edx & 0x000000FF);
			AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L3, NULL, L3Size * 512, L3Assoc, 0, L3LineSize, false));
		}

		void CPUID::AddCacheDescription(int processor, const char *description)
		{
			CoreAssert(this != NULL);

			if (!description) return;

			char *temp = new char[strlen(description) + 1];

			CoreAssert(temp);
			strcpy(temp, description);
			proc[processor]->m_caches.insert(temp);
			temp = NULL;
		}

		void CPUID::AddIntelCacheData(int processor, int x)
		{
			CoreAssert(this != NULL);

			/* Compliant with Intel document #241618, save for the ENABLE_SANDPILE sections. */

			x &= 0xff;
			switch (x)
			{
			case 0:         break;
			case 0x1:       AddCacheDescription(processor, "Code TLB: 4KB pages, 4-way set associative, 32 entries"); break;
			case 0x2:       AddCacheDescription(processor, "Code TLB: 4MB pages, fully associative, 2 entries"); break;
			case 0x3:       AddCacheDescription(processor, "Data TLB: 4KB pages, 4-way set associative, 64 entries"); break;
			case 0x4:       AddCacheDescription(processor, "Data TLB: 4MB pages, 4-way set associative, 8 entries"); break;
			case 0x5:       AddCacheDescription(processor, "Data TLB: 4MB pages, 4-way set associative, 32 entries"); break;
			case 0x6:       AddCacheDescription(processor, "1st-level code cache: 8KB, 4-way set associative, 32 byte line size"); break;
			case 0x8:       AddCacheDescription(processor, "1st-level code cache: 16KB, 4-way set associative, 32 byte line size"); break;
			case 0x9:       AddCacheDescription(processor, "1st-level code cache: 32KB, 4-way set associative, 32 byte line size"); break;
			case 0xa:       AddCacheDescription(processor, "1st-level data cache: 8KB, 2-way set associative, 32 byte line size"); break;
			case 0xc:       AddCacheDescription(processor, "1st-level data cache: 16KB, 4-way set associative, 32 byte line size"); break;
			case 0xd:       AddCacheDescription(processor, "1st-level data cache: 16KB, 4-way set associative, 64 byte line size, ECC"); break;
			case 0x21:      AddCacheDescription(processor, "2nd-level cache: 256KB, 8-way set associative, 64 byte line size"); break;
			case 0x22:      AddCacheDescription(processor, "3rd-level cache: 512KB, 4-way set associative, sectored cache, 64 byte line size"); break;
			case 0x23:      AddCacheDescription(processor, "3rd-level cache: 1MB, 8-way set associative, sectored cache, 64 byte line size"); break;
			case 0x25:      AddCacheDescription(processor, "3rd-level cache: 2MB, 8-way set associative, sectored cache, 64 byte line size"); break;
			case 0x29:      AddCacheDescription(processor, "3rd-level cache: 4MB, 8-way set associative, sectored cache, 64 byte line size"); break;
			case 0x2C:      AddCacheDescription(processor, "1st-level data cache: 32KB, 8-way set assocative, 64 byte line size"); break;
			case 0x30:      AddCacheDescription(processor, "1st-level code cache: 32KB, 8-way set associative, 64 byte line size"); break;
			case 0x39:      AddCacheDescription(processor, "2nd-level cache: 128KB, 4-way set associative, sectored cache, 64 byte line size"); break;
			case 0x3A:      AddCacheDescription(processor, "2nd-level cache: 192KB, 6-way set associative, sectored cache, 64 byte line size"); break;
			case 0x3B:      AddCacheDescription(processor, "2nd-level cache: 128KB, 2-way set associative, sectored cache, 64 byte line size"); break;
			case 0x3C:      AddCacheDescription(processor, "2nd-level cache: 256KB, 4-way set associative, sectored cache, 64 byte line size"); break;
			case 0x3D:      AddCacheDescription(processor, "2nd-level cache: 384KB, 6-way set associative, sectored cache, 64 byte line size"); break;
			case 0x3E:      AddCacheDescription(processor, "2nd-level cache: 512KB, 4-way set associative, sectored cache, 64 byte line size"); break;
			case 0x40:      AddCacheDescription(processor, "No 2nd-level cache, or if 2nd-level cache exists, no 3rd-level cache\n"); break;
			case 0x41:      AddCacheDescription(processor, "2nd-level cache: 128KB, 4-way set associative, 32 byte line size"); break;
			case 0x42:      AddCacheDescription(processor, "2nd-level cache: 256KB, 4-way set associative, 32 byte line size"); break;
			case 0x43:      AddCacheDescription(processor, "2nd-level cache: 512KB, 4-way set associative, 32 byte line size"); break;
			case 0x44:      AddCacheDescription(processor, "2nd-level cache: 1MB, 4-way set associative, 32 byte line size"); break;
			case 0x45:      AddCacheDescription(processor, "2nd-level cache: 2MB, 4-way set associative, 32 byte line size"); break;
			case 0x46:      AddCacheDescription(processor, "3rd-level cache: 4MB, 4-way set associative, 64 byte line size"); break;
			case 0x47:      AddCacheDescription(processor, "3rd-level cache: 8MB, 8-way set associative, 64 byte line size"); break;
			case 0x48:      AddCacheDescription(processor, "2nd-level cache: 3MB, 12-way set associative, 64 byte line size"); break;
			case 0x49:      AddCacheDescription(processor, /* This is an L3 on the P4 and an L2 on the Core 2 */
				                                    proc[processor]->m_features.exists("SSSE3") ?
													"2nd-level cache: 4MB, 16-way set associative, 64 byte line size":
													"3rd-level cache: 4MB, 16-way set associative, 64 byte line size"); break;
			case 0x4A:      AddCacheDescription(processor, "3rd-level cache: 6MB, 12-way set associative, 64 byte line size"); break;
			case 0x4B:      AddCacheDescription(processor, "3rd-level cache: 8MB, 16-way set associative, 64 byte line size"); break;
			case 0x4C:      AddCacheDescription(processor, "3rd-level cache: 12MB, 12-way set associative, 64 byte line size"); break;
			case 0x4D:      AddCacheDescription(processor, "3rd-level cache: 16MB, 16-way set associative, 64 byte line size"); break;
			case 0x4E:      AddCacheDescription(processor, "2nd-level cache: 6MB, 24-way set associative, 64 byte line size"); break;
			case 0x50:      AddCacheDescription(processor, "Code TLB: 4KB, 2MB, or 4MB pages, fully associative, 64 entries"); break;
			case 0x51:      AddCacheDescription(processor, "Code TLB: 4KB, 2MB, or 4MB pages, fully associative, 128 entries"); break;
			case 0x52:      AddCacheDescription(processor, "Code TLB: 4KB, 2MB, or 4MB pages, fully associative, 256 entries"); break;
			case 0x55:      AddCacheDescription(processor, "Code TLB: 2MB or 4MB pages, fully associative, 256 entries"); break;
			case 0x56:      AddCacheDescription(processor, "L1 Data TLB: 4MB pages, 4-way set associative associative, 16 entries"); break;
			case 0x57:      AddCacheDescription(processor, "L1 Data TLB: 4KB pages, 4-way set associative, 16 entries"); break;
			case 0x5a:      AddCacheDescription(processor, "Data TLB0: 2MB or 4MB pages, 4-way set associative, 32 entries"); break;
			case 0x5b:      AddCacheDescription(processor, "Data TLB: 4KB or 4MB pages, fully associative, 64 entries"); break;
			case 0x5c:      AddCacheDescription(processor, "Data TLB: 4KB or 4MB pages, fully associative, 128 entries"); break;
			case 0x5d:      AddCacheDescription(processor, "Data TLB: 4KB or 4MB pages, fully associative, 256 entries"); break;
			case 0x60:      AddCacheDescription(processor, "1st-level data cache: 16KB, 8-way set associative, sectored cache, 64 byte line size"); break;
			case 0x66:      AddCacheDescription(processor, "1st-level data cache: 8KB, 4-way set associative, sectored cache, 64 byte line size"); break;
			case 0x67:      AddCacheDescription(processor, "1st-level data cache: 16KB, 4-way set associative, sectored cache, 64 byte line size"); break;
			case 0x68:      AddCacheDescription(processor, "1st-level data cache: 32KB, 4-way set associative, sectored cache, 64 byte line size"); break;
			case 0x70:      AddCacheDescription(processor, "12K-uops, 8-way set associative"); break;
			case 0x71:      AddCacheDescription(processor, "16K-uops, 8-way set associative"); break;
			case 0x72:      AddCacheDescription(processor, "32K-uops, 8-way set associative"); break;
			case 0x73:      AddCacheDescription(processor, "64K-uops, 8-way set associative"); break;
			case 0x78:      AddCacheDescription(processor, "2nd-level cache: 1MB, 4-way set associative, 64 byte line size"); break;
			case 0x79:      AddCacheDescription(processor, "2nd-level cache: 128KB, 8-way set associative, sectored cache, 64 byte line size"); break;
			case 0x7A:      AddCacheDescription(processor, "2nd-level cache: 256KB, 4-way set associative, sectored cache, 64 byte line size"); break;
			case 0x7B:      AddCacheDescription(processor, "2nd-level cache: 512KB, 4-way set associative, sectored cache, 64 byte line size"); break;
			case 0x7C:      AddCacheDescription(processor, "2nd-level cache: 1MB, 4-way set associative, sectored cache, 64 byte line size"); break;
			case 0x7D:      AddCacheDescription(processor, "2nd-level cache: 2MB, 8-way set associative, 64 byte line size"); break;
			case 0x7F:      AddCacheDescription(processor, "2nd-level cache: 512KB, 2-way set associative, 64 byte line size"); break;
			case 0x82:      AddCacheDescription(processor, "2nd-level cache: 256KB, 8-way set associative, 32 byte line size"); break;
			case 0x83:      AddCacheDescription(processor, "2nd-level cache: 512KB, 8-way set associative, 32 byte line size"); break;
			case 0x84:      AddCacheDescription(processor, "2nd-level cache: 1MB, 8-way set associative, 32 byte line size"); break;
			case 0x85:      AddCacheDescription(processor, "2nd-level cache: 2MB, 8-way set associative, 32 byte line size"); break;
			case 0x86:      AddCacheDescription(processor, "2nd-level cache: 512KB, 4-way set associative, 64 byte line size"); break;
			case 0x87:      AddCacheDescription(processor, "2nd-level cache: 1MB, 8-way set associative, 64 byte line size"); break;
			case 0xB0:      AddCacheDescription(processor, "Code TLB: 4KB pages, 4-way set associative, 128 entries"); break;
			case 0xB1:      /* Intel oddly says this can be either of the below two, but doesn't say any conditions for when these will show up. */
			                AddCacheDescription(processor, "Code TLB: 2MB pages, 4-way set associative, 8 entries");
			                AddCacheDescription(processor, "Code TLB: 4MB pages, 4-way set associative, 4 entries"); break;
			case 0xB2:      AddCacheDescription(processor, "Data TLB: 4KB pages, 4-way set associative, 64 entries"); break;
			case 0xB3:      AddCacheDescription(processor, "Data TLB: 4KB pages, 4-way set associative, 128 entries"); break;
			case 0xB4:      AddCacheDescription(processor, "Data TLB: 4KB pages, 4-way set associative, 256 entries"); break;
			case 0xCA:      AddCacheDescription(processor, "Shared 2nd-level TLB: 4KB pages, 4-way set associative, 512 entries"); break;
			case 0xD0:      AddCacheDescription(processor, "3rd-level cache: 512KB, 4-way set associative, 64 byte line size"); break;
			case 0xD1:      AddCacheDescription(processor, "3rd-level cache: 1MB, 4-way set associative, 64 byte line size"); break;
			case 0xD2:      AddCacheDescription(processor, "3rd-level cache: 2MB, 4-way set associative, 64 byte line size"); break;
			case 0xD6:      AddCacheDescription(processor, "3rd-level cache: 1MB, 8-way set associative, 64 byte line size"); break;
			case 0xD7:      AddCacheDescription(processor, "3rd-level cache: 2MB, 8-way set associative, 64 byte line size"); break;
			case 0xD8:      AddCacheDescription(processor, "3rd-level cache: 4MB, 8-way set associative, 64 byte line size"); break;
			case 0xDC:      AddCacheDescription(processor, "3rd-level cache: 1.5MB, 12-way set associative, 64 byte line size"); break;
			case 0xDD:      AddCacheDescription(processor, "3rd-level cache: 3MB, 12-way set associative, 64 byte line size"); break;
			case 0xDE:      AddCacheDescription(processor, "3rd-level cache: 6MB, 12-way set associative, 64 byte line size"); break;
			case 0xE2:      AddCacheDescription(processor, "3rd-level cache: 2MB, 16-way set associative, 64 byte line size"); break;
			case 0xE3:      AddCacheDescription(processor, "3rd-level cache: 4MB, 16-way set associative, 64 byte line size"); break;
			case 0xE4:      AddCacheDescription(processor, "3rd-level cache: 8MB, 16-way set associative, 64 byte line size"); break;
			case 0xEA:      AddCacheDescription(processor, "3rd-level cache: 12MB, 24-way set associative, 64 byte line size"); break;
			case 0xEB:      AddCacheDescription(processor, "3rd-level cache: 18MB, 24-way set associative, 64 byte line size"); break;
			case 0xEC:      AddCacheDescription(processor, "3rd-level cache: 24MB, 24-way set associative, 64 byte line size"); break;
			case 0xF0:      AddCacheDescription(processor, "64 byte prefetching"); break;
			case 0xF1:      AddCacheDescription(processor, "128 byte prefetching"); break;
			default:
			{
				char temp[256];
				sprintf(temp, "Unknown cache descriptor 0x%02x", x);
				AddCacheDescription(processor, temp);
			}
			break;
			}
		}

		void CPUID::DetectFMS(int processor)
		{
			CoreAssert(this != NULL);

			/* Compliant with Intel document #241618. */
			proc[processor]->m_family = (char)(((Std[1].eax >> 8) + (Std[1].eax >> 20)) & 0xff);
			proc[processor]->m_model = (char)(((((Std[1].eax >> 16) & 0xf) << 4) + ((Std[1].eax >> 4) & 0xf)) & 0xff);
			proc[processor]->m_stepping = (char)(Std[1].eax & 0xf);
		}

		void CPUID::DetectBrandID(int processor)
		{
			CoreAssert(this != NULL);

			/* Compliant with Intel document #241618. */
			proc[processor]->m_brandID = (char)(Std[1].ebx & 0xff);
		}

		void CPUID::DetectCount(int processor)
		{
			CoreAssert(this != NULL);

			/* Compliant with Intel document #241618. */

			/* Do we have HTT flag set? */
			if (proc[processor]->m_features.exists("HTT")) {
				/* AMD and Intel documentations state that if HTT is supported */
				/* then this the EBX:16 will reflect the logical processor count */
				/* otherwise the flag is reserved. */

				proc[processor]->m_features.insert("CMP", NULL);

				proc[processor]->m_cores = (char)((Std[4].eax & 0xFC000000) >> 26) + 1;
				proc[processor]->m_logical = (char)((Std[1].ebx & 0x00FF0000) >> 16);

				if (proc[processor]->m_cores < 1)
					proc[processor]->m_cores = 1;

				if (proc[processor]->m_logical < 1)
					proc[processor]->m_logical = 1;

				if (proc[processor]->m_cores > 1 &&
				    proc[processor]->m_logical > proc[processor]->m_cores) {
					/* Hyperthreaded multi-core. */
				} else if (proc[processor]->m_cores > 1 &&
				           proc[processor]->m_logical == proc[processor]->m_cores) {
					/* Multi-core processor not presenting HTT */
					proc[processor]->m_features.erase("HTT");
				} else if (proc[processor]->m_cores == 1 &&
				           proc[processor]->m_logical > proc[processor]->m_cores) {
					/* Hyperthreaded. */
					proc[processor]->m_features.erase("CMP");
				}
			} else {
				/* HTT not supported. Report cores and logical processor count as equal. */
				proc[processor]->m_cores = (char)(((Std[4].eax & 0xFC000000) >> 26) + 1);
				proc[processor]->m_logical = proc[processor]->m_cores;
			}
		}

		void CPUID::DetectAPIC(int processor)
		{
			CoreAssert(this != NULL);

			/* Found at http://www.intel.com/cd/ids/developer/asmo-na/eng/211924.htm */
			proc[processor]->m_apicID = (char)((Std[1].ebx & 0xFF000000) >> 24);
		}

		void CPUID::DetectFeatures(int processor)
		{
			CoreAssert(this != NULL);
			for (CPUFeature *p = features; p->m_name; p++) {
				if ((cc_uint32_t)proc[processor]->m_vendor & p->m_vendor) {
					cc_uint32_t regval = 0;
					switch (p->m_register) {
					case REG_EAX:
						regval = p->m_level & 0x80000000 ? Ext[p->m_level & 0x7FFFFFFF].eax : Std[p->m_level].eax;
						break;
					case REG_EBX:
						regval = p->m_level & 0x80000000 ? Ext[p->m_level & 0x7FFFFFFF].ebx : Std[p->m_level].ebx;
						break;
					case REG_ECX:
						regval = p->m_level & 0x80000000 ? Ext[p->m_level & 0x7FFFFFFF].ecx : Std[p->m_level].ecx;
						break;
					case REG_EDX:
						regval = p->m_level & 0x80000000 ? Ext[p->m_level & 0x7FFFFFFF].edx : Std[p->m_level].edx;
						break;
					}
					if (regval & p->m_bitmask) {
						proc[processor]->m_features.insert(p->m_name, NULL);
					}
				}
			}
		}
	}
}
#endif
