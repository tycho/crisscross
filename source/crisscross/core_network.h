/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2008 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_core_network_h
#define __included_cc_core_network_h

#include <crisscross/error.h>

namespace CrissCross
{
	namespace Network
	{
		/* Undocumented for a reason. Don't call them. */
		/*! @cond */
		CrissCross::Errors __initialise_network();
		CrissCross::Errors __cleanup_network();
		/*! @endcond */
	}
}

#endif
