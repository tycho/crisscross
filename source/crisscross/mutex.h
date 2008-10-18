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

#ifndef __included_cc_mutex_h
#define __included_cc_mutex_h

#ifndef __GNUC__
#include <crisscross/universal_include.h>
#endif

#ifndef TARGET_OS_NDSFIRMWARE

#if defined (TARGET_OS_WINDOWS)
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace CrissCross
{
	namespace System
	{
		/*! \brief The safe threading mutex class. */
		/*!
		 *  Allows for safe threading by locking via thread ID.
		 */
		class Mutex
		{
			protected:

				unsigned            m_lockCount;

		#ifdef TARGET_OS_WINDOWS
				/*! \brief The critical section for the mutex. */
				/*!
				 * Windows uses "critical sections" for safe threading.
				 */
				CRITICAL_SECTION    m_criticalSection;
		#else
				/*! \brief POSIX threading mutex. */

				pthread_mutexattr_t m_mutexAttr;
				pthread_mutex_t     m_hMutex;
		#endif
			public:

				/*! \brief The constructor. */
				Mutex();

				/*! \brief The destructor. */
				~Mutex();

				/*! \brief Locks the mutex. */
				void Lock();

				/*! \brief Unlocks the mutex. */
				void Unlock();
		};
	}
}

#endif

#endif
