/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2021 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_system_h
#define __included_cc_system_h

#include <crisscross/platform_detect.h>

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
		long RandomNumber();

		/*! \brief Generates a random number. */
		/*!
		 * Seeds the random number generator with the current time.
		 */
		void SeedRandom();
	}
}
#endif
