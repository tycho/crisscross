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
#include <crisscross/debug.h>
#include <crisscross/mutex.h>

#ifndef TARGET_OS_NDSFIRMWARE

namespace CrissCross
{
	namespace System
	{
		Mutex::Mutex()
		{
			m_lockCount = 0;
#ifdef TARGET_OS_WINDOWS
			InitializeCriticalSection(&m_criticalSection);
#else
			int error;
			error = pthread_mutexattr_init(&m_mutexAttr);
			CoreAssert(error == 0);
			error = pthread_mutexattr_settype(&m_mutexAttr, PTHREAD_MUTEX_RECURSIVE);
			CoreAssert(error == 0);
			error = pthread_mutex_init(&m_hMutex, &m_mutexAttr);
			CoreAssert(error == 0);
#endif
		}

		Mutex::~Mutex()
		{
			CoreAssert(m_lockCount == 0);
#ifdef TARGET_OS_WINDOWS
			DeleteCriticalSection(&m_criticalSection);
#else
			pthread_mutex_destroy(&m_hMutex);
#endif
		}

		void Mutex::Lock()
		{
			m_lockCount++;
#ifdef TARGET_OS_WINDOWS
			EnterCriticalSection(&m_criticalSection);
#else
			int error = pthread_mutex_lock(&m_hMutex);
			CoreAssert(error == 0);
#endif
		}

		void Mutex::Unlock()
		{
			m_lockCount--;
#ifdef TARGET_OS_WINDOWS
			LeaveCriticalSection(&m_criticalSection);
#else
			int error = pthread_mutex_unlock(&m_hMutex);
			CoreAssert(error == 0);
#endif
		}
	}
}

#endif
