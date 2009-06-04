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

#ifndef __included_cc_zombiehunter_h
#define __included_cc_zombiehunter_h

#ifndef TARGET_OS_WINDOWS

#include <crisscross/darray.h>
#include <crisscross/avltree.h>

#include <unistd.h>

namespace CrissCross
{
	namespace System
	{
		//! A forked process watcher
		/*!
			This class will track a given process ID and
			asynchronously wait for it to exit (via a thread).
			This allows your program to continue executing, but
			eliminates the possibility of a zombie process
			caused by a fork() without a wait().
			
			\note UNIX-based platforms only (*BSD, Linux, Mac OS X)
		 */
		class ZombieHunter
		{
		public:
			typedef struct {
				size_t m_maxWait;
				pid_t m_pid;
				pthread_t m_thread;
				int m_ret;
				bool m_active;
				bool m_storeExitCode;
			} ZombieWatcherDossier;

		private:
			//! Stores the results and active watcher stores.
			CrissCross::Data::AVLTree<pid_t, ZombieWatcherDossier *>	m_pids;

		public:
			ZombieHunter();
			~ZombieHunter();
			
			//! Watches a child process for an exit code.
			/*!
				This function will start a thread to asynchronously
				check for an exit code for a specific process.
				\param _pid The process to wait for.
				\param _maxWait The maximum time that the process
					should be allowed to live (in seconds). -1 means
					wait indefinitely.
				\param _storeExitCode Do we want to store the exit code
					of the process being watched? If 'false', the
					watcher thread will delete the ZombieWatcherDossier
					structure upon completion.
			 */
			void watch(pid_t _pid, size_t _maxWait = -1, bool _storeExitCode = true);
			
			//! Gets the exit code for the given process.
			/*!
				\param _pid The process ID to get the exit code for.
				\param _ret The integer to store the return value in.
				\return True if the child process returned and an
					exit code was stored in _ret. False on failure.
			 */
			bool returned(pid_t _pid, int *_ret);
			
			//! Erase the return code from the history.
			/*!
				\param _pid The process ID to remove the exit code for.
			 */
			void erase(pid_t _pid);
		};
	}
}

#endif

#endif
