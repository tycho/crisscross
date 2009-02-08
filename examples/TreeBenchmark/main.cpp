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

using namespace CrissCross::Data;
using namespace CrissCross::IO;
using namespace CrissCross::System;
using namespace std;

Console *console = NULL;

typedef unsigned long ktype_t;
const size_t MaxSearches = 100000;

#define XORSwap(x,y) { (x) = (x) ^ (y); (y) = (y) ^ (x); (x) = (x) ^ (y); }

void shuffleElements(DArray<ktype_t> *_array) 
{ 
	ktype_t temporary;
	size_t randomNum, last, size = _array->used();
	DArray<ktype_t> &array = *_array;

	for (last = size; last > 1; last--) 
	{ 
		randomNum = RandomNumber() % last;
		temporary = array[randomNum];
		array[randomNum] = array[last - 1];
		array[last - 1] = temporary;
	}
}

template <class T>
void TestTree(T _tree, DArray<ktype_t> *dataset, unsigned long _size)
{
	console->Write("%10lu ", _size);
	
	// fill tree
	Stopwatch sw;
	sw.Start();
	for (size_t i = 0; i < _size; i++) {
		ktype_t item = dataset->get(i);
		_tree->insert(item, item);
	}
	sw.Stop();
	console->Write("%9.5lfs ", sw.Elapsed());
	
	DArray<ktype_t> searchItems;
	
	// create valid item list
	searchItems.empty();
	while (searchItems.used() < MaxSearches) {
		unsigned long idx = RandomNumber() % _size;
		searchItems.insert(dataset->get(idx));
	}
	
	// successful searches
	sw.Start();
	for (size_t i = 0; i < MaxSearches; i++) {
		_tree->find(searchItems[i]);
	}
	sw.Stop();
	console->Write("%9.5lfs ", sw.Elapsed());
	
	// create mixed item list
	searchItems.empty();
	while (searchItems.used() < MaxSearches) {
		unsigned long idx;
		
		idx = RandomNumber() % _size;
		searchItems.insert(dataset->get(idx));
		
		idx = _size + (RandomNumber() % _size);
		searchItems.insert(dataset->get(idx));
	}
	
	// mixed success searches
	sw.Start();
	for (size_t i = 0; i < MaxSearches; i++) {
		_tree->find(searchItems[i]);
	}
	sw.Stop();
	console->Write("%9.5lfs ", sw.Elapsed());
	
	// create invalid item list
	searchItems.empty();
	while (searchItems.used() < MaxSearches) {
		unsigned long idx = _size + (RandomNumber() % _size);
		searchItems.insert(dataset->get(idx));
	}
	
	// invalid searches
	sw.Start();
	for (size_t i = 0; i < MaxSearches; i++) {
		_tree->find(searchItems[i]);
	}
	sw.Stop();
	console->Write("%9.5lfs ", sw.Elapsed());

	// empty tree
	sw.Start();
	_tree->empty();
	sw.Stop();
	console->WriteLine("%9.5lfs", sw.Elapsed());
}

int RunApplication(int argc, char * *argv)
{
	console = new Console();

	/* Begin your application here. */

	SeedRandom();
	Stopwatch sw;
	size_t sizes [] = { 100000,  1000000, 2000000,
			    3000000, 4000000, 5000000,
			    0 };
	size_t biggest = 0;
	
	// Locate the last element in the sizes list.
	for (size_t *p = sizes; *p != 0; p++) {
		biggest = max(biggest, *p);
	}
	
	DArray<ktype_t> *dataset = new DArray<ktype_t>(biggest*2);
	console->Write("Building data set of %lu items... ", biggest * 2);
	sw.Start();
	size_t i = 0;
	while (dataset->used() < biggest * 2) {
		dataset->insert(i++);
	}
	shuffleElements(dataset);
	sw.Stop();
	console->WriteLine("%8.5lfs", sw.Elapsed());

	console->WriteLine();
	console->WriteLine("Testing AVLTree...");
	console->WriteLine("%10s %10s %10s %10s %10s %10s", "size", "add", "srch+", "srch", "srch-", "empty");
	AVLTree<ktype_t, char> *avltree = new AVLTree<ktype_t, char>();
	for (size_t *p = sizes; *p != 0; p++) {
		TestTree<AVLTree<ktype_t, char> *> (avltree, dataset, *p);
	}
	console->WriteLine("AVLTree tests complete.");
	delete avltree;
	avltree = NULL;

	console->WriteLine();
	console->WriteLine("Testing RedBlackTree...");
	console->WriteLine("%10s %10s %10s %10s %10s %10s", "size", "add", "srch+", "srch", "srch-", "empty");
	RedBlackTree<ktype_t, char> *rbtree = new RedBlackTree<ktype_t, char>();
	for (size_t *p = sizes; *p != 0; p++) {
		TestTree<RedBlackTree<ktype_t, char> *> (rbtree, dataset, *p);
	}
	console->WriteLine("RedBlackTree tests complete.");
	delete rbtree;
	rbtree = NULL;

	console->WriteLine();
	console->WriteLine("Testing SplayTree...");
	console->WriteLine("%10s %10s %10s %10s %10s %10s", "size", "add", "srch+", "srch", "srch-", "empty");
	SplayTree<ktype_t, char> *splaytree = new SplayTree<ktype_t, char>();
	for (size_t *p = sizes; *p != 0; p++) {
		TestTree<SplayTree<ktype_t, char> *> (splaytree, dataset, *p);
	}
	console->WriteLine("SplayTree tests complete.");
	delete splaytree;
	splaytree = NULL;

	console->WriteLine();
	console->WriteLine("Testing STLTree...");
	console->WriteLine("%10s %10s %10s %10s %10s %10s", "size", "add", "srch+", "srch", "srch-", "empty");
	STLTree<ktype_t, char> *stltree = new STLTree<ktype_t, char>();
	for (size_t *p = sizes; *p != 0; p++) {
		TestTree<STLTree<ktype_t, char> *> (stltree, dataset, *p);
	}
	console->WriteLine("STLTree tests complete.");
	delete stltree;
	stltree = NULL;
	
	delete dataset;
	console->WriteLine();

	/* End your application here. */

#ifdef TARGET_OS_WINDOWS
	system("pause");
#endif

	delete console;
	return 0;
}
