/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2020 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include <crisscross/universal_include.h>

#if defined (TARGET_OS_WINDOWS)
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#if defined (TARGET_OS_MACOSX)
#include <mach/mach.h>
#include <mach/mach_time.h>
#elif defined (TARGET_OS_LINUX)
#include <sched.h>
#include <time.h>
#endif

#include <crisscross/system.h>

namespace CrissCross
{
	namespace System
	{
		namespace
		{
			long holdrand = 1L;

			double pausedAt = 0.0;
			double timeShift = 0.0;
		    bool timerPaused = false;
	#if defined (TARGET_OS_WINDOWS)

			double __m_tickInterval;

	#elif defined (TARGET_OS_MACOSX)

			uint64_t __m_start;
			mach_timebase_info_data_t __m_timebase;

	#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_FREEBSD) || \
		defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD) || \
		defined (TARGET_OS_NDSFIRMWARE)

			timeval __m_start;

	#endif
		}

		void InitTimer()
		{
	#if defined (TARGET_OS_WINDOWS)
			LARGE_INTEGER freq;

			QueryPerformanceFrequency(&freq);
			__m_tickInterval = 1.0 / ( double )freq.QuadPart;
	#elif defined (TARGET_OS_MACOSX)
			mach_timebase_info(&__m_timebase);
			__m_start = mach_absolute_time();
	#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_FREEBSD) || \
			defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD) || \
			defined (TARGET_OS_NDSFIRMWARE)
			gettimeofday(&__m_start, NULL);
	#endif
			timerPaused = false;
		}

		void SetTimerState(bool _paused)
		{
			if (_paused && !timerPaused) {
				pausedAt = GetHighResTime();
				timerPaused = true;
			} else if (!_paused && timerPaused) {
				timeShift = GetHighResTime() - pausedAt;
				timerPaused = false;
			}
		}

		void AdvancePausedTimer(double _seconds)
		{
			pausedAt += _seconds;
		}

		double GetHighResTime()
		{
			double retval;
	#if defined (TARGET_OS_WINDOWS)
			LARGE_INTEGER count;

			QueryPerformanceCounter(&count);
			retval = ( double )count.QuadPart * __m_tickInterval;
	#elif defined (TARGET_OS_MACOSX)
			uint64_t elapsed = mach_absolute_time() - __m_start;
			retval = double (elapsed) * (__m_timebase.numer / __m_timebase.denom) /
			         1000000000.0;
	#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_FREEBSD) || \
			defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD) || \
			defined (TARGET_OS_NDSFIRMWARE)
			timeval now;
			double t1, t2;

			gettimeofday(&now, NULL);

			t1 = ( double )__m_start.tv_sec +
			     ( double )__m_start.tv_usec / (1000 * 1000);
			t2 = ( double )now.tv_sec + ( double )now.tv_usec / (1000 * 1000);
			retval = t2 - t1;
	#endif
			return retval - timeShift;
		}

		void ThreadSleep(int _msec)
		{
			if (_msec < 0) return;

	#if defined (TARGET_OS_WINDOWS)
			Sleep(_msec);
	#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_MACOSX)
			unsigned int sleep_time = _msec * 1000;

			while (sleep_time > 1000000) {
				usleep(1000000);
				sleep_time -= 1000000;
			}
			usleep(sleep_time);
	#endif
		}

		long RandomNumber()
		{
#ifdef TARGET_OS_WINDOWS
			return (((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
#else
			return lrand48();
#endif
		}

		void SeedRandom()
		{
#if defined (TARGET_OS_WINDOWS)
			holdrand = GetTickCount();
#else
			struct timeval t;
			gettimeofday(&t, NULL);
			holdrand = t.tv_usec;
			srand48(holdrand);
#endif
		}
	}
}
