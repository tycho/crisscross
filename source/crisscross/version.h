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

#ifndef __included_cc_version_h
#define __included_cc_version_h

namespace CrissCross
{
	namespace Version
	{
		/*!
		 * \return A C string containing the version number of the CrissCross library.
		 */
		const char *Version();

		/*!
		 * \return The major version number (i.e. in v1.2.3.4, it would be '1').
		 */
		int Major();

		/*!
		 * \return The minor version number (i.e. in v1.2.3.4, it would be '2').
		 */
		int Minor();

		/*!
		 * \return The revision version number (i.e. in v1.2.3.4, it would be '3').
		 */
		int Revision();

		/*!
		 * \return The build number (i.e. in v1.2.3.4, it would be '4').
		 */
		int Build();
	}
}

#endif
