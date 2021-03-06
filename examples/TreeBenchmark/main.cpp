/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2021 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include "header.h"

#include <cmath>

using namespace CrissCross::Data;
using namespace CrissCross::IO;
using namespace CrissCross::System;
using namespace std;

Console *console = NULL;

typedef unsigned long ktype_t;

#define XORSwap(x,y) { (x) = (x) ^ (y); (y) = (y) ^ (x); (x) = (x) ^ (y); }

void shuffleElements(ktype_t *_array, size_t size)
{ 
	ktype_t temporary;
	size_t randomNum, last;

	for (last = size; last > 1; last--) 
	{ 
		randomNum = RandomNumber() % last;
		temporary = _array[randomNum];
		_array[randomNum] = _array[last - 1];
		_array[last - 1] = temporary;
	}
}

template <class T>
void RunTestcase(T _tree, unsigned long _size, bool _ordered_insert)
{
	console->Write("%10lu ", _size);
	
	unsigned long realsize = _size * 2;

	ktype_t *elems = NULL;
	if (!_ordered_insert) {
		elems = new ktype_t[_size + 1];
		for (size_t i = 1; i < _size + 1; i++) {
			elems[i] = 2*i;
		}
		elems[_size] = 0;
		shuffleElements(elems, _size);
	}

	// fill tree
	Stopwatch sw;
	sw.Start();
	if (_ordered_insert) {
		for (size_t i = 0; i < _size; i++) {
			_tree->insert(2 * i, 1);
		}
	} else {
		for (ktype_t *p = elems; *p; p++) {
			_tree->insert(*p, 1);
		}
	}
	sw.Stop();
	console->Write("%9.5lfs ", sw.Elapsed());
	delete [] elems;
	elems = NULL;

	// successful searches
	sw.Start();
	for (size_t i = 0; i < _size; i++) {
		_tree->find((2 * RandomNumber()) % realsize, 0);
	}
	sw.Stop();
	console->Write("%9.5lfs ", sw.Elapsed());

	// mixed success searches
	sw.Start();
	for (size_t i = 0; i < _size; i++) {
		_tree->find(RandomNumber() % realsize, 0);
	}
	sw.Stop();
	console->Write("%9.5lfs ", sw.Elapsed());

	// invalid searches
	sw.Start();
	for (size_t i = 0; i < _size; i++) {
		_tree->find((1 + 2 * RandomNumber()) % realsize, 0);
	}
	sw.Stop();
	console->Write("%9.5lfs ", sw.Elapsed());

	// empty tree
	sw.Start();
	_tree->empty();
	sw.Stop();
	console->WriteLine("%9.5lfs", sw.Elapsed());
}

template <class T>
void Test(const char *_name, size_t *sizes)
{
	T *tree;
	console->WriteLine("Testing %s...", _name);
	console->WriteLine();
	console->WriteLine("Dataset: Random");
	console->WriteLine("%10s %10s %10s %10s %10s %10s", "size", "add", "srch+", "srch", "srch-", "empty");
	tree = new T();
	for (size_t *p = sizes; *p != 0; p++) {
		RunTestcase<T *> (tree, *p, false);
	}
	delete tree;
	tree = NULL;
	console->WriteLine();
	console->WriteLine("Dataset: Ordered");
	console->WriteLine("%10s %10s %10s %10s %10s %10s", "size", "add", "srch+", "srch", "srch-", "empty");
	tree = new T();
	for (size_t *p = sizes; *p != 0; p++) {
		RunTestcase<T *> (tree, *p, true);
	}
	delete tree;
	tree = NULL;
	console->WriteLine();
	console->WriteLine("%s tests complete.", _name);
	console->WriteLine();
	console->WriteLine();
}

int main(int argc, char * *argv)
{
	console = new Console();

	/* Begin your application here. */

	SeedRandom();

	const size_t count = 11;
	size_t *sizes = new size_t[count + 1];
	for(size_t i = 0; i < count; i++) {
		sizes[i] = (size_t)pow(2.0,i+10.0);
	}
	sizes[count] = 0;

	Test< AVLTree<ktype_t, char> >("AVLTree", sizes);
	Test< RedBlackTree<ktype_t, char> >("RedBlackTree", sizes);
	Test< SplayTree<ktype_t, char> >("SplayTree", sizes);
//	Test< STree<ktype_t, char> >("STree", sizes);
#ifdef ENABLE_STLTREE
	Test< STLTree<ktype_t, char> >("STLTree", sizes);
#endif

	console->WriteLine();

	/* End your application here. */

#ifdef TARGET_OS_WINDOWS
	system("pause");
#endif

	delete console;
	return 0;
}
