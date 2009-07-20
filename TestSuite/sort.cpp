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
#include "testutils.h"
#include "sort.h"

#ifdef ENABLE_SORTS

using namespace CrissCross::Data;

int TestSort_IntArray(Sorter<int> *_sorter)
{
	int int_array [SORT_ITEMS];
	for (int i = 0; i < SORT_ITEMS; i++) {
		int_array[i] = CrissCross::System::RandomNumber();
	}

	_sorter->Sort(int_array, SORT_ITEMS);

	for (int i = 0; i < SORT_ITEMS - 1; i++) {
		TEST_ASSERT(int_array[i] <= int_array[i+1]);
	}

	return 0;
}

int TestSort_DArray(Sorter<int> *_sorter)
{
	DArray<int>    *darray = new DArray<int>();

	for (int i = 0; i < SORT_ITEMS; i++) {
		darray->insert(CrissCross::System::RandomNumber());
	}

	darray->sort(_sorter);

	for (int i = 0; i < SORT_ITEMS - 1; i++) {
		TEST_ASSERT(darray->get(i) <= darray->get(i+1));
	}

	delete darray;
	return 0;
}

int TestSort_LList(Sorter<int> *_sorter)
{
	LList<int>     *llist = new LList<int>();

	for (int i = 0; i < SORT_ITEMS; i++) {
		llist->insert(rand());
	}

	llist->sort(_sorter);

	for (int i = 0; i < SORT_ITEMS - 1; i++) {
		TEST_ASSERT(llist->get(i) <= llist->get(i+1));
	}

	delete llist;
	return 0;
}

#endif
