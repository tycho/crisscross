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

#include "header.h"

using namespace CrissCross::IO;
using namespace CrissCross::System;

/* Gets up to about sqrt(MAX_INT) */
#define PREGEN 6544

#define USE_INTEGERS

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

bool isPrime(unsigned long _candidate)
{
	prime_t i, limit, next, n;
	prime_t *p;

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

	/* Iterate through the prime cache first to check divisors. */
	for (p = primeCache; *p != 0 && *p <= limit; p++)
		if (_modulus(n, *p) == 0) return false;

	/* Why did we exit the loop? Too high a divisor? */
	if (*p >= limit) return true;

	/* No cache hit, we're moving on to odd numbers. */
	if (primeCache[PREGEN - 2])
		next = primeCache[PREGEN - 2] + 2;
	else
		next = 3;

	/* Now test all other odd numbers up to sqrt(n) */
	for (i = next; i <= limit; i += 2) if (_modulus(n, i) == 0) return false;

	return true;
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
}

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
	primeCachePtr = primeCache;
	for (n = 3; primeCachePtr - primeCache <= PREGEN - 2; n += 2)
		if (isPrime(n))
			AddPrimeToCache((prime_t)n);
}

int main(int argc, char * *argv)
{
	Console *console = new Console();

	/* Begin your application here. */

	Stopwatch sw;
	unsigned long lastprime;
	unsigned long inc, max, i = 1, ret = 0;
	char pregen = 0;

	if (argc < 3) {
		console->WriteLine("Not enough parameters.");
		ret = 1;
		goto bomb;
	}

	while (argv[i][0] == '-') {
		if (argv[i][1] == 'p')
			pregen = (char)-1;
		i++;
	}

	inc = atoi(argv[i++]);
	max = atoi(argv[i++]);

	if (inc == 0 || max == 0) {
		console->WriteLine("Increment of %d and max of %d isn't a valid configuration.", inc, max);
		ret = 1;
		goto bomb;
	}

	if (!(inc <= max)) {
		console->WriteLine("Increment cannot be less than zero or greater than the max.");
		ret = 1;
		goto bomb;
	}

	if (!(max < ((unsigned long)-1) / 2)) {
		console->WriteLine("Maximum is too high.");
		ret = 1;
		goto bomb;
	}

#ifdef USE_INTEGERS
	console->WriteLine("Using integer math.\n");
#else
	console->WriteLine("Using floating-point math.\n");
#endif

	memset(primeCache, 0, sizeof(primeCache));
	if (pregen) {
		console->Write("Pregenerating %d primes... ", PREGEN);
		PrecalculatePrimes();
		console->WriteLine("OK\n");
	}

#ifdef TARGET_OS_WINDOWS
	/* These braces are here because the Borland compiler complains
	 * that the above goto statements skip these variable declarations
	 * and initializations. It's right, but we also don't care.
	 */
	{
	HANDLE hThread = GetCurrentThread();
	HANDLE hProcess = GetCurrentProcess();
	SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
	SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS);
	}
#endif

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
		console->WriteLine("Time for %9d primes: %8.3lf seconds (%8lu PPS, LP: %9lu)", i, sw.Elapsed(),
		                   (unsigned long)((double)i / sw.Elapsed()), lastprime);
#endif
	}

#ifdef TARGET_OS_WINDOWS
	system("PAUSE");
#endif

	/* End your application here. */

bomb:
	delete console;
	return ret;
}
