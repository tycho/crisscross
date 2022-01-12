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
#include <crisscross/debug.h>
#include <crisscross/mutex.h>

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
		struct MutexImpl {
		#ifdef TARGET_OS_WINDOWS
			CRITICAL_SECTION m_mutex;
		#else
			pthread_mutexattr_t m_mutexAttr;
			pthread_mutex_t m_mutex;
		#endif
		};

		Mutex::Mutex(MutexType _type)
			: m_type(_type)
		{
			m_impl = new MutexImpl;
			m_lockCount = 0;
#ifdef TARGET_OS_WINDOWS
			InitializeCriticalSection(&m_impl->m_mutex);
#else
			int error;
			error = pthread_mutexattr_init(&m_impl->m_mutexAttr);
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
			error = pthread_mutexattr_settype(&m_impl->m_mutexAttr, ptype);
			CoreAssert(error == 0);
			error = pthread_mutex_init(&m_impl->m_mutex, &m_impl->m_mutexAttr);
			CoreAssert(error == 0);
#endif
		}

		Mutex::~Mutex()
		{
			CoreAssert(m_lockCount == 0);
#ifdef TARGET_OS_WINDOWS
			DeleteCriticalSection(&m_impl->m_mutex);
#else
			pthread_mutex_destroy(&m_impl->m_mutex);
			pthread_mutexattr_destroy(&m_impl->m_mutexAttr);
#endif
			delete m_impl;
			m_impl = nullptr;
		}

		void Mutex::Lock()
		{
#ifdef TARGET_OS_WINDOWS
			EnterCriticalSection(&m_impl->m_mutex);
#else
			int error = pthread_mutex_lock(&m_impl->m_mutex);
			CoreAssert(error == 0);
#endif
			m_lockCount++;
		}

		void Mutex::Unlock()
		{
			CoreAssert(m_lockCount > 0);
			m_lockCount--;
#ifdef TARGET_OS_WINDOWS
			LeaveCriticalSection(&m_impl->m_mutex);
#else
			int error = pthread_mutex_unlock(&m_impl->m_mutex);
			CoreAssert(error == 0);
#endif
		}

		MutexHolder::MutexHolder(Mutex *_mutex)
			: m_mutex(_mutex)
		{
			m_mutex->Lock();
		}

		MutexHolder::~MutexHolder()
		{
			m_mutex->Unlock();
		}

		struct RWLockImpl {
#ifdef TARGET_OS_WINDOWS
			bool wrpriority;

			DWORD rdcount;
			DWORD rdwaiting;

			DWORD wrcount;
			DWORD wrwaiting;

			HANDLE rdgreen, wrgreen;
			CRITICAL_SECTION rwcs;
#else
			pthread_rwlock_t m_rwlock;
			pthread_rwlockattr_t m_rwlockAttr;
#endif
		};

		ReadWriteLock::ReadWriteLock(bool _writePriority)
		{
			m_impl = new RWLockImpl;
#ifdef TARGET_OS_WINDOWS
			m_impl->wrpriority = _writePriority;

			m_impl->rdcount = m_impl->rdwaiting =
				m_impl->wrcount = m_impl->wrwaiting = 0;

			InitializeCriticalSection(&m_impl->rwcs);

			m_impl->rdgreen = CreateEvent(nullptr, FALSE, TRUE, nullptr);
			m_impl->wrgreen = CreateEvent(nullptr, FALSE, TRUE, nullptr);
#else
			int ret;
			ret = pthread_rwlockattr_init(&m_impl->m_rwlockAttr);
			CoreAssert(ret == 0);
			ret = pthread_rwlockattr_setpshared(&m_impl->m_rwlockAttr, PTHREAD_PROCESS_PRIVATE);
			CoreAssert(ret == 0);
			ret = pthread_rwlock_init(&m_impl->m_rwlock, &m_impl->m_rwlockAttr);
			CoreAssert(ret == 0);
#endif
		}

		ReadWriteLock::~ReadWriteLock()
		{
#ifdef TARGET_OS_WINDOWS
			CloseHandle(m_impl->rdgreen);
			CloseHandle(m_impl->wrgreen);
			DeleteCriticalSection(&m_impl->rwcs);
#else
			pthread_rwlock_destroy(&m_impl->m_rwlock);
			pthread_rwlockattr_destroy(&m_impl->m_rwlockAttr);
#endif
		}

		bool ReadWriteLock::LockRead()
		{
#ifdef TARGET_OS_WINDOWS
			bool wait = false;
			DWORD timeout = INFINITE;

			do {
				EnterCriticalSection(&m_impl->rwcs);

				//
				// acquire lock if 
				//   - there are no active writers and 
				//     - readers have priority or
				//     - writers have priority and there are no waiting writers
				//
				if(!m_impl->wrcount &&
					(!m_impl->wrpriority || !m_impl->wrwaiting)) {
					if(wait) {
						m_impl->rdwaiting--;
						wait = false;
					}
					m_impl->rdcount++;
				}
				else {
					if(!wait) {
						m_impl->rdwaiting++;
						wait = true;
					}
					// always reset the event to avoid 100% CPU usage
					ResetEvent(m_impl->rdgreen);
				}

				LeaveCriticalSection(&m_impl->rwcs);

				if (wait) {
					if (WaitForSingleObject(m_impl->rdgreen, timeout) != WAIT_OBJECT_0) {
						EnterCriticalSection(&m_impl->rwcs);
						m_impl->rdwaiting--;
						SetEvent(m_impl->rdgreen); SetEvent(m_impl->wrgreen);
						LeaveCriticalSection(&m_impl->rwcs);
						return false;
					}
				}

			} while (wait);

			return true;
#else
			int ret = pthread_rwlock_rdlock(&m_impl->m_rwlock);
			return (ret == 0);
#endif
		}

		bool ReadWriteLock::LockWrite()
		{
#ifdef TARGET_OS_WINDOWS
			bool wait = false;
			DWORD timeout = INFINITE;

			do {
				EnterCriticalSection(&m_impl->rwcs);

				//
				// acquire lock if 
				//   - there are no active readers nor writers and 
				//     - writers have priority or
				//     - readers have priority and there are no waiting readers
				//
				if (!m_impl->rdcount && !m_impl->wrcount &&
					(m_impl->wrpriority || !m_impl->rdwaiting)) {
					if(wait) {
						m_impl->wrwaiting--;
						wait = false;
					}
					m_impl->wrcount++;
				}
				else {
					if(!wait) {
						m_impl->wrwaiting++;
						wait = true;
					}
					// always reset the event to avoid 100% CPU usage
					ResetEvent(m_impl->wrgreen);
				}

				LeaveCriticalSection(&m_impl->rwcs);

				if (wait) {
					if (WaitForSingleObject(m_impl->wrgreen, timeout) != WAIT_OBJECT_0) {
						EnterCriticalSection(&m_impl->rwcs);
						m_impl->wrwaiting--;
						SetEvent(m_impl->rdgreen); SetEvent(m_impl->wrgreen);
						LeaveCriticalSection(&m_impl->rwcs);
						return false;
					}
				}

			} while (wait);

			return true;
#else
			int ret = pthread_rwlock_wrlock(&m_impl->m_rwlock);
			return (ret == 0);
#endif
		}

#ifdef TARGET_OS_WINDOWS
		void ReadWriteLock::UnlockRead()
		{
			EnterCriticalSection(&m_impl->rwcs);

			m_impl->rdcount--;

			// always release waiting threads (do not check for rdcount == 0)
			if (m_impl->wrpriority) {
				if (m_impl->wrwaiting)
					SetEvent(m_impl->wrgreen);
				else if (m_impl->rdwaiting)
					SetEvent(m_impl->rdgreen);
			}
			else {
				if (m_impl->rdwaiting)
					SetEvent(m_impl->rdgreen);
				else if (m_impl->wrwaiting)
					SetEvent(m_impl->wrgreen);
			}

			LeaveCriticalSection(&m_impl->rwcs);
		}

		void ReadWriteLock::UnlockWrite()
		{
			EnterCriticalSection(&m_impl->rwcs);

			m_impl->wrcount--;

			if (m_impl->wrpriority) {
				if (m_impl->wrwaiting)
					SetEvent(m_impl->wrgreen);
				else if (m_impl->rdwaiting)
					SetEvent(m_impl->rdgreen);
			}
			else {
				if (m_impl->rdwaiting)
					SetEvent(m_impl->rdgreen);
				else if (m_impl->wrwaiting)
					SetEvent(m_impl->wrgreen);
			}

			LeaveCriticalSection(&m_impl->rwcs);
		}
#else
		void ReadWriteLock::Unlock()
		{
			pthread_rwlock_unlock(&m_impl->m_rwlock);
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
			_lock.m_copied = true;
			m_lock = _lock.m_lock;
			m_type = _lock.m_type;
		}
	}
}

#endif
