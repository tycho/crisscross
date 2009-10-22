/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2009 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#include "header.h"

using namespace CrissCross::IO;
using namespace CrissCross::System;
using namespace std;

int main(int argc, char * *argv)
{
	Console *console = new Console();

	/* Begin your application here. */

#ifdef ENABLE_CPUID

	CPUID *cpuid = new CPUID();

	console->SetColour(console->FG_RED | console->FG_INTENSITY);
	console->WriteLine("======================");
	console->WriteLine("= CPU IDENTIFICATION =");
	console->WriteLine("======================");
	console->SetColour(0);
	console->WriteLine();

	cpuid->Go();

	/* NOTES */
	/* The Virtual count is the number of processors that the operating system sees, */
	/* but does not take into account whether any of the processors counted are */
	/* hyperthreads, cores, or truly physical CPUs. */
	/* */
	/* If the Physical and Logical counts are equal, the number of Physical/Logical is the */
	/* core count, because it's a multi-core system. */
	/* */
	/* If a count of the processor packages is needed, simply divide VirtualCount */
	/* by LogicalPerPackage to get the number.  This may fail in some situations where */
	/* the OS or a HyperVisor (VMWare or HyperV) limit the number of processors the OS */
	/* sees - the CPU hardware will still report the total number of cores it has, but */
	/* the underlying software will not have access to it. */

	console->WriteLine("There are %d processors in the system (%d cores per package, %d logical per package).",
	                   cpuid->VirtualCount(),
	                   cpuid->CoresPerPackage(),
	                   cpuid->LogicalPerPackage());

	if (cpuid->VirtualCount() > 1 || cpuid->CoresPerPackage() > 1 || cpuid->LogicalPerPackage() > 1) {
		if (cpuid->CoresPerPackage() == cpuid->LogicalPerPackage())
			console->WriteLine("This is a multi-core system.");
		else if (cpuid->CoresPerPackage() > 1 && cpuid->LogicalPerPackage() > cpuid->CoresPerPackage())
			console->WriteLine("This is a hyperthreaded multi-core system.");
		else if (cpuid->CoresPerPackage() == 1 && cpuid->LogicalPerPackage() > 1)
			console->WriteLine("This is a hyperthreaded system.");

		if (cpuid->VirtualCount() > cpuid->LogicalPerPackage())
			console->WriteLine("This is a multi-processor system.");
	} else {
		console->WriteLine("This is a single processor system.");
	}

	/*
	 * v32 l8 = 32/8 = 4
	 * v4 l2 = 4/2 = 2
	 * v2 l2 = 2/2 = 1
	 * v1 l2 = 1/2 = 0.5
	 */
	double ratio = (double)cpuid->VirtualCount() / (double)cpuid->LogicalPerPackage();
	if ((int)ratio < 1) {
		console->WriteLine();
		console->SetColour(console->FG_YELLOW | console->FG_INTENSITY);
		console->WriteLine("WARNING: Expected at least 1 physical package, but detected %0.2lf. Are you\n"
		                   "running this under a hypervisor?", ratio);
		console->SetColour();
	}

	console->WriteLine();

	for (int i = 0; i < MAX_PROCESSORS; i++) {
		/* Go through each Virtual processor. */
		if (cpuid->proc[i])
			cpuid->proc[i]->Print(console);
	}

	delete cpuid;

#else

	console->WriteLine("CPUID isn't supported on your platform or compiler.");

#endif

	/* End your application here. */

#ifdef TARGET_OS_WINDOWS
	system("pause");
#endif

	delete console;
	return 0;
}
