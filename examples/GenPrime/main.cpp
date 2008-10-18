/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of IO.IN Research.
 *
 *   (c) 2006-2008 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include "header.h"

using namespace CrissCross::IO;
using namespace CrissCross::System;

/* 16K of data. Should fit in most L1 caches. */
#define PREGEN 4096

/* #define USE_INTEGERS */

#ifdef USE_INTEGERS
typedef unsigned long prime_t;
#define _modulus(x, y) x % y
#else
typedef float prime_t;
#define _modulus(x, y) (int)fmod(x, y)
#endif

typedef bool (*isPrimeFunc)(unsigned long _number);

#ifdef PREGEN
prime_t primeCache[PREGEN]; prime_t *primeCachePtr;
#endif

/* */
/* NOTE: The '__asm nop;' lines keep the compiler from marking the function */
/*		 as a waste of time and optimizing it out. */
/* */

bool isPrime(unsigned long _candidate)
{
	prime_t i, limit, next, *p, n;

	/* 1 and 0 aren't prime */
	if (_candidate < 2) return false;

	/* All numbers less than 4 are prime, except '1'. */
	if (_candidate < 4) return true;

	if (_candidate == 5) return true;

	/* All other numbers divisible by 2 are not prime. */
	if (_candidate % 2 == 0) return false;

	/* All other numbers divisible by 5 are not prime. */
	if (_candidate % 5 == 0) return false;

	if ((_candidate + 1) % 6 != 0 && (_candidate - 1) % 6 != 0) return false;

	n = (prime_t)_candidate;

	/*
	 * if n is composite then it can be factored into two values,
	 * at least one of which is less than or equal to sqrt(n)
	 */
	limit = (prime_t)sqrt((double)_candidate);

#ifdef PREGEN
	/* Iterate through the prime cache first to check divisors. */
	for (p = primeCache; *p != 0 && *p <= limit; p++)
		if (_modulus(n, *p) == 0) return false;

	/* Why did we exit the loop? Too high a divisor? */
	if (*p >= limit) return true;

	/* No cache hit, we're moving on to odd numbers. */
	if (primeCache[PREGEN - 2])
		next = primeCache[PREGEN - 2] + 2;
	else
#endif
	next = 3;

	/* Now test all other odd numbers up to sqrt(n) */
	for (i = next; i <= limit; i += 2) if (_modulus(n, i) == 0) return false;

	return true;
#if defined (TARGET_CPU_X86) || defined (TARGET_CPU_X64)
#if defined (TARGET_COMPILER_GCC)
	asm ("nop");
#endif
#endif

#if defined (TARGET_CPU_X86)
#if defined (TARGET_COMPILER_VC)
	__asm nop;
#endif
#elif defined (TARGET_CPU_X64)
#if defined (TARGET_OS_WINDOWS) && defined (TARGET_COMPILER_ICC)
	__asm nop;
#endif
#endif
}

unsigned long genPrime(unsigned long _maxToFind, isPrimeFunc _func)
{
	unsigned long count = 0, lastprime = 0;
	for (unsigned long num = 1; count < _maxToFind; num++) {
		/* CoreAssert ( isPrime ( num ) == _func ( num ) ); */
		if (_func(num))	{
			count++;
			lastprime = num;
		}
	}

	return lastprime;
#if defined (TARGET_CPU_X86) || defined (TARGET_CPU_X64)
#if defined (TARGET_COMPILER_GCC)
	asm ("nop");
#endif
#endif

#if defined (TARGET_CPU_X86)
#if defined (TARGET_COMPILER_VC)
	__asm nop;
#endif
#elif defined (TARGET_CPU_X64)
#if defined (TARGET_OS_WINDOWS) && defined (TARGET_COMPILER_ICC)
	__asm nop;
#endif
#endif
}

#ifdef PREGEN
void AddPrimeToCache(prime_t _prime)
{
	/* inUse is a simple lock for multithreading. */
	static char inUse = 0;
	if (primeCachePtr - primeCache > PREGEN - 2)
		return;

	while (inUse != 0) ;
	inUse = 1;
	*primeCachePtr = _prime;
	primeCachePtr++;
	inUse = 0;
}

void PrecalculatePrimes()
{
	/* Find a predetermined number of primes to use as divisors. */

	unsigned long n;
	memset(primeCache, 0, sizeof(primeCache));
	primeCachePtr = primeCache;
	for (n = 3; primeCachePtr - primeCache <= PREGEN - 2; n += 2)
		if (isPrime(n))
			AddPrimeToCache((prime_t)n);
}
#endif

int RunApplication(int argc, char * *argv)
{
	Console      *console = new Console();

	/* Begin your application here. */

	unsigned long inc, max;

	if (argc < 3) {
		console->WriteLine("Not enough parameters.");
		delete console;
		return 0;
	}

	inc = atoi(argv[1]);
	max = atoi(argv[2]);

	if (!(inc <= max)) {
		console->WriteLine("Increment cannot be less than zero or greater than the max.");
		delete console;
		return 0;
	}

	if (!(max < ((unsigned long)-1) / 2)) {
		console->WriteLine("Maximum is too high.");
		delete console;
		return 0;
	}

#ifdef USE_INTEGERS
	console->WriteLine("Using integer math.\n");
#else
	console->WriteLine("Using floating-point math.\n");
#endif

#ifdef PREGEN
	console->Write("Pregenerating %d primes... ", PREGEN);
	PrecalculatePrimes();
	console->WriteLine("OK\n");
#endif

#ifdef TARGET_OS_WINDOWS
	HANDLE        hThread = GetCurrentThread();
	HANDLE        hProcess = GetCurrentProcess();
	SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
	SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS);

#endif

	Stopwatch     sw;

	unsigned long lastprime;

#ifdef TARGET_OS_NDSFIRMWARE
	for (unsigned long i = 10000; i <= 50000; i += 1000)
#else
	for (unsigned long i = inc; i <= max; i += inc)
#endif
	{
		sw.Start();
		lastprime = genPrime(i, isPrime);
		sw.Stop();
#ifdef TARGET_OS_NDSFIRMWARE
		console->WriteLine("%5d primes: %d.%03ds (%lu PPS, LP: %lu)", i, sw.ElapsedMS() / 1000,
		                   sw.ElapsedMS() % 1000, (unsigned long)(i / sw.Elapsed()), lastprime);
#else
		console->WriteLine("Time for %9d primes: %6.3lf seconds (%lu PPS, LP: %lu)", i, sw.Elapsed(),
		                   (unsigned long)((double)i / sw.Elapsed()), lastprime);
#endif
	}

#ifdef TARGET_OS_WINDOWS
	system("PAUSE");
#endif

	/* End your application here. */

	delete console;

	return 0;
}
