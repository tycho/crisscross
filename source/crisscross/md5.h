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

#ifndef __included_cc_md5_h
#define __included_cc_md5_h

#ifdef ENABLE_HASHES

#define MD5_DIGEST_LENGTH       16

/* MD5 context. */
/* @cond */
typedef struct  {
	cc_uint32_t state[4];       /* state (ABCD) */
	cc_uint32_t count[2];       /* number of bits, modulo 2^64 (lsb first) */
	union   {
		cc_uint8_t  buf8[64];           /* undigested input */
		cc_uint32_t buf32[16];                  /* realigned input */
	} buf_un;
} cc_md5_ctx;
/* @endcond */

namespace CrissCross
{
	namespace Crypto
	{
		/*! \brief An MD5 hash generator. */
		/*!
		 *      In recent years, MD5 hashes have waned in popularity because researchers
		 *      have found that collisions for MD5 are easy to generate. However, this
		 *      sort of attack can be rendered useless when a salt is added to the
		 *      input dataset.
		 *
		 * \warning Because MD5 is known to have weaknesses, use at your own risk!
		 * \sa Hash MD2Hash MD4Hash
		 */
		class MD5Hash
		{
			private:
				mutable char *m_hashString;
				unsigned char *m_hash;
				cc_md5_ctx     m_state;

			public:
				/*! \brief The default constructor. */
				MD5Hash();

				/*! \brief The destructor. */
				~MD5Hash();

				/*! \brief Runs an MD5 hash on the data provided. */
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

				/*! \brief Resets the internal MD5 context and hash buffer. */
				void Reset();

				/*! \brief Converts the internal hash data into an hex string, a human readable format. */
				/*!
				 *      The memory location returned by this function is freed when the class
				 *      is destructed.
				 */
				const char *ToString() const;

				/*! \brief Equality operator. */
				/*!
				 *      Compares two instances of MD5Hash to see if the hashes are equal.
				 * \param _other The other instance of MD5Hash to compare to.
				 */
				bool operator==(const MD5Hash &_other) const;

				/*! \brief Inequality operator. */
				/*!
				 *      Compares two instances of MD5Hash to see if the hashes are not equal.
				 * \param _other The other instance of MD5Hash to compare to.
				 */
				inline bool operator!=(const MD5Hash &_other) const
				{
					return !(*this == _other);
				};
		};
	}
}

#endif

#endif
