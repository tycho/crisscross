/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2020 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_endian_h
#define __included_cc_endian_h

#include <crisscross/platform_detect.h>
#include <crisscross/cc_attr.h>

namespace CrissCross
{
	namespace Endian
	{
		/* Use inline functions for compilers that support them, and static
		   functions for those that do not.  Because these functions become
		   static for compilers that do not support inline functions, this
		   header should only be included in files that actually use them.
		*/
		#if defined(__GNUC__) && defined(__i386__) && \
		   !(__GNUC__ == 2 && __GNUC_MINOR__ <= 95 /* broken gcc version */)
		static uint16_t Swap16(uint16_t x)
		{
			__asm__("xchgb %b0,%h0" : "=q" (x) :  "0" (x));
			return x;
		}
		#elif defined(__GNUC__) && defined(__x86_64__)
		static uint16_t Swap16(uint16_t x)
		{
			__asm__("xchgb %b0,%h0" : "=Q" (x) :  "0" (x));
			return x;
		}
		#elif defined(__GNUC__) && (defined(__powerpc__) || defined(__ppc__))
		static uint16_t Swap16(uint16_t x)
		{
			uint16_t result;

			__asm__("rlwimi %0,%2,8,16,23" : "=&r" (result) : "0" (x >> 8), "r" (x));
			return result;
		}
		#elif defined(__GNUC__) && (defined(__M68000__) || defined(__M68020__))
		static uint16_t Swap16(uint16_t x)
		{
			__asm__("rorw #8,%0" : "=d" (x) :  "0" (x) : "cc");
			return x;
		}
		#else
		static uint16_t Swap16(uint16_t x) {
			return((x<<8)|(x>>8));
		}
		#endif

		#if defined(__GNUC__) && defined(__i386__) && \
		   !(__GNUC__ == 2 && __GNUC_MINOR__ <= 95 /* broken gcc version */)
		static uint32_t Swap32(uint32_t x)
		{
			__asm__("bswap %0" : "=r" (x) : "0" (x));
			return x;
		}
		#elif defined(__GNUC__) && defined(__x86_64__)
		static uint32_t Swap32(uint32_t x)
		{
			__asm__("bswapl %0" : "=r" (x) : "0" (x));
			return x;
		}
		#elif defined(__GNUC__) && (defined(__powerpc__) || defined(__ppc__))
		static uint32_t Swap32(uint32_t x)
		{
			uint32_t result;

			__asm__("rlwimi %0,%2,24,16,23" : "=&r" (result) : "0" (x>>24), "r" (x));
			__asm__("rlwimi %0,%2,8,8,15"   : "=&r" (result) : "0" (result),    "r" (x));
			__asm__("rlwimi %0,%2,24,0,7"   : "=&r" (result) : "0" (result),    "r" (x));
			return result;
		}
		#elif defined(__GNUC__) && (defined(__M68000__) || defined(__M68020__))
		static uint32_t Swap32(uint32_t x)
		{
			__asm__("rorw #8,%0\n\tswap %0\n\trorw #8,%0" : "=d" (x) :  "0" (x) : "cc");
			return x;
		}
		#else
		static uint32_t Swap32(uint32_t x) {
			return((x<<24)|((x<<8)&0x00FF0000)|((x>>8)&0x0000FF00)|(x>>24));
		}
		#endif

		#if defined(__GNUC__) && defined(__i386__) && \
		   !(__GNUC__ == 2 && __GNUC_MINOR__ <= 95 /* broken gcc version */)
		static uint64_t Swap64(uint64_t x)
		{
			union {
				struct { uint32_t a,b; } s;
				uint64_t u;
			} v;
			v.u = x;
			__asm__("bswapl %0 ; bswapl %1 ; xchgl %0,%1"
					: "=r" (v.s.a), "=r" (v.s.b)
					: "0" (v.s.a), "1" (v.s.b));
			return v.u;
		}
		#elif defined(__GNUC__) && defined(__x86_64__)
		static uint64_t Swap64(uint64_t x)
		{
			__asm__("bswapq %0" : "=r" (x) : "0" (x));
			return x;
		}
		#else
		static uint64_t Swap64(uint64_t x)
		{
			uint32_t hi, lo;

			/* Separate into high and low 32-bit values and swap them */
			lo = (uint32_t)(x&0xFFFFFFFF);
			x >>= 32;
			hi = (uint32_t)(x&0xFFFFFFFF);
			x = Swap32(lo);
			x <<= 32;
			x |= Swap32(hi);
			return(x);
		}
		#endif
	}
}

/* Byteswap item from the specified endianness to the native endianness */
#ifdef TARGET_LITTLE_ENDIAN
#	define CC_SwapLE16(X)	(X)
#	define CC_SwapLE32(X)	(X)
#	define CC_SwapLE64(X)	(X)
#	define CC_SwapBE16(X)	CrissCross::Endian::Swap16(X)
#	define CC_SwapBE32(X)	CrissCross::Endian::Swap32(X)
#	define CC_SwapBE64(X)	CrissCross::Endian::Swap64(X)
#else
#	define CC_SwapLE16(X)	CrissCross::Endian::Swap16(X)
#	define CC_SwapLE32(X)	CrissCross::Endian::Swap32(X)
#	define CC_SwapLE64(X)	CrissCross::Endian::Swap64(X)
#	define CC_SwapBE16(X)	(X)
#	define CC_SwapBE32(X)	(X)
#	define CC_SwapBE64(X)	(X)
#endif

#endif
