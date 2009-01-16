/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2009 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_tiger_h
#define __included_cc_tiger_h

#ifdef ENABLE_HASHES

#include <crisscross/deprecate.h>

#define TIGER_DIGEST_SIZE 24

/*! \brief@cond */
typedef struct {
	cc_uint64_t a, b, c;
	unsigned char buf[64];
	int count;
	cc_uint32_t nblocks;
} cc_tiger_ctx;
/*! \brief@endcond */

namespace CrissCross
{
	namespace Crypto
	{
		/*! \brief A Tiger hash generator. */
		/*!
		 * \warning When compiled as 32-bit code, this hash is particularly slow.
		 *              When compiled as 64-bit code, this hash type can actually be faster
		 *              to generate than a SHA-256 hash or even a SHA-512 hash.
		 * \sa Hash SHA1Hash SHA256Hash SHA512Hash
		 */
		class TigerHash
		{
			private:
				mutable char *m_hashString;
				unsigned char *m_hash;
				cc_tiger_ctx   m_state;

			public:
				/*! \brief The default constructor. */
				TigerHash();

				/*! \brief The destructor. */
				~TigerHash();

				/*! \brief Runs a Tiger hash on the data provided. */
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

				/*! \brief Resets the internal Tiger context and hash buffer. */
				void Reset();

				/*! \brief Converts the internal hash data into an hex string, a human readable format. */
				/*!
				 *      The memory location returned by this function is freed when the class
				 *      is destructed.
				 * \warning There's no standardized Tiger hex output format yet. It's highly recommended
				 *              that hashes generated by this class ONLY be compared with hashes generated by
				 *              another instance of this class.
				 */
				const char *ToString() const;

				/*! \brief Equality operator. */
				/*!
				 *      Compares two instances of TigerHash to see if the hashes are equal.
				 * \param _other The other instance of TigerHash to compare to.
				 */
				bool operator==(const TigerHash &_other) const;

				/*! \brief Inequality operator. */
				/*!
				 *      Compares two instances of TigerHash to see if the hashes are not equal.
				 * \param _other The other instance of TigerHash to compare to.
				 */
				inline bool operator!=(const TigerHash &_other) const
				{
					return !(*this == _other);
				};
		};
	}
}

#endif

#endif
