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

#include <crisscross/universal_include.h>

#ifndef TARGET_OS_WINDOWS

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <pthread.h>

#include <crisscross/zombiehunter.h>

using namespace CrissCross::System;

namespace
{
	void *prepareToShotgunTheZombieMenace(void *param)
	{
		ZombieHunter::ZombieWatcherDossier *data = (ZombieHunter::ZombieWatcherDossier *)param;
		CoreAssert(data != NULL);
		size_t ticks = 0;
		pid_t ret = 0;
		
		// Wait for us to hit the time limit.
		while (ticks < data->m_maxWait) {
			// With WNOHANG, this can exit immediately.
			ret = waitpid(data->m_pid, &data->m_ret, WNOHANG);
			
			// ret == 0 means it exited because it was
			// instructed not to hang.
			if (ret != 0) break;
			
			// Sleep for 1s.
			for (size_t i = 0; i < 100; i++)
				usleep(10000);
	
			ticks++;
		}
	
		// Kill the child process because it took too long.
		if (ret == 0)
			kill(data->m_pid, 9);
	
		data->m_active = false;
		if (!data->m_storeExitCode)
			delete data;
	
		return NULL;
	}
}

namespace CrissCross
{
	namespace System
	{
		ZombieHunter::ZombieHunter()
		{
		}
		
		ZombieHunter::~ZombieHunter()
		{
		}
		
		void ZombieHunter::watch(pid_t _pid, size_t _maxWait, bool _storeExitCode)
		{
			if (m_pids.exists(_pid)) return;

			ZombieWatcherDossier *dossier = new ZombieWatcherDossier();
			memset(dossier, 0, sizeof(ZombieWatcherDossier));
			dossier->m_maxWait = _maxWait;
			dossier->m_active = true;
			dossier->m_pid = _pid;
			dossier->m_storeExitCode = _storeExitCode;
			int ret = pthread_create(&dossier->m_thread, NULL, prepareToShotgunTheZombieMenace, dossier);
			CoreAssert(ret == 0);
			
			if (_storeExitCode)
				m_pids.insert(_pid, dossier);
		}
		
		bool ZombieHunter::returned(pid_t _pid, int *_ret)
		{
			ZombieWatcherDossier *dossier = m_pids.find(_pid);
			if (!dossier) return false;
			if (dossier->m_active) return false;
			if (_ret) {
				*_ret = dossier->m_ret;
			}
			return true;
		}
		
		void ZombieHunter::erase(pid_t _pid)
		{
			m_pids.erase(_pid);
		}
	}
}

#endif
