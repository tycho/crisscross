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
#include "testutils.h"
#include "combsort.h"

#include <crisscross/universal_include.h>
#include <crisscross/darray.h>
#include <crisscross/combsort.h>

using namespace CrissCross::Data;

int TestCombSort_IntArray()
{
	int            int_array [] = {0, 4, 3, 6, 2, 1, 5, 7, 9, 8 };
	CombSort<int> *cs = new CombSort<int>();
	cs->Sort(int_array, 10);
	for (int i = 0; i < 10; i++) {
		TEST_ASSERT(int_array[i] == i);
	}

	delete cs;
	return 0;
}

int TestCombSort_DArray()
{
	DArray<int>   *darray = new DArray<int>();

	darray->insert(4);
	darray->insert(2);
	darray->insert(0);
	darray->insert(3);
	darray->insert(1);

	CombSort<int> *cs = new CombSort<int>();
	darray->sort(cs);
	delete cs;

	for (int i = 0; i < 5; i++) {
		TEST_ASSERT(darray->get(i) == i);
	}

	delete darray;
	return 0;
}

int TestCombSort_LList()
{
	LList<int>    *llist = new LList<int>();

	llist->insert(4);
	llist->insert(2);
	llist->insert(0);
	llist->insert(3);
	llist->insert(1);

	CombSort<int> *cs = new CombSort<int>();
	llist->sort(cs);
	delete cs;

	for (int i = 0; i < 5; i++) {
		TEST_ASSERT(llist->get(i) == i);
	}

	delete llist;
	return 0;
}
