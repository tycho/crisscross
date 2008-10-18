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
#include "stopwatch.h"

#include <crisscross/stopwatch.h>

using namespace CrissCross;

int TestStopwatch()
{
	System::Stopwatch  sw1;
	sw1.Start();
	System::ThreadSleep(10);
	sw1.Stop();
	TEST_ASSERT(sw1.Elapsed() < 1.0 && sw1.Elapsed() > 0.0);

	System::Stopwatch *sw2 = new System::Stopwatch();
	TEST_ASSERT(sw2);

	sw2->Start();
	System::ThreadSleep(10);
	sw2->Stop();
	TEST_ASSERT(sw2->Elapsed() < 1.0 && sw2->Elapsed() > 0.0);

	delete sw2;

	return 0;
}
