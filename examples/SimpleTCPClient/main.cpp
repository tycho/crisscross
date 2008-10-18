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

using namespace CrissCross;
using namespace CrissCross::IO;
using namespace CrissCross::Network;
using namespace std;

Console *console = NULL;

int RunApplication(int argc, char * *argv)
{
	console = new Console();

	/* Begin your application here. */
	int         cc_err;

	TCPSocket * sock = new TCPSocket();
	console->WriteLine("Connecting...");
	cc_err = sock->Connect("www.example.com", 80);
	if (cc_err == CC_ERR_WOULD_BLOCK)
		console->WriteLine("Non-blocking sockets are enabled. Waiting for connection...");
	while (sock->State() == SOCKET_STATE_CONNECTING) ;
	if (sock->State() != SOCKET_STATE_CONNECTED) {
		console->WriteLine("Connection failed.");
		return 1;
	} else
		console->WriteLine("Connection seems OK.");
	console->Write("Requesting http://www.example.com/... ");
	cc_err = sock->Send("GET / HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
	if (cc_err < 1)
		console->WriteLine("Failed.");
	else
		console->WriteLine("OK");

	while (sock->State() == SOCKET_STATE_CONNECTED) {
		string in;
		cc_err = sock->Read(in);
		if (cc_err == 0) {
			console->WriteLine(in);
			break;
		} else {
			if (cc_err != CC_ERR_WOULD_BLOCK)
				console->WriteLine("Read error %d (%s)", cc_err, GetErrorDescription(cc_err));
			System::ThreadSleep(10);
		}
	}

	sock->Close();

	delete sock;

	return 0;
	/* End your application here. */

#ifdef TARGET_OS_WINDOWS
	system("pause");
#endif

	delete console;
	return 0;
}
