/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2022 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_error_h
#define __included_cc_error_h

namespace CrissCross
{
	/*! \brief The error types. */
	/*!
	 *  These errors are used throughout the entire project.
	 *  If you get an error while calling a function, find that
	 *  error number in this list.
	 */
	enum Errors
	{
		CC_ERR_BADPARAMETER = -9,                  /*!< \brief One of the parameters passed to the function was not valid. */
		CC_ERR_NOT_IMPLEMENTED = -8,
		CC_ERR_FILE_OPEN = -7,                     /*!< \brief The specified file could not be opened. */
		CC_ERR_READ = -6,                          /*!< \brief The buffer could not be written to. */
		CC_ERR_WRITE = -5,                         /*!< \brief The buffer could not be written to. */
		CC_ERR_INTERNAL = -4,                      /*!< \brief A call inside the function failed. */
		CC_ERR_INVALID_CALL = -3,                  /*!< \brief The function call was invalid. */
		CC_ERR_INVALID_BUFFER = -2,                /*!< \brief The buffer was nullptr (file not opened?) */
		CC_ERR_INCOMPATIBLE_BUFFER = -1,           /*!< \brief The buffer was incompatible (i.e. reading unicode data on a non-unicode buffer). */
		CC_ERR_NONE = 0                            /*!< \brief Everything is fine, nothing is ruined. */
	};

	/*! \brief Converts an errno number to a CrissCross::Errors value. */
	int TranslateError(int why);

	/*! \brief Gets the name of the CrissCross::Errors value. */
	const char *GetErrorDescription(int why);
}

#endif
