/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2022 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include <crisscross/universal_include.h>
#include <crisscross/stopwatch.h>

#if defined (TARGET_OS_MACOSX)
#include <mach/mach.h>
#include <mach/mach_time.h>
#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_NDSFIRMWARE) || \
	defined(TARGET_OS_FREEBSD) || defined (TARGET_OS_NETBSD) || \
	defined(TARGET_OS_OPENBSD) || defined (TARGET_OS_HAIKU)
#include <sys/time.h>
#include <time.h>
#elif defined (TARGET_OS_WINDOWS)
#include <windows.h>
#endif

namespace CrissCross
{
	namespace System
	{
		struct StopwatchImpl {
#if defined (TARGET_OS_WINDOWS)
			LARGE_INTEGER m_start, m_finish;
			double m_tickInterval;

			void RecalculateFrequency();
#elif defined (TARGET_OS_MACOSX)
			uint64_t m_start;
			uint64_t m_finish;
			mach_timebase_info_data_t m_timebase;
#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_FREEBSD) || \
			defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD) || \
			defined (TARGET_OS_HAIKU)
			struct timeval m_start;
			struct timeval m_finish;
#elif defined (TARGET_OS_NDSFIRMWARE)
			/* Nothing here :) */
#else
#error No target OS defined (did you forget to include crisscross/universal_include.h?)
#endif
		};
		Stopwatch::Stopwatch()
		{
			m_impl = new StopwatchImpl;
#if defined (TARGET_OS_WINDOWS)
			m_impl->RecalculateFrequency();
#elif defined (TARGET_OS_MACOSX)
			mach_timebase_info(&m_impl->m_timebase);
#endif

			/* We start it here for static Stopwatch instances */
			/* where it's impractical to do an initial Start() call */
			Start();
		}

		Stopwatch::~Stopwatch()
		{
			delete m_impl;
		}

		void Stopwatch::Start()
		{
#if defined (TARGET_OS_WINDOWS)
			m_impl->RecalculateFrequency();
			QueryPerformanceCounter(&m_impl->m_start);
#elif defined (TARGET_OS_MACOSX)
			m_impl->m_start = mach_absolute_time();
#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_FREEBSD) || \
			defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD) || \
			defined (TARGET_OS_HAIKU)
			gettimeofday(&m_impl->m_start, nullptr);
#elif defined (TARGET_OS_NDSFIRMWARE)
			TIMER0_CR = 0;
			TIMER1_CR = 0;
			TIMER0_DATA = 0;
			TIMER1_DATA = 0;
			TIMER1_CR = TIMER_ENABLE | TIMER_CASCADE;
			TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1;
#endif
		}

		void Stopwatch::Stop()
		{
#if defined (TARGET_OS_WINDOWS)
			QueryPerformanceCounter(&m_impl->m_finish);
#elif defined (TARGET_OS_MACOSX)
			m_impl->m_finish = mach_absolute_time();
#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_FREEBSD) || \
			defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD)
			gettimeofday(&m_impl->m_finish, nullptr);
#elif defined (TARGET_OS_NDSFIRMWARE)
			TIMER0_CR = 0;
#endif
		}

#if defined (TARGET_OS_WINDOWS)
		void StopwatchImpl::RecalculateFrequency()
		{

			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			m_tickInterval = 1.0 / (double)freq.QuadPart;
		}
#endif

		double Stopwatch::Elapsed()
		{

#if defined (TARGET_OS_WINDOWS)
			return ((double)m_impl->m_finish.QuadPart - (double)m_impl->m_start.QuadPart) * m_impl->m_tickInterval;
#elif defined (TARGET_OS_MACOSX)
			uint64_t elapsed = m_impl->m_finish - m_impl->m_start;
			return double (elapsed) * (m_impl->m_timebase.numer / m_impl->m_timebase.denom) / 1000000000.0;
#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_FREEBSD) || \
			defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD) || \
			defined (TARGET_OS_HAIKU)
			return (double)(m_impl->m_finish.tv_sec - m_impl->m_start.tv_sec) +
			       ((double)(m_impl->m_finish.tv_usec) - (double)(m_impl->m_start.tv_usec)) / 1000000.0;
#elif defined (TARGET_OS_NDSFIRMWARE)
			return (TIMER0_DATA | (TIMER1_DATA << 16)) / 33513982.0;
#endif
		}

		unsigned long Stopwatch::ElapsedMS()
		{

#if defined (TARGET_OS_WINDOWS)
			return (unsigned long)(((double)m_impl->m_finish.QuadPart - (double)m_impl->m_start.QuadPart) * m_impl->m_tickInterval * 1000.0);
#elif defined (TARGET_OS_MACOSX)
			uint64_t elapsed = m_impl->m_finish - m_impl->m_start;
			return double (elapsed) * (m_impl->m_timebase.numer / m_impl->m_timebase.denom) / 1000000.0;
#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_FREEBSD) || \
			defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD) || \
			defined (TARGET_OS_HAIKU)
			return (unsigned long)((m_impl->m_finish.tv_sec - m_impl->m_start.tv_sec) * 1000 +
			                       (m_impl->m_finish.tv_usec - m_impl->m_start.tv_usec) / 1000);
#elif defined (TARGET_OS_NDSFIRMWARE)
			return (TIMER0_DATA | (TIMER1_DATA << 16)) / 33514;
#endif
		}
	}
}
