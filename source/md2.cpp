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

#ifdef ENABLE_HASHES
#if !defined(DISABLE_DEPRECATED_CODE)

#include <crisscross/core_io_reader.h>
#include <crisscross/md2.h>


#define MD2_CTX cc_md2_ctx

/* PROTOTYPES should be set to one if and only if the compiler supports
 *   function argument prototyping.
 * The following makes PROTOTYPES default to 0 if it has not already
 *   been defined with C compiler flags.
 */
#ifndef PROTOTYPES
#define PROTOTYPES 1
#endif

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;
typedef const unsigned char *CONST_POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
 * If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
 *   returns an empty list.
 */

#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif

static void MD2Transform    PROTO_LIST
                                                                                                                                              ((unsigned char [16], unsigned char [16], const unsigned char [16]));

/* Permutation of 0..255 constructed from the digits of pi. It gives a
 * "random" nonlinear byte substitution operation.
 */
static unsigned char        PI_SUBST[256] = {
	41, 46, 67, 201, 162, 216, 124, 1, 61, 54, 84, 161, 236, 240, 6,
	19, 98, 167, 5, 243, 192, 199, 115, 140, 152, 147, 43, 217, 188,
	76, 130, 202, 30, 155, 87, 60, 253, 212, 224, 22, 103, 66, 111, 24,
	138, 23, 229, 18, 190, 78, 196, 214, 218, 158, 222, 73, 160, 251,
	245, 142, 187, 47, 238, 122, 169, 104, 121, 145, 21, 178, 7, 63,
	148, 194, 16, 137, 11, 34, 95, 33, 128, 127, 93, 154, 90, 144, 50,
	39, 53, 62, 204, 231, 191, 247, 151, 3, 255, 25, 48, 179, 72, 165,
	181, 209, 215, 94, 146, 42, 172, 86, 170, 198, 79, 184, 56, 210,
	150, 164, 125, 182, 118, 252, 107, 226, 156, 116, 4, 241, 69, 157,
	112, 89, 100, 113, 135, 32, 134, 91, 207, 101, 230, 45, 168, 2, 27,
	96, 37, 173, 174, 176, 185, 246, 28, 70, 97, 105, 52, 64, 126, 15,
	85, 71, 163, 35, 221, 81, 175, 58, 195, 92, 249, 206, 186, 197,
	234, 38, 44, 83, 13, 110, 133, 40, 132, 9, 211, 223, 205, 244, 65,
	129, 77, 82, 106, 220, 55, 200, 108, 193, 171, 250, 36, 225, 123,
	8, 12, 189, 177, 74, 120, 136, 149, 139, 227, 99, 232, 109, 233,
	203, 213, 254, 59, 0, 29, 57, 242, 239, 183, 14, 102, 88, 208, 228,
	166, 119, 114, 248, 235, 117, 75, 10, 49, 68, 80, 180, 143, 237,
	31, 26, 219, 153, 141, 51, 159, 17, 131, 20
};

static const unsigned char *PADDING [] = {
	(const unsigned char *)"",
	(const unsigned char *)"\001",
	(const unsigned char *)"\002\002",
	(const unsigned char *)"\003\003\003",
	(const unsigned char *)"\004\004\004\004",
	(const unsigned char *)"\005\005\005\005\005",
	(const unsigned char *)"\006\006\006\006\006\006",
	(const unsigned char *)"\007\007\007\007\007\007\007",
	(const unsigned char *)"\010\010\010\010\010\010\010\010",
	(const unsigned char *)"\011\011\011\011\011\011\011\011\011",
	(const unsigned char *)"\012\012\012\012\012\012\012\012\012\012",
	(const unsigned char *)"\013\013\013\013\013\013\013\013\013\013\013",
	(const unsigned char *)"\014\014\014\014\014\014\014\014\014\014\014\014",
	(const unsigned char *)
	"\015\015\015\015\015\015\015\015\015\015\015\015\015",
	(const unsigned char *)
	"\016\016\016\016\016\016\016\016\016\016\016\016\016\016",
	(const unsigned char *)
	"\017\017\017\017\017\017\017\017\017\017\017\017\017\017\017",
	(const unsigned char *)
	"\020\020\020\020\020\020\020\020\020\020\020\020\020\020\020\020"
};

/* MD2 initialization. Begins an MD2 operation, writing a new context.
 */
void MD2Init(MD2_CTX *context)
{
	context->count = 0;
	memset((POINTER)context->state, 0, sizeof(context->state));
	memset((POINTER)context->checksum, 0, sizeof(context->checksum));
}

/* MD2 block update operation. Continues an MD2 message-digest
 *   operation, processing another message block, and updating the
 *   context.
 */
static void MD2Update(MD2_CTX *context, const unsigned char *input, unsigned int inputLen)
{
	unsigned int i, index, partLen;

	/* Update number of bytes mod 16 */
	index = context->count;
	context->count = (index + inputLen) & 0xf;

	partLen = 16 - index;

	/* Transform as many times as possible.
	 */
	if (inputLen >= partLen) {
		memcpy((POINTER)&context->buffer[index], (CONST_POINTER)input, partLen);
		MD2Transform(context->state, context->checksum, context->buffer);

		for (i = partLen; i + 15 < inputLen; i += 16)
			MD2Transform(context->state, context->checksum, &input[i]);

		index = 0;
	} else
		i = 0;

	/* Buffer remaining input */
	memcpy((POINTER)&context->buffer[index], (CONST_POINTER)&input[i], inputLen - i);
}

/* MD2 finalization. Ends an MD2 message-digest operation, writing the
 *   message digest and zeroizing the context.
 */
static void MD2Final(unsigned char digest[16], MD2_CTX *context)
{
	unsigned int index, padLen;

	/* Pad out to multiple of 16.
	 */
	index = context->count;
	padLen = 16 - index;
	MD2Update(context, PADDING[padLen], padLen);

	/* Extend with checksum */
	MD2Update(context, context->checksum, 16);

	/* Store state in digest */
	memcpy((POINTER)digest, (POINTER)context->state, 16);

	/* Zeroize sensitive information.
	 */
	memset((POINTER)context, 0, sizeof(*context));
}

/* MD2 basic transformation. Transforms state and updates checksum
 *   based on block.
 */
static void MD2Transform(unsigned char state[16], unsigned char checksum[16], const unsigned char block[16])
{
	unsigned int  i, j, t;
	unsigned char x[48];

	/* Form encryption block from state, block, state ^ block.
	 */
	memcpy((POINTER)x, (CONST_POINTER)state, 16);
	memcpy((POINTER)x + 16, (CONST_POINTER)block, 16);
	for (i = 0; i < 16; i++)
		x[i + 32] = state[i] ^ block[i];

	/* Encrypt block (18 rounds).
	 */
	t = 0;
	for (i = 0; i < 18; i++) {
		for (j = 0; j < 48; j++)
			t = x[j] ^= PI_SUBST[t];

		t = (t + i) & 0xff;
	}

	/* Save new state */
	memcpy((POINTER)state, (POINTER)x, 16);

	/* Update checksum.
	 */
	t = checksum[15];
	for (i = 0; i < 16; i++)
		t = checksum[i] ^= PI_SUBST[block[i] ^ t];

	/* Zeroize sensitive information.
	 */
	memset((POINTER)x, 0, sizeof(x));
}



namespace CrissCross
{
	namespace Crypto
	{
#if !defined(DISABLE_DEPRECATED_CODE)
		MD2Hash::MD2Hash() : m_hashString(NULL), m_hash(NULL)
		{
			Reset();
		}

		MD2Hash::~MD2Hash()
		{
			Reset();
		}

		int MD2Hash::Process(const void * _data, size_t _length)
		{
			Reset();
			if (!_data) return -1;

			MD2Update(&m_state, (unsigned char *)_data, _length);
			m_hash = new unsigned char[MD2_DIGEST_LENGTH];
			MD2Final((unsigned char *)m_hash, &m_state);
			return 0;
		}

		int MD2Hash::Process(CrissCross::IO::CoreIOReader *_reader)
		{
			Reset();
			if (!_reader) return -1;

			cc_int64_t pos = _reader->Position();
			_reader->Seek(0);
			char       buffer[8192]; int bytesRead = 0;
			do
			{
				bytesRead = _reader->Read(buffer, sizeof(buffer), 0, sizeof(buffer));
				if (bytesRead >= 0)
					ProcessBlock(buffer, bytesRead);
			} while (bytesRead == sizeof(buffer) && !_reader->EndOfFile());
			Finalize();
			_reader->Seek(pos);
			return 0;
		}

		int MD2Hash::ProcessBlock(const void * _data, size_t _length)
		{
			if (!_data) return -1;

			MD2Update(&m_state, (unsigned char *)_data, _length);
			return 0;
		}

		void MD2Hash::Finalize()
		{
			if (m_hash) delete [] m_hash;

			m_hash = new unsigned char[MD2_DIGEST_LENGTH];
			MD2Final((unsigned char *)m_hash, &m_state);
		}

		const char *MD2Hash::ToString() const
		{
			if (m_hashString) return m_hashString;

			m_hashString = new char[33];
			for (int i = 0; i < MD2_DIGEST_LENGTH; i++)
				sprintf(m_hashString + (i * 2), "%02x", m_hash[i]);

			return m_hashString;
		}

		void MD2Hash::Reset()
		{
			delete [] m_hash; m_hash = NULL;
			delete [] m_hashString; m_hashString = NULL;

			MD2Init(&m_state);
		}

		bool MD2Hash::operator==(const MD2Hash &_other) const
		{
			return (memcmp(m_hash, _other.m_hash, MD2_DIGEST_LENGTH) == 0);
		}
#endif
	}
}

#endif /* !defined(DISABLE_DEPRECATED_CODE) */
#endif
