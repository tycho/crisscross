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

#ifndef __included_cc_sha1_h
#define __included_cc_sha1_h

#ifdef ENABLE_HASHES

#define SHA1_DIGEST_SIZE 20

/* @cond */
typedef struct {
	unsigned int state[5];
	unsigned int count[2];
	unsigned char buffer[64];
} cc_sha1_ctx;
/* @endcond */

namespace CrissCross
{
	namespace Crypto
	{
		/*! \brief A SHA-1 hash generator. */
		/*!
		 * \sa Hash SHA256Hash SHA512Hash TigerHash
		 */
		class SHA1Hash
		{
			private:
				mutable char *m_hashString;
				cc_sha1_ctx    m_state;
				unsigned char *m_hash;

			public:
				/*! \brief The default constructor. */
				SHA1Hash();

				/*! \brief The destructor. */
				~SHA1Hash();

				/*! \brief Runs a SHA-1 hash on the data provided. */
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

				/*! \brief Resets the internal SHA-1 context and hash buffer. */
				void Reset();

				/*! \brief Converts the internal hash data into an hex string, a human readable format. */
				/*!
				 *      The memory location returned by this function is freed when the class
				 *      is destructed.
				 */
				const char *ToString() const;

				/*! \brief Equality operator. */
				/*!
				 *      Compares two instances of SHA1Hash to see if the hashes are equal.
				 * \param _other The other instance of SHA1Hash to compare to.
				 */
				bool operator==(const SHA1Hash &_other) const;

				/*! \brief Inequality operator. */
				/*!
				 *      Compares two instances of SHA1Hash to see if the hashes are not equal.
				 * \param _other The other instance of SHA1Hash to compare to.
				 */
				inline bool operator!=(const SHA1Hash &_other) const
				{
					return !(*this == _other);
				};
		};
	}
}

#endif

#endif
