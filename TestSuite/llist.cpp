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
#include "llist.h"
#include "testutils.h"

#include <crisscross/llist.h>

using namespace CrissCross::Data;

int TestLList()
{
	LList<char *> *llist = new LList<char *>();
	TEST_ASSERT(llist);

	TEST_ASSERT(!llist->valid(-1));
	TEST_ASSERT(!llist->valid(1));
	TEST_ASSERT(!llist->valid(0));

	llist->insert(newStr("one"));
	llist->insert(newStr("two"));
	llist->insert(newStr("three"));
	llist->insert(newStr("four"));

	TEST_ASSERT(strcmp(llist->get(0), "one") == 0);
	TEST_ASSERT(strcmp(llist->get(2), "three") == 0);
	TEST_ASSERT(strcmp(llist->get(3), "four") == 0);
	TEST_ASSERT(strcmp(llist->get(1), "two") == 0);

	delete [] llist->get(1);
	llist->remove(1);

	TEST_ASSERT(strcmp(llist->get(0), "one") == 0);
	TEST_ASSERT(strcmp(llist->get(1), "three") == 0);
	TEST_ASSERT(strcmp(llist->get(2), "four") == 0);

	while (llist->valid(0))	{
		delete [] llist->get(0);
		llist->remove(0);
	}

	TEST_ASSERT(!llist->valid(-1));
	TEST_ASSERT(!llist->valid(1));
	TEST_ASSERT(!llist->valid(0));

	delete llist;

	return 0;
}
