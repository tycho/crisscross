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

#ifndef __included_quicksort_test_h
#define __included_quicksort_test_h

#ifdef ENABLE_SORTS

int TestSort_IntArray(CrissCross::Data::Sorter<int> *_sorter);
int TestSort_DArray(CrissCross::Data::Sorter<int> *_sorter);
int TestSort_LList(CrissCross::Data::Sorter<int> *_sorter);

#endif

#endif
