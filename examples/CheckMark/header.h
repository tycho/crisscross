/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of IO.IN Research.
 *
 *   (c) 2006-2008 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_header_h
#define __included_header_h

#include <math.h>
#include <time.h>

#include <crisscross/crisscross.h>

#define DATASET_SIZE    10
#ifdef TARGET_OS_NDSFIRMWARE
#define ENTRY_LENGTH    1024
#define MAX_RUNS        10
#else
#define ENTRY_LENGTH    1048576
#define MAX_RUNS        20
#endif

#endif
