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
#include "rijndael.h"

using namespace CrissCross::IO;
using namespace std;

Console *console = NULL;

int RunApplication(int argc, char * *argv)
{
	console = new Console();

	/* Begin your application here. */

	console->SetColour(Console::FG_RED | Console::FG_INTENSITY);
	console->WriteLine("=======================================");
	console->WriteLine("= RIJNDAEL (AES) ENCRYPTION BENCHMARK =");
	console->WriteLine("=======================================");
	console->SetColour();
	console->WriteLine();
	rijndaelSpeed(128);
	rijndaelSpeed(192);
	rijndaelSpeed(256);

	/* End your application here. */

#ifdef TARGET_OS_WINDOWS
	system("pause");
#endif

	delete console;
	return 0;
}
