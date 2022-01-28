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

#include <crisscross/universal_include.h>

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <crisscross/debug.h>
#include <crisscross/endian.h>
#include <crisscross/core_io.h>
#include <crisscross/system.h>

using namespace CrissCross::System;

namespace CrissCross
{
	namespace IO
	{
		CoreIOWriter::CoreIOWriter(FILE *_fileBuffer, LineEnding _lnEnding, Endian _outputEndianness)
			: m_fileOutputPointer(_fileBuffer)
		{
			SetLineEndings(_lnEnding);
			SetEndian(_outputEndianness);
		}

		CoreIOWriter::~CoreIOWriter()
		{
		}

		void CoreIOWriter::Flush()
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);
			if (!IsOpen())
				return;
			fflush(m_fileOutputPointer);
#ifdef TARGET_OS_NDSFIRMWARE
			swiWaitForVBlank();
#endif
		}

		bool CoreIOWriter::IsOpen()
		{
			if (m_fileOutputPointer == nullptr)
				return false;
			else
				return true;
		}

		CrissCross::Errors CoreIOWriter::SetLineEndings(LineEnding _ending)
		{
			if (_ending == LineEnding::Native) {
#if defined(TARGET_OS_WINDOWS)
				_ending = LineEnding::CRLF;
#else
				_ending = LineEnding::LF;
#endif
			}

			switch (_ending) {
			case LineEnding::CR:
				m_lineEnding = "\r";
				break;
			case LineEnding::LF:
				m_lineEnding = "\n";
				break;
			case LineEnding::CRLF:
				m_lineEnding = "\r\n";
				break;
			default:
				return CC_ERR_BADPARAMETER;
			}
			return CC_ERR_NONE;
		}

		void CoreIOWriter::SetEndian(Endian _inputEndianness)
		{
			m_endianness = _inputEndianness;
		}

		CrissCross::Errors CoreIOWriter::WriteLine(const char *_format, ...)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;

			if (_format == nullptr)
				return CC_ERR_BADPARAMETER;

			va_list args;
			int err;

			/* Print out the string */
			va_start(args, _format);
			err = vfprintf(m_fileOutputPointer, _format, args);
			va_end(args);

			if (err < 0)
				return CC_ERR_WRITE;

			if (fputs(m_lineEnding, m_fileOutputPointer) < 0)
				return CC_ERR_WRITE;

			Flush();

			return CC_ERR_NONE;
		}

		CrissCross::Errors CoreIOWriter::WriteLine(std::string &_string)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;

			if (_string.empty() == true)
				return CC_ERR_BADPARAMETER;

			if (fputs(_string.c_str(), m_fileOutputPointer) < 0)
				return CC_ERR_WRITE;

			if (fputs(m_lineEnding, m_fileOutputPointer) < 0)
				return CC_ERR_WRITE;

			Flush();

			return CC_ERR_NONE;
		}

		CrissCross::Errors CoreIOWriter::Write(std::string &_string)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;

			if (_string.empty() == true)
				return CC_ERR_BADPARAMETER;

			if (fputs(_string.c_str(), m_fileOutputPointer) < 0)
				return CC_ERR_WRITE;

			return CC_ERR_NONE;
		}

		CrissCross::Errors CoreIOWriter::WriteLine()
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;

			if (fputs(m_lineEnding, m_fileOutputPointer))
				return CC_ERR_WRITE;

			return CC_ERR_NONE;
		}

		CrissCross::Errors CoreIOWriter::Write(const char *_format, ...)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return CC_ERR_INVALID_BUFFER;

			if (_format == nullptr)
				return CC_ERR_BADPARAMETER;

			va_list args;
			int err;

			/* Print out the string */
			va_start(args, _format);
			err = vfprintf(m_fileOutputPointer, _format, args);
			va_end(args);

			fflush(m_fileOutputPointer);

			if (err < 0)
				return CC_ERR_WRITE;

			return CC_ERR_NONE;
		}

		int CoreIOWriter::WriteBlock(const void *_buffer, size_t _count)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return -1;

			if (_buffer == nullptr)
				return -1;

			return fwrite(_buffer, _count, 1, m_fileOutputPointer);
		}

		int CoreIOWriter::WriteU8(uint8_t _data)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return 0;
			return fwrite(&_data, sizeof(uint8_t), 1, m_fileOutputPointer);
		}

		int CoreIOWriter::WriteU16(uint16_t _data)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return -1;

			switch (m_endianness) {
			case Endian::Little:
				_data = CC_SwapBE16(_data);
				break;
			case Endian::Big:
				_data = CC_SwapLE16(_data);
				break;
			case Endian::Native:
				/* Do nothing */
				break;
			}

			return fwrite(&_data, sizeof(uint16_t), 1, m_fileOutputPointer);
		}

		int CoreIOWriter::WriteU32(uint32_t _data)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return -1;

			switch (m_endianness) {
			case Endian::Little:
				_data = CC_SwapBE32(_data);
				break;
			case Endian::Big:
				_data = CC_SwapLE32(_data);
				break;
			case Endian::Native:
				/* Do nothing */
				break;
			}

			return fwrite(&_data, sizeof(uint32_t), 1, m_fileOutputPointer);
		}

		int CoreIOWriter::WriteU64(uint64_t _data)
		{
			std::lock_guard<std::recursive_mutex> lock(m_ioMutex);

			if (!IsOpen())
				return -1;

			switch (m_endianness) {
			case Endian::Little:
				_data = CC_SwapBE64(_data);
				break;
			case Endian::Big:
				_data = CC_SwapLE64(_data);
				break;
			case Endian::Native:
				/* Do nothing */
				break;
			}

			return fwrite(&_data, sizeof(uint64_t), 1, m_fileOutputPointer);
		}
	}
}
