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

#include <crisscross/universal_include.h>

#include <cstdio>
#include <cstring>

#include <crisscross/core_socket.h>
#include <crisscross/udpsocket.h>

/* We're leaving sockets unimplemented on the Nintendo DS for the moment. We
 * need to familiarize ourselves with the devkitARM API for sockets first */
#if !defined (TARGET_OS_NDSFIRMWARE)

#include "core_socket_impl.h"

namespace CrissCross
{
	namespace Network
	{
		UDPSocket::UDPSocket() : CoreSocket()
		{
			m_proto = PROTOCOL_UDP;
		}

		UDPSocket::~UDPSocket()
		{
		}

		int UDPSocket::Bind(const char *_address, unsigned short _port)
		{
			CoreAssert(this != NULL);

			struct sockaddr_in sin;
			struct hostent *host;

			if (m_impl->m_sock != INVALID_SOCKET) return CC_ERR_NOT_SOCKET;

			m_impl->m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
			if (m_impl->m_sock == INVALID_SOCKET)
				return GetError();

			SetSocketAttributes(m_impl->m_sock);

			host = gethostbyname((char *)_address);
			if (!host) return GetError();

			memset(&sin, 0, sizeof(sin));
			sin.sin_family = AF_INET;
			sin.sin_addr.s_addr = (( struct in_addr * )(host->h_addr))->s_addr;
			sin.sin_port = htons(_port);

			if (connect(m_impl->m_sock, (( struct sockaddr * )&sin), sizeof(sin)) != 0) {
				int err = GetError();

				/* Close the connection, it failed. */
#ifdef TARGET_OS_WINDOWS
				closesocket(m_impl->m_sock);
#elif defined(TARGET_OS_HAIKU)
				shutdown(m_impl->m_sock, SHUT_RDWR);
#else
				close(m_impl->m_sock);
#endif

				return err;
			}

			return CC_ERR_NONE;
		}

		int UDPSocket::Listen(unsigned short _port)
		{
			CoreAssert(this != NULL);

			struct sockaddr_in sin;

			if (m_impl->m_sock != INVALID_SOCKET) return CC_ERR_NOT_SOCKET;

			memset(&sin, 0, sizeof(sin));

			sin.sin_family = PF_INET;
			sin.sin_addr.s_addr = INADDR_ANY;
			sin.sin_port = htons(_port);
			m_impl->m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

			if (m_impl->m_sock == INVALID_SOCKET)
				return GetError();

			SetSocketAttributes(m_impl->m_sock);

#if 0
			unsigned long arg = 1;
#if defined (TARGET_OS_WINDOWS)
			ioctlsocket(m_impl->m_sock, FIONBIO, &arg);
#else
			ioctl(m_impl->m_sock, FIONBIO, &arg);
#endif
#endif

			if (bind(m_impl->m_sock, (sockaddr *)&sin, sizeof(sin)) != 0) {
				int err = GetError();

				/* Close the connection, it failed. */
#ifdef TARGET_OS_WINDOWS
				closesocket(m_impl->m_sock);
#elif defined(TARGET_OS_HAIKU)
				shutdown(m_impl->m_sock, SHUT_RDWR);
#else
				close(m_impl->m_sock);
#endif
				return err;
			}

			return CC_ERR_NONE;
		}

		static int SetSocketAttributes(socket_t _socket)
		{
			return 0;
		}
	}
}

#endif
