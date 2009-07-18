/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2009 Steven Noonan.
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
		Mutex::Mutex(MutexType _type)
			: m_type(_type)
		{
			m_lockCount = 0;
#ifdef TARGET_OS_WINDOWS
			InitializeCriticalSection(&m_mutex);
#else
			int error;
			error = pthread_mutexattr_init(&m_mutexAttr);
			CoreAssert(error == 0);
			int ptype = 0;
			switch (m_type)
			{
			case MUTEX_TYPE_NORMAL:
				ptype = PTHREAD_MUTEX_NORMAL;
				break;
			case MUTEX_TYPE_ERRORCHECK:
				ptype = PTHREAD_MUTEX_ERRORCHECK;
				break;
			case MUTEX_TYPE_RECURSIVE:
			default:
				ptype = PTHREAD_MUTEX_RECURSIVE;
				break;
			}
			error = pthread_mutexattr_settype(&m_mutexAttr, ptype);
			CoreAssert(error == 0);
			error = pthread_mutex_init(&m_mutex, &m_mutexAttr);
			CoreAssert(error == 0);
#endif
		}

		Mutex::~Mutex()
		{
			CoreAssert(this != NULL);

			CoreAssert(m_lockCount == 0);
#ifdef TARGET_OS_WINDOWS
			DeleteCriticalSection(&m_mutex);
#else
			pthread_mutex_destroy(&m_mutex);
			pthread_mutexattr_destroy(&m_mutexAttr);
#endif
		}

		void Mutex::Lock()
		{
			CoreAssert(this != NULL);
#ifdef TARGET_OS_WINDOWS
			EnterCriticalSection(&m_mutex);
#else
			int error = pthread_mutex_lock(&m_mutex);
			CoreAssert(error == 0);
#endif
			m_lockCount++;
		}

		void Mutex::Unlock()
		{
			CoreAssert(this != NULL);
			CoreAssert(m_lockCount > 0);
#ifdef TARGET_OS_WINDOWS
			LeaveCriticalSection(&m_mutex);
#else
			int error = pthread_mutex_unlock(&m_mutex);
			CoreAssert(error == 0);
#endif
			m_lockCount--;
		}

		MutexHolder::MutexHolder(Mutex *_mutex)
			: m_mutex(_mutex)
		{
			CoreAssert(this != NULL);
			m_mutex->Lock();
		}

		MutexHolder::~MutexHolder()
		{
			CoreAssert(this != NULL);
			m_mutex->Unlock();
		}

		ReadWriteLock::ReadWriteLock(bool _writePriority)
		{
#ifdef TARGET_OS_WINDOWS
			wrpriority = _writePriority;

			rdcount = rdwaiting = wrcount = wrwaiting = 0;

			InitializeCriticalSection(&rwcs);

			rdgreen = CreateEvent(NULL, FALSE, TRUE, NULL);
			wrgreen = CreateEvent(NULL, FALSE, TRUE, NULL);
#else
			int ret;
			ret = pthread_rwlockattr_init(&m_rwlockAttr);
			CoreAssert(ret == 0);
			ret = pthread_rwlockattr_setpshared(&m_rwlockAttr, PTHREAD_PROCESS_PRIVATE);
			CoreAssert(ret == 0);
			ret = pthread_rwlock_init(&m_rwlock, &m_rwlockAttr);
			CoreAssert(ret == 0);
#endif
		}

		ReadWriteLock::~ReadWriteLock()
		{
#ifdef TARGET_OS_WINDOWS
			CloseHandle(rdgreen);
			CloseHandle(wrgreen);
			DeleteCriticalSection(&rwcs);
#else
			pthread_rwlock_destroy(&m_rwlock);
			pthread_rwlockattr_destroy(&m_rwlockAttr);
#endif
		}

		bool ReadWriteLock::LockRead()
		{
			CoreAssert(this != NULL);
#ifdef TARGET_OS_WINDOWS
			bool wait = false;
			DWORD timeout = INFINITE;

			do {
				EnterCriticalSection(&rwcs);

				//
				// acquire lock if 
				//   - there are no active writers and 
				//     - readers have priority or
				//     - writers have priority and there are no waiting writers
				//
				if(!wrcount && (!wrpriority || !wrwaiting)) {
					if(wait) {
						rdwaiting--;
						wait = false;
					}
					rdcount++;
				}
				else {
					if(!wait) {
						rdwaiting++;
						wait = true;
					}
					// always reset the event to avoid 100% CPU usage
					ResetEvent(rdgreen);
				}

				LeaveCriticalSection(&rwcs);

				if (wait) {
					if (WaitForSingleObject(rdgreen, timeout) != WAIT_OBJECT_0) {
						EnterCriticalSection(&rwcs);
						rdwaiting--;
						SetEvent(rdgreen); SetEvent(wrgreen);
						LeaveCriticalSection(&rwcs);
						return false;
					}
				}

			} while (wait);

			return true;
#else
			int ret = pthread_rwlock_rdlock(&m_rwlock);
			return (ret == 0);
#endif
		}

		bool ReadWriteLock::LockWrite()
		{
			CoreAssert(this != NULL);
#ifdef TARGET_OS_WINDOWS
			bool wait = false;
			DWORD timeout = INFINITE;

			do {
				EnterCriticalSection(&rwcs);

				//
				// acquire lock if 
				//   - there are no active readers nor writers and 
				//     - writers have priority or
				//     - readers have priority and there are no waiting readers
				//
				if (!rdcount && !wrcount && (wrpriority || !rdwaiting)) {
					if(wait) {
						wrwaiting--;
						wait = false;
					}
					wrcount++;
				}
				else {
					if(!wait) {
						wrwaiting++;
						wait = true;
					}
					// always reset the event to avoid 100% CPU usage
					ResetEvent(wrgreen);
				}

				LeaveCriticalSection(&rwcs);

				if (wait) {
					if (WaitForSingleObject(wrgreen, timeout) != WAIT_OBJECT_0) {
						EnterCriticalSection(&rwcs);
						wrwaiting--;
						SetEvent(rdgreen); SetEvent(wrgreen);
						LeaveCriticalSection(&rwcs);
						return false;
					}
				}

			} while (wait);

			return true;
#else
			int ret = pthread_rwlock_wrlock(&m_rwlock);
			return (ret == 0);
#endif
		}

#ifdef TARGET_OS_WINDOWS
		void ReadWriteLock::UnlockRead()
		{
			CoreAssert(this != NULL);
			EnterCriticalSection(&rwcs);

			rdcount--;

			// always release waiting threads (do not check for rdcount == 0)
			if (wrpriority) {
				if (wrwaiting)
					SetEvent(wrgreen);
				else if (rdwaiting)
					SetEvent(rdgreen);
			}
			else {
				if (rdwaiting)
					SetEvent(rdgreen);
				else if (wrwaiting)
					SetEvent(wrgreen);
			}

			LeaveCriticalSection(&rwcs);
		}

		void ReadWriteLock::UnlockWrite()
		{
			CoreAssert(this != NULL);
			EnterCriticalSection(&rwcs);

			wrcount--;

			if (wrpriority) {
				if (wrwaiting)
					SetEvent(wrgreen);
				else if (rdwaiting)
					SetEvent(rdgreen);
			}
			else {
				if (rdwaiting)
					SetEvent(rdgreen);
				else if (wrwaiting)
					SetEvent(wrgreen);
			}

			LeaveCriticalSection(&rwcs);
		}
#else
		void ReadWriteLock::Unlock()
		{
			CoreAssert(this != NULL);
			pthread_rwlock_unlock(&m_rwlock);
		}
#endif

		RWLockHolder::RWLockHolder(ReadWriteLock *_lock, RWLockHolderType _type)
			: m_lock(_lock), m_type(_type), m_copied(false)
		{
			CoreAssert(_lock);
			switch(_type)
			{
			case RW_LOCK_READ:
				m_lock->LockRead();
				break;
			case RW_LOCK_WRITE:
				m_lock->LockWrite();
				break;
			default:
				CoreAssert(_type != RW_LOCK_READ && _type != RW_LOCK_WRITE);
			}
		}

		RWLockHolder::~RWLockHolder()
		{
			CoreAssert(this != NULL);
			if (!m_copied) {
#ifdef TARGET_OS_WINDOWS
				switch(m_type)
				{
				case RW_LOCK_READ:
					m_lock->UnlockRead();
					break;
				case RW_LOCK_WRITE:
					m_lock->UnlockWrite();
					break;
				default:
					CoreAssert(m_type != RW_LOCK_READ && m_type != RW_LOCK_WRITE);
				}
#else
				m_lock->Unlock();
#endif
			}
		}

		RWLockHolder::RWLockHolder(RWLockHolder const &_lock)
		{
			CoreAssert(this != &_lock);
			_lock.m_copied = true;
			m_lock = _lock.m_lock;
			m_type = _lock.m_type;
		}
	}
}

#endif
