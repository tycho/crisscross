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

#include <cstdio>
#include <cstring>

#include <crisscross/debug.h>
#include <crisscross/filereader.h>

namespace CrissCross
{
	namespace IO
	{
		FileReader::FileReader() : CoreIOReader(nullptr, false), m_filePath(nullptr)
		{
		}

		FileReader::~FileReader()
		{
			Close();
		}

		CrissCross::Errors FileReader::Open(const char *_file)
		{

			Close();

			int _filePathLength = 0;

			if (_file == nullptr)
				return CC_ERR_BADPARAMETER;

			if ((_filePathLength = (int)strlen(_file)) < 1)
				return CC_ERR_BADPARAMETER;

			delete [] (char *)m_filePath;
			m_filePath = new char[_filePathLength + 1];
			strcpy((char *)m_filePath, _file);

			m_fileInputPointer = fopen(m_filePath, "rb");

			if (m_fileInputPointer == nullptr)
				return CC_ERR_FILE_OPEN;
			else
				return CC_ERR_NONE;
		}

		CrissCross::Errors FileReader::Close()
		{

			if (m_fileInputPointer)
				fclose(m_fileInputPointer);

			m_fileInputPointer = nullptr;

			delete [] (char *)m_filePath;
			m_filePath = nullptr;

			return CC_ERR_NONE;
		}
	}
}
