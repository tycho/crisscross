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
#include <crisscross/debug.h>
#include <crisscross/core_network.h>
#include <crisscross/core_socket.h>

/* We're leaving sockets unimplemented on the Nintendo DS for the moment. We
 * need to familiarize ourselves with the devkitARM API for sockets first */
#if !defined (TARGET_OS_NDSFIRMWARE)

#if defined (TARGET_OS_WINDOWS)
#pragma comment (lib,"ws2_32.lib")
typedef int socklen_t;
#endif

#if !defined (TARGET_OS_WINDOWS) && !defined (TARGET_OS_NDSFIRMWARE)
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
#endif

#if defined (TARGET_OS_NDSFIRMWARE)
#include <sys/socket.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

/* #define PACKET_DEBUG */

namespace CrissCross
{
	namespace Network
	{
		CoreSocket::CoreSocket()
		{
			int retval = __initialise_network();
			m_calledInitialise = 1;
			CoreAssert(retval == CC_ERR_NONE);
			memset(&m_sock, 0, sizeof(socket_t));
			m_sock = INVALID_SOCKET;
			m_state = SOCKET_STATE_NOT_CREATED;
			m_bufferSize = 8192;
		}

		CoreSocket::CoreSocket(socket_t socket) : m_calledInitialise(0)
		{
			/* Calling __initialise_network() is NOT
			 * necessary if we have a socket to copy. */
			CoreAssert(socket != INVALID_SOCKET);
			m_sock = socket;
			m_state = SOCKET_STATE_UNKNOWN;
			m_bufferSize = 8192;
		}

		CoreSocket::~CoreSocket()
		{
			Close();
			if (m_calledInitialise == 1)
				__cleanup_network();
		}

		int CoreSocket::Close()
		{
			if (m_sock == INVALID_SOCKET) return CC_ERR_NOT_SOCKET;

			/* Close the socket. */
#ifdef TARGET_OS_WINDOWS
			closesocket(m_sock);
#else
			close(m_sock);
#endif
			m_sock = INVALID_SOCKET;
			m_state = SOCKET_STATE_NOT_CREATED;
			return CC_ERR_NONE;
		}

		const char *CoreSocket::GetRemoteIP()
		{
			/* Verify the socket. */
			if (m_sock == INVALID_SOCKET) return NULL;

			static char        buffer[15];
			struct sockaddr_in sock; int sock_size = sizeof(sock);
			memset(&sock, 0, sizeof(sock));

			/* Resolve the remote IP. */
			getpeername(m_sock, (sockaddr *)&sock, (socklen_t *)&sock_size);

			/* Print it to a buffer. */
			sprintf(buffer, inet_ntoa(sock.sin_addr));

			/* Return it. */
			return buffer;
		}

		u_long CoreSocket::GetLocalHost()
		{
			char                 str[512];
			long                 result = gethostname(str, sizeof(str));
			if (result < 0) {
				return 0;
			}

			struct hostent      *lpHostEnt;
			lpHostEnt = gethostbyname(str);

			struct sockaddr_in   addr;
			memcpy(&addr.sin_addr, lpHostEnt->h_addr_list[0], sizeof(addr.sin_addr));
			addr.sin_port = 0;

			const unsigned char *a = (const unsigned char *)&addr.sin_addr;
			result = (a[0] << 24) + (a[1] << 16) + (a[2] << 8) + a[3];

			return result;
		}

		u_long CoreSocket::GetRemoteHost()
		{
			if (m_sock == INVALID_SOCKET) return 0;

			struct sockaddr_in addr; int sock_size = sizeof(addr);
			memset(&addr, 0, sizeof(addr));
			getpeername(m_sock, (sockaddr *)&addr, (socklen_t *)&sock_size);

			/* Return the remote host (not the IP, but the bare host). */
			return addr.sin_addr.s_addr;
		}

		socket_t CoreSocket::GetSocket()
		{
			/* ugh. Allow the user to do whatever they want with the socket. */
			return m_sock;
		}

		int CoreSocket::Read(std::string &_output)
		{
			if (m_sock == INVALID_SOCKET) return CC_ERR_NOT_SOCKET;
			if (!IsReadable()) return CC_ERR_WOULD_BLOCK;

			_output = "";

			int   errbefore = GetError();

			char *buf = new char[m_bufferSize];
			int   recvlen = 0;
			bool  receivedData = false;
			do {
				memset(buf, 0, m_bufferSize);
				recvlen = recv(m_sock, buf, 1, MSG_PEEK);
				if (recvlen > 0) {
					recvlen = recv(m_sock, buf, m_bufferSize - 1, 0);
					_output += std::string(buf);
					receivedData = true;
				}
			} while (recvlen > 0);

			delete [] buf;

			int err = GetError();

			/* The error wasn't triggered by what we did here */
			if (err == errbefore)
				err = CC_ERR_NONE;

			return err;
		}

		int CoreSocket::Read(char *_output, unsigned int *_len)
		{
			/* Sanity checks. */
			if (m_sock == INVALID_SOCKET) return CC_ERR_NOT_SOCKET;
			if (_len == NULL || *_len < 1) return CC_ERR_BADPARAMETER;
			if (_output == NULL) return CC_ERR_BADPARAMETER;

			/* Set up a buffer. */
			int recvlen = 0;
			memset(_output, 0, *_len);

			/* Receive some data. */
			recvlen = recv(m_sock, _output, *_len, 0);
			if (recvlen < 0) {
				/* Something's wrong here... */
				return GetError();
			}

			*_len = recvlen;

			return CC_ERR_NONE;
		}

		int CoreSocket::GetError() const
		{
			CoreAssert(m_sock != 0);
			int retval = 0;

#if !defined (TARGET_OS_WINDOWS)
			retval = errno;
#else
			int retsize = sizeof(int), ret = 0;

			retval = WSAGetLastError();

			if (retval == WSAEWOULDBLOCK || retval == 0) {
				getsockopt(m_sock, SOL_SOCKET, SO_ERROR, (char *)&ret, (socklen_t *)&retsize);
				if (ret != 0)
					return TranslateError(ret);
			}

#endif
			return TranslateError(retval);
		}

		int CoreSocket::Send(const void *_data, size_t _length)
		{
			if (m_sock == INVALID_SOCKET) return CC_ERR_NOT_SOCKET;
			if (!IsWritable()) return CC_ERR_WOULD_BLOCK;

			int   sent = 0;
#ifdef PACKET_DEBUG
			char *temp_buf = new char[m_bufferSize];
			memset(temp_buf, 0, m_bufferSize);
			char *p = temp_buf, *d = (char *)_data;
			while (*d != '\x0') {
				if (!(*d == '\n' || *d == '\r'))
					*p = *d;

				p++; d++;
			}
			fprintf(stdout, "<<< '%s'\n", temp_buf);
			delete [] temp_buf;
#endif
			sent = send(m_sock, (const char *)_data, (int)_length, 0);

			return sent;
		}

		int CoreSocket::Send(std::string _data)
		{
			return Send(_data.c_str(), _data.size());
		}

		bool CoreSocket::IsReadable() const
		{
			int            ret;
			fd_set         read;
			struct timeval timeout;

			/* We only take 0.001 seconds to check */
			timeout.tv_sec = 0;
			timeout.tv_usec = 1000;

			FD_ZERO(&read);
			FD_SET(m_sock, &read);

			int            errbefore = GetError(), errafter;

			/* Select to check if it's readable. */
			ret = select(m_sock + 1, &read, NULL, NULL, &timeout);

			errafter = GetError();

			/* ret < 0   is error */
			/* ret == 0  is in progress */
			/* ret > 0   is success */
			if (ret < 0 || (errafter && errafter != errbefore && errafter != CC_ERR_WOULD_BLOCK && errafter != TRY_AGAIN)) {
				/* Bugger. Operation timed out. */
				m_state = SOCKET_STATE_ERROR;
				return false;
			} else if (ret == 0) {
				/* Keep going. No state change. */
				return false;
			} else {
				return true;
			}
		}

		bool CoreSocket::IsWritable() const
		{
			int            ret;
			fd_set         write;
			struct timeval timeout;

			/* We only take 0.001 seconds to check */
			timeout.tv_sec = 0;
			timeout.tv_usec = 1000;

			FD_ZERO(&write);
			FD_SET(m_sock, &write);

			int            errbefore = GetError(), errafter;

			/* Select to check if it's readable. */
			ret = select(m_sock + 1, NULL, &write, NULL, &timeout);

			errafter = GetError();

			/* ret < 0   is error */
			/* ret == 0  is in progress */
			/* ret > 0   is success */
			if (ret < 0 || (errafter && errafter != errbefore && errafter != CC_ERR_WOULD_BLOCK && errafter != TRY_AGAIN)) {
				/* Bugger. Operation timed out. */
				m_state = SOCKET_STATE_ERROR;
				return false;
			} else if (ret == 0) {
				/* Keep going. No state change. */
				return false;
			} else {
				return true;
			}
		}

		socketState CoreSocket::State() const
		{
			/* SO_ACCEPTCONN (DWORD:boolean, get) : determine whether a socket is listening. */
			/* SO_CONDITIONAL_ACCEPT (DWORD:boolean, get/set): determines whether a socket on */
			/*    the stack level will accept connections or not. */
			/* SO_ERROR (DWORD, get): per-socket error code. */
			/* */
			return m_state;
		}
	}
}

#endif
