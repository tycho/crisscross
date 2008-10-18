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

#ifndef __included_cc_console_h
#define __included_cc_console_h

#include <crisscross/core_io.h>

namespace CrissCross
{
	namespace IO
	{
		/*! \brief The core console input/output class. */
		class Console : public CoreIOWriter, public CoreIOReader
		{
			protected:
				bool  m_consoleAllocated;
#if defined (TARGET_OS_MACOSX) || defined (TARGET_OS_FREEBSD) || \
				defined (TARGET_OS_OPENBSD) || defined (TARGET_OS_NETBSD)
				int   m_ptyfd;
				int   m_ttyfd;
				char  m_slaveName[32];
				pid_t m_childPID;
#endif
				bool AllocateConsole();
			public:

				/*! \brief Flags used for describing console colour output. */
				typedef enum
				{
					FG_BLUE = 0x0001,           /*! \brief< Blue Foreground */
					FG_GREEN = 0x0002,          /*! \brief< Green Foreground */
					FG_RED = 0x0004,            /*! \brief< Red Foreground */
					FG_INTENSITY = 0x0008,      /*! \brief< Foreground intensity (makes the foreground colour a shade brighter) */
					BG_BLUE = 0x0010,           /*! \brief< Blue Background */
					BG_GREEN = 0x0020,          /*! \brief< Green Background */
					BG_RED = 0x0040,            /*! \brief< Red Background */
					BG_INTENSITY = 0x0080,      /*! \brief< Background intensity (makes the foreground colour a shade brighter) */
		#if !defined (ANSI_COLOUR) && defined (TARGET_OS_WINDOWS)
					FG_BROWN = 0x0000,                                  /*! \brief< Brown Foreground (POSIX only) */
					FG_MAGENTA = FG_BLUE | FG_RED,                      /*! \brief< Magenta Foreground */
					FG_CYAN = FG_BLUE | FG_GREEN,                       /*! \brief< Cyan Foreground */
					FG_GRAY = FG_BLUE | FG_GREEN | FG_RED,              /*! \brief< Gray Foreground */
					FG_WHITE = FG_BLUE | FG_GREEN | FG_RED | FG_INTENSITY, /*! \brief< White Foreground */
					FG_YELLOW = FG_GREEN | FG_RED,                      /*! \brief< Yellow Foreground */
					BG_BROWN = 0x0000,                                  /*! \brief< Brown Background (POSIX only) */
					BG_MAGENTA = BG_BLUE | BG_RED,                      /*! \brief< Magenta Background */
					BG_CYAN = BG_GREEN | BG_BLUE,                       /*! \brief< Cyan Background */
					BG_GRAY = BG_BLUE | BG_GREEN | BG_RED,              /*! \brief< Gray Background */
					BG_WHITE = BG_BLUE | BG_GREEN | BG_RED | BG_INTENSITY, /*! \brief< White Background */
					BG_YELLOW = BG_GREEN | BG_RED                       /*! \brief< Yellow Background */
		#elif defined (ANSI_COLOUR)
					FG_BROWN = 0x0100,      /*! \brief< Brown Foreground (POSIX only) */
					FG_MAGENTA = 0x0200,    /*! \brief< Magenta Foreground */
					FG_CYAN = 0x0400,       /*! \brief< Cyan Foreground */
					FG_GRAY = 0x0800,       /*! \brief< Gray Foreground */
					FG_WHITE = 0x1000,      /*! \brief< White Foreground */
					FG_YELLOW = FG_BROWN | BG_INTENSITY, /*! \brief< Yellow Foreground */
					BG_BROWN = 0x4000,      /*! \brief< Brown Background (POSIX only) */
					BG_MAGENTA = 0x8000,    /*! \brief< Magenta Background */
					BG_CYAN = 0x10000,      /*! \brief< Cyan Background */
					BG_GRAY = 0x20000,      /*! \brief< Gray Background */
					BG_WHITE = 0x40000,     /*! \brief< White Background */
					BG_YELLOW = BG_BROWN | FG_INTENSITY /*! \brief< Yellow Background */
		#endif
				} ColourTypes;

			public:

				/*! \brief The default constructor. */
				/*!
				 * Allocates a new console for stdout and stderr output.
				 * \param _clearOnInit If true, clears the output console if supported.
				 * \param _fillScreen This moves the console window to the extreme left and switches to the maximum possible window height.
				 */
				Console(bool _clearOnInit = false, bool _fillScreen = false);

				/*! \brief The alternate constructor */
				/*!
				 * Does not allocate a new console, and instead uses the specified FILE* parameters for input/output.
				 * \param _outputBuffer The buffer for Console output.
				 * \param _inputBuffer The buffer for Console input.
				 * \sa Console()
				 */
				Console(FILE * _outputBuffer, FILE *_inputBuffer);

				/*! \brief The destructor. */
				~Console();

				/*! \brief Sets the console output colour to the default. */
				void SetColour();

				/*! \brief Sets the console output colour. */
				/*!
				 * Sets the console output colour using the flags specified in _flags.
				 * \param _flags A bitmask created by OR-ing Console::ColourTypes flags.
				 */
				void SetColour(int _flags);

				/*! \brief Sets the title of the terminal window. */
				/*!
				 * Sets the console window title using the value in the _title parameter.
				 * \param _title The title requested for the console window.
				 */
				void SetTitle(const char *_title);

				/*! \brief Sets the title of the terminal window. */
				/*!
				 * Sets the console window title using the value in the _title parameter.
				 * \param _title The title requested for the console window.
				 */
				void SetTitle(std::string &_title);

				/*! \brief Clears the console. */
				/*!
				 * Clears the console output (similar to commands 'cls' on Windows and 'clear' on Linux).
				 */
				void Clear();

				/*! \brief Move the cursor up a given number of lines. */
				/*!
				 * Permits you to overwrite previous lines. Good for a status display.
				 * \param _lines Number of lines to move the cursor
				 */
				void MoveUp(int _lines);

				/*! \brief Reads a char from the console. */
				char ReadChar();

				/*! \brief Reads a char from the console. */
				/*!
				 *  If both _min and _max are zero, the input will be returned, no matter what the value is (no bounds checking).
				 * \param _min The minimum input value.
				 * \param _max The maximum input value.
				 */
				char ReadChar(char _min, char _max);

				/*! \brief Reads an integer from the console. */
				int ReadInt();

				/*! \brief Reads an integer from the console. */
				/*!
				 *  If both _min and _max are zero, the input will be returned, no matter what the value is (no bounds checking).
				 * \param _min The minimum input value.
				 * \param _max The maximum input value.
				 */
				int ReadInt(int _min = 0, int _max = 0);

				/*! \brief Reads a long integer from the console. */
				long ReadLong();

				/*! \brief Reads a long integer from the console. */
				/*!
				 *  If both _min and _max are zero, the input will be returned, no matter what the value is (no bounds checking).
				 * \param _min The minimum input value.
				 * \param _max The maximum input value.
				 */
				long ReadLong(long _min = 0, long _max = 0);

				/*! \brief Reads a float from the console. */
				float ReadFloat();

				/*! \brief Reads a float from the console. */
				/*!
				 *  If both _min and _max are zero, the input will be returned, no matter what the value is (no bounds checking).
				 * \param _min The minimum input value.
				 * \param _max The maximum input value.
				 */
				float ReadFloat(float _min = 0, float _max = 0);

				/*! \brief Reads a double from the console. */
				double ReadDouble();

				/*! \brief Reads a double from the console. */
				/*!
				 *  If both _min and _max are zero, the input will be returned, no matter what the value is (no bounds checking).
				 * \param _min The minimum input value.
				 * \param _max The maximum input value.
				 */
				double ReadDouble(double _min = 0, double _max = 0);

				/*! \brief Flushes the input and output buffers. */
				void Flush();

			private:
				bool EndOfFile();
				int Forward(cc_int64_t _position);
				int Seek(cc_int64_t _position);
				cc_int64_t Length();
				int Read(char *_buffer, size_t _bufferLength, size_t _bufferIndex, size_t _count);
		};
	}
}

#endif
