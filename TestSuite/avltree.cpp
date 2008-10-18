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
#include "avltree.h"
#include "testutils.h"

#include <crisscross/avltree.h>

using namespace CrissCross::Data;

int TestAVLTree_CString()
{
	AVLTree<const char *, const char *> *avltree = new AVLTree<const char *, const char *>();
	char                                *tmp;

	TEST_ASSERT(avltree->size() == 0);

	/* If the tree is properly encapsulated, this won't cause an error on test #1. */
	tmp = cc_strdup("first");
	avltree->insert(tmp, "one");
	free(tmp); tmp = NULL;

	TEST_ASSERT(avltree->size() == 1);

	avltree->insert("second", "two");
	avltree->insert("third", "three");
	avltree->insert("fourth", "four");

	TEST_ASSERT(avltree->size() == 4);

	const char *tmp1 = "one", *tmp2 = NULL;
	TEST_ASSERT(avltree->find("first", tmp2));
	TEST_ASSERT(Compare(tmp2, tmp1) == 0);
	TEST_ASSERT(!avltree->exists("fifth"));
	TEST_ASSERT(avltree->find("second", tmp2));

	tmp1 = "two";
	TEST_ASSERT(Compare(tmp2, tmp1) == 0);
	TEST_ASSERT(!avltree->erase("fifth"));
	TEST_ASSERT(avltree->size() == 4);

	TEST_ASSERT(avltree->exists("first"));
	TEST_ASSERT(avltree->erase("first"));
	TEST_ASSERT(avltree->size() == 3);

	TEST_ASSERT(avltree->exists("second"));
	TEST_ASSERT(avltree->erase("second"));

	TEST_ASSERT(avltree->exists("third"));
	TEST_ASSERT(avltree->erase("third"));

	TEST_ASSERT(avltree->exists("fourth"));
	TEST_ASSERT(avltree->erase("fourth"));

	TEST_ASSERT(avltree->size() == 0);

	delete avltree;
	return 0;
}

int TestAVLTree_String()
{
	AVLTree<std::string, std::string> *avltree = new AVLTree<std::string, std::string>();

	TEST_ASSERT(avltree->size() == 0);

	avltree->insert("first", "one");
	avltree->insert("second", "two");
	avltree->insert("third", "three");
	avltree->insert("fourth", "four");

	TEST_ASSERT(avltree->size() == 4);

	std::string                        tmp;
	TEST_ASSERT(avltree->find("first", tmp));
	TEST_ASSERT(tmp == "one");
	TEST_ASSERT(!avltree->exists("fifth"));

	TEST_ASSERT(avltree->find("second", tmp));
	TEST_ASSERT(tmp == "two");
	TEST_ASSERT(!avltree->exists("fifth"));
	TEST_ASSERT(avltree->size() == 4);

	TEST_ASSERT(avltree->erase("first"));
	TEST_ASSERT(avltree->size() == 3);

	TEST_ASSERT(avltree->erase("second"));
	TEST_ASSERT(avltree->size() == 2);

	TEST_ASSERT(avltree->erase("third"));
	TEST_ASSERT(avltree->size() == 1);

	TEST_ASSERT(avltree->erase("fourth"));
	TEST_ASSERT(avltree->size() == 0);

	delete avltree;
	return 0;
}

int TestAVLTree_Int()
{
	AVLTree<int, int> *avltree = new AVLTree<int, int>();

	TEST_ASSERT(avltree->size() == 0);

	avltree->insert(1, 1);
	avltree->insert(2, 2);
	avltree->insert(3, 3);
	avltree->insert(4, 4);

	int                tmp;
	TEST_ASSERT(avltree->size() == 4);

	TEST_ASSERT(avltree->find(1, tmp));
	TEST_ASSERT(tmp == 1);

	TEST_ASSERT(!avltree->exists(5));

	TEST_ASSERT(avltree->find(2, tmp));
	TEST_ASSERT(tmp == 2);

	TEST_ASSERT(!avltree->erase(5));

	TEST_ASSERT(avltree->size() == 4);

	TEST_ASSERT(avltree->exists(1));
	TEST_ASSERT(avltree->erase(1));

	TEST_ASSERT(avltree->size() == 3);

	TEST_ASSERT(avltree->exists(2));
	TEST_ASSERT(avltree->erase(2));

	TEST_ASSERT(avltree->size() == 2);

	TEST_ASSERT(avltree->exists(3));
	TEST_ASSERT(avltree->erase(3));

	TEST_ASSERT(avltree->size() == 1);

	TEST_ASSERT(avltree->exists(4));
	TEST_ASSERT(avltree->erase(4));

	TEST_ASSERT(avltree->size() == 0);

	delete avltree;
	return 0;
}
