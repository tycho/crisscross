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

#include <crisscross/universal_include.h>
#include <crisscross/stopwatch.h>

namespace CrissCross
{
	namespace System
	{
		Stopwatch::Stopwatch()
		{
#if defined (TARGET_OS_WINDOWS)
			RecalculateFrequency();
#elif defined (TARGET_OS_MACOSX)
			mach_timebase_info(&m_timebase);
#endif

			/* We start it here for static Stopwatch instances */
			/* where it's impractical to do an initial Start() call */
			Start();
		}

		Stopwatch::~Stopwatch()
		{
		}

#if defined (TARGET_OS_WINDOWS)
		void Stopwatch::RecalculateFrequency()
		{
			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			m_tickInterval = 1.0 / (double)freq.QuadPart;
		}
#endif

		double Stopwatch::Elapsed()
		{
#if defined (TARGET_OS_WINDOWS)
			return ((double)m_finish.QuadPart - (double)m_start.QuadPart) * m_tickInterval;
#elif defined (TARGET_OS_MACOSX)
			uint64_t elapsed = m_finish - m_start;
			return double( elapsed ) * (m_timebase.numer / m_timebase.denom) / 1000000000.0;
#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_FREEBSD) || \
			defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD)
			return (double)(m_finish.tv_sec - m_start.tv_sec) +
			       ((double)(m_finish.tv_usec) - (double)(m_start.tv_usec)) / 1000000.0;
#elif defined (TARGET_OS_NDSFIRMWARE)
			return (TIMER0_DATA | (TIMER1_DATA << 16)) / 33513982.0;
#endif
		}

		unsigned long Stopwatch::ElapsedMS()
		{
#if defined (TARGET_OS_WINDOWS)
			return   (unsigned long)(((double)m_finish.QuadPart - (double)m_start.QuadPart) * m_tickInterval * 1000.0);
#elif defined (TARGET_OS_MACOSX)
			uint64_t elapsed = m_finish - m_start;
			return double( elapsed ) * (m_timebase.numer / m_timebase.denom) / 1000000.0;
#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_FREEBSD) || \
			defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD)
			return   (unsigned long)((m_finish.tv_sec - m_start.tv_sec) * 1000 +
			                         (m_finish.tv_usec - m_start.tv_usec) / 1000);
#elif defined (TARGET_OS_NDSFIRMWARE)
			return (TIMER0_DATA | (TIMER1_DATA << 16)) / 33514;
#endif
		}
	}
}
