/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2010 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include "header.h"
#include "testutils.h"

#include "backtrace.h"
#include "compare.h"
#include "datatypes.h"
#include "darray.h"
#include "dstack.h"
#include "llist.h"
#include "splaytree.h"
#include "stltree.h"
#include "avltree.h"
#include "rbtree.h"
#include "hashtable.h"
#include "sanity.h"
#include "stopwatch.h"
#include "stree.h"
#include "sort.h"
#include "md4.h"
#include "md5.h"
#include "sha1.h"
#include "sha256.h"
#include "sha512.h"
#include "tiger.h"

using namespace CrissCross;

CrissCross::IO::Console *g_console;

int main(int argc, char * *argv)
{
	g_console = new IO::Console();

	g_console->SetColour(IO::Console::FG_RED | IO::Console::FG_INTENSITY);
	g_console->WriteLine("CrissCross Test Suite");
	g_console->SetColour();
	g_console->WriteLine();

	int retval = 0;

	System::SeedRandom();
	g_console->WriteLine("Testing PrintStackTrace()");
	TestBacktrace();

	WritePrefix("Size of uint8_t"); retval |= WriteResult(TestDatatypes_Uint8());
	WritePrefix("Size of uint16_t"); retval |= WriteResult(TestDatatypes_Uint16());
	WritePrefix("Size of uint32_t"); retval |= WriteResult(TestDatatypes_Uint32());
	WritePrefix("Size of uint64_t"); retval |= WriteResult(TestDatatypes_Uint64());
	WritePrefix("Size of ulong_t"); retval |= WriteResult(TestDatatypes_Ulong());
	WritePrefix("Size of bool"); retval |= WriteResult(TestDatatypes_Bool());
	WritePrefix("Size of float"); retval |= WriteResult(TestDatatypes_Float());
	WritePrefix("Size of double"); retval |= WriteResult(TestDatatypes_Double());
	WritePrefix("Size of uintptr_t"); retval |= WriteResult(TestDatatypes_Pointer());

#ifdef TARGET_LITTLE_ENDIAN
	WritePrefix("Type-based aliasing sanity"); retval |= WriteResult(TestSanity_TypeBasedAliasing());
#endif

	WritePrefix("MD4"); retval |= WriteResult(TestMD4());
	WritePrefix("MD5"); retval |= WriteResult(TestMD5());
	WritePrefix("SHA-1"); retval |= WriteResult(TestSHA1());
	WritePrefix("SHA-256"); retval |= WriteResult(TestSHA256());
	WritePrefix("SHA-512"); retval |= WriteResult(TestSHA512());
	WritePrefix("Tiger"); retval |= WriteResult(TestTiger());

	WritePrefix("Stopwatch"); retval |= WriteResult(TestStopwatch());

	WritePrefix("char* Compare"); retval |= WriteResult(TestComparison_CString());
	WritePrefix("std::string Compare"); retval |= WriteResult(TestComparison_String());
	WritePrefix("Integer Compare"); retval |= WriteResult(TestComparison_Integer());

	WritePrefix("LList"); retval |= WriteResult(TestLList());
	WritePrefix("DArray"); retval |= WriteResult(TestDArray());
	WritePrefix("DStack"); retval |= WriteResult(TestDStack());

	WritePrefix("SplayTree<const char *, const char *>"); retval |= WriteResult(TestSplayTree_CString());
	WritePrefix("SplayTree<std::string, std::string>"); retval |= WriteResult(TestSplayTree_String());
	WritePrefix("SplayTree<int, int>"); retval |= WriteResult(TestSplayTree_Int());

#ifdef ENABLE_STLTREE
	WritePrefix("STLTree<const char *, const char *>"); retval |= WriteResult(TestSTLTree_CString());
	WritePrefix("STLTree<std::string, std::string>"); retval |= WriteResult(TestSTLTree_String());
	WritePrefix("STLTree<int, int>"); retval |= WriteResult(TestSTLTree_Int());
#endif

	WritePrefix("AVLTree<const char *, const char *>"); retval |= WriteResult(TestAVLTree_CString());
	WritePrefix("AVLTree<std::string, std::string>"); retval |= WriteResult(TestAVLTree_String());
	WritePrefix("AVLTree<int, int>"); retval |= WriteResult(TestAVLTree_Int());

	WritePrefix("RedBlackTree<const char *, const char *>"); retval |= WriteResult(TestRedBlackTree_CString());
	WritePrefix("RedBlackTree<std::string, std::string>"); retval |= WriteResult(TestRedBlackTree_String());
	WritePrefix("RedBlackTree<int, int>"); retval |= WriteResult(TestRedBlackTree_Int());

	WritePrefix("STree<const char *, const char *>"); retval |= WriteResult(TestSTree_CString());
	WritePrefix("STree<std::string, std::string>"); retval |= WriteResult(TestSTree_String());
	WritePrefix("STree<int, int>"); retval |= WriteResult(TestSTree_Int());

	WritePrefix("HashTable<const char *, const char *>"); retval |= WriteResult(TestHashTable_CString());

	CrissCross::Data::CombSort<int> cs;
	WritePrefix("CombSort (integer array)"); retval |= WriteResult(TestSort_IntArray(&cs));
	WritePrefix("CombSort (integer DArray)"); retval |= WriteResult(TestSort_DArray(&cs));
	WritePrefix("CombSort (integer LList)"); retval |= WriteResult(TestSort_LList(&cs));

	CrissCross::Data::HeapSort<int> hs;
	WritePrefix("HeapSort (integer array)"); retval |= WriteResult(TestSort_IntArray(&hs));
	WritePrefix("HeapSort (integer DArray)"); retval |= WriteResult(TestSort_DArray(&hs));
	WritePrefix("HeapSort (integer LList)"); retval |= WriteResult(TestSort_LList(&hs));

	CrissCross::Data::ShellSort<int> ss;
	WritePrefix("ShellSort (integer array)"); retval |= WriteResult(TestSort_IntArray(&ss));
	WritePrefix("ShellSort (integer DArray)"); retval |= WriteResult(TestSort_DArray(&ss));
	WritePrefix("ShellSort (integer LList)"); retval |= WriteResult(TestSort_LList(&ss));

	CrissCross::Data::QuickSort<int> qs;
	WritePrefix("QuickSort (integer array)"); retval |= WriteResult(TestSort_IntArray(&qs));
	WritePrefix("QuickSort (integer DArray)"); retval |= WriteResult(TestSort_DArray(&qs));
	WritePrefix("QuickSort (integer LList)"); retval |= WriteResult(TestSort_LList(&qs));

	if (retval == 0) {
		g_console->WriteLine();
		g_console->WriteLine("All tests passed.");
		g_console->WriteLine();
	}

	g_console->Flush();

#ifdef TARGET_OS_WINDOWS
	system("pause");
#endif

	delete g_console;

	return retval;
}
