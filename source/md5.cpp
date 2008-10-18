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

#include <crisscross/core_io_reader.h>
#include <crisscross/md5.h>

#define MD5_CTX cc_md5_ctx

/*
 * MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm
 */

/*
 * Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 *
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 *
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 *
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 *
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 */


/* constants, as provided in RFC 1321 */

#define MD5_CONST_0             (cc_uint32_t)0xd76aa478
#define MD5_CONST_1             (cc_uint32_t)0xe8c7b756
#define MD5_CONST_2             (cc_uint32_t)0x242070db
#define MD5_CONST_3             (cc_uint32_t)0xc1bdceee
#define MD5_CONST_4             (cc_uint32_t)0xf57c0faf
#define MD5_CONST_5             (cc_uint32_t)0x4787c62a
#define MD5_CONST_6             (cc_uint32_t)0xa8304613
#define MD5_CONST_7             (cc_uint32_t)0xfd469501
#define MD5_CONST_8             (cc_uint32_t)0x698098d8
#define MD5_CONST_9             (cc_uint32_t)0x8b44f7af
#define MD5_CONST_10            (cc_uint32_t)0xffff5bb1
#define MD5_CONST_11            (cc_uint32_t)0x895cd7be
#define MD5_CONST_12            (cc_uint32_t)0x6b901122
#define MD5_CONST_13            (cc_uint32_t)0xfd987193
#define MD5_CONST_14            (cc_uint32_t)0xa679438e
#define MD5_CONST_15            (cc_uint32_t)0x49b40821
#define MD5_CONST_16            (cc_uint32_t)0xf61e2562
#define MD5_CONST_17            (cc_uint32_t)0xc040b340
#define MD5_CONST_18            (cc_uint32_t)0x265e5a51
#define MD5_CONST_19            (cc_uint32_t)0xe9b6c7aa
#define MD5_CONST_20            (cc_uint32_t)0xd62f105d
#define MD5_CONST_21            (cc_uint32_t)0x2441453
#define MD5_CONST_22            (cc_uint32_t)0xd8a1e681
#define MD5_CONST_23            (cc_uint32_t)0xe7d3fbc8
#define MD5_CONST_24            (cc_uint32_t)0x21e1cde6
#define MD5_CONST_25            (cc_uint32_t)0xc33707d6
#define MD5_CONST_26            (cc_uint32_t)0xf4d50d87
#define MD5_CONST_27            (cc_uint32_t)0x455a14ed
#define MD5_CONST_28            (cc_uint32_t)0xa9e3e905
#define MD5_CONST_29            (cc_uint32_t)0xfcefa3f8
#define MD5_CONST_30            (cc_uint32_t)0x676f02d9
#define MD5_CONST_31            (cc_uint32_t)0x8d2a4c8a
#define MD5_CONST_32            (cc_uint32_t)0xfffa3942
#define MD5_CONST_33            (cc_uint32_t)0x8771f681
#define MD5_CONST_34            (cc_uint32_t)0x6d9d6122
#define MD5_CONST_35            (cc_uint32_t)0xfde5380c
#define MD5_CONST_36            (cc_uint32_t)0xa4beea44
#define MD5_CONST_37            (cc_uint32_t)0x4bdecfa9
#define MD5_CONST_38            (cc_uint32_t)0xf6bb4b60
#define MD5_CONST_39            (cc_uint32_t)0xbebfbc70
#define MD5_CONST_40            (cc_uint32_t)0x289b7ec6
#define MD5_CONST_41            (cc_uint32_t)0xeaa127fa
#define MD5_CONST_42            (cc_uint32_t)0xd4ef3085
#define MD5_CONST_43            (cc_uint32_t)0x4881d05
#define MD5_CONST_44            (cc_uint32_t)0xd9d4d039
#define MD5_CONST_45            (cc_uint32_t)0xe6db99e5
#define MD5_CONST_46            (cc_uint32_t)0x1fa27cf8
#define MD5_CONST_47            (cc_uint32_t)0xc4ac5665
#define MD5_CONST_48            (cc_uint32_t)0xf4292244
#define MD5_CONST_49            (cc_uint32_t)0x432aff97
#define MD5_CONST_50            (cc_uint32_t)0xab9423a7
#define MD5_CONST_51            (cc_uint32_t)0xfc93a039
#define MD5_CONST_52            (cc_uint32_t)0x655b59c3
#define MD5_CONST_53            (cc_uint32_t)0x8f0ccc92
#define MD5_CONST_54            (cc_uint32_t)0xffeff47d
#define MD5_CONST_55            (cc_uint32_t)0x85845dd1
#define MD5_CONST_56            (cc_uint32_t)0x6fa87e4f
#define MD5_CONST_57            (cc_uint32_t)0xfe2ce6e0
#define MD5_CONST_58            (cc_uint32_t)0xa3014314
#define MD5_CONST_59            (cc_uint32_t)0x4e0811a1
#define MD5_CONST_60            (cc_uint32_t)0xf7537e82
#define MD5_CONST_61            (cc_uint32_t)0xbd3af235
#define MD5_CONST_62            (cc_uint32_t)0x2ad7d2bb
#define MD5_CONST_63            (cc_uint32_t)0xeb86d391

/* initialization constants, as given in RFC 1321. used in MD5Init */

#define MD5_INIT_CONST_1        (cc_uint32_t)0x67452301
#define MD5_INIT_CONST_2        (cc_uint32_t)0xefcdab89
#define MD5_INIT_CONST_3        (cc_uint32_t)0x98badcfe
#define MD5_INIT_CONST_4        (cc_uint32_t)0x10325476

/* shift constants, as given in RFC 1321.  used in MD5Transform */

#define MD5_SHIFT_11            7
#define MD5_SHIFT_12            12
#define MD5_SHIFT_13            17
#define MD5_SHIFT_14            22
#define MD5_SHIFT_21            5
#define MD5_SHIFT_22            9
#define MD5_SHIFT_23            14
#define MD5_SHIFT_24            20
#define MD5_SHIFT_31            4
#define MD5_SHIFT_32            11
#define MD5_SHIFT_33            16
#define MD5_SHIFT_34            23
#define MD5_SHIFT_41            6
#define MD5_SHIFT_42            10
#define MD5_SHIFT_43            15
#define MD5_SHIFT_44            21

static cc_uint8_t PADDING[64] = { 0x80, /* all zeros */ };

/*
 * F, G, H and I are the basic MD5 functions.
 */
#define F(b, c, d)      (((b) & (c)) | ((~b) & (d)))
#define G(b, c, d)      (((b) & (d)) | ((c) & (~d)))
#define H(b, c, d)      ((b) ^ (c) ^ (d))
#define I(b, c, d)      ((c) ^ ((b) | (~d)))

/*
 * ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n)       \
        (((x) << (n)) | ((x) >> ((sizeof(x) << 3) - (n))))

/*
 * FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
 * Rotation is separate from addition to prevent recomputation.
 */

#define FF(a, b, c, d, x, s, ac) { \
		(a) += F((b), (c), (d)) + (x) + ((cc_uint64_t)(ac)); \
		(a) = ROTATE_LEFT((a), (s)); \
		(a) += (b); \
}

#define GG(a, b, c, d, x, s, ac) { \
		(a) += G((b), (c), (d)) + (x) + ((cc_uint64_t)(ac)); \
		(a) = ROTATE_LEFT((a), (s)); \
		(a) += (b); \
}

#define HH(a, b, c, d, x, s, ac) { \
		(a) += H((b), (c), (d)) + (x) + ((cc_uint64_t)(ac)); \
		(a) = ROTATE_LEFT((a), (s)); \
		(a) += (b); \
}

#define II(a, b, c, d, x, s, ac) { \
		(a) += I((b), (c), (d)) + (x) + ((cc_uint64_t)(ac)); \
		(a) = ROTATE_LEFT((a), (s)); \
		(a) += (b); \
}

/*
 * Loading 32-bit constants on a RISC is expensive since it involves both a
 * `sethi' and an `or'.  thus, we instead have the compiler generate `ld's to
 * load the constants from an array called `md5_consts'.  however, on intel
 * (and other CISC processors), it is cheaper to load the constant
 * directly.  thus, the c code in MD5Transform() uses the macro MD5_CONST()
 * which either expands to a constant or an array reference, depending on the
 * architecture the code is being compiled for.
 *
 * Right now, i386 and amd64 are the CISC exceptions.
 * If we get another CISC ISA, we'll have to change the ifdef.
 */

#if defined (TARGET_CPU_X86) || defined (TARGET_CPU_X64)

#define MD5_CONST(x)            (MD5_CONST_ ## x)
#define MD5_CONST_e(x)          MD5_CONST(x)
#define MD5_CONST_o(x)          MD5_CONST(x)

#else
/*
 * sparc/RISC optimization:
 *
 * while it is somewhat counter-intuitive, on sparc (and presumably other RISC
 * machines), it is more efficient to place all the constants used in this
 * function in an array and load the values out of the array than to manually
 * load the constants.  this is because setting a register to a 32-bit value
 * takes two ops in most cases: a `sethi' and an `or', but loading a 32-bit
 * value from memory only takes one `ld' (or `lduw' on v9).  while this
 * increases memory usage, the compiler can find enough other things to do
 * while waiting to keep the pipeline does not stall.  additionally, it is
 * likely that many of these constants are cached so that later accesses do
 * not even go out to the bus.
 *
 * this array is declared `static' to keep the compiler from having to
 * bcopy() this array onto the stack frame of MD5Transform() each time it is
 * called -- which is unacceptably expensive.
 *
 * the `const' is to ensure that callers are good citizens and do not try to
 * munge the array.  since these routines are going to be called from inside
 * multithreaded kernelland, this is a good safety check. -- `constants' will
 * end up in .rodata.
 *
 * unfortunately, loading from an array in this manner hurts performance under
 * intel (and presumably other CISC machines).  so, there is a macro,
 * MD5_CONST(), used in MD5Transform(), that either expands to a reference to
 * this array, or to the actual constant, depending on what platform this code
 * is compiled for.
 */

#ifdef sun4v

/*
 * Going to load these consts in 8B chunks, so need to enforce 8B alignment
 */

/* CSTYLED */
#pragma align 64 (md5_consts)
#define _MD5_CHECK_ALIGNMENT

#endif     /* sun4v */

static const cc_uint32_t md5_consts [] = {
	MD5_CONST_0, MD5_CONST_1, MD5_CONST_2, MD5_CONST_3,
	MD5_CONST_4, MD5_CONST_5, MD5_CONST_6, MD5_CONST_7,
	MD5_CONST_8, MD5_CONST_9, MD5_CONST_10, MD5_CONST_11,
	MD5_CONST_12, MD5_CONST_13, MD5_CONST_14, MD5_CONST_15,
	MD5_CONST_16, MD5_CONST_17, MD5_CONST_18, MD5_CONST_19,
	MD5_CONST_20, MD5_CONST_21, MD5_CONST_22, MD5_CONST_23,
	MD5_CONST_24, MD5_CONST_25, MD5_CONST_26, MD5_CONST_27,
	MD5_CONST_28, MD5_CONST_29, MD5_CONST_30, MD5_CONST_31,
	MD5_CONST_32, MD5_CONST_33, MD5_CONST_34, MD5_CONST_35,
	MD5_CONST_36, MD5_CONST_37, MD5_CONST_38, MD5_CONST_39,
	MD5_CONST_40, MD5_CONST_41, MD5_CONST_42, MD5_CONST_43,
	MD5_CONST_44, MD5_CONST_45, MD5_CONST_46, MD5_CONST_47,
	MD5_CONST_48, MD5_CONST_49, MD5_CONST_50, MD5_CONST_51,
	MD5_CONST_52, MD5_CONST_53, MD5_CONST_54, MD5_CONST_55,
	MD5_CONST_56, MD5_CONST_57, MD5_CONST_58, MD5_CONST_59,
	MD5_CONST_60, MD5_CONST_61, MD5_CONST_62, MD5_CONST_63
};


#ifdef sun4v
/*
 * To reduce the number of loads, load consts in 64-bit
 * chunks and then split.
 *
 * No need to mask upper 32-bits, as just interested in
 * low 32-bits (saves an & operation and means that this
 * optimization doesn't increases the icount.
 */
#define MD5_CONST_e(x)          (md5_consts64[x / 2] >> 32)
#define MD5_CONST_o(x)          (md5_consts64[x / 2])

#else

#define MD5_CONST_e(x)          (md5_consts[x])
#define MD5_CONST_o(x)          (md5_consts[x])

#endif     /* sun4v */

#endif

#if defined (TARGET_LITTLE_ENDIAN)

/*
 * Little-endian optimization:  I don't need to do any weirdness.   On
 * some little-endian boxen, I'll have to do alignment checks, but I can do
 * that below.
 */

#if !defined (TARGET_CPU_X86) && !defined (TARGET_CPU_X64)
/*
 * i386 and amd64 don't require aligned 4-byte loads.  The symbol
 * _MD5_CHECK_ALIGNMENT indicates below whether the MD5Transform function
 * requires alignment checking.
 */
#define _MD5_CHECK_ALIGNMENT
#endif     /* !__i386 && !__amd64 */

#define LOAD_LITTLE_32(addr)    (*(cc_uint32_t *)(addr))

#else   /* !TARGET_LITTLE_ENDIAN */

/*
 * sparc v9/v8plus optimization:
 *
 * on the sparc v9/v8plus, we can load data little endian.  however, since
 * the compiler doesn't have direct support for little endian, we
 * link to an assembly-language routine `load_little_32' to do
 * the magic.  note that special care must be taken to ensure the
 * address is 32-bit aligned -- in the interest of speed, we don't
 * check to make sure, since careful programming can guarantee this
 * for us.
 */
#if defined (sun4u)

/* Define alignment check because we can 4-byte load as little endian. */
#define _MD5_CHECK_ALIGNMENT
#define LOAD_LITTLE_32(addr)    load_little_32((cc_uint32_t *)(addr))

#if !defined (__lint) && defined (__GNUC__)

static __inline__ cc_uint32_t load_little_32(cc_uint32_t *addr)
{
	cc_uint32_t value;

	__asm__(
	        "lduwa    [%1] %2, %0\n\t"
		: "=r" (value)
		: "r" (addr), "i" (ASI_PL));

	return (value);
}

static __inline__ uint16_t load_little_16(uint16_t *addr)
{
	uint16_t value;

	__asm__(
	        "lduha    [%1] %2, %0\n\t"
		: "=r" (value)
		: "r" (addr), "i" (ASI_PL));

	return (value);
}

#endif       /* !__lint && __GNUC__ */

#if !defined (__GNUC__)
extern cc_uint32_t load_little_32(cc_uint32_t *);
#endif       /* !__GNUC__ */

/* Placate lint */
#if defined (__lint)
cc_uint32_t load_little_32(cc_uint32_t *addr)
{
	return (*addr);
}
#endif       /* __lint */

#else     /* !sun4u */

/* big endian -- will work on little endian, but slowly */
/* Since we do byte operations, we don't have to check for alignment. */
#define LOAD_LITTLE_32(addr)    \
        ((addr)[0] | ((addr)[1] << 8) | ((addr)[2] << 16) | ((addr)[3] << 24))

#endif     /* sun4u */

#if defined (sun4v)

/*
 * For N1 want to minimize number of arithmetic operations. This is best
 * achieved by using the %asi register to specify ASI for the lduwa operations.
 * Also, have a separate inline template for each word, so can utilize the
 * immediate offset in lduwa, without relying on the compiler to do the right
 * thing.
 *
 * Moving to 64-bit loads might also be beneficial.
 */
#define LOAD_LITTLE_32_0(addr)  load_little_32_0((cc_uint32_t *)(addr))
#define LOAD_LITTLE_32_1(addr)  load_little_32_1((cc_uint32_t *)(addr))
#define LOAD_LITTLE_32_2(addr)  load_little_32_2((cc_uint32_t *)(addr))
#define LOAD_LITTLE_32_3(addr)  load_little_32_3((cc_uint32_t *)(addr))
#define LOAD_LITTLE_32_4(addr)  load_little_32_4((cc_uint32_t *)(addr))
#define LOAD_LITTLE_32_5(addr)  load_little_32_5((cc_uint32_t *)(addr))
#define LOAD_LITTLE_32_6(addr)  load_little_32_6((cc_uint32_t *)(addr))
#define LOAD_LITTLE_32_7(addr)  load_little_32_7((cc_uint32_t *)(addr))
#define LOAD_LITTLE_32_8(addr)  load_little_32_8((cc_uint32_t *)(addr))
#define LOAD_LITTLE_32_9(addr)  load_little_32_9((cc_uint32_t *)(addr))
#define LOAD_LITTLE_32_a(addr)  load_little_32_a((cc_uint32_t *)(addr))
#define LOAD_LITTLE_32_b(addr)  load_little_32_b((cc_uint32_t *)(addr))
#define LOAD_LITTLE_32_c(addr)  load_little_32_c((cc_uint32_t *)(addr))
#define LOAD_LITTLE_32_d(addr)  load_little_32_d((cc_uint32_t *)(addr))
#define LOAD_LITTLE_32_e(addr)  load_little_32_e((cc_uint32_t *)(addr))
#define LOAD_LITTLE_32_f(addr)  load_little_32_f((cc_uint32_t *)(addr))

#if !defined (__lint) && defined (__GNUC__)

/*
 * This actually sets the ASI register, not necessarily to ASI_PL.
 */
static __inline__ void set_little(uint8_t asi)
{
	__asm__ __volatile__ (
	        "wr    %%g0, %0, %%asi\n\t"
		: /* Nothing */
		: "r" (asi));
}

static __inline__ uint8_t get_little(void)
{
	uint8_t asi;

	__asm__ __volatile__ (
	        "rd    %%asi, %0\n\t"
		: "=r" (asi));

	return (asi);
}

/*
 * We have 16 functions which differ only in the offset from which they
 * load.  Use this preprocessor template to simplify maintenance.  Its
 * argument is the offset in hex, without the 0x.
 */
#define LL_TEMPLATE(__off)                      \
        static __inline__ cc_uint32_t \
        load_little_32_ ## __off(cc_uint32_t * addr)               \
	{                                               \
		cc_uint32_t value;                              \
		__asm__( \
		        "lduwa    [%1 + %2]%%asi, %0\n\t" \
			: "=r" (value)                            \
			: "r" (addr), "i" ((0x ## __off) << 2));      \
		return (value);                         \
	}

LL_TEMPLATE(0)
LL_TEMPLATE(1)
LL_TEMPLATE(2)
LL_TEMPLATE(3)
LL_TEMPLATE(4)
LL_TEMPLATE(5)
LL_TEMPLATE(6)
LL_TEMPLATE(7)
LL_TEMPLATE(8)
LL_TEMPLATE(9)
LL_TEMPLATE(a)
LL_TEMPLATE(b)
LL_TEMPLATE(c)
LL_TEMPLATE(d)
LL_TEMPLATE(e)
LL_TEMPLATE(f)
#undef  LL_TEMPLATE

#endif       /* !__lint && __GNUC__ */

#if !defined (__GNUC__)
/*
 * Using the %asi register to achieve little endian loads - register
 * is set using a inline template.
 *
 * Saves a few arithmetic ops as can now use an immediate offset with the
 * lduwa instructions.
 */
extern void set_little(cc_uint32_t);
extern cc_uint32_t get_little(void);

extern cc_uint32_t load_little_32_0(cc_uint32_t *);
extern cc_uint32_t load_little_32_1(cc_uint32_t *);
extern cc_uint32_t load_little_32_2(cc_uint32_t *);
extern cc_uint32_t load_little_32_3(cc_uint32_t *);
extern cc_uint32_t load_little_32_4(cc_uint32_t *);
extern cc_uint32_t load_little_32_5(cc_uint32_t *);
extern cc_uint32_t load_little_32_6(cc_uint32_t *);
extern cc_uint32_t load_little_32_7(cc_uint32_t *);
extern cc_uint32_t load_little_32_8(cc_uint32_t *);
extern cc_uint32_t load_little_32_9(cc_uint32_t *);
extern cc_uint32_t load_little_32_a(cc_uint32_t *);
extern cc_uint32_t load_little_32_b(cc_uint32_t *);
extern cc_uint32_t load_little_32_c(cc_uint32_t *);
extern cc_uint32_t load_little_32_d(cc_uint32_t *);
extern cc_uint32_t load_little_32_e(cc_uint32_t *);
extern cc_uint32_t load_little_32_f(cc_uint32_t *);
#endif       /* !__GNUC__ */
#endif     /* sun4v */

#endif   /* TARGET_LITTLE_ENDIAN */

static void MD5Transform(cc_uint32_t a, cc_uint32_t b, cc_uint32_t c, cc_uint32_t d, MD5_CTX *ctx, const cc_uint8_t block[64]);
static void Encode(cc_uint8_t * output, const cc_uint32_t * input, size_t input_len);

/*
 * MD5Init()
 *
 * purpose: initializes the md5 context and begins and md5 digest operation
 *   input: MD5_CTX *    : the context to initialize.
 *  output: void
 */

static void MD5Init(MD5_CTX *ctx)
{
	ctx->count[0] = ctx->count[1] = 0;

	/* load magic initialization constants */
	ctx->state[0] = MD5_INIT_CONST_1;
	ctx->state[1] = MD5_INIT_CONST_2;
	ctx->state[2] = MD5_INIT_CONST_3;
	ctx->state[3] = MD5_INIT_CONST_4;
}

/*
 * MD5Update()
 *
 * purpose: continues an md5 digest operation, using the message block
 *          to update the context.
 *   input: MD5_CTX *    : the context to update
 *          cc_uint8_t *    : the message block
 *          cc_uint32_t    : the length of the message block in bytes
 *  output: void
 *
 * MD5 crunches in 64-byte blocks.  All numeric constants here are related to
 * that property of MD5.
 */

static void MD5Update(MD5_CTX *ctx, const void *inpp, unsigned int input_len)
{
	cc_uint32_t          i, buf_index, buf_len;
	const unsigned char *input = (const unsigned char *)inpp;

	/* compute (number of bytes computed so far) mod 64 */
	buf_index = (ctx->count[0] >> 3) & 0x3F;

	/* update number of bits hashed into this MD5 computation so far */
	if ((ctx->count[0] += (input_len << 3)) < (input_len << 3))
		ctx->count[1]++;

	ctx->count[1] += (input_len >> 29);

	buf_len = 64 - buf_index;

	/* transform as many times as possible */
	i = 0;
	if (input_len >= buf_len) {
		/*
		 * general optimization:
		 *
		 * only do initial memcpy() and MD5Transform() if
		 * buf_index != 0.  if buf_index == 0, we're just
		 * wasting our time doing the bcopy() since there
		 * wasn't any data left over from a previous call to
		 * MD5Update().
		 */

		if (buf_index) {
			memcpy(&ctx->buf_un.buf8[buf_index], input, buf_len);

			MD5Transform(ctx->state[0], ctx->state[1],
			             ctx->state[2], ctx->state[3], ctx,
			             ctx->buf_un.buf8);

			i = buf_len;
		}

		for (; i + 63 < input_len; i += 64)
			MD5Transform(ctx->state[0], ctx->state[1],
			             ctx->state[2], ctx->state[3], ctx, &input[i]);

		/*
		 * general optimization:
		 *
		 * if i and input_len are the same, return now instead
		 * of calling bcopy(), since the bcopy() in this
		 * case will be an expensive nop.
		 */

		if (input_len == i)
			return;

		buf_index = 0;
	}

	/* buffer remaining input */
	memcpy(&ctx->buf_un.buf8[buf_index], &input[i], input_len - i);
}

/*
 * MD5Final()
 *
 * purpose: ends an md5 digest operation, finalizing the message digest and
 *          zeroing the context.
 *   input: cc_uint8_t *    : a buffer to store the digest in
 *          MD5_CTX *   : the context to finalize, save, and zero
 *  output: void
 */

static void MD5Final(unsigned char *digest, MD5_CTX *ctx)
{
	cc_uint8_t  bitcount_le[sizeof(ctx->count)];
	cc_uint32_t index = (ctx->count[0] >> 3) & 0x3f;

	/* store bit count, little endian */
	Encode(bitcount_le, ctx->count, sizeof(bitcount_le));

	/* pad out to 56 mod 64 */
	MD5Update(ctx, PADDING, ((index < 56) ? 56 : 120) - index);

	/* append length (before padding) */
	MD5Update(ctx, bitcount_le, sizeof(bitcount_le));

	/* store state in digest */
	Encode(digest, ctx->state, sizeof(ctx->state));

	/* zeroize sensitive information */
	memset(ctx, 0, sizeof(*ctx));
}

/*
 * MD5Transform()
 *
 * purpose: md5 transformation -- updates the digest based on `block'
 *   input: cc_uint32_t    : bytes  1 -  4 of the digest
 *          cc_uint32_t    : bytes  5 -  8 of the digest
 *          cc_uint32_t    : bytes  9 - 12 of the digest
 *          cc_uint32_t    : bytes 12 - 16 of the digest
 *          MD5_CTX *   : the context to update
 *          cc_uint8_t [64]: the block to use to update the digest
 *  output: void
 */

static void MD5Transform(cc_uint32_t a, cc_uint32_t b, cc_uint32_t c, cc_uint32_t d, MD5_CTX *ctx, const cc_uint8_t block[64])
{
	/*
	 * general optimization:
	 *
	 * use individual integers instead of using an array.  this is a
	 * win, although the amount it wins by seems to vary quite a bit.
	 */

	register cc_uint32_t x_0, x_1, x_2, x_3, x_4, x_5, x_6, x_7;
	register cc_uint32_t x_8, x_9, x_10, x_11, x_12, x_13, x_14, x_15;
#ifdef sun4v
	cc_uint64_t         *md5_consts64;

	/* LINTED E_BAD_PTR_CAST_ALIGN */
	md5_consts64 = (cc_uint64_t *)md5_consts;
#endif   /* sun4v */

	/*
	 * general optimization:
	 *
	 * the compiler (at least SC4.2/5.x) generates better code if
	 * variable use is localized.  in this case, swapping the integers in
	 * this order allows `x_0 'to be swapped nearest to its first use in
	 * FF(), and likewise for `x_1' and up.  note that the compiler
	 * prefers this to doing each swap right before the FF() that
	 * uses it.
	 */

	/*
	 * sparc v9/v8plus optimization:
	 *
	 * if `block' is already aligned on a 4-byte boundary, use the
	 * optimized load_little_32() directly.  otherwise, bcopy()
	 * into a buffer that *is* aligned on a 4-byte boundary and
	 * then do the load_little_32() on that buffer.  benchmarks
	 * have shown that using the bcopy() is better than loading
	 * the bytes individually and doing the endian-swap by hand.
	 *
	 * even though it's quite tempting to assign to do:
	 *
	 * blk = bcopy(blk, ctx->buf_un.buf32, sizeof (ctx->buf_un.buf32));
	 *
	 * and only have one set of LOAD_LITTLE_32()'s, the compiler (at least
	 * SC4.2/5.x) *does not* like that, so please resist the urge.
	 */

#ifdef _MD5_CHECK_ALIGNMENT
	if ((uintptr_t)block & 0x3) {                /* not 4-byte aligned? */
		bcopy(block, ctx->buf_un.buf32, sizeof(ctx->buf_un.buf32));

#ifdef sun4v
		x_15 = LOAD_LITTLE_32_f(ctx->buf_un.buf32);
		x_14 = LOAD_LITTLE_32_e(ctx->buf_un.buf32);
		x_13 = LOAD_LITTLE_32_d(ctx->buf_un.buf32);
		x_12 = LOAD_LITTLE_32_c(ctx->buf_un.buf32);
		x_11 = LOAD_LITTLE_32_b(ctx->buf_un.buf32);
		x_10 = LOAD_LITTLE_32_a(ctx->buf_un.buf32);
		x_9 = LOAD_LITTLE_32_9(ctx->buf_un.buf32);
		x_8 = LOAD_LITTLE_32_8(ctx->buf_un.buf32);
		x_7 = LOAD_LITTLE_32_7(ctx->buf_un.buf32);
		x_6 = LOAD_LITTLE_32_6(ctx->buf_un.buf32);
		x_5 = LOAD_LITTLE_32_5(ctx->buf_un.buf32);
		x_4 = LOAD_LITTLE_32_4(ctx->buf_un.buf32);
		x_3 = LOAD_LITTLE_32_3(ctx->buf_un.buf32);
		x_2 = LOAD_LITTLE_32_2(ctx->buf_un.buf32);
		x_1 = LOAD_LITTLE_32_1(ctx->buf_un.buf32);
		x_0 = LOAD_LITTLE_32_0(ctx->buf_un.buf32);
#else
		x_15 = LOAD_LITTLE_32(ctx->buf_un.buf32 + 15);
		x_14 = LOAD_LITTLE_32(ctx->buf_un.buf32 + 14);
		x_13 = LOAD_LITTLE_32(ctx->buf_un.buf32 + 13);
		x_12 = LOAD_LITTLE_32(ctx->buf_un.buf32 + 12);
		x_11 = LOAD_LITTLE_32(ctx->buf_un.buf32 + 11);
		x_10 = LOAD_LITTLE_32(ctx->buf_un.buf32 + 10);
		x_9 = LOAD_LITTLE_32(ctx->buf_un.buf32 + 9);
		x_8 = LOAD_LITTLE_32(ctx->buf_un.buf32 + 8);
		x_7 = LOAD_LITTLE_32(ctx->buf_un.buf32 + 7);
		x_6 = LOAD_LITTLE_32(ctx->buf_un.buf32 + 6);
		x_5 = LOAD_LITTLE_32(ctx->buf_un.buf32 + 5);
		x_4 = LOAD_LITTLE_32(ctx->buf_un.buf32 + 4);
		x_3 = LOAD_LITTLE_32(ctx->buf_un.buf32 + 3);
		x_2 = LOAD_LITTLE_32(ctx->buf_un.buf32 + 2);
		x_1 = LOAD_LITTLE_32(ctx->buf_un.buf32 + 1);
		x_0 = LOAD_LITTLE_32(ctx->buf_un.buf32 + 0);
#endif     /* sun4v */
	} else
#endif
	{
#ifdef sun4v
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_15 = LOAD_LITTLE_32_f(block);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_14 = LOAD_LITTLE_32_e(block);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_13 = LOAD_LITTLE_32_d(block);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_12 = LOAD_LITTLE_32_c(block);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_11 = LOAD_LITTLE_32_b(block);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_10 = LOAD_LITTLE_32_a(block);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_9 = LOAD_LITTLE_32_9(block);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_8 = LOAD_LITTLE_32_8(block);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_7 = LOAD_LITTLE_32_7(block);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_6 = LOAD_LITTLE_32_6(block);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_5 = LOAD_LITTLE_32_5(block);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_4 = LOAD_LITTLE_32_4(block);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_3 = LOAD_LITTLE_32_3(block);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_2 = LOAD_LITTLE_32_2(block);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_1 = LOAD_LITTLE_32_1(block);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_0 = LOAD_LITTLE_32_0(block);
#else
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_15 = LOAD_LITTLE_32(block + 60);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_14 = LOAD_LITTLE_32(block + 56);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_13 = LOAD_LITTLE_32(block + 52);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_12 = LOAD_LITTLE_32(block + 48);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_11 = LOAD_LITTLE_32(block + 44);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_10 = LOAD_LITTLE_32(block + 40);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_9 = LOAD_LITTLE_32(block + 36);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_8 = LOAD_LITTLE_32(block + 32);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_7 = LOAD_LITTLE_32(block + 28);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_6 = LOAD_LITTLE_32(block + 24);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_5 = LOAD_LITTLE_32(block + 20);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_4 = LOAD_LITTLE_32(block + 16);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_3 = LOAD_LITTLE_32(block + 12);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_2 = LOAD_LITTLE_32(block + 8);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_1 = LOAD_LITTLE_32(block + 4);
		/* LINTED E_BAD_PTR_CAST_ALIGN */
		x_0 = LOAD_LITTLE_32(block + 0);
#endif   /* sun4v */
	}

	/* round 1 */
	FF(a, b, c, d, x_0, MD5_SHIFT_11, MD5_CONST_e(0));          /* 1 */
	FF(d, a, b, c, x_1, MD5_SHIFT_12, MD5_CONST_o(1));          /* 2 */
	FF(c, d, a, b, x_2, MD5_SHIFT_13, MD5_CONST_e(2));          /* 3 */
	FF(b, c, d, a, x_3, MD5_SHIFT_14, MD5_CONST_o(3));          /* 4 */
	FF(a, b, c, d, x_4, MD5_SHIFT_11, MD5_CONST_e(4));          /* 5 */
	FF(d, a, b, c, x_5, MD5_SHIFT_12, MD5_CONST_o(5));          /* 6 */
	FF(c, d, a, b, x_6, MD5_SHIFT_13, MD5_CONST_e(6));          /* 7 */
	FF(b, c, d, a, x_7, MD5_SHIFT_14, MD5_CONST_o(7));          /* 8 */
	FF(a, b, c, d, x_8, MD5_SHIFT_11, MD5_CONST_e(8));          /* 9 */
	FF(d, a, b, c, x_9, MD5_SHIFT_12, MD5_CONST_o(9));          /* 10 */
	FF(c, d, a, b, x_10, MD5_SHIFT_13, MD5_CONST_e(10));        /* 11 */
	FF(b, c, d, a, x_11, MD5_SHIFT_14, MD5_CONST_o(11));        /* 12 */
	FF(a, b, c, d, x_12, MD5_SHIFT_11, MD5_CONST_e(12));        /* 13 */
	FF(d, a, b, c, x_13, MD5_SHIFT_12, MD5_CONST_o(13));        /* 14 */
	FF(c, d, a, b, x_14, MD5_SHIFT_13, MD5_CONST_e(14));        /* 15 */
	FF(b, c, d, a, x_15, MD5_SHIFT_14, MD5_CONST_o(15));        /* 16 */

	/* round 2 */
	GG(a, b, c, d, x_1, MD5_SHIFT_21, MD5_CONST_e(16));         /* 17 */
	GG(d, a, b, c, x_6, MD5_SHIFT_22, MD5_CONST_o(17));         /* 18 */
	GG(c, d, a, b, x_11, MD5_SHIFT_23, MD5_CONST_e(18));        /* 19 */
	GG(b, c, d, a, x_0, MD5_SHIFT_24, MD5_CONST_o(19));         /* 20 */
	GG(a, b, c, d, x_5, MD5_SHIFT_21, MD5_CONST_e(20));         /* 21 */
	GG(d, a, b, c, x_10, MD5_SHIFT_22, MD5_CONST_o(21));        /* 22 */
	GG(c, d, a, b, x_15, MD5_SHIFT_23, MD5_CONST_e(22));        /* 23 */
	GG(b, c, d, a, x_4, MD5_SHIFT_24, MD5_CONST_o(23));         /* 24 */
	GG(a, b, c, d, x_9, MD5_SHIFT_21, MD5_CONST_e(24));         /* 25 */
	GG(d, a, b, c, x_14, MD5_SHIFT_22, MD5_CONST_o(25));        /* 26 */
	GG(c, d, a, b, x_3, MD5_SHIFT_23, MD5_CONST_e(26));         /* 27 */
	GG(b, c, d, a, x_8, MD5_SHIFT_24, MD5_CONST_o(27));         /* 28 */
	GG(a, b, c, d, x_13, MD5_SHIFT_21, MD5_CONST_e(28));        /* 29 */
	GG(d, a, b, c, x_2, MD5_SHIFT_22, MD5_CONST_o(29));         /* 30 */
	GG(c, d, a, b, x_7, MD5_SHIFT_23, MD5_CONST_e(30));         /* 31 */
	GG(b, c, d, a, x_12, MD5_SHIFT_24, MD5_CONST_o(31));        /* 32 */

	/* round 3 */
	HH(a, b, c, d, x_5, MD5_SHIFT_31, MD5_CONST_e(32));         /* 33 */
	HH(d, a, b, c, x_8, MD5_SHIFT_32, MD5_CONST_o(33));         /* 34 */
	HH(c, d, a, b, x_11, MD5_SHIFT_33, MD5_CONST_e(34));        /* 35 */
	HH(b, c, d, a, x_14, MD5_SHIFT_34, MD5_CONST_o(35));        /* 36 */
	HH(a, b, c, d, x_1, MD5_SHIFT_31, MD5_CONST_e(36));         /* 37 */
	HH(d, a, b, c, x_4, MD5_SHIFT_32, MD5_CONST_o(37));         /* 38 */
	HH(c, d, a, b, x_7, MD5_SHIFT_33, MD5_CONST_e(38));         /* 39 */
	HH(b, c, d, a, x_10, MD5_SHIFT_34, MD5_CONST_o(39));        /* 40 */
	HH(a, b, c, d, x_13, MD5_SHIFT_31, MD5_CONST_e(40));        /* 41 */
	HH(d, a, b, c, x_0, MD5_SHIFT_32, MD5_CONST_o(41));         /* 42 */
	HH(c, d, a, b, x_3, MD5_SHIFT_33, MD5_CONST_e(42));         /* 43 */
	HH(b, c, d, a, x_6, MD5_SHIFT_34, MD5_CONST_o(43));         /* 44 */
	HH(a, b, c, d, x_9, MD5_SHIFT_31, MD5_CONST_e(44));         /* 45 */
	HH(d, a, b, c, x_12, MD5_SHIFT_32, MD5_CONST_o(45));        /* 46 */
	HH(c, d, a, b, x_15, MD5_SHIFT_33, MD5_CONST_e(46));        /* 47 */
	HH(b, c, d, a, x_2, MD5_SHIFT_34, MD5_CONST_o(47));         /* 48 */

	/* round 4 */
	II(a, b, c, d, x_0, MD5_SHIFT_41, MD5_CONST_e(48));         /* 49 */
	II(d, a, b, c, x_7, MD5_SHIFT_42, MD5_CONST_o(49));         /* 50 */
	II(c, d, a, b, x_14, MD5_SHIFT_43, MD5_CONST_e(50));        /* 51 */
	II(b, c, d, a, x_5, MD5_SHIFT_44, MD5_CONST_o(51));         /* 52 */
	II(a, b, c, d, x_12, MD5_SHIFT_41, MD5_CONST_e(52));        /* 53 */
	II(d, a, b, c, x_3, MD5_SHIFT_42, MD5_CONST_o(53));         /* 54 */
	II(c, d, a, b, x_10, MD5_SHIFT_43, MD5_CONST_e(54));        /* 55 */
	II(b, c, d, a, x_1, MD5_SHIFT_44, MD5_CONST_o(55));         /* 56 */
	II(a, b, c, d, x_8, MD5_SHIFT_41, MD5_CONST_e(56));         /* 57 */
	II(d, a, b, c, x_15, MD5_SHIFT_42, MD5_CONST_o(57));        /* 58 */
	II(c, d, a, b, x_6, MD5_SHIFT_43, MD5_CONST_e(58));         /* 59 */
	II(b, c, d, a, x_13, MD5_SHIFT_44, MD5_CONST_o(59));        /* 60 */
	II(a, b, c, d, x_4, MD5_SHIFT_41, MD5_CONST_e(60));         /* 61 */
	II(d, a, b, c, x_11, MD5_SHIFT_42, MD5_CONST_o(61));        /* 62 */
	II(c, d, a, b, x_2, MD5_SHIFT_43, MD5_CONST_e(62));         /* 63 */
	II(b, c, d, a, x_9, MD5_SHIFT_44, MD5_CONST_o(63));         /* 64 */

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;

	/*
	 * zeroize sensitive information -- compiler will optimize
	 * this out if everything is kept in registers
	 */

	x_0 = x_1 = x_2 = x_3 = x_4 = x_5 = x_6 = x_7 = x_8 = 0;
	x_9 = x_10 = x_11 = x_12 = x_13 = x_14 = x_15 = 0;
}

/*
 * Encode()
 *
 * purpose: to convert a list of numbers from big endian to little endian
 *   input: cc_uint8_t *    : place to store the converted little endian numbers
 *        cc_uint32_t *    : place to get numbers to convert from
 *          size_t    : the length of the input in bytes
 *  output: void
 */

static void Encode(cc_uint8_t * output, const cc_uint32_t * input, size_t input_len)
{
	size_t i, j;

	for (i = 0, j = 0; j < input_len; i++, j += sizeof(cc_uint32_t)) {
#ifdef TARGET_LITTLE_ENDIAN

#ifdef _MD5_CHECK_ALIGNMENT
		if ((uintptr_t)output & 0x3)   /* Not 4-byte aligned */
			bcopy(input + i, output + j, 4);
		else *(cc_uint32_t *)(output + j) = input[i];

#else
		/*LINTED E_BAD_PTR_CAST_ALIGN*/
		*(cc_uint32_t *)(output + j) = input[i];
#endif     /* _MD5_CHECK_ALIGNMENT */

#else   /* big endian -- will work on little endian, but slowly */

		output[j] = input[i] & 0xff;
		output[j + 1] = (input[i] >> 8) & 0xff;
		output[j + 2] = (input[i] >> 16) & 0xff;
		output[j + 3] = (input[i] >> 24) & 0xff;
#endif
	}
}

namespace CrissCross
{
	namespace Crypto
	{
		MD5Hash::MD5Hash() : m_hashString(NULL), m_hash(NULL)
		{
			Reset();
		}

		MD5Hash::~MD5Hash()
		{
			Reset();
		}

		int MD5Hash::Process(const void * _data, size_t _length)
		{
			Reset();
			if (!_data) return -1;

			MD5Update(&m_state, (unsigned char *)_data, _length);
			m_hash = new unsigned char[MD5_DIGEST_LENGTH];
			MD5Final((unsigned char *)m_hash, &m_state);
			return 0;
		}

		int MD5Hash::Process(CrissCross::IO::CoreIOReader *_reader)
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

		int MD5Hash::ProcessBlock(const void * _data, size_t _length)
		{
			if (!_data) return -1;

			MD5Update(&m_state, (unsigned char *)_data, _length);
			return 0;
		}

		void MD5Hash::Finalize()
		{
			if (m_hash) delete [] m_hash;

			m_hash = new unsigned char[MD5_DIGEST_LENGTH];
			MD5Final((unsigned char *)m_hash, &m_state);
		}

		const char *MD5Hash::ToString() const
		{
			if (m_hashString) return m_hashString;

			m_hashString = new char[33];
			for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
				sprintf(m_hashString + (i * 2), "%02x", m_hash[i]);

			return m_hashString;
		}

		void MD5Hash::Reset()
		{
			delete [] m_hash; m_hash = NULL;
			delete [] m_hashString; m_hashString = NULL;

			MD5Init(&m_state);
		}

		bool MD5Hash::operator==(const MD5Hash &_other) const
		{
			return (memcmp(m_hash, _other.m_hash, MD5_DIGEST_LENGTH) == 0);
		}
	}
}

#endif
