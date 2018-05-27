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

#ifndef __included_cc_core_io_reader_h
#define __included_cc_core_io_reader_h

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
		/*! \brief The core input class. */
		/*!
		 *  A class inherited by most I/O classes, including Console and FileReader.
		 */
		class CoreIOReader
		{
			protected:
				/*! \brief Line ending buffer. */
				/*!
				 * Stores the line ending selected by CoreIOReader::SetLineEndings.
				 */
				char m_lineEnding[4];

				/*! \brief Input/output FILE pointer. */
				FILE *m_fileInputPointer;

				/*! \brief Indicates whether the buffer is to be read in unicode or not. (UNIMPLEMENTED) */
				bool m_unicode;

				/*! \brief Indicates the expected input endianness. */
				Endian m_endianness;

		#ifndef TARGET_COMPILER_GCC
				/*! \brief Thread-safe mutex. */
				/*!
				 * Prevents more than one read from occurring simultaneously.
				 */
				CrissCross::System::Mutex m_ioMutex;
		#endif

			public:
				/*! \brief The constructor. */
				/*!
				 * Creates a new CoreIOReader instance. Will initialize line endings to the platform's
				 * default, also initializes CoreIOReader::m_ioMutex.
				 * \param _inputBuffer The buffer to be used for read operations.
				 * \param _isUnicode The buffer is going to be a Unicode read buffer. (UNIMPLEMENTED)
				 * \param _lnEnding The line ending to use.
				 * \param _inputEndianness The expected input endianness. Will be converted to native.
				 */
				CoreIOReader(FILE * _inputBuffer, bool _isUnicode, LineEndingType _lnEnding = CC_LN_NATIVE,
					Endian _inputEndianness = CC_ENDIAN_NATIVE);

				/*! \brief The destructor. */
				virtual ~CoreIOReader();

				/*! \brief Determines whether the end of the file has been reached. */
				/*!
				 * \return Boolean indicating whether the end of the file has been reached.
				 */
				virtual bool EndOfFile();

				/*! \brief Determines the length of the file buffer. */
				/*!
				 * \return Indicates the length of the buffer in bytes.
				 */
				virtual int64_t Length();

				/*! \brief Validates that the file buffer isn't NULL. */
				/*!
				 * \return Boolean indicating whether the file is safe to read from.
				 */
				virtual bool IsOpen();

				/*! \brief Reads a block of data from the file buffer. */
				/*!
				 * \param _buffer The output buffer to read to.
				 * \param _count The number of bytes to read.
				 * \return The actual number of bytes read.
				 */
				virtual int ReadBlock(void *_buffer, size_t _count);

				/*! \brief Read an unsigned 8-bit integer. */
				virtual int ReadU8(uint8_t *_buffer);

				/*! \brief Read an unsigned 16-bit integer. */
				virtual int ReadU16(uint16_t *_buffer);

				/*! \brief Read an unsigned 32-bit integer. */
				virtual int ReadU32(uint32_t *_buffer);

				/*! \brief Read an unsigned 64-bit integer. */
				virtual int ReadU64(uint64_t *_buffer);

				/*! \brief Reads a line of data. */
				/*!
				 * Data returned by this function should be copied to another location before being parsed.
				 * \param _string A reference of an std::string where the data will be stored.
				 * \return The number of bytes read. On failure, -1 is returned.
				 */
				virtual int ReadLine(std::string &_string);

				/*! \brief Reads a line of data. */
				/*!
				 * \param _buffer A character buffer for the data to be stored in.
				 * \param _bufferLength The length of the buffer specified in _buffer.
				 * \return The number of bytes read. On failure, -1 is returned.
				 */
				virtual int ReadLine(char *_buffer, size_t _bufferLength);

				/*! \brief Sets the line ending convention used by this CoreIOReader instance. */
				/*!
				 * \param _ending Any of the LineEndingType values.
				 */
				virtual CrissCross::Errors SetLineEndings(LineEndingType _ending);

				/*! \brief Gets the current position in the buffer. */
				/*!
				 * \return The current position in the buffer.
				 */
				virtual int64_t Position();

				/*! \brief Seeks to a location in the buffer. */
				/*!
				 * \param _position Position to seek to, relative to the first byte of the buffer.
				 * \return An integer indicating the result of the operation. 0 indicates
				 * success. Any non-zero number indicates failure.
				 */
				virtual int Seek(int64_t _position);

				/*! \brief Moves forward to a location in the buffer. */
				/*!
				 * \param _position Position to seek to, relative to the current position in the buffer.
				 * \return An integer indicating the result of the operation. 0 indicates
				 * success. Any non-zero number indicates failure.
				 */
				virtual int Forward(int64_t _position);

				/*! \brief Sets which endianness to convert from when reading. */
				/*!
				 * \param _inputEndianness The endianness to expect when reading. Will convert
				 *		from this endianness to the native endianness when using the size-specific reads.
				 */
				virtual void SetEndian(Endian _inputEndianness);

				/*! \brief Flushes the input buffer. */
				void Flush();

			protected:
				/*! \brief Seeks to a location in the buffer. */
				/*!
				 * \param _position Position to seek to, relative to _origin.
				 * \param _origin Can be one of SEEK_SET, SEEK_CUR, or SEEK_END.
				 * \return An integer indicating the result of the operation. 0 indicates
				 * success. Any non-zero number indicates failure.
				 */
				int Seek(int64_t _position, int _origin);
		};
	}
}
#endif
