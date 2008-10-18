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
#include "md5.h"

using namespace CrissCross::Crypto;
using namespace CrissCross::IO;

int TestMD5()
{
	MD5Hash     md5;

	const char *teststring = NULL;
	size_t      length = 0;

	/* These tests are from FIPS PUB 180-1 */

	teststring = ""; length = strlen(teststring);
	md5.Process(teststring, length);
	TEST_ASSERT(strcmp(md5.ToString(), "d41d8cd98f00b204e9800998ecf8427e") == 0);

	teststring = "a"; length = strlen(teststring);
	md5.Process(teststring, length);
	TEST_ASSERT(strcmp(md5.ToString(), "0cc175b9c0f1b6a831c399e269772661") == 0);

	teststring = "abc"; length = strlen(teststring);
	md5.Process(teststring, length);
	TEST_ASSERT(strcmp(md5.ToString(), "900150983cd24fb0d6963f7d28e17f72") == 0);

	teststring = "message digest"; length = strlen(teststring);
	md5.Process(teststring, length);
	TEST_ASSERT(strcmp(md5.ToString(), "f96b697d7cb7938d525a2f31aaf161d0") == 0);

	teststring = "abcdefghijklmnopqrstuvwxyz"; length = strlen(teststring);
	md5.Process(teststring, length);
	TEST_ASSERT(strcmp(md5.ToString(), "c3fcd3d76192e4007dfb496cca67e13b") == 0);

	teststring = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"; length = strlen(teststring);
	md5.Process(teststring, length);
	TEST_ASSERT(strcmp(md5.ToString(), "d174ab98d277d9f5a5611c2c9f419d9f") == 0);

	teststring = "12345678901234567890123456789012345678901234567890123456789012345678901234567890"; length = strlen(teststring);
	md5.Process(teststring, length);
	TEST_ASSERT(strcmp(md5.ToString(), "57edf4a22be3c955ac49da2e2107b67a") == 0);

	MD5Hash    otherhash;
	otherhash.Process("cheese", 6);
	TEST_ASSERT(otherhash != md5 && md5 != otherhash);

	otherhash.Process(teststring, length);
	TEST_ASSERT(otherhash == md5 && md5 == otherhash);

#ifdef FILE_CHECKSUM
	TextReader file;
	file.Open("testfile");
	md5.Process((CoreIOReader *)&file);
	TEST_ASSERT(strcmp(md5.ToString(), "bc2471d759c6ae2eb44482f571b02a40") == 0);
#endif

	return 0;
}
