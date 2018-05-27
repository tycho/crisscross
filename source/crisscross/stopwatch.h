/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2018 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_stopwatch_h
#define __included_cc_stopwatch_h

#include <crisscross/platform_detect.h>

namespace CrissCross
{
	namespace System
	{
		struct StopwatchImpl;

		/*! \brief A timer class designed for code profiling. */
		class Stopwatch
		{
			protected:
				StopwatchImpl *m_impl;

			public:
				/*! \brief The constructor. */
				Stopwatch();

				/*! \brief The destructor. */
				~Stopwatch();

				/*! \brief Starts the timer counter. */
				void Start();

				/*! \brief Stops the timer counter. */
				void Stop();

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
