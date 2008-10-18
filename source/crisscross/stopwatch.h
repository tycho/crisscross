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

#ifndef __included_cc_stopwatch_h
#define __included_cc_stopwatch_h

#ifndef __GNUC__
#include <crisscross/universal_include.h>
#endif

#if defined (TARGET_OS_MACOSX)
#include <mach/mach.h>
#include <mach/mach_time.h>
#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_NDSFIRMWARE)
#include <sys/time.h>
#include <time.h>
#endif

namespace CrissCross
{
	namespace System
	{
		/*! \brief A timer class designed for code profiling. */
		class Stopwatch
		{
			protected:
#if defined (TARGET_OS_WINDOWS)
				LARGE_INTEGER m_start, m_finish;
				double                    m_tickInterval;

				void RecalculateFrequency();
#elif defined (TARGET_OS_MACOSX)
				uint64_t m_start;
				uint64_t                  m_finish;
				mach_timebase_info_data_t m_timebase;
#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_FREEBSD) || \
				defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD)
				struct timeval            m_start;
				struct timeval            m_finish;
#elif defined (TARGET_OS_NDSFIRMWARE)
				/* Nothing here :) */
#else
#error No target OS defined (did you forget to include crisscross/universal_include.h?)
#endif
			public:
				/*! \brief The constructor. */
				Stopwatch();

				/*! \brief The destructor. */
				~Stopwatch();

				/*! \brief Starts the timer counter. */
				inline void Start()
				{
#if defined (TARGET_OS_WINDOWS)
					RecalculateFrequency();
					QueryPerformanceCounter(&m_start);
#elif defined (TARGET_OS_MACOSX)
					m_start = mach_absolute_time();
#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_FREEBSD) || \
					defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD)
					gettimeofday(&m_start, NULL);
#elif defined (TARGET_OS_NDSFIRMWARE)
					TIMER0_CR = 0;
					TIMER1_CR = 0;
					TIMER0_DATA = 0;
					TIMER1_DATA = 0;
					TIMER1_CR = TIMER_ENABLE | TIMER_CASCADE;
					TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1;
#endif
				};

				/*! \brief Stops the timer counter. */
				inline void Stop()
				{
#if defined (TARGET_OS_WINDOWS)
					QueryPerformanceCounter(&m_finish);
#elif defined (TARGET_OS_MACOSX)
					m_finish = mach_absolute_time();
#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_FREEBSD) || \
					defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD)
					gettimeofday(&m_finish, NULL);
#elif defined (TARGET_OS_NDSFIRMWARE)
					TIMER0_CR = 0;
#endif
				};

				/*! \brief Indicates the number of seconds elapsed. */
				/*!
				 * \return Number of seconds elapsed between the most recent set of Start() and Stop() calls.
				 */
				double Elapsed();

				/*! \brief Indicates the number of milliseconds elapsed. */
				/*!
				 * \return Number of milliseconds elapsed between the most recent set of Start() and Stop() calls.
				 */
				unsigned long ElapsedMS();
		};
	}
}

#endif
