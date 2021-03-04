/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2020 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_core_io_writer_h
#define __included_cc_core_io_writer_h

#include <cstdio>
#include <string>

#include <crisscross/datatypes.h>
#include <crisscross/error.h>
#include <crisscross/mutex.h>
#include <crisscross/core_io.h>

namespace CrissCross
{
	namespace IO
	{
		/*! \brief The core output class. */
		/*!
		 *  A class inherited by most I/O classes, including Console and FileWriter.
		 */
		class CoreIOWriter
		{
			protected:

				/*! \brief Line ending buffer. */
				/*!
				 * Stores the line ending selected by SetLineEndings.
				 */
				char m_lineEnding[4];

				/*! \brief Input/output FILE pointer. */
				FILE *m_fileOutputPointer;

				/*! \brief Indicates whether the buffer is to be written in unicode or not. (UNIMPLEMENTED) */
				bool m_unicode;

				/*! \brief Indicates the expected input endianness. */
				Endian m_endianness;

		#ifndef TARGET_COMPILER_GCC
				/*! \brief Thread-safe mutex. */
				/*!
				 * Prevents more than one read or write from occurring simultaneously.
				 */
				CrissCross::System::Mutex m_ioMutex;
		#endif

			public:
				/*! \brief The constructor. */
				/*!
				 * Creates a new CoreIOWriter instance. Will initialize line endings to the platform's
				 * default, also initializes CoreIOWriter::m_ioMutex.
				 * \param _outputBuffer The buffer to be used for write operations.
				 * \param _isUnicode The buffer is going to be a Unicode I/O buffer. (UNIMPLEMENTED)
				 * \param _lnEnding The line ending to use.
				 * \param _outputEndianness The endianness to write with.
				 */
				CoreIOWriter(FILE * _outputBuffer, bool _isUnicode, LineEndingType _lnEnding = CC_LN_NATIVE,
					Endian _outputEndianness = CC_ENDIAN_NATIVE);

				/*! \brief The destructor. */
				virtual ~CoreIOWriter();

				/*! \brief Validates that the file buffer isn't NULL. */
				/*!
				 * \return Boolean indicating whether the file is safe to write to.
				 */
				virtual bool IsOpen();

				/*! \brief Sets the line ending convention used by this CoreIOWriter instance. */
				/*!
				 * \param _ending Any of the LineEndingType values.
				 */
				virtual CrissCross::Errors SetLineEndings(LineEndingType _ending);

				/*! \brief Sets which endianness to convert from when reading. */
				/*!
				 * \param _inputEndianness The endianness to expect when reading. Will convert
				 *		from this endianness to the native endianness when using the size-specific reads.
				 */
				virtual void SetEndian(Endian _inputEndianness);

				/*! \brief Write a block of data to the file buffer. */
				/*!
				 * \param _buffer The buffer to write.
				 * \param _count The number of bytes to write.
				 * \return The actual number of bytes written.
				 */
				virtual size_t WriteBlock(const void *_buffer, size_t _count);

				/*! \brief Write an unsigned 8-bit integer. */
				virtual size_t WriteU8(uint8_t _data);

				/*! \brief Write an unsigned 16-bit integer. */
				virtual size_t WriteU16(uint16_t _data);

				/*! \brief Write an unsigned 32-bit integer. */
				virtual size_t WriteU32(uint32_t _data);

				/*! \brief Write an unsigned 64-bit integer. */
				virtual size_t WriteU64(uint64_t _data);

				/*! \brief Writes a string to the buffer. */
				/*!
				 * \param _format The format of the string to be written.
				 */
				virtual CrissCross::Errors Write(const char *_format, ...);

				/*! \brief Writes a string to the buffer. */
				/*!
				 * \param _string The string to be written.
				 */
				virtual CrissCross::Errors Write(std::string &_string);

				/*! \brief Writes an empty line to the buffer. */
				/*!
				 * Writes CoreIOWriter::m_lineEnding to the buffer.
				 */
				virtual CrissCross::Errors WriteLine();

				/*! \brief Writes a string to the buffer with a newline appended. */
				/*!
				 * Prints the string to the buffer, and then prints the line terminator in the
				 * format specified by CoreIOWriter::m_lineEnding.
				 * \param _format The format of the string to be written.
				 * \return Standard CrissCross::Errors values.
				 */
				virtual CrissCross::Errors WriteLine(const char *_format, ...);

				/*! \brief Writes a string to the buffer with a newline appended. */
				/*!
				 * Prints the string to the buffer, and then prints the line terminator in the
				 * format specified by CoreIOWriter::m_lineEnding.
				 * \param _string The std::string to write.
				 * \return Standard CrissCross::Errors values.
				 */
				virtual CrissCross::Errors WriteLine(std::string &_string);

				/*! \brief Flushes the output buffer. */
				void Flush();
		};
	}
}
#endif
