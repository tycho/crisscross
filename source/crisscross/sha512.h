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

#ifndef __included_cc_sha512_h
#define __included_cc_sha512_h

#ifdef ENABLE_HASHES

#include <crisscross/deprecate.h>

#define SHA512_DIGEST_SIZE (512 / 8)
#define SHA512_BLOCK_SIZE  (1024 / 8)

/* @cond */
typedef struct {
	unsigned int tot_len;
	unsigned int len;
	unsigned char block[2 * SHA512_BLOCK_SIZE];
	cc_uint64_t h[8];
} cc_sha512_ctx;
/* @endcond */

namespace CrissCross
{
	namespace Crypto
	{
		/*! \brief A SHA-512 hash generator. */
		/*!
		 * \warning When compiled as 32-bit code, this hash is particularly slow.
		 *              When compiled as 64-bit code, this hash type can actually be faster
		 *              to generate than a SHA-256 hash.
		 * \sa Hash SHA1Hash SHA256Hash TigerHash
		 */
		class SHA512Hash
		{
			private:
				mutable char *m_hashString;
				cc_sha512_ctx  m_state;
				unsigned char *m_hash;

			public:
				/*! \brief The default constructor. */
				SHA512Hash();

				/*! \brief The destructor. */
				~SHA512Hash();

				/*! \brief Runs a SHA-512 hash on the data provided. */
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

				/*! \brief Resets the internal SHA-512 context and hash buffer. */
				void Reset();

				/*! \brief Converts the internal hash data into an hex string, a human readable format. */
				/*!
				 *      The memory location returned by this function is freed when the class
				 *      is destructed.
				 */
				const char *ToString() const;

				/*! \brief Equality operator. */
				/*!
				 *      Compares two instances of SHA512Hash to see if the hashes are equal.
				 * \param _other The other instance of SHA512Hash to compare to.
				 */
				bool operator==(const SHA512Hash &_other) const;

				/*! \brief Inequality operator. */
				/*!
				 *      Compares two instances of SHA512Hash to see if the hashes are not equal.
				 * \param _other The other instance of SHA512Hash to compare to.
				 */
				inline bool operator!=(const SHA512Hash &_other) const
				{
					return !(*this == _other);
				};
		};
	}
}

#endif

#endif
