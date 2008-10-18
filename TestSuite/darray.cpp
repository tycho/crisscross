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
#include "darray.h"

#include <crisscross/darray.h>

using namespace CrissCross::Data;

int TestDArray()
{
	DArray<int> *darray = new DArray<int>();

	darray->insert(1);
	darray->insert(3);
	darray->insert(5);
	darray->insert(7);

	TEST_ASSERT(darray->used() == 4);
	TEST_ASSERT(darray->get(0) == 1);
	TEST_ASSERT(darray->get(3) == 7);
	TEST_ASSERT(darray->get(1) == 3);
	TEST_ASSERT(darray->valid(0));
	TEST_ASSERT(darray->valid(1));
	TEST_ASSERT(darray->valid(2));
	TEST_ASSERT(darray->valid(3));
	TEST_ASSERT(!darray->valid(-1));
	TEST_ASSERT(!darray->valid(4));
	darray->remove(0);
	TEST_ASSERT(darray->used() == 3);
	darray->remove(3);
	TEST_ASSERT(darray->used() == 2);
	darray->empty();
	TEST_ASSERT(darray->used() == 0);

	delete darray;

	return 0;
}
