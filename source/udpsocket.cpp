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
#include <crisscross/core_socket.h>
#include <crisscross/udpsocket.h>

/* We're leaving sockets unimplemented on the Nintendo DS for the moment. We
 * need to familiarize ourselves with the devkitARM API for sockets first */
#if !defined (TARGET_OS_NDSFIRMWARE)

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
typedef int socklen_t;
#endif

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
			struct sockaddr_in sin;
			struct hostent    *host;

			if (m_sock != INVALID_SOCKET) return CC_ERR_NOT_SOCKET;

			m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
			if (m_sock == INVALID_SOCKET)
				return GetError();

			SetAttributes(m_sock);

			host = gethostbyname(_address);
			if (!host) return GetError();

			memset(&sin, 0, sizeof(sin));
			sin.sin_family = AF_INET;
			sin.sin_addr.s_addr = (( struct in_addr * )(host->h_addr))->s_addr;
			sin.sin_port = htons(_port);

			if (connect(m_sock, (( struct sockaddr * )&sin), sizeof(sin)) != 0) {
				int err = GetError();

				/* Close the connection, it failed. */
#ifdef TARGET_OS_WINDOWS
				closesocket(m_sock);
#else
				close(m_sock);
#endif

				return err;
			}

			return CC_ERR_NONE;
		}

		int UDPSocket::SetAttributes(socket_t _socket)
		{
			return 0;
		}

		int UDPSocket::Listen(unsigned short _port)
		{
			struct sockaddr_in sin;

			if (m_sock != INVALID_SOCKET) return CC_ERR_NOT_SOCKET;

			memset(&sin, 0, sizeof(sin));

			sin.sin_family = PF_INET;
			sin.sin_addr.s_addr = INADDR_ANY;
			sin.sin_port = htons(_port);
			m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

			if (m_sock == INVALID_SOCKET)
				return GetError();

			SetAttributes(m_sock);

#if 0
			unsigned long arg = 1;
#if defined (TARGET_OS_WINDOWS)
			ioctlsocket(m_sock, FIONBIO, &arg);
#else
			ioctl(m_sock, FIONBIO, &arg);
#endif
#endif

			if (bind(m_sock, (sockaddr *)&sin, sizeof(sin)) != 0) {
				int err = GetError();

				/* Close the connection, it failed. */
#ifdef TARGET_OS_WINDOWS
				closesocket(m_sock);
#else
				close(m_sock);
#endif

				return err;
			}

			return CC_ERR_NONE;
		}
	}
}

#endif
