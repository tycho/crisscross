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

#ifndef __included_cc_system_h
#define __included_cc_system_h

#ifndef __GNUC__
#include <crisscross/universal_include.h>
#endif
#include <crisscross/cpuid.h>

#if defined (TARGET_OS_WINDOWS)
#include <windows.h>
#elif defined (TARGET_OS_MACOSX)
#include <mach/mach.h>
#include <mach/mach_time.h>
#elif defined (TARGET_OS_LINUX)
#include <sys/time.h>
#include <sched.h>
#include <time.h>
#endif

namespace CrissCross
{
	namespace System
	{
		/*! \brief Initializes the high-resolution timer. */
		void InitTimer();

		/*! \brief Pauses or resumes the high-resolution timer. */
		/*!
		 *  If _paused is true, the current timestamp on the high-resolution timer is locked.
		 *  If _paused is false, the GetHighResTime() will compensate for the pause duration.
		 *  \param _paused Indicates whether to pause or unpause the timer.
		 */
		void SetTimerState(bool _paused);

		/*! \brief Increments the paused timer by the specified value. */
		/*!
		 *  \param _seconds Time in seconds to increase the timer by.
		 */
		void AdvancePausedTimer(double _seconds);

		/*! \brief Gets the time since InitTimer() was called. */
		/*!
		 *  \return Time in seconds since InitTimer().
		 */
		double GetHighResTime();

		/*! \brief Sleeps the current thread for a specified time. */
		/*!
		 * \param _msec Time to sleep for, in milliseconds.
		 */
		void ThreadSleep(int _msec);

		/*! \brief Generates a random number. */
		/*!
		 * Don't forget to use SeedRandom() first!
		 * \sa SeedRandom()
		 */
		int RandomNumber();

		/*! \brief Generates a random number. */
		/*!
		 * Seeds the random number generator with the current time.
		 */
		void SeedRandom();

#if defined (TARGET_OS_WINDOWS)
		/*! \brief Waits for the specified thread to finish executing. */
		/*!
		 * \param _thread Thread to wait for.
		 * \param _timeout The maximum wait time. (currently ignored)
		 * \return Always zero, until _timeout is implemented.
		 */
		int WaitForThread(HANDLE _thread, DWORD _timeout);
#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_FREEBSD) || defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD)
		/*! \brief Waits for the specified thread to finish executing. */
		/*!
		 * \param _thread Thread to wait for.
		 * \param _timeout The maximum wait time. (currently ignored)
		 * \return Always zero, until _timeout is implemented.
		 */
		int WaitForThread(pthread_t _thread, int _timeout);
#endif
	}
}
#endif
