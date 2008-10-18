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
#include <crisscross/tcpsocket.h>

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
		TCPSocket::TCPSocket() : CoreSocket()
		{
			m_proto = PROTOCOL_TCP;
		}

		TCPSocket::TCPSocket(socket_t _socket) : CoreSocket(_socket)
		{
			m_proto = PROTOCOL_TCP;
			m_state = SOCKET_STATE_CONNECTED;             /* Assumed. */
		}

		TCPSocket::~TCPSocket()
		{
		}

		int TCPSocket::Accept(TCPSocket * *_socket)
		{
			/* We're forced to accept any incoming connection, unfortunately. */
			socket_t sock = accept(m_sock, 0, 0);

			/* Did we truly accept a connection? */
			if (sock != INVALID_SOCKET) {
				/* Set up the typical transmission attributes. */
				SetAttributes(sock);

#if 0
				unsigned long arg = 1;

				/* Non-blocking I/O, if possible. Ignore any errors. */
#if defined (TARGET_OS_WINDOWS)
				ioctlsocket(m_sock, FIONBIO, &arg);
#else
				ioctl(m_sock, FIONBIO, &arg);
#endif
#endif

				/* Create a new wrapper for our socket. */
				TCPSocket    *csock = new TCPSocket(sock);

				m_state = SOCKET_STATE_CONNECTED;

				/* We're done. */
				*_socket = csock;
				return CC_ERR_NONE;
			}

			/* Nothing accepted. */
			*_socket = NULL;
			return CC_ERR_NO_SOCK;
		}

		int TCPSocket::Connect(const char *_address, unsigned short _port)
		{
			struct sockaddr_in sin;
			struct hostent    *host;

			/* Close any existing connections on this socket. */
			Close();

			/* Open a new TCP/IP socket. */
			m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

			/* Verify the socket. */
			if (m_sock == INVALID_SOCKET)
				return GetError();

			/* Set up the typical transmission attributes. */
			/* SetAttributes ( m_sock ); */

#if 0
			unsigned long arg = 1;

			/* Non-blocking I/O, if possible. Ignore any errors. */
#if defined (TARGET_OS_WINDOWS)
			ioctlsocket(m_sock, FIONBIO, &arg);
#else
			ioctl(m_sock, FIONBIO, &arg);
#endif
#endif

			/* Resolve the IP of the host we're trying to connect to. */
			host = gethostbyname(_address);
			if (!host) return GetError();

			/* Set up our sockaddr_in. */
			memset(&sin, 0, sizeof(sin));
			sin.sin_family = AF_INET;
			sin.sin_addr.s_addr = (( struct in_addr * )(host->h_addr))->s_addr;
			sin.sin_port = htons(_port);

			/* Attempt a connection. */
			if (connect(m_sock, (( struct sockaddr * )&sin), sizeof(sin)) != 0) {
				int err = GetError();

				/* If this is a non-blocking socket, we need to handle appropriately. */
				if (err == CC_ERR_WOULD_BLOCK) {
					m_state = SOCKET_STATE_CONNECTING;
					return CC_ERR_WOULD_BLOCK;
				} else {
					m_state = SOCKET_STATE_ERROR;
					/* Close the connection, it failed. */
#ifdef TARGET_OS_WINDOWS
					closesocket(m_sock);
#else
					close(m_sock);
#endif
					return err;
				}
			}

			m_state = SOCKET_STATE_CONNECTED;
			return CC_ERR_NONE;
		}

		int TCPSocket::Listen(unsigned short _port)
		{
			struct sockaddr_in sin;

			/* Verify our socket isn't in use. */
			if (m_sock != INVALID_SOCKET) return CC_ERR_INVALID_CALL;

			/* Set up our sockaddr_in */
			memset(&sin, 0, sizeof(sin));
			sin.sin_family = PF_INET;
			sin.sin_addr.s_addr = INADDR_ANY;
			sin.sin_port = htons(_port);

			/* Open a new TCP/IP socket. */
			m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

			/* Verify the socket. */
			if (m_sock == INVALID_SOCKET)
				return GetError();

			/* Set up the typical transmission attributes. */
			SetAttributes(m_sock);

#if 0
			unsigned long arg = 1;

			/* Non-blocking I/O, if possible. Ignore any errors. */
#if defined (TARGET_OS_WINDOWS)
			ioctlsocket(m_sock, FIONBIO, &arg);
#else
			ioctl(m_sock, FIONBIO, &arg);
#endif
#endif

			/* Bind our socket to our given port number. */
			if (bind(m_sock, (sockaddr *)&sin, sizeof(sin)) != 0) {
				/* Bind failure, for some reason. */
				int err = GetError();

#ifdef TARGET_OS_WINDOWS
				closesocket(m_sock);
#else
				close(m_sock);
#endif

				return err;
			}

			/* Listen on the given port, with a maximum of 10 half-open connections. */
			if (listen(m_sock, 10) == SOCKET_ERROR) {
				/* Listen failure, for some reason. */
#ifdef TARGET_OS_WINDOWS
				closesocket(m_sock);
#else
				close(m_sock);
#endif
				return GetError();
			}

			m_state = SOCKET_STATE_LISTENING;
			return CC_ERR_NONE;
		}

		int TCPSocket::SetAttributes(socket_t _socket)
		{
			/* TCP_NODELAY */
			int           err, optval = 1, optlen = sizeof optval;
			err = setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY,
			                 (char *)&optval, optlen);
			if (err == -1) return errno;

			/* SO_LINGER */
			struct linger linger_opts;
			linger_opts.l_onoff = 1;
			linger_opts.l_linger = 10;
			optlen = sizeof linger_opts;
			err = setsockopt(_socket, SOL_SOCKET, SO_LINGER,
			                 (char *)&linger_opts, optlen);
			if (err == -1) return errno;

			/* SO_REUSEADDR */
#ifndef TARGET_OS_WINDOWS
			optval = 1;
			optlen = sizeof(optval);
			setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR,
			           (char *)&optval, optlen);
#endif

			/* SO_KEEPALIVE */
#ifdef TARGET_OS_WINDOWS
			DWORD         bytesReturned = 0;
			tcp_keepalive vals;
			vals.keepalivetime = 30000;
			vals.keepaliveinterval = 10000;
			vals.onoff = 1;
			err = WSAIoctl(_socket, SIO_KEEPALIVE_VALS,
			               (char *)&vals, sizeof(vals), NULL, 0,
			               &bytesReturned, NULL, NULL);

			if (err == -1) return WSAGetLastError();

#endif
			optlen = sizeof optval;
			err = setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE,
			                 (char *)&optval, optlen);
			if (err == -1) return errno;

			/* SO_SNDTIMEO and SO_RCVTIMEO */
			struct timeval tv;
			tv.tv_sec = 5;
			tv.tv_usec = 0;

			err = setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO,
			                 (char *)&tv, sizeof(tv));
			if (err == -1) return errno;

			err = setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO,
			                 (char *)&tv, sizeof(tv));
			if (err == -1) return errno;

			return CC_ERR_NONE;
		}

		socketState TCPSocket::State() const
		{
			/* Make sure there have been no spontaneous state changes. */
			switch (m_state)
			{
			case SOCKET_STATE_CONNECTING:
			{
				if (IsReadable() || IsWritable())
					m_state = SOCKET_STATE_CONNECTED;
			}
			default:
				break;
			}
			return m_state;
		}
	}
}

#endif
