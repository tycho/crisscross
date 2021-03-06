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

#ifndef __included_cc_mutex_h
#define __included_cc_mutex_h

#include <crisscross/platform_detect.h>

#ifndef TARGET_OS_NDSFIRMWARE

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
		struct MutexImpl;

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
			MutexImpl *m_impl;

			void Lock();
			void Unlock();

		public:
			/*! \brief The constructor. */
			/*!
			 *  By default creates a mutex of type MUTEX_TYPE_RECURSIVE.
			 *  \param _type The type of mutex to create.
			 *  \sa MutexType
			 */
			Mutex(MutexType _type = MUTEX_TYPE_RECURSIVE);

			/*! \brief The destructor. */
			~Mutex();

			friend class MutexHolder;
		};

		class RWLockHolder;
		struct RWLockImpl;

		/*! \brief A read-write lock. */
		/*!
		 *  A read/write lock allows concurrent read access to an object but requires exclusive access for write operations.
		 */
		class ReadWriteLock
		{
		protected:
			RWLockImpl *m_impl;

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

		private:
			MutexHolder(MutexHolder const &);
			MutexHolder &operator =(const MutexHolder &);
		};

		/*! \brief The type of mutex holder to create. */
		enum RWLockHolderType
		{
			RW_LOCK_READ,
			RW_LOCK_WRITE
		};

		/*! \brief A read/write lock holder which locks a Mutex class. */
		class RWLockHolder
		{
		protected:
			ReadWriteLock *m_lock;
			RWLockHolderType m_type;
			mutable bool m_copied;

		public:
			/*! \brief The constructor. */
			/*!
			 *  Locks the specified read/write lock.
			 *  \param _lock The read/write lock to be held.
			 *  \param _type Indicates whether this lock holder should
			 *               lock for reading or for writing.
			 */
			RWLockHolder(ReadWriteLock *_lock, RWLockHolderType _type);

			/*! \brief The copy constructor. */
			/*!
			 *  Copies the RWLock and invalidates the original (the
			 *  copy takes over the task of the original).
			 */
			RWLockHolder(RWLockHolder const &);

			/*! \brief The destructor. */
			/*!
			 *  Unlocks the held lock.
			 */
			~RWLockHolder();

		private:
			RWLockHolder &operator =(RWLockHolder const &);
		};
	}
}

#endif

#endif
