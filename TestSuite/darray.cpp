/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2015 Steven Noonan.
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

	size_t pos;
	pos = darray->insert(1);
	TEST_ASSERT(darray->valid(pos));
	TEST_ASSERT(darray->get(pos) == 1);
	pos = darray->insert(3);
	TEST_ASSERT(darray->valid(pos));
	TEST_ASSERT(darray->get(pos) == 3);
	pos = darray->insert(5);
	TEST_ASSERT(darray->valid(pos));
	TEST_ASSERT(darray->get(pos) == 5);
	pos = darray->insert(7);
	TEST_ASSERT(darray->valid(pos));
	TEST_ASSERT(darray->get(pos) == 7);

	TEST_ASSERT(darray->used() == 4);
	TEST_ASSERT(darray->get(0) == 1);
	TEST_ASSERT(darray->get(3) == 7);
	TEST_ASSERT(darray->get(1) == 3);
	TEST_ASSERT(darray->valid(0));
	TEST_ASSERT(darray->valid(1));
	TEST_ASSERT(darray->valid(2));
	TEST_ASSERT(darray->valid(3));
	TEST_ASSERT(!darray->valid((unsigned int)-1));
	TEST_ASSERT(!darray->valid(4));
	darray->remove(0);
	TEST_ASSERT(darray->used() == 3);
	TEST_ASSERT(!darray->valid(0));
	TEST_ASSERT(darray->valid(1));
	TEST_ASSERT(darray->get(1) == 3);
	TEST_ASSERT(darray->get(3) == 7);
	darray->remove(3);
	TEST_ASSERT(darray->used() == 2);
	TEST_ASSERT(!darray->valid(0));
	TEST_ASSERT(darray->valid(1));
	TEST_ASSERT(darray->valid(2));
	TEST_ASSERT(!darray->valid(3));
	TEST_ASSERT(darray->get(1) == 3);
	TEST_ASSERT(darray->get(2) == 5);
	darray->empty();
	TEST_ASSERT(darray->used() == 0);

	delete darray;

	return 0;
}
