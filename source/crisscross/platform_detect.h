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

/*
 *
 * Preprocessor Definitions
 * ------------------------
 *
 * TARGET_CPU_ARM
 *  ARM processor
 * TARGET_CPU_ALPHA
 *  DEC Alpha processor
 * TARGET_CPU_SPARC
 *  Sun Microsystems SPARC processor
 * TARGET_CPU_X86
 *  Intel x86 processor
 * TARGET_CPU_IA64
 *  Intel Itanic processor
 * TARGET_CPU_X64
 *  Intel 64-bit processor
 * TARGET_CPU_PPC
 *  IBM PowerPC processor
 *
 * TARGET_OS_WINDOWS
 *  Windows
 * TARGET_OS_LINUX
 *  Linux
 * TARGET_OS_MACOSX
 *  Mac OS X
 * TARGET_OS_FREEBSD
 *  FreeBSD
 * TARGET_OS_NETBSD
 *  NetBSD
 * TARGET_OS_OPENBSD
 *  OpenBSD
 * TARGET_OS_NDSFIRMWARE
 *  Nintendo DS firmware
 *
 * TARGET_COMPILER_GCC
 *  GNU C++ Compiler
 * TARGET_COMPILER_VC
 *  Visual C++
 * TARGET_COMPILER_ICC
 *  Intel C++ Compiler
 * TARGET_COMPILER_CLANG
 *  LLVM/Clang compiler
 *
 * TARGET_BIG_ENDIAN
 *      Running on a big endian byte order architecture.
 * TARGET_LITTLE_ENDIAN
 *      Running on a little endian byte order architecture.
 *
 */

#undef PROCESSOR_DETECTED
#undef COMPILER_DETECTED
#undef OS_DETECTED

/* Clear all macros before continuing, in case another header (e.g.
 * TargetConditionals.h in macOS) decides to use the same macro names.
 */
#undef TARGET_BIG_ENDIAN
#undef TARGET_COMPILER_BORLAND
#undef TARGET_COMPILER_CLANG
#undef TARGET_COMPILER_CYGWIN
#undef TARGET_COMPILER_GCC
#undef TARGET_COMPILER_ICC
#undef TARGET_COMPILER_MINGW
#undef TARGET_COMPILER_VC
#undef TARGET_CPU_ALPHA
#undef TARGET_CPU_ARM
#undef TARGET_CPU_BITS
#undef TARGET_CPU_IA64
#undef TARGET_CPU_PPC
#undef TARGET_CPU_SPARC
#undef TARGET_CPU_X64
#undef TARGET_CPU_X86
#undef TARGET_CPU_X86_64
#undef TARGET_LITTLE_ENDIAN
#undef TARGET_OS_FREEBSD
#undef TARGET_OS_HAIKU
#undef TARGET_OS_LINUX
#undef TARGET_OS_MACOSX
#undef TARGET_OS_NDSFIRMWARE
#undef TARGET_OS_NETBSD
#undef TARGET_OS_OPENBSD
#undef TARGET_OS_WINDOWS


/* ------------------- *
* PROCESSOR DETECTION *
* ------------------- */

/* ARM */
#if !defined (PROCESSOR_DETECTED)
#if defined (__arm__) || defined (__AARCH64EL__) || defined(_M_ARM) || defined(_M_ARM64)
#define PROCESSOR_DETECTED
#define TARGET_CPU_ARM
#define TARGET_LITTLE_ENDIAN
#endif
#endif

/* DEC Alpha */
#if !defined (PROCESSOR_DETECTED)
#if defined (__alpha) || defined (__alpha__)
#define PROCESSOR_DETECTED
#define TARGET_CPU_ALPHA
#define TARGET_LITTLE_ENDIAN /* How should bi-endianness be handled? */
#endif
#endif

/* Sun SPARC */
#if !defined (PROCESSOR_DETECTED)
#if defined (__sparc) || defined (__sparc__)
#define PROCESSOR_DETECTED
#define TARGET_CPU_SPARC
#define TARGET_BIG_ENDIAN
#endif
#endif

/* PowerPC */
#if !defined (PROCESSOR_DETECTED)
#if defined (_ARCH_PPC) || defined (__ppc__) || defined (__ppc64__) || defined (__PPC) || defined (powerpc) || defined (__PPC__) || defined (__powerpc64__) || defined (__powerpc64)
#define PROCESSOR_DETECTED
#if defined (__ppc64__) || defined (__powerpc64__) || defined (__powerpc64)
#define TARGET_CPU_PPC 64
#else
#define TARGET_CPU_PPC 32
#endif
#define TARGET_BIG_ENDIAN
#endif
#endif

/* x86_64 or AMD64 or x64 */
#if !defined (PROCESSOR_DETECTED)
#if defined (__x86_64__) || defined (__x86_64) || defined (__amd64) || defined (__amd64__) || defined (_AMD64_) || defined (_M_X64)
#define PROCESSOR_DETECTED
#define TARGET_CPU_X64
#define TARGET_CPU_X86_64
#define TARGET_LITTLE_ENDIAN
#endif
#endif

/* Intel x86 */
#if !defined (PROCESSOR_DETECTED)
#if defined (__i386__) || defined (__i386) || defined (i386) || defined (_X86_) || defined (_M_IX86)
#define PROCESSOR_DETECTED
#define TARGET_CPU_X86
#define TARGET_LITTLE_ENDIAN
#endif
#endif

/* IA64 */
#if !defined (PROCESSOR_DETECTED)
#if defined (__ia64__) || defined (_IA64) || defined (__ia64) || defined (_M_IA64)
#define PROCESSOR_DETECTED
#define TARGET_CPU_IA64
#define TARGET_LITTLE_ENDIAN
#endif
#endif

/* ------------------- *
* COMPILER DETECTION  *
* ------------------- */

#if !defined (COMPILER_DETECTED)
#if defined (__GNUC__)
#define COMPILER_DETECTED
#define TARGET_COMPILER_GCC
#endif
#if defined (__clang__)
#define TARGET_COMPILER_CLANG
#endif
#if defined (__CYGWIN__)
#define TARGET_COMPILER_CYGWIN
#endif
#if defined (__MINGW32__) || defined(__MINGW64__)
#define TARGET_COMPILER_MINGW
#endif
#endif

#if !defined (COMPILER_DETECTED)
#if defined (__INTEL_COMPILER) || defined (__ICL)
#define COMPILER_DETECTED
#define TARGET_COMPILER_ICC
#endif
#endif

#if !defined (COMPILER_DETECTED)
#if defined (_MSC_VER)
#define COMPILER_DETECTED
#define TARGET_COMPILER_VC
#endif
#endif

#if !defined (COMPILER_DETECTED)
#if defined (__BORLANDC__)
/* Earlier Borland compilers break terribly */
#if __BORLANDC__ >= 0x0600
#define COMPILER_DETECTED
#define TARGET_COMPILER_BORLAND
#endif
#endif
#endif

/* ------------ *
* OS DETECTION *
* ------------ */

#if !defined (OS_DETECTED)
#if defined (TARGET_COMPILER_VC) || defined (_WIN32) || defined (_WIN64)
#define OS_DETECTED
#define TARGET_OS_WINDOWS
#endif
#endif

#if !defined (OS_DETECTED)
#if defined (__linux__) || defined (linux) || defined (__linux) || defined (__gnu_linux__) || defined (__CYGWIN__)
#define OS_DETECTED
#define TARGET_OS_LINUX
#endif
#endif

#if !defined (OS_DETECTED)
#if defined (__APPLE__) || defined (__MACH__)
#define OS_DETECTED
#define TARGET_OS_MACOSX
#endif
#endif

#if !defined (OS_DETECTED)
#if defined (TARGET_CPU_ARM)
#define OS_DETECTED
#define TARGET_OS_NDSFIRMWARE
#endif
#endif

#if !defined (OS_DETECTED)
#if defined (__HAIKU__)
#define OS_DETECTED
#define TARGET_OS_HAIKU
#endif
#endif

#if !defined (OS_DETECTED)
#if defined (__FreeBSD__)
#define OS_DETECTED
#define TARGET_OS_FREEBSD
#endif
#endif

#if !defined (OS_DETECTED)
#if defined (__NetBSD__)
#define OS_DETECTED
#define TARGET_OS_NETBSD
#endif
#endif

#if !defined (OS_DETECTED)
#if defined (__OpenBSD__)
#define OS_DETECTED
#define TARGET_OS_OPENBSD
#endif
#endif

#if defined (_LP64) || defined (__LP64__) || defined (_M_X64) || defined(_M_IA64) || defined(__arm64__) || defined(_M_ARM64)
#define TARGET_CPU_BITS 64
#else
#define TARGET_CPU_BITS 32
#endif

#if defined (TARGET_CPU_PPC)
#if TARGET_CPU_PPC == 32
#define TARGET_CPU_BITS 32
#elif TARGET_CPU_PPC == 64
#define TARGET_CPU_BITS 64
#endif
#endif

#if !defined (PROCESSOR_DETECTED)
#error "Could not detect target CPU."
#endif

#if !defined (COMPILER_DETECTED)
#error "Could not detect target compiler."
#endif

#if !defined (OS_DETECTED)
#error "Could not detect target OS."
#endif

#if !defined (TARGET_CPU_BITS)
#error "Could not detect 32-bit/64-bit architecture."
#endif
