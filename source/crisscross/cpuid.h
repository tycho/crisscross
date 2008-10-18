/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2008 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_cpuid_h
#define __included_cc_cpuid_h

#ifndef __GNUC__
#include <crisscross/universal_include.h>
#endif

#ifdef ENABLE_CPUID

#ifdef TARGET_OS_WINDOWS
#include <windows.h>
#endif

#include "darray.h"
#include "rbtree.h"

namespace CrissCross
{
	namespace System
	{
		class Feature
		{
			public:
				bool Enabled;
				Feature()
				{
					Enabled = 0;
				}

				~Feature()
				{
				}
		};

		class Processor
		{
			public:
				const char                                             *Manufacturer;
				const char                                             *ProcessorName;
				unsigned int                                            LogicalPerPackage;
				unsigned int                                            CoresPerPackage;
				char                                                    Family;
				char                                                    Model;
				char                                                    Stepping;
				char                                                    BrandID;
				char                                                    APICID;
				CrissCross::Data::RedBlackTree<const char *, Feature *> features;
				CrissCross::Data::DArray <char *>                       caches;
			public:
				Processor()
				{
				}

				~Processor()
				{
				}
		};


		class CPUID
		{
			protected:

#ifdef TARGET_OS_WINDOWS

				struct GoThreadProc_Params
				{
					CPUID *cpuid_class;
					int processor;
				};

				static DWORD CALLBACK s_GoThreadProc(LPVOID lpParameter)
				{
					return (( GoThreadProc_Params * )lpParameter)->cpuid_class->
					       GoThread(( LPVOID * )&
					                (( GoThreadProc_Params * )lpParameter)->processor);
				};
				DWORD WINAPI GoThread(LPVOID * params);
#else
				long int GoThread(int processor);
#endif

				typedef enum
				{
					CACHE_TYPE_TRACE,

					CACHE_TYPE_L0DATA_TLB,
					CACHE_TYPE_L1DATA_TLB,
					CACHE_TYPE_L2DATA_TLB,

					CACHE_TYPE_L0CODE_TLB,
					CACHE_TYPE_L1CODE_TLB,
					CACHE_TYPE_L2CODE_TLB,

					CACHE_TYPE_DATA_TLB,
					CACHE_TYPE_CODE_TLB,

					CACHE_TYPE_L1CODE,
					CACHE_TYPE_L1DATA,

					CACHE_TYPE_L2,

					CACHE_TYPE_L3
				} cacheType;

				const char *CreateCacheDescription(cacheType _type, const char *_pages, unsigned int _size, unsigned int _assoc, unsigned int _entries, unsigned int _linesize, bool _sectored);
				void AddCacheDescription(int processor, const char *description);
				void AddIntelCacheData(int processor, int x);
				void DetectManufacturer(int processor);
				void DetectProcessorName(int processor);
				void DetectCacheInfo(int processor);
				void DecodeAMDCacheIdentifiers(int processor);
				void DetectFMS(int processor);
				void DetectBrandID(int processor);
				void DetectCount(int processor);
				void DetectAPIC(int processor);
				void DetectFeatures(int processor);
				void DetectFeature(const unsigned int *_register, long _flag, int _processor, const char *_name);

			public:
				CPUID();
				~CPUID();
				void Go();

				/*!
				 * \return The number of hardware threads per physical package.
				 */
				int LogicalPerPackage();

				/*!
				 * \return The number of physical cores per package.
				 */
				int CoresPerPackage();

				/*!
				 * \return The number of virtual processors detected, including all cores and hardware threads.
				 */
				int VirtualCount();

				Processor *proc[MAX_PROCESSORS];            /* Support up to MAX_PROCESSORS */
		};
	}
}

#endif

#endif
