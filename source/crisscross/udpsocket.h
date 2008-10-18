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

#ifndef __included_cc_udpsocket_h
#define __included_cc_udpsocket_h

#include <crisscross/error.h>
#include <crisscross/core_socket.h>

/* We're leaving sockets unimplemented on the Nintendo DS for the moment. We
 * need to familiarize ourselves with the devkitARM API for sockets first */
#if !defined (TARGET_OS_NDSFIRMWARE)

namespace CrissCross
{
	namespace Network
	{
		/*! \brief UDP/IP socket class. */
		/*!
		 *  Used for connectionless UDP/IP communication. Note that this is one-way,
		 *  and there must be a second UDP/IP socket created for communication in
		 *  the opposite direction.
		 */
		class UDPSocket : public CoreSocket
		{
			private:

				int SetAttributes(socket_t _socket);

			public:
				/*! \brief The constructor. */
				UDPSocket();
				/*! \brief The destructor. */
				~UDPSocket();

				/*! \brief Binds an outbound UDP/IP socket to the specified address and port. */
				/*!
				 * \param _address The remote address to bind to. Can be a hostname, as it will be resolved by gethostbyname().
				 * \param _port The remote port to bind to.
				 * \return Due to the use of Berkley sockets, this returns zero on success, and a nonzero value on failure.
				 */
				int Bind(const char *_address, unsigned short _port);

				/*! \brief Binds an inbound UDP/IP socket on the specified port. */
				/*!
				 * \param _port The port to bind.
				 * \return Due to the use of Berkley sockets, this returns zero on success, and a nonzero value on failure.
				 */
				int Listen(unsigned short _port);
		};
	}
}

#endif

#endif
