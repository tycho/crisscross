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
#include <crisscross/core_network.h>

#if !defined (TARGET_OS_WINDOWS)
#include <sys/socket.h>
#include <signal.h>
#endif

static int s_initialised = 0;

CrissCross::Errors CrissCross::Network::__initialise_network()
{
	if (!s_initialised) {
#ifdef TARGET_OS_WINDOWS
		/* Start up the windows networking */
		WORD    version_wanted = MAKEWORD(2, 2);
		WSADATA wsaData;

		if (WSAStartup(version_wanted, &wsaData) != 0)
			return CC_ERR_INTERNAL;

		if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
			return CC_ERR_INTERNAL;

#else
		/* SIGPIPE is generated when a remote socket is closed */
		void    (*handler)(int);
		handler = signal(SIGPIPE, SIG_IGN);
		if (handler != SIG_DFL) {
			signal(SIGPIPE, handler);
		}

#endif
	}

	++s_initialised;
	return CC_ERR_NONE;
}

CrissCross::Errors CrissCross::Network::__cleanup_network()
{
	if (s_initialised == 0) {
		return CC_ERR_NONE;
	}

	if (--s_initialised == 0) {
#ifdef TARGET_OS_WINDOWS
		/* Clean up windows networking */
		if (WSACleanup() == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAEINPROGRESS) {
				WSACancelBlockingCall();
				WSACleanup();
			}
		}

#else
		/* Restore the SIGPIPE handler */
		void (*handler)(int);
		handler = signal(SIGPIPE, SIG_DFL);
		if (handler != SIG_IGN) {
			signal(SIGPIPE, handler);
		}

#endif
	}

	return CC_ERR_NONE;
}
