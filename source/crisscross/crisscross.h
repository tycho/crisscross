/*
 *   CrissCross
 *   A multi-purpose cross-platform library.
 *
 *   A product of Uplink Laboratories.
 *
 *   (c) 2006-2010 Steven Noonan.
 *   Licensed under the New BSD License.
 *
 */

#ifndef __included_cc_crisscross_h
#define __included_cc_crisscross_h

#define CC_LIB_NAME                "CrissCross"

/* #  define CC_LIB_CODENAME            "Technetium"    // v0.1.0 codename */
/* #  define CC_LIB_CODENAME            "Freya"         // v0.2.0 codename */
/* #  define CC_LIB_CODENAME            "Hodur"         // v0.3.0 codename */
/* #  define CC_LIB_CODENAME            "Baldur"        // v0.4.0 codename */
/* #  define CC_LIB_CODENAME            "Loki"          // v0.5.0 codename */
/* #  define CC_LIB_CODENAME            "Ragnarok"      // v0.6.0 codename */
/* #  define CC_LIB_CODENAME            "Heimdall"      // v0.7.0 codename */
#define      CC_LIB_CODENAME            "Odin"          /* v0.8.0 codename */
/* #  define CC_LIB_CODENAME            "Thor"          // v0.9.0 codename */
/* #  define CC_LIB_CODENAME            "Valhalla"      // v1.0.0 codename */

#define CC_LIB_URL                 "http://www.uplinklabs.net/crisscross/"
#define CC_LIB_COPYRIGHT           "(c) 2006-2010 by Uplink Laboratories. Licensed under the New BSD License."
#define CC_LIB_NDS_COPYRIGHT       "(c) by Uplink Laboratories.\nLicensed under the BSD License."

#include <crisscross/version.h>
#include <crisscross/platform_detect.h>
#include <crisscross/cc_attr.h>
#include <crisscross/deprecate.h>
#include <crisscross/datatypes.h>
#include <crisscross/string_utils.h>
#include <crisscross/sort.h>
#include <crisscross/filesystem.h>
#include <crisscross/dstack.h>
#include <crisscross/darray.h>
#include <crisscross/llist.h>
#include <crisscross/node.h>
#include <crisscross/hash.h>
#include <crisscross/hashtable.h>
#include <crisscross/stltree.h>
#include <crisscross/splaytree.h>
#include <crisscross/avltree.h>
#include <crisscross/rbtree.h>
#include <crisscross/stree.h>
#include <crisscross/error.h>
#include <crisscross/core_io.h>
#include <crisscross/console.h>
#include <crisscross/filereader.h>
#include <crisscross/filewriter.h>
#include <crisscross/core_network.h>
#include <crisscross/core_socket.h>
#include <crisscross/tcpsocket.h>
#include <crisscross/udpsocket.h>
#include <crisscross/debug.h>
#include <crisscross/mutex.h>
#include <crisscross/stopwatch.h>
#include <crisscross/system.h>
#include <crisscross/md4.h>
#include <crisscross/md5.h>
#include <crisscross/sha1.h>
#include <crisscross/sha256.h>
#include <crisscross/sha512.h>
#include <crisscross/tiger.h>

#endif
