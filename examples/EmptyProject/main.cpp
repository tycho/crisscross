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

using namespace CrissCross::IO;
using namespace std;

Console *console = NULL;

int RunApplication(int argc, char * *argv)
{
	console = new Console();

	/* Begin your application here. */


	/* End your application here. */

#ifdef TARGET_OS_WINDOWS
	system("pause");
#endif

	delete console;
	return 0;
}
