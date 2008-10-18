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

#ifndef __included_cc_md2_h
#define __included_cc_md2_h

#ifdef ENABLE_HASHES

#include <crisscross/deprecate.h>

#define MD2_DIGEST_LENGTH       16

/* MD2 context. */
/* @cond */
typedef struct  {
	unsigned char state[16];                               /* state */
	unsigned char checksum[16];                         /* checksum */
	unsigned int count;               /* number of bytes, modulo 16 */
	unsigned char buffer[16];                       /* input buffer */
} cc_md2_ctx;
/* @endcond */

namespace CrissCross
{
	namespace Crypto
	{
#if !defined(DISABLE_DEPRECATED_CODE)
		/*! \brief An MD2 hash generator. */
		/*!
		 * \deprecated MD2 hashes are rarely used anymore because they are very slow to
		 *              generate. The code for them is optimized for 8-bit machines and is no longer
		 *              suitable for production use. It's highly recommended that you use MD4 or MD5
		 *              as a replacement of MD2.
		 * \sa Hash MD4Hash MD5Hash
		 */
		class MD2Hash
		{
			private:
				mutable char *m_hashString;
				unsigned char *m_hash;
				cc_md2_ctx     m_state;

			public:
				/*! \brief The default constructor. */
				_CC_DEPRECATE_SLOW("SEVERE") MD2Hash();

				/*! \brief The destructor. */
				~MD2Hash();

				/*! \brief Runs an MD2 hash on the data provided. */
				/*!
				 * \param _data The data to hash. The buffer does not need to be null
				 *              terminated.
				 * \param _length The data length in bytes.
				 * \return Zero on success, nonzero on failure.
				 */
				int Process(const void *_data, size_t _length);

				/*! \brief Runs a hash on the file provided. */
				/*!
				 * \param _reader The pre-opened CoreIOReader to run the hash on.
				 * \return Zero on success, nonzero on failure.
				 */
				int Process(CrissCross::IO::CoreIOReader *_reader);

				/*! \brief Processes a piece of the dataset. */
				/*!
				 * This function will process only a segment of a larger dataset. It is designed
				 * to be called multiple times before an eventual Finalize() call.
				 * \param _data The data segment to hash.
				 * \param _length The length of the data segment in bytes.
				 */
				int ProcessBlock(const void *_data, size_t _length);

				/*! \brief Finalizes the ProcessBlock() calls and generates the final hash value. */
				void Finalize();

				/*! \brief Resets the internal MD2 context and hash buffer. */
				void Reset();

				/*! \brief Converts the internal hash data into an hex string, a human readable format. */
				/*!
				 *      The memory location returned by this function is freed when the class
				 *      is destructed.
				 */
				const char *ToString() const;

				/*! \brief Equality operator. */
				/*!
				 *      Compares two instances of MD2Hash to see if the hashes are equal.
				 * \param _other The other instance of MD2Hash to compare to.
				 */
				bool operator==(const MD2Hash &_other) const;

				/*! \brief Inequality operator. */
				/*!
				 *      Compares two instances of MD2Hash to see if the hashes are not equal.
				 * \param _other The other instance of MD2Hash to compare to.
				 */
				inline bool operator!=(const MD2Hash &_other) const
				{
					return !(*this == _other);
				};
		};
#endif
	}
}

#endif

#endif
