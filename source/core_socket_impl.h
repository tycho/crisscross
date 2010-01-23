/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2010 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */


#ifndef __included_cc_core_socket_impl_h
#define __included_cc_core_socket_impl_h

#if !defined (TARGET_OS_WINDOWS)
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <signal.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#else
#include <winsock2.h>
#include <windows.h>
#ifdef TARGET_COMPILER_VC
#include <mstcpip.h>
#endif
#endif

namespace CrissCross
{
	namespace Network
	{
		struct CoreSocketImpl {
#ifdef TARGET_OS_WINDOWS
			SOCKET m_sock;
#else
			int m_sock;
#endif
		};
#ifdef TARGET_OS_WINDOWS
		typedef SOCKET socket_t;
		typedef int socklen_t;
#else
		typedef int socket_t;
#endif
		static int SetSocketAttributes(socket_t _socket);
	}
}

#endif