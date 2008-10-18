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
#include "md2.h"

#if !defined(DISABLE_DEPRECATED_CODE)

using namespace CrissCross::Crypto;
using namespace CrissCross::IO;

int TestMD2()
{
	MD2Hash     md2;

	const char *teststring = NULL;
	size_t      length = 0;

	/* These tests are from http://tools.ietf.org/html/rfc1319 */

	teststring = ""; length = strlen(teststring);
	md2.Process(teststring, length);
	TEST_ASSERT(strcmp(md2.ToString(), "8350e5a3e24c153df2275c9f80692773") == 0);

	teststring = "a"; length = strlen(teststring);
	md2.Process(teststring, length);
	TEST_ASSERT(strcmp(md2.ToString(), "32ec01ec4a6dac72c0ab96fb34c0b5d1") == 0);

	teststring = "abc"; length = strlen(teststring);
	md2.Process(teststring, length);
	TEST_ASSERT(strcmp(md2.ToString(), "da853b0d3f88d99b30283a69e6ded6bb") == 0);

	teststring = "message digest"; length = strlen(teststring);
	md2.Process(teststring, length);
	TEST_ASSERT(strcmp(md2.ToString(), "ab4f496bfb2a530b219ff33031fe06b0") == 0);

	teststring = "abcdefghijklmnopqrstuvwxyz"; length = strlen(teststring);
	md2.Process(teststring, length);
	TEST_ASSERT(strcmp(md2.ToString(), "4e8ddff3650292ab5a4108c3aa47940b") == 0);

	teststring = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"; length = strlen(teststring);
	md2.Process(teststring, length);
	TEST_ASSERT(strcmp(md2.ToString(), "da33def2a42df13975352846c30338cd") == 0);

	teststring = "12345678901234567890123456789012345678901234567890123456789012345678901234567890"; length = strlen(teststring);
	md2.Process(teststring, length);
	TEST_ASSERT(strcmp(md2.ToString(), "d5976f79d83d3a0dc9806c3c66f3efd8") == 0);

	MD2Hash    otherhash;
	otherhash.Process("cheese", 6);
	TEST_ASSERT(otherhash != md2 && md2 != otherhash);

	otherhash.Process(teststring, length);
	TEST_ASSERT(otherhash == md2 && md2 == otherhash);

#ifdef FILE_CHECKSUM
	TextReader file;
	file.Open("testfile");
	md2.Process((CoreIOReader *)&file);
	TEST_ASSERT(strcmp(md2.ToString(), "2d6190f1df530f8056fbdb4b9bacf966") == 0);
#endif

	return 0;
}

#endif
