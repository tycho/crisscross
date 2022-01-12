/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2022 Steven Noonan.
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

	TEST_ASSERT(!llist->valid((unsigned int)-1));
	TEST_ASSERT(!llist->valid(1));
	TEST_ASSERT(!llist->valid(0));

	// three -> one -> two -> four
	llist->insert(newStr("one"));
	llist->insert(newStr("two"));
	llist->insert_front(newStr("three"));
	llist->insert_back(newStr("four"));

	TEST_ASSERT(strcmp(llist->get(0), "three") == 0);
	TEST_ASSERT(strcmp(llist->get(3), "four") == 0);
	TEST_ASSERT(strcmp(llist->get(1), "one") == 0);
	TEST_ASSERT(strcmp(llist->get(2), "two") == 0);

	// three -> two -> four
	delete [] llist->get(1);
	llist->remove(1);

	TEST_ASSERT(strcmp(llist->get(0), "three") == 0);
	TEST_ASSERT(strcmp(llist->get(1), "two") == 0);
	TEST_ASSERT(strcmp(llist->get(2), "four") == 0);

	while (llist->valid(0))	{
		delete [] llist->get(0);
		llist->remove(0);
	}

	TEST_ASSERT(!llist->valid((unsigned int)-1));
	TEST_ASSERT(!llist->valid(1));
	TEST_ASSERT(!llist->valid(0));

	delete llist;

	return 0;
}
