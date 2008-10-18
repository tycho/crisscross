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
#include "dstack.h"

#include <crisscross/dstack.h>

using namespace CrissCross::Data;

int TestDStack()
{
	DStack<int> *dstack = new DStack<int>();

	for (int i = 0; i < 16; i++) {
		dstack->push(i);
	}

	TEST_ASSERT(dstack->count() == 16);
	TEST_ASSERT(dstack->peek() == 15);

	for (int i = 15; i > 0; i--) {
		TEST_ASSERT(dstack->pop() == i);
	}

	TEST_ASSERT(dstack->count() == 1);

	dstack->empty();
	TEST_ASSERT(dstack->count() == 0);

	delete dstack;

	return 0;
}
