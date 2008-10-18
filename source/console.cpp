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
#include <crisscross/console.h>

#ifndef TARGET_OS_WINDOWS
#ifndef TARGET_OS_NDSFIRMWARE
#include <sys/ioctl.h>
#endif
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#endif

namespace CrissCross
{
	namespace IO
	{
		Console::Console(bool _clearOnInit, bool _fillScreen) : CoreIOWriter(stdout, false, CC_LN_LF),
			CoreIOReader(stdin, false, CC_LN_LF),
			m_consoleAllocated(false)
		{
			AllocateConsole();
#ifdef TARGET_OS_WINDOWS
			if (m_consoleAllocated) {
				/* Redirect stdout to the console. */
				int   hCrt = _open_osfhandle(( intptr_t )GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
				FILE *hf = _fdopen(hCrt, "w");

				*stdout = *hf;
				int   i = setvbuf(stdout, NULL, _IONBF, 0);

				/* Redirect stderr to the console. */
				hCrt = _open_osfhandle(( intptr_t )GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
				hf = _fdopen(hCrt, "w");
				*stderr = *hf;
				i = setvbuf(stdout, NULL, _IONBF, 0);

				if (_fillScreen) {
					char   findWindowFlag[64];
					sprintf(findWindowFlag, "%s%08X", CC_LIB_NAME, (unsigned long)this);
					RECT   rect; CONSOLE_SCREEN_BUFFER_INFO csbi;
					HWND   consoleWindowHandle = NULL;
					HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

					/* We need to know the console's maximum sizes. */
					GetConsoleScreenBufferInfo(consoleHandle, &csbi);

					/* Set us up for the maximums. */
					csbi.srWindow.Right = csbi.dwMaximumWindowSize.X - 1;
					csbi.srWindow.Bottom = csbi.dwMaximumWindowSize.Y - 1;
					csbi.srWindow.Top = 0;
					csbi.srWindow.Left = 0;

					/* Our console needs to fill the screen. It gets hairy after this. */
					SetConsoleWindowInfo(consoleHandle, TRUE, &csbi.srWindow);

					/* We have to cheat to find the window, unfortunately... */
					SetTitle(findWindowFlag);
					Sleep(1);

					/* Try a few times to find the window or else bail out. */
					for (int i = 0; i < 50 && !consoleWindowHandle; i++) {
						consoleWindowHandle = FindWindowA(NULL, findWindowFlag);
						Sleep(10);
					}

					if (consoleWindowHandle) {
						if (GetWindowRect(consoleWindowHandle, &rect))
							MoveWindow(consoleWindowHandle, 0, 0, rect.right - rect.left, rect.bottom - rect.top - 16, TRUE);

						/* Windows API is kind of clunky and requires a bit of kicking to get it to work. */
						/* In this case, we have to set the window to be topmost and then immediately after, not topmost, in order */
						/* to bring the window to the top of the Zorder. Believe me, other methods work -sparingly-. */
						SetWindowPos(consoleWindowHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
						SetWindowPos(consoleWindowHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
					}
				}
			}

#elif defined (TARGET_OS_MACOSX)
			if (m_consoleAllocated) {
				freopen(m_slaveName, "w", stdout);
				freopen(m_slaveName, "w", stderr);
			}

			if (_fillScreen) {
				/* Set position */
				Write("\033[3;0;0;t");

				/* Set size */
				Write("\033[8;60;150;t");
			}

#endif
			if (_clearOnInit) Clear();

			SetTitle(CC_LIB_NAME " " CC_LIB_VERSION);

#ifdef ENABLE_CREDITS
#if defined (TARGET_OS_NDSFIRMWARE)
			g_stdout->SetColour(g_stdout->FG_GREEN | g_stdout->FG_INTENSITY);
			g_stdout->WriteLine("Powered by " CC_LIB_NAME " v" CC_LIB_VERSION);
			g_stdout->SetColour(0);
			g_stdout->WriteLine(CC_LIB_NDS_COPYRIGHT);
			g_stdout->WriteLine();
#else
			g_stdout->SetColour(g_stdout->FG_GREEN | g_stdout->FG_INTENSITY);
			g_stdout->WriteLine("Powered by " CC_LIB_NAME " " CC_LIB_VERSION "\n    " CC_LIB_URL);
			g_stdout->SetColour(0);
			g_stdout->WriteLine(CC_LIB_COPYRIGHT);
			g_stdout->WriteLine();
#endif
#endif
		}

		Console::Console(FILE * _outputBuffer, FILE *_inputBuffer) : CoreIOWriter(_outputBuffer, false, CC_LN_LF),
			CoreIOReader(_inputBuffer, false, CC_LN_LF)
		{
		}

		Console::~Console()
		{
			SetColour(0);
#ifdef TARGET_OS_WINDOWS
			if (m_consoleAllocated) FreeConsole();

#elif defined (TARGET_OS_MACOSX)
			if (m_consoleAllocated) {
				kill(m_childPID, SIGTERM);
				wait(0);
			}

#endif
		}

		bool Console::AllocateConsole()
		{
#ifdef TARGET_OS_WINDOWS
			m_consoleAllocated = (AllocConsole() == TRUE);
#elif defined (TARGET_OS_MACOSX) && 0
			/* BSD-style pty code. */
			char        buf[64];
			const char *ptymajors = "pqrstuvwxyzabcdefghijklmnoABCDEFGHIJKLMNOPQRSTUVWXYZ";
			const char *ptyminors = "0123456789abcdef";
			int         num_minors = strlen(ptyminors);
			int         num_ptys = strlen(ptymajors) * num_minors;

			for (int i = 0; i < num_ptys; i++) {
				sprintf(buf, "/dev/pty%c%c", ptymajors[i / num_minors], ptyminors[i % num_minors]);
				m_ptyfd = open(buf, O_RDWR | O_NOCTTY);
				if (m_ptyfd < 0)
					continue;

				sprintf(m_slaveName, "/dev/tty%c%c", ptymajors[i / num_minors], ptyminors[i % num_minors]);

				/* Open the slave side. */
				m_ttyfd = open(m_slaveName, O_RDWR | O_NOCTTY);
				if (m_ttyfd < 0) {
					close(m_ptyfd);
					break;
				}

				m_consoleAllocated = true;
				break;
			}

			if (m_consoleAllocated) {
				m_childPID = fork();
				if (m_childPID == -1) {
					m_consoleAllocated = false;
					close(m_ttyfd);
					close(m_ptyfd);
					return m_consoleAllocated;
				}

				if (m_childPID == 0) {
					/* child */
					close(m_ttyfd);
					char        call[128];
					char       *pt = &m_slaveName[strlen(m_slaveName) - 1];
					const char *xterm = "/usr/X11/bin/xterm";
					sprintf(call, "-S%s/%d", pt, m_ptyfd);
					execl(xterm, xterm, "-bg", "black", "-fg", "white", "-geometry", "132x50", call, NULL);
					exit(0);
				} else {
					/* master */
					close(m_ptyfd);
				}
			}

#elif defined (TARGET_OS_NDSFIRMWARE)
			irqInit();
			irqEnable(IRQ_VBLANK);
			videoSetMode(MODE_0_2D);
			videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);
			vramSetBankC(VRAM_C_SUB_BG);
			SUB_BG0_CR = BG_MAP_BASE(31);
			BG_PALETTE_SUB[255] = RGB15(31, 31, 31);
			consoleInitDefault((u16 *)SCREEN_BASE_BLOCK_SUB(31), (u16 *)CHAR_BASE_BLOCK_SUB(0), 16);
			m_consoleAllocated = true;
#endif
			return m_consoleAllocated;
		}

		void Console::SetColour()
		{
			SetColour(0);
		}

		void Console::SetColour(int _flags)
		{
			CoreAssert(this != NULL);

#if !defined (ANSI_COLOUR) && defined (TARGET_OS_WINDOWS)
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

			if (_flags == 0)
				SetConsoleTextAttribute(hConsole, FG_GRAY);
			else
				SetConsoleTextAttribute(hConsole, _flags);

#elif defined (ANSI_COLOUR)
			/* Reset colours to defaults. */
			Write("\033[0m");

			if (_flags == 0)
				return;

			char codes[16];

			sprintf(codes, "\033[");

			if (_flags & FG_INTENSITY)
				strcat(codes, "1;");

			if (_flags & FG_RED)
				strcat(codes, "31;");

			if (_flags & FG_GREEN)
				strcat(codes, "32;");

			if (_flags & FG_BROWN)
				strcat(codes, "33;");

			if (_flags & FG_BLUE)
				strcat(codes, "34;");

			if (_flags & FG_MAGENTA)
				strcat(codes, "35;");

			if (_flags & FG_CYAN)
				strcat(codes, "36;");

			if (_flags & FG_GRAY)
				strcat(codes, "37;");

			if (_flags & FG_WHITE)
				strcat(codes, "39;");

			/*
			 * TODO: Determine if there is an ANSI code for background color intensity.
			 * if ( _flags & BG_INTENSITY )
			 * strcat ( codes, "???????" );
			 */
			if (_flags & BG_RED)
				strcat(codes, "41;");

			if (_flags & BG_GREEN)
				strcat(codes, "42;");

			if (_flags & BG_BROWN)
				strcat(codes, "43;");

			if (_flags & BG_BLUE)
				strcat(codes, "44;");

			if (_flags & BG_MAGENTA)
				strcat(codes, "45;");

			if (_flags & BG_CYAN)
				strcat(codes, "46;");

			if (_flags & BG_WHITE)
				strcat(codes, "47;");

			codes[strlen(codes) - 1] = 'm';

			Write("%s", codes);
#endif
		}

		void Console::SetTitle(const char *_title)
		{
#ifdef TARGET_OS_WINDOWS
			SetConsoleTitleA(_title);
#else
#if 0 /* Random bug just cropped up round this. */
			char buffer[4096];
			sprintf(buffer, "\033]2;%s\007", _title);
			Write(buffer);
#endif
#endif
		}

		void Console::SetTitle(std::string &_title)
		{
			SetTitle(_title.c_str());
		}

		void Console::Clear()
		{
			CoreAssert(this != NULL);

#if defined (TARGET_OS_WINDOWS)
			COORD                      coordScreen = { 0, 0 };
			DWORD                      cCharsWritten;
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			DWORD                      dwConSize;
			HANDLE                     hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

			GetConsoleScreenBufferInfo(hConsole, &csbi);
			dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
			FillConsoleOutputCharacter(hConsole, TEXT(' '), dwConSize,
			                           coordScreen, &cCharsWritten);
			GetConsoleScreenBufferInfo(hConsole, &csbi);
			FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize,
			                           coordScreen, &cCharsWritten);
			SetConsoleCursorPosition(hConsole, coordScreen);
#elif defined (TARGET_OS_MACOSX) || defined (TARGET_OS_LINUX) || \
			defined (TARGET_OS_NDSFIRMWARE)
			Write("%s", "\033[2J");
#endif
		}

		void Console::Flush()
		{
			CoreIOReader::Flush();
			CoreIOWriter::Flush();
		}

		void Console::MoveUp(int _lines)
		{
#if defined (TARGET_OS_WINDOWS)
			COORD                      coordScreen = { 0, 0 };
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			HANDLE                     hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

			GetConsoleScreenBufferInfo(hConsole, &csbi);
			coordScreen = csbi.dwCursorPosition;
			coordScreen.Y -= (short)_lines;
			if (coordScreen.Y < 0) coordScreen.Y = 0;

			SetConsoleCursorPosition(hConsole, coordScreen);
#elif defined (TARGET_OS_MACOSX) || defined (TARGET_OS_LINUX)
			Write("%s%d%s", "\033[", _lines, "A");
#endif
		}

		char Console::ReadChar()
		{
			char _result = 0;
			fscanf(m_fileInputPointer, "%c", &_result);
			CoreIOReader::Flush();
			return _result;
		}

		int Console::ReadInt()
		{
			int _result = 0;
			fscanf(m_fileInputPointer, "%d", &_result);
			CoreIOReader::Flush();
			return _result;
		}

		long Console::ReadLong()
		{
			long _result = 0;
			fscanf(m_fileInputPointer, "%ld", &_result);
			CoreIOReader::Flush();
			return _result;
		}

		float Console::ReadFloat()
		{
			float _result = 0;
			fscanf(m_fileInputPointer, "%f", &_result);
			CoreIOReader::Flush();
			return _result;
		}

		double Console::ReadDouble()
		{
			double _result = 0;
			fscanf(m_fileInputPointer, "%lf", &_result);
			CoreIOReader::Flush();
			return _result;
		}

		char Console::ReadChar(char _min, char _max)
		{
			char _result = 0;
			fscanf(m_fileInputPointer, "%c", &_result);
			CoreIOReader::Flush();
			if ((_min && _max) && (_min < _max)) {
				if (_result < _min)
					_result = _min;

				if (_result > _max)
					_result = _max;
			}

			return _result;
		}

		int Console::ReadInt(int _min, int _max)
		{
			int _result = 0;
			fscanf(m_fileInputPointer, "%d", &_result);
			CoreIOReader::Flush();
			if ((_min && _max) && (_min < _max)) {
				if (_result < _min)
					_result = _min;

				if (_result > _max)
					_result = _max;
			}

			return _result;
		}

		long Console::ReadLong(long _min, long _max)
		{
			long _result = 0;
			fscanf(m_fileInputPointer, "%ld", &_result);
			CoreIOReader::Flush();
			if ((_min && _max) && (_min < _max)) {
				if (_result < _min)
					_result = _min;

				if (_result > _max)
					_result = _max;
			}

			return _result;
		}

		float Console::ReadFloat(float _min, float _max)
		{
			float _result = 0;
			fscanf(m_fileInputPointer, "%f", &_result);
			CoreIOReader::Flush();
			if ((_min && _max) && (_min < _max)) {
				if (_result < _min)
					_result = _min;

				if (_result > _max)
					_result = _max;
			}

			return _result;
		}

		double Console::ReadDouble(double _min, double _max)
		{
			double _result = 0;
			fscanf(m_fileInputPointer, "%lf", &_result);
			CoreIOReader::Flush();
			if ((_min && _max) && (_min < _max)) {
				if (_result < _min)
					_result = _min;

				if (_result > _max)
					_result = _max;
			}

			return _result;
		}

		int Console::Seek(cc_int64_t _position)
		{
			return 0;
		}
		int Console::Forward(cc_int64_t _position)
		{
			return 0;
		}
		cc_int64_t Console::Length()
		{
			return 0;
		}
		int Console::Read(char *_buffer, size_t _bufferLength, size_t _bufferIndex, size_t _count)
		{
			return 0;
		}
		bool Console::EndOfFile()
		{
			return false;
		}
	}
}
