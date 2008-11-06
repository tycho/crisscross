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

#include <crisscross/universal_include.h>

#include <crisscross/debug.h>
#include <crisscross/core_io.h>
#include <crisscross/system.h>

using namespace CrissCross::System;

namespace CrissCross
{
	namespace IO
	{
		CoreIOReader::CoreIOReader(FILE * _fileBuffer, bool _isUnicode, LineEndingType _lnEnding) : m_fileInputPointer(_fileBuffer),
			m_unicode(_isUnicode)
		{
			SetLineEndings(_lnEnding);
		}

		CoreIOReader::~CoreIOReader()
		{
		}

		bool CoreIOReader::EndOfFile()
		{
			CoreAssert(this != NULL);

			if (!m_fileInputPointer)
				return true;

			return (feof(m_fileInputPointer) != 0);
		}

		void CoreIOReader::Flush()
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return;

#ifndef __GNUC__
			m_ioMutex.Lock();
#endif
			fflush(m_fileInputPointer);
#ifndef __GNUC__
			m_ioMutex.Unlock();
#endif
		}

		bool CoreIOReader::IsOpen()
		{
			CoreAssert(this != NULL);

			if (m_fileInputPointer == NULL)
				return false;
			else
				return true;
		}

		int CoreIOReader::Forward(cc_int64_t _position)
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return CC_ERR_INVALID_BUFFER;

			int res = Seek(_position, SEEK_CUR);
			return (res == 0);
		}

		cc_int64_t CoreIOReader::Position()
		{
			CoreAssert(this != NULL);
			CoreAssert(IsOpen());
#ifdef HAS_FPOS64
			fpos64_t lastpos;
#ifdef TARGET_OS_WINDOWS
			lastpos = _ftelli64(m_fileInputPointer);
			return lastpos;
#elif defined (TARGET_OS_MACOSX) || defined (TARGET_OS_NETBSD) || \
			defined (TARGET_OS_FREEBSD) || defined (TARGET_OS_OPENBSD)
			fgetpos(m_fileInputPointer, &lastpos);
			return lastpos;
#else
			fgetpos64(m_fileInputPointer, &lastpos);
			return lastpos.__pos;
#endif
#else
			fpos_t lastpos;
			lastpos = ftell(m_fileInputPointer);
			return lastpos;
#endif
		}

		cc_int64_t CoreIOReader::Length()
		{
			CoreAssert(this != NULL);
			CoreAssert(IsOpen());

#ifndef __GNUC__
			m_ioMutex.Lock();
#endif

#ifdef HAS_FPOS64
			fpos64_t lastpos, endpos;
#ifdef TARGET_OS_WINDOWS
			lastpos = _ftelli64(m_fileInputPointer);
			_fseeki64(m_fileInputPointer, 0, SEEK_END);
			endpos = _ftelli64(m_fileInputPointer);
			_fseeki64(m_fileInputPointer, lastpos, SEEK_SET);
#elif defined (TARGET_OS_MACOSX) || defined (TARGET_OS_NETBSD) || \
			defined (TARGET_OS_FREEBSD) || defined (TARGET_OS_OPENBSD)
			fgetpos(m_fileInputPointer, &lastpos);
			fseek(m_fileInputPointer, 0, SEEK_END);
			fgetpos(m_fileInputPointer, &endpos);
			fsetpos(m_fileInputPointer, &lastpos);
#else
			fgetpos64(m_fileInputPointer, &lastpos);
			fseeko64(m_fileInputPointer, 0, SEEK_END);
			fgetpos64(m_fileInputPointer, &endpos);
			fsetpos64(m_fileInputPointer, &lastpos);
#endif
#else
			fpos_t lastpos, endpos;
			lastpos = ftell(m_fileInputPointer);
			fseek(m_fileInputPointer, 0, SEEK_END);
			endpos = ftell(m_fileInputPointer);
			fseek(m_fileInputPointer, lastpos, SEEK_SET);
#endif
#ifndef __GNUC__
			m_ioMutex.Unlock();
#endif

#if defined (TARGET_OS_WINDOWS) || defined (TARGET_OS_MACOSX) || defined (TARGET_OS_FREEBSD) || \
			defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD) || defined (TARGET_COMPILER_CYGWIN) || \
			defined (TARGET_OS_NDSFIRMWARE)
			return endpos;
#elif defined (TARGET_OS_LINUX)
			return endpos.__pos;
#endif
		}

		int CoreIOReader::Read(char *_destination)
		{
			CoreAssert(this != NULL);
			CoreAssert(_destination != NULL);
			if (!IsOpen()) return CC_ERR_INVALID_BUFFER;

#ifndef __GNUC__
			m_ioMutex.Lock();
#endif
			*_destination = (char)fgetc(m_fileInputPointer);
#ifndef __GNUC__
			m_ioMutex.Unlock();
#endif
			return sizeof(char);
		}

		int CoreIOReader::Read(char *_buffer, size_t _bufferLength, size_t _bufferIndex, size_t _count)
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return CC_ERR_INVALID_BUFFER;

			size_t retval;

			CoreAssert(_buffer != NULL);
			CoreAssert(_bufferLength - _bufferIndex <= _count);
			CoreAssert(_count > 0);
#ifndef __GNUC__
			m_ioMutex.Lock();
#endif
			retval = fread(&_buffer[_bufferIndex], sizeof(char), _count, m_fileInputPointer);
			_buffer[_bufferIndex + retval] = '\x0';
#ifndef __GNUC__
			m_ioMutex.Unlock();
#endif
			return (int)retval;
		}

		int CoreIOReader::ReadLine(char *_buffer, size_t _bufferLength)
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return CC_ERR_INVALID_BUFFER;

#ifndef __GNUC__
			m_ioMutex.Lock();
#endif

			/* We use fgets because it detects line endings. */
			_buffer[0] = '\x0';
			fgets(_buffer, (int)_bufferLength, m_fileInputPointer);

			/* Detect line endings. */
			char *endl = NULL;
			char *cr = strchr(_buffer, '\r');
			char *lf = strchr(_buffer, '\n');
			char *crlf = strstr(_buffer, "\r\n");
			if (crlf) {
				SetLineEndings(CC_LN_CRLF); endl = crlf;
			} else if (cr) {
				SetLineEndings(CC_LN_CR); endl = cr;
			} else if (lf) {
				SetLineEndings(CC_LN_LF); endl = lf;
			}

			if (endl)
				*endl = '\x0';

#ifndef __GNUC__
			m_ioMutex.Unlock();
#endif

			return (int)strlen(_buffer);
		}

		int CoreIOReader::ReadLine(std::string &_string)
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return CC_ERR_INVALID_BUFFER;

#ifndef __GNUC__
			m_ioMutex.Lock();
#endif
			char               c = (char)fgetc(m_fileInputPointer);

			if (c == (char)EOF)
				return 0;

			std::string buffer;

			while (c != (char)EOF && c != '\n') {
				buffer += c;
				c = (char)fgetc(m_fileInputPointer);
			}

			int                len = (int)buffer.length();

			if (len && buffer[len - 1] == '\r')
				buffer.resize(len - 1);

#ifndef __GNUC__
			m_ioMutex.Unlock();
#endif

			_string = buffer;

			return (int)_string.length() * sizeof(char);
		}

		int CoreIOReader::Seek(cc_int64_t _position, int _origin)
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return CC_ERR_INVALID_BUFFER;

#ifndef __GNUC__
			m_ioMutex.Lock();
#endif
#ifdef HAS_FPOS64
#ifdef TARGET_OS_WINDOWS
			int res = _fseeki64(m_fileInputPointer, _position, _origin);
#elif defined (TARGET_OS_MACOSX) || defined (TARGET_OS_NETBSD) || \
			defined (TARGET_OS_FREEBSD) || defined (TARGET_OS_OPENBSD)
			int res = fseek(m_fileInputPointer, _position, _origin);
#else
			int res = fseeko64(m_fileInputPointer, _position, _origin);
#endif
#else
			int res = fseek(m_fileInputPointer, _position, _origin);
#endif
#ifndef __GNUC__
			m_ioMutex.Unlock();
#endif
			return res;
		}

		int CoreIOReader::Seek(cc_int64_t _position)
		{
			CoreAssert(this != NULL);
			if (!IsOpen()) return CC_ERR_INVALID_BUFFER;

			int res = Seek(_position, SEEK_SET);
			return (res == 0);
		}

		CrissCross::Errors CoreIOReader::SetLineEndings(LineEndingType _ending)
		{
			CoreAssert(this != NULL);

			if (_ending == CC_LN_NATIVE) {
#if defined (TARGET_OS_WINDOWS)
				_ending = CC_LN_CRLF;
#elif defined (TARGET_OS_LINUX) || defined (TARGET_OS_MACOSX) || defined (TARGET_OS_FREEBSD) || \
				defined (TARGET_OS_NETBSD) || defined (TARGET_OS_OPENBSD) || defined (TARGET_OS_NDSFIRMWARE)
				_ending = CC_LN_LF;
#else
#error You are not using a supported OS.
#endif
			}

			switch (_ending)
			{
			case CC_LN_CR:
				sprintf(m_lineEnding, "\r");
				break;
			case CC_LN_LF:
				sprintf(m_lineEnding, "\n");
				break;
			case CC_LN_CRLF:
				sprintf(m_lineEnding, "\r\n");
				break;
			default:
				return CC_ERR_BADPARAMETER;
			}
			return CC_ERR_NONE;
		}
	}
}
