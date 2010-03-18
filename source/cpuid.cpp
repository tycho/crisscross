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
#define _GNU_SOURCE
#include <pthread.h>
#define CPUSET_T cpu_set_t
#endif

/* The following definition enables some rather suspicious cache descriptors */
/* from sandpile.org which haven't been verified with Intel's docs. */
/* #    define ENABLE_SANDPILE */

#include <crisscross/cpuid.h>
#include <crisscross/core_io.h>

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

namespace CrissCross
{
	namespace System
	{
#define FPU_FLAG 0x0001
#define LAHF_FLAG 0x0001
#define SSE3_FLAG 0x0001
#define VME_FLAG 0x0002
#define DE_FLAG 0x0004
#define DTES64_FLAG 0x0004
#define MONITOR_FLAG 0x0008
#define PSE_FLAG 0x0008
#define DS_CPL_FLAG 0x0010
#define TSC_FLAG 0x0010
#define MSR_FLAG 0x0020
#define VMX_FLAG 0x0020
#define PAE_FLAG 0x0040
#define SMX_FLAG 0x0040
#define EIST_FLAG 0x0080
#define MCE_FLAG 0x0080
#define CX8_FLAG 0x0100
#define TM2_FLAG 0x0100
#define APIC_FLAG 0x0200
#define SSSE3_FLAG 0x0200
#define CNXTID_FLAG 0x0400
#define SEP_FLAG 0x0800
#define SYSCALL_FLAG 0x0800
#define MTRR_FLAG 0x1000
#define CX16_FLAG 0x2000
#define PGE_FLAG 0x2000
#define MCA_FLAG 0x4000
#define XTPR_FLAG 0x4000
#define PDCM_FLAG 0x8000
#define CMOV_FLAG 0x8000
#define PAT_FLAG 0x10000
#define PSE36_FLAG 0x20000
#define DCA_FLAG 0x40000
#define PSNUM_FLAG 0x40000
#define CLFLUSH_FLAG 0x80000
#define SSE4_1_FLAG 0x80000
#define SSE4_2_FLAG 0x100000
#define XD_FLAG 0x100000
#define DTS_FLAG 0x200000
#define ACPI_FLAG 0x400000
#define MMX_FLAG 0x800000
#define FXSR_FLAG 0x1000000
#define SSE_FLAG 0x2000000
#define SSE2_FLAG 0x4000000
#define SS_FLAG 0x8000000
#define HTT_FLAG 0x10000000
#define EM64T_FLAG 0x20000000
#define TM1_FLAG 0x20000000
#define IA64_FLAG 0x40000000
#define PBE_FLAG 0x80000000

		/* AMD 8000_0001 EDX flags */
#define _3DNOW_FLAG 0x80000000
#define _3DNOWEXT_FLAG 0x40000000
#define LM_FLAG 0x20000000
#define RDTSCP_FLAG 0x8000000
#define FFXSR_FLAG 0x2000000
#define FXSR_FLAG 0x1000000
#define MMXEXT_FLAG 0x800000
#define NX_FLAG 0x100000

		/* AMD 8000_0001 ECX flags */
#define WDT_FLAG 0x2000
#define SKINIT_FLAG 0x1000
#define SSE5_FLAG 0x800
#define IBS_FLAG 0x400
#define OSVW_FLAG 0x200
#define _3DNP_FLAG 0x100
#define MAS_FLAG 0x80
#define SSE4A_FLAG 0x40
#define ABM_FLAG 0x20
#define AMC8_FLAG 0x10
#define EAS_FLAG 0x8
#define SVM_FLAG 0x4
#define CL_FLAG 0x2
#define LS_FLAG 0x1

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
		: m_manufacturer(NULL),
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
		: m_manufacturer(cc_newstr(_copy.m_manufacturer)),
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
			if (Manufacturer())
				_writer->WriteLine("CPU[%u] Manufacturer: %s", m_index, Manufacturer());

			/* Print the Name tag if it was read properly. */
			if (Name())
				_writer->WriteLine("CPU[%u] Name: %s", m_index, Name());

			_writer->WriteLine("CPU[%u] Family %2d Model %2d Stepping %2d", m_index,
				Family(), Model(), Stepping());

			/* Print out the CPU cache info. */
			const CrissCross::System::caches_t *caches = Caches();
			if (caches->size() > 0) {
				_writer->WriteLine("CPU[%u] Caches:", m_index);
				for (size_t j = 0; j < caches->size(); j++) {
					if (caches->valid(j))
						_writer->Write("  %s", caches->get(j));
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

		const char *X86Processor::Manufacturer() const
		{
			return m_manufacturer;
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

				delete [] (char *)proc[i]->m_manufacturer;
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
			char *manufacturer = new char[(4 * 3) + 1];
			char *_man = &manufacturer[0];

			memcpy(_man, &Std[0].ebx, 4);
			_man += 4;
			memcpy(_man, &Std[0].edx, 4);
			_man += 4;
			memcpy(_man, &Std[0].ecx, 4);
			_man += 4;
			*_man = '\x0';
			proc[processor]->m_manufacturer = manufacturer;
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

			if (proc[processor]->m_manufacturer) {
				if (strcmp(proc[processor]->m_manufacturer, "GenuineIntel") == 0)	{
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
				} else if (strcmp(proc[processor]->m_manufacturer, "AuthenticAMD") == 0) {
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
			case 0x1: AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_CODE_TLB, "4KB", 0, 4, 32, 0, false)); break;
			case 0x2:       AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_CODE_TLB, "4MB", 0, 255, 2, 0, false)); break;
			case 0x3:       AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_DATA_TLB, "4KB", 0, 4, 64, 0, false)); break;
			case 0x4:       AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_DATA_TLB, "4MB", 0, 4, 8, 0, false)); break;
			case 0x5:       AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_DATA_TLB, "4MB", 0, 4, 32, 0, false)); break;
			case 0x6:       AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1CODE, NULL, 8, 4, 0, 32, false)); break;
			case 0x8:       AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1CODE, NULL, 16, 4, 0, 32, false)); break;
			case 0xa:       AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1DATA, NULL, 8, 2, 0, 32, false)); break;
#if defined (ENABLE_SANDPILE)
			case 0xb:       AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_CODE_TLB, "4MB", 0, 4, 4, 0, false)); break;
#endif
			case 0xc:       AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1DATA, NULL, 16, 4, 0, 32, false)); break;
#if defined (ENABLE_SANDPILE)
			case 0x10:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1DATA, NULL, 16, 4, 0, 32, false)); break;
			case 0x15:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1CODE, NULL, 16, 4, 0, 32, false)); break;
			case 0x1A:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 96, 6, 0, 64, false)); break;
#endif
			case 0x22:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L3, NULL, 512, 4, 0, 64, true)); break;
			case 0x23:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L3, NULL, 1024, 8, 0, 64, true)); break;
			case 0x25:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L3, NULL, 2048, 8, 0, 64, true)); break;
			case 0x29:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L3, NULL, 4096, 8, 0, 64, true)); break;
			case 0x2C:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1DATA, NULL, 32, 8, 0, 64, false)); break;
			case 0x30:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1CODE, NULL, 32, 8, 0, 64, false)); break;
			case 0x39:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 128, 4, 0, 64, true)); break;
			case 0x3A:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 192, 6, 0, 64, true)); break;
			case 0x3B:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 128, 2, 0, 64, true)); break;
			case 0x3C:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 256, 4, 0, 64, true)); break;
			case 0x3D:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 384, 6, 0, 64, true)); break;
			case 0x3E:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 512, 4, 0, 64, true)); break;
			case 0x40:      AddCacheDescription(processor, "No 2nd-level cache, or if 2nd-level cache exists, no 3rd-level cache\n"); break;
			case 0x41:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 128, 4, 0, 32, false)); break;
			case 0x42:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 256, 4, 0, 32, false)); break;
			case 0x43:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 512, 4, 0, 32, false)); break;
			case 0x44:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 1024, 4, 0, 32, false)); break;
			case 0x45:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 2048, 4, 0, 32, false)); break;
			case 0x46:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L3, NULL, 4096, 4, 0, 64, false)); break;
			case 0x47:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L3, NULL, 8192, 8, 0, 64, false)); break;
#if defined (ENABLE_SANDPILE)
			case 0x48:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 3072, 12, 0, 64, false)); break;
#endif
			case 0x49:      AddCacheDescription(processor, CreateCacheDescription(
				                                    /* This is an L3 on the P4 and an L2 on the Core 2 */
				                                    proc[processor]->m_features.exists("SSSE3") ? CACHE_TYPE_L2 : CACHE_TYPE_L3, NULL, 4096, 16, 0, 64, false)); break;
			case 0x4A:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L3, NULL, 6144, 12, 0, 64, false)); break;
			case 0x4B:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L3, NULL, 8192, 16, 0, 64, false)); break;
			case 0x4C:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L3, NULL, 12288, 12, 0, 64, false)); break;
			case 0x4D:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L3, NULL, 16384, 16, 0, 64, false)); break;
			case 0x4E:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 6144, 24, 0, 64, false)); break;
			case 0x50:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_CODE_TLB, "4KB, 2MB or 4MB", 0, 255, 64, 0, false)); break;
			case 0x51:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_CODE_TLB, "4KB, 2MB or 4MB", 0, 255, 128, 0, false)); break;
			case 0x52:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_CODE_TLB, "4KB, 2MB or 4MB", 0, 255, 256, 0, false)); break;
			case 0x56:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1DATA_TLB, "4MB", 0, 4, 16, 0, false)); break;
			case 0x57:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1DATA_TLB, "4KB", 0, 4, 16, 0, false)); break;
			case 0x5b:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_DATA_TLB, "4KB or 4MB", 0, 255, 64, 0, false)); break;
			case 0x5c:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_DATA_TLB, "4KB or 4MB", 0, 255, 128, 0, false)); break;
			case 0x5d:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_DATA_TLB, "4KB or 4MB", 0, 255, 256, 0, false)); break;
			case 0x60:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1DATA, NULL, 16, 8, 0, 64, true)); break;
			case 0x66:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1DATA, NULL, 8, 4, 0, 64, true)); break;
			case 0x67:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1DATA, NULL, 16, 4, 0, 64, true)); break;
			case 0x68:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1DATA, NULL, 32, 4, 0, 64, true)); break;
			case 0x70:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_TRACE, NULL, 12, 8, 0, 0, false)); break;
			case 0x71:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_TRACE, NULL, 16, 8, 0, 0, false)); break;
			case 0x72:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_TRACE, NULL, 32, 8, 0, 0, false)); break;
			case 0x73:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_TRACE, NULL, 64, 8, 0, 0, false)); break;
#if defined (ENABLE_SANDPILE)
			case 0x77:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1CODE, NULL, 16, 4, 0, 64, true)); break;
#endif
			case 0x78:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 1024, 4, 0, 64, false)); break;
			case 0x79:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 128, 8, 0, 64, true)); break;
			case 0x7A:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 256, 8, 0, 64, true)); break;
			case 0x7B:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 512, 8, 0, 64, true)); break;
			case 0x7C:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 1024, 8, 0, 64, true)); break;
			case 0x7D:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 2048, 8, 0, 64, false)); break;
#if defined (ENABLE_SANDPILE)
			case 0x7E:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 256, 8, 0, 128, true)); break;
#endif
			case 0x7F:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 512, 2, 0, 64, false)); break;
#if defined (ENABLE_SANDPILE)
			case 0x81:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 128, 8, 0, 128, true)); break;
#endif
			case 0x82:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 256, 8, 0, 32, false)); break;
			case 0x83:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 512, 8, 0, 32, false)); break;
			case 0x84:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 1024, 8, 0, 32, false)); break;
			case 0x85:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 2048, 8, 0, 32, false)); break;
			case 0x86:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 512, 4, 0, 64, false)); break;
			case 0x87:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2, NULL, 1024, 8, 0, 64, false)); break;
#if defined (ENABLE_SANDPILE)
			case 0x88:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L3, NULL, 2048, 4, 0, 64, false)); break;
			case 0x89:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L3, NULL, 4096, 4, 0, 64, false)); break;
			case 0x8A:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L3, NULL, 8192, 4, 0, 64, false)); break;
			case 0x8D:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L3, NULL, 3072, 12, 0, 128, false)); break;
			case 0x90:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_CODE_TLB, "4KB or 256MB", 0, 255, 64, 0, false)); break;
			case 0x96:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L1DATA_TLB, "4KB or 256MB", 0, 255, 32, 0, false)); break;
			case 0x9B:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_L2DATA_TLB, "4KB or 256MB", 0, 255, 96, 0, false)); break;
#endif
			case 0xB0:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_CODE_TLB, "4KB", 0, 4, 128, 0, false)); break;
			case 0xB1:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_CODE_TLB, "4MB", 0, 4, 4, 0, false));
				AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_CODE_TLB, "2MB", 0, 4, 8, 0, false)); break;
			case 0xB3:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_DATA_TLB, "4KB", 0, 4, 128, 0, false)); break;
			case 0xB4:      AddCacheDescription(processor, CreateCacheDescription(CACHE_TYPE_DATA_TLB, "4KB", 0, 4, 256, 0, false)); break;
			case 0xF0:      AddCacheDescription(processor, "64 byte prefetching\n"); break;
			case 0xF1:      AddCacheDescription(processor, "128 byte prefetching\n"); break;
			default:
			{
				char temp[256];
				sprintf(temp, "Unknown cache descriptor 0x%02x\n", x);
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

		void CPUID::DetectFeature(const unsigned int *_register, long _flag, int _processor, const char *_name)
		{
			CoreAssert(this != NULL);

			/* Compliant with Intel document #241618. */

			bool supported = (*_register & _flag) > 0;
			if (supported)
				proc[_processor]->m_features.insert(_name, NULL);
		}

		void CPUID::DetectFeatures(int processor)
		{
			CoreAssert(this != NULL);

			/* Compliant with Intel document #241618. */

			DetectFeature(&Std[1].edx, FPU_FLAG, processor, "FPU");
			DetectFeature(&Std[1].edx, VME_FLAG, processor, "VME");
			DetectFeature(&Std[1].edx, DE_FLAG, processor, "DE");
			DetectFeature(&Std[1].edx, PSE_FLAG, processor, "PSE");
			DetectFeature(&Std[1].edx, TSC_FLAG, processor, "TSC");
			DetectFeature(&Std[1].edx, MSR_FLAG, processor, "MSR");
			DetectFeature(&Std[1].edx, PAE_FLAG, processor, "PAE");
			DetectFeature(&Std[1].edx, MCE_FLAG, processor, "MCE");
			DetectFeature(&Std[1].edx, CX8_FLAG, processor, "CX8");
			DetectFeature(&Std[1].edx, APIC_FLAG, processor, "APIC");
			DetectFeature(&Std[1].edx, SEP_FLAG, processor, "SEP");
			DetectFeature(&Std[1].edx, MTRR_FLAG, processor, "MTRR");
			DetectFeature(&Std[1].edx, PGE_FLAG, processor, "PGE");
			DetectFeature(&Std[1].edx, MCA_FLAG, processor, "MCA");
			DetectFeature(&Std[1].edx, CMOV_FLAG, processor, "CMOV");
			DetectFeature(&Std[1].edx, PAT_FLAG, processor, "PAT");
			DetectFeature(&Std[1].edx, PSE36_FLAG, processor, "PSE36");
			DetectFeature(&Std[1].edx, PSNUM_FLAG, processor, "PSNUM");
			DetectFeature(&Std[1].edx, CLFLUSH_FLAG, processor, "CLFLUSH");
			DetectFeature(&Std[1].edx, DTS_FLAG, processor, "DTS");
			DetectFeature(&Std[1].edx, ACPI_FLAG, processor, "ACPI");
			DetectFeature(&Std[1].edx, MMX_FLAG, processor, "MMX");
			DetectFeature(&Std[1].edx, FXSR_FLAG, processor, "FXSR");
			DetectFeature(&Std[1].edx, SSE_FLAG, processor, "SSE");
			DetectFeature(&Std[1].edx, SSE2_FLAG, processor, "SSE2");
			DetectFeature(&Std[1].edx, SS_FLAG, processor, "SS");
			DetectFeature(&Std[1].edx, HTT_FLAG, processor, "HTT");
			DetectFeature(&Std[1].edx, TM1_FLAG, processor, "TM1");

			DetectFeature(&Std[1].ecx, SSE3_FLAG, processor, "SSE3");
			DetectFeature(&Std[1].ecx, CX16_FLAG, processor, "CX16");

			if (proc[processor]->m_manufacturer) {
				if (strcmp(proc[processor]->m_manufacturer, "GenuineIntel") == 0)	{
					/* IA64 and PBE are on Intel where the 3DNow! flags are on AMD */
					DetectFeature(&Std[1].edx, IA64_FLAG, processor, "IA64");
					DetectFeature(&Std[1].edx, PBE_FLAG, processor, "PBE");

					/* Intel-only flags */
					DetectFeature(&Ext[1].ecx, LAHF_FLAG, processor, "LAHF");
					DetectFeature(&Std[1].ecx, DS_CPL_FLAG, processor, "DS_CPL");
					DetectFeature(&Std[1].ecx, MONITOR_FLAG, processor, "MONITOR");
					DetectFeature(&Std[1].ecx, DTES64_FLAG, processor, "DTES64");
					DetectFeature(&Std[1].ecx, EIST_FLAG, processor, "EIST");
					DetectFeature(&Std[1].ecx, TM2_FLAG, processor, "TM2");
					DetectFeature(&Std[1].ecx, SSSE3_FLAG, processor, "SSSE3");
					DetectFeature(&Std[1].ecx, CNXTID_FLAG, processor, "CNXTID");
					DetectFeature(&Ext[1].edx, SYSCALL_FLAG, processor, "SYSCALL");
					DetectFeature(&Std[1].ecx, XTPR_FLAG, processor, "XTPR");
					DetectFeature(&Ext[1].edx, XD_FLAG, processor, "XD");
					DetectFeature(&Std[1].ecx, DCA_FLAG, processor, "DCA");
					DetectFeature(&Ext[1].edx, EM64T_FLAG, processor, "EM64T");
					DetectFeature(&Std[1].ecx, SSE4_1_FLAG, processor, "SSE4.1");
					DetectFeature(&Std[1].ecx, SSE4_2_FLAG, processor, "SSE4.2");
					DetectFeature(&Std[1].ecx, VMX_FLAG, processor, "VMX");
					DetectFeature(&Std[1].ecx, SMX_FLAG, processor, "SMX");
					DetectFeature(&Std[1].ecx, PDCM_FLAG, processor, "PDCM");
				} else if (strcmp(proc[processor]->m_manufacturer, "AuthenticAMD") == 0) {
					/* AMD-only flags, EDX 8000_0001 */
					DetectFeature(&Ext[1].edx, NX_FLAG, processor, "NX");
					DetectFeature(&Ext[1].edx, MMXEXT_FLAG, processor, "MMXEXT");
					DetectFeature(&Ext[1].edx, FFXSR_FLAG, processor, "FFXSR");
					DetectFeature(&Ext[1].edx, RDTSCP_FLAG, processor, "RDTSCP");
					DetectFeature(&Ext[1].edx, LM_FLAG, processor, "LM");
					DetectFeature(&Ext[1].edx, _3DNOWEXT_FLAG, processor, "3DNOWEXT");
					DetectFeature(&Ext[1].edx, _3DNOW_FLAG, processor, "3DNOW");

					/* AMD-only flags, ECX 8000_0001 */
					DetectFeature(&Ext[1].ecx, LS_FLAG, processor, "LAHF_SAHF");
					DetectFeature(&Ext[1].ecx, CL_FLAG, processor, "CL");
					DetectFeature(&Ext[1].ecx, SVM_FLAG, processor, "SVM");
					DetectFeature(&Ext[1].ecx, EAS_FLAG, processor, "EAS");
					DetectFeature(&Ext[1].ecx, AMC8_FLAG, processor, "AMC8");
					DetectFeature(&Ext[1].ecx, ABM_FLAG, processor, "ABM");
					DetectFeature(&Ext[1].ecx, SSE4A_FLAG, processor, "SSE4A");
					DetectFeature(&Ext[1].ecx, MAS_FLAG, processor, "MAS");
					DetectFeature(&Ext[1].ecx, _3DNP_FLAG, processor, "3DNP");
					DetectFeature(&Ext[1].ecx, OSVW_FLAG, processor, "OSVW");
					DetectFeature(&Ext[1].ecx, SSE5_FLAG, processor, "SSE5");
					DetectFeature(&Ext[1].ecx, IBS_FLAG, processor, "IBS");
					DetectFeature(&Ext[1].ecx, SKINIT_FLAG, processor, "SKINIT");
					DetectFeature(&Ext[1].ecx, WDT_FLAG, processor, "WDT");
				}
			}
		}
	}
}
#endif
