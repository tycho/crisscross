/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2018 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_version_h
#define __included_cc_version_h

namespace CrissCross
{
	namespace Version
	{
		//! The short version string.
		/*!
		 * This version will generally match the last version tag. For instance,
		 * if the last tag of CrissCross before this build was '0.1.2', you'd see
		 * '0.1.2'. This version number does not include some rather important
		 * extra information useful for getting the exact revision (the Git commit
		 * hash and the number of revisions since the tag). For that extra information,
		 * use CrissCross::Version::Long() instead.
		 *
		 * For extracting individual components of the version, you should use the
		 * Major(), Minor(), Revision() and Build() functions.
		 *
		 * \return The version number of the CrissCross library.
		 */
		const char *Short();

		//! The long version string.
		/*!
		 * This string contains detailed version information about the CrissCross
		 * build in use. The string will always start with the Git tag that this
		 * build descended from. If this build is not an exact match for a given
		 * tag, this string will also include the number of commits since the tag
		 * and the Git commit id (the SHA-1 hash).
		 *
		 * \return The version number of the CrissCross library.
		 */
		const char *Long();

		//! The major version number.
		/*!
		 * This is the first number to appear in a version tag. For instance,
		 * if the tag is '0.1.2', this function would return '0'.
		 * \return The major version number.
		 */
		int Major();

		//! The minor version number.
		/*!
		 * This is the second number to appear in a version tag. For instance,
		 * if the tag is '0.1.2', this function would return '1'.
		 * \return The minor version number.
		 */
		int Minor();

		//! The revision number.
		/*!
		 * This is the third number to appear in a version tag. For instance,
		 * if the tag is '0.1.2', this function would return '2'.
		 * \return The revision version number.
		 */
		int Revision();

		//! The build number.
		/*!
		 * \warning The build number isn't the most reliable number to submit
		 *	with a bug report. Considering how Git works, you can have several
		 *	build XYZs that have completely different histories. Besides,
		 *	this number will reset to zero on every tag. This function
		 *	is essentially here just to allow you an easy way to format
		 *	the version string in a way that is appealing to you.
		 * \return The number of commits since the last Git tag.
		 */
		int Build();
	}
}

#endif
