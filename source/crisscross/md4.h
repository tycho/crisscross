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

#ifndef __included_cc_md4_h
#define __included_cc_md4_h

#ifdef ENABLE_HASHES

#include <crisscross/deprecate.h>

#define MD4_DIGEST_LENGTH       16

/* MD4 context. */
/* @cond */
typedef struct  {
	cc_uint32_t state[4];                                 /* state (ABCD) */
	cc_uint32_t count[2];      /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];                       /* input buffer */
} cc_md4_ctx;
/* @endcond */

namespace CrissCross
{
	namespace Crypto
	{
		/*! \brief An MD4 hash generator. */
		/*!
		 *      MD4 hashes are rarely used anymore due to the advent of MD5. MD5 was
		 *      created because several speculated that MD4 was vulnerable to collisions
		 *      (that is, two different sets of initialization vectors could result in
		 *      the same hash). However, MD5 has proven no more secure than MD4 in the
		 *      long run, and an MD4 hash is generally faster to generate than an MD5
		 *      hash.
		 * \warning Because of popular belief that MD4 is less secure than MD5,
		 *              use at your own risk!
		 * \sa Hash MD2Hash MD5Hash
		 */
		class MD4Hash
		{
			private:
				mutable char *m_hashString;
				unsigned char *m_hash;
				cc_md4_ctx     m_state;

			public:
				/*! \brief The default constructor. */
				MD4Hash();

				/*! \brief The destructor. */
				~MD4Hash();

				/*! \brief Runs an MD4 hash on the data provided. */
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

				/*! \brief Resets the internal MD4 context and hash buffer. */
				void Reset();

				/*! \brief Converts the internal hash data into an hex string, a human readable format. */
				/*!
				 *      The memory location returned by this function is freed when the class
				 *      is destructed.
				 */
				const char *ToString() const;

				/*! \brief Equality operator. */
				/*!
				 *      Compares two instances of MD4Hash to see if the hashes are equal.
				 * \param _other The other instance of MD4Hash to compare to.
				 */
				bool operator==(const MD4Hash &_other) const;

				/*! \brief Inequality operator. */
				/*!
				 *      Compares two instances of MD4Hash to see if the hashes are not equal.
				 * \param _other The other instance of MD4Hash to compare to.
				 */
				inline bool operator!=(const MD4Hash &_other) const
				{
					return !(*this == _other);
				};
		};
	}
}

#endif

#endif
