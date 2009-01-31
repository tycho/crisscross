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
		/*! \brief The type of mutex to be created. */
		enum MutexType
		{
			MUTEX_TYPE_NORMAL,     //!< This type of mutex does not detect deadlock. A thread attempting to relock this mutex without first unlocking it will deadlock. Attempting to unlock a mutex locked by a different thread results in undefined behaviour. Attempting to unlock an unlocked mutex results in undefined behaviour.
			MUTEX_TYPE_RECURSIVE,  //!< A thread attempting to relock this mutex without first unlocking it will succeed in locking the mutex. The relocking deadlock which can occur with mutexes of type MUTEX_TYPE_NORMAL cannot occur with this type of mutex. Multiple locks of this mutex require the same number of unlocks to release the mutex before another thread can acquire the mutex. A thread attempting to unlock a mutex which another thread has locked will return with an error. A thread attempting to unlock an unlocked mutex will return with an error.
			MUTEX_TYPE_ERRORCHECK  //!< This type of mutex provides error checking. A thread attempting to relock this mutex without first unlocking it will return with an error. A thread attempting to unlock a mutex which another thread has locked will return with an error. A thread attempting to unlock an unlocked mutex will return with an error.
		};

		class MutexHolder;

		/*! \brief A mutex class for safe multithreading. */
		/*!
		 *  Mutual exclusion (often abbreviated to mutex) algorithms are used in 
		 *  concurrent programming to avoid the simultaneous use of a common
		 *  resource, such as a global variable, by pieces of computer code called
		 *  critical sections.
		 */
		class Mutex
		{
		protected:
			unsigned m_lockCount;
			MutexType m_type;

		#ifdef TARGET_OS_WINDOWS
			/*! \brief The critical section for the mutex. */
			/*!
			 *  Windows uses "critical sections" for safe threading.
			 */
			CRITICAL_SECTION m_mutex;
		#else
			/*! \brief POSIX threading mutex. */
			pthread_mutexattr_t m_mutexAttr;
			pthread_mutex_t m_mutex;
		#endif
			void Lock();
			void Unlock();

		public:
			/*! \brief The constructor. */
			/*!
			 *  By default creates a mutex of type MUTEX_TYPE_RECURSIVE.
			 *  \sa MutexType
			 */
			Mutex(MutexType _type = MUTEX_TYPE_RECURSIVE);

			/*! \brief The destructor. */
			~Mutex();

			friend class MutexHolder;
		};

		class RWLockHolder;

		/*! \brief A read-write lock. */
		/*!
		 *  A read/write lock allows concurrent read access to an object but requires exclusive access for write operations.
		 */
		class ReadWriteLock
		{
		protected:
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

			bool LockRead();
			bool LockWrite();
#ifdef TARGET_OS_WINDOWS
			void UnlockRead();
			void UnlockWrite();
#else
			void Unlock();
#endif

		public:
			ReadWriteLock(bool _writePriority = false);
			~ReadWriteLock();

			friend class RWLockHolder;
		};

		/*! \brief A mutex holder which locks a Mutex class. */
		class MutexHolder
		{
		protected:
			Mutex *m_mutex;

		public:
			/*! \brief The constructor. */
			/*!
			 *  Locks the specified mutex.
			 *  \param _mutex The mutex to be locked.
			 */
			MutexHolder(Mutex *_mutex);

			/*! \brief The destructor. */
			/*!
			 *  Unlocks the held mutex.
			 */
			~MutexHolder();
		};

		/*! \brief The type of mutex holder to create. */
		enum RWLockHolderType
		{
			LOCK_READ,
			LOCK_WRITE
		};

		/*! \brief A read/write lock holder which locks a Mutex class. */
		class RWLockHolder
		{
		protected:
			ReadWriteLock *m_lock;
			RWLockHolderType m_type;

		public:
			/*! \brief The constructor. */
			/*!
			 *  Locks the specified read/write lock.
			 *  \param _mutex The read/write lock to be held.
			 *  \param _type Indicates whether this lock holder should lock for reading or for writing.
			 */
			RWLockHolder(ReadWriteLock *_lock, RWLockHolderType _type);

			/*! \brief The destructor. */
			/*!
			 *  Unlocks the held lock.
			 */
			~RWLockHolder();
		};
	}
}

#endif

#endif
