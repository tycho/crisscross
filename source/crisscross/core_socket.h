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

#ifndef __included_cc_core_socket_h
#define __included_cc_core_socket_h

#ifndef __GNUC__
#include <crisscross/universal_include.h>
#endif

#include <crisscross/core_network.h>
#include <crisscross/rbtree.h>

#ifdef TARGET_OS_WINDOWS
#include <windows.h>
#define socket_t SOCKET
#else
#define socket_t int
#endif

/* We're leaving sockets unimplemented on the Nintendo DS for the moment. We
 * need to familiarize ourselves with the devkitARM API for sockets first */
#if !defined (TARGET_OS_NDSFIRMWARE)

namespace CrissCross
{
	namespace Network
	{
		/*! \brief Possible states of socket instances. */
		typedef enum
		{
			SOCKET_STATE_UNKNOWN,           /*! \brief< The state has not yet been specified or is currently not known. */
			SOCKET_STATE_NOT_CREATED,       /*! \brief< The socket is not yet instantiated. */
			SOCKET_STATE_ERROR,             /*! \brief< The socket has encountered an error. */
			SOCKET_STATE_CLOSED,            /*! \brief< The socket is closed. */
			SOCKET_STATE_LISTENING,         /*! \brief< The socket is listening for incoming connections (or data, if UDP). */
			SOCKET_STATE_CONNECTING,        /*! \brief< The socket is doing an asynchronous connect to a remote host. */
			SOCKET_STATE_CONNECTED          /*! \brief< The socket is connected to a remote host. */
		} socketState;

		/*! \brief Socket protocols. */
		typedef enum
		{
			PROTOCOL_NONE,                          /*! \brief< No protocol specified. */
			PROTOCOL_TCP,                            /*! \brief< Transmission Control Protocol. */
			PROTOCOL_UDP                            /*! \brief< User Datagram Protocol. */
		} socketProtocol;

		/*! \brief The abstract core socket class. */
		/*!
		 *  Abstract class only. Must be inherited.
		 */
		class CoreSocket
		{
			protected:
				/*! \brief The maximum number of bytes to read per CoreSocket::Read or CoreSocket::ReadLine call. */
				int            m_bufferSize;

				/*! \brief Indicates whether __socket_initialise() was called when the class was initialized. */
				char           m_calledInitialise;

				/*! \brief Stores the socket data. */
				socket_t       m_sock;

				/*! \brief Indicates the protocol used by this socket instance. */
				socketProtocol m_proto;

				/*! \brief Indicates the current state of m_sock. */
				mutable socketState m_state;

				/*! \brief Sets some important attributes on the socket. */
				/*!
				 *  Will set SO_LINGER and TCP_NODELAY on TCP sockets.
				 * \param _socket The socket to modify.
				 * \return CC_ERR_NONE if no error is encountered, otherwise returns 'errno'.
				 */
				virtual int SetAttributes(socket_t _socket) = 0;
			public:

				/*! \brief The default constructor. */
				CoreSocket();

				/*! \brief The constructor for an existing socket. */
				CoreSocket(socket_t socket);

				/*! \brief The destructor. */
				virtual ~CoreSocket();

				/*! \brief Close the socket. */
				/*!
				 *  Doesn't need to be called before an instance is destructed, because
				 *  it is automatically called in the destructor.
				 * \return Currently always returns CC_ERR_NONE.
				 */
				virtual int Close();

				/*! \brief Get the error value for the socket. */
				/*!
				 * \return The error value for the socket.
				 */
				virtual int GetError() const;

				/*! \brief Fetches the IP address of the remote host. */
				/*!
				 * \return The host represented in old-style sockaddr_in format.
				 */
				u_long GetRemoteHost();

				/*! \brief Fetches the IP address of the remote host. */
				/*!
				 * \return A pointer to the IP address string in dot notation. Note that the
				 *  pointer this returns will be reused on the next GetRemoteIP call, so the
				 *  data pointed at by the return value should be copied into another buffer.
				 */
				const char *GetRemoteIP();

				/*! \brief Fetches the IP address of the remote host. */
				/*!
				 * \return The host represented in old-style sockaddr_in format.
				 */
				u_long GetLocalHost();

				/*! \brief Fetches the IP address of the remote host. */
				/*!
				 * \return A pointer to the IP address string in dot notation. Note that the
				 *  pointer this returns will be reused on the next GetRemoteIP call, so the
				 *  data pointed at by the return value should be copied into another buffer.
				 */
				const char *GetLocalIP();

				/*! \brief Gives access to the socket itself (for extensibility only). */
				/*!
				 * \return CoreSocket::m_sock
				 */
				socket_t GetSocket();

				/*! \brief Determines whether the socket is ready for a Read operation. */
				/*!
				 * \return True if the socket is writable, false otherwise.
				 * \warning If the return value is false, check the return value of State()
				 * to make sure that the socket hasn't entered an error state.
				 * \sa State
				 * \sa CrissCross::Network::socketState
				 */
				virtual bool IsReadable() const;

				/*! \brief Determines whether the socket is ready for a Read operation. */
				/*!
				 * \return True if the socket is writable, false otherwise.
				 * \warning If the return value is false, check the return value of State()
				 * to make sure that the socket hasn't entered an error state.
				 * \sa State
				 * \sa CrissCross::Network::socketState
				 */
				virtual bool IsWritable() const;

				/*! \brief Fetch the state of the socket. */
				/*!
				 * \return The current state of m_sock.
				 * \sa CrissCross::Network::socketState
				 */
				virtual socketState State() const;

				/*! \brief Opens the port specified to listen for incoming connections. */
				/*!
				 * \param _port The port to listen on.
				 * \return If the return value is greater than zero, it is an 'errno'
				 *  value. If it is less than zero, it is a socketError value.
				 */
				virtual int Listen(unsigned short _port) = 0;

				/*! \brief Reads a block of data with a specified maximum size. */
				/*!
				 * \param _output A buffer with size _len. Will contain the received
				 *  data on return.
				 * \param _len The maximum number of bytes to read. On return, this
				 *  will contain the size of data received.
				 * \return  If the return value is greater than zero, it is an 'errno'
				 *  value. If it is less than zero, it is a socketError value.
				 */
				virtual int Read(char *_output, unsigned int *_len);

				/*! \brief Reads a block of data with a specified maximum size. */
				/*!
				 * \param _output An std::string in which the data will be stored.
				 * \return  If the return value is greater than zero, it is an 'errno'
				 *  value. If it is less than zero, it is a socketError value.
				 */
				virtual int Read(std::string &_output);

				/*! \brief Sends a block of data. */
				/*!
				 * \param _data The data to be sent.
				 * \param _length The number of bytes of _data to send (must NOT exceed the size of _data).
				 * \return The actual number of bytes sent.
				 */
				virtual int Send(const void *_data, size_t _length);

				/*! \brief Sends a string. */
				/*!
				 * \param _data The string to be sent.
				 * \return The actual number of bytes sent.
				 */
				virtual int Send(std::string _data);
		};
	}
}

#endif

#endif
