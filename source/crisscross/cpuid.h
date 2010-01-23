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

#ifndef __included_cc_cpuid_h
#define __included_cc_cpuid_h

#include <crisscross/platform_detect.h>

#include <crisscross/darray.h>
#include <crisscross/rbtree.h>

namespace CrissCross
{
	namespace System
	{
		typedef CrissCross::Data::RedBlackTree<const char *, void *> features_t;
		typedef CrissCross::Data::DArray<char *> caches_t;

		class X86Processor
		{
			protected:
				const char *m_manufacturer;
				const char *m_name;
				unsigned int m_logical;
				unsigned int m_cores;
				unsigned int m_index;
				char m_family;
				char m_model;
				char m_stepping;
				char m_brandID;
				char m_apicID;
				features_t m_features;
				caches_t m_caches;

			public:
				X86Processor();
				X86Processor(X86Processor const &_copy);
				~X86Processor();
				const char *Manufacturer() const;
				const char *Name() const;
				char Family() const;
				char Model() const;
				char Stepping() const;
				char BrandID() const;
				char APICID() const;
				const features_t *Features() const;
				const caches_t *Caches() const;

				void Print(CrissCross::IO::CoreIOWriter *_writer = g_stdout) const;

				friend class CPUID;
		};


		class CPUID
		{
			protected:
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

				CrissCross::Data::DArray<X86Processor *> proc;

				/*!
				 * Not to be directly called. For internal use only.
				 */
				long GoThread(int processor);
		};
	}
}

#endif
