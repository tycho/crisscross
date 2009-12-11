ifneq ($(findstring $(MAKEFLAGS),s),s)
ifndef V
        QUIET_CXX      = @echo '   ' CXX $@;
        QUIET_GEN      = @echo '   ' GEN $@;
        QUIET_AR       = @echo '   ' AR $@;
        QUIET_RANLIB   = @echo '   ' RANLIB $@;
        QUIET_INSTALL  = @echo '   ' INSTALL $<;
        export V
endif
endif

LIB    = libCrissCross.a
AR    ?= ar
CXX   ?= g++
RANLIB?= ranlib
RM    ?= rm -f

prefix ?= /usr/local
libdir := $(prefix)/lib
includedir := $(prefix)/include/crisscross

HEADERS = \
	crisscross/avltree.cpp \
	crisscross/avltree.h \
	crisscross/combsort.h \
	crisscross/compare.h \
	crisscross/console.h \
	crisscross/core_io.h \
	crisscross/core_io_reader.h \
	crisscross/core_io_writer.h \
	crisscross/core_network.h \
	crisscross/core_socket.h \
	crisscross/cpuid.h \
	crisscross/crisscross.h \
	crisscross/darray.cpp \
	crisscross/darray.h \
	crisscross/datatypes.h \
	crisscross/debug.h \
	crisscross/deprecate.h \
	crisscross/dstack.cpp \
	crisscross/dstack.h \
	crisscross/endian.h \
	crisscross/error.h \
	crisscross/filereader.h \
	crisscross/filesystem.h \
	crisscross/filewriter.h \
	crisscross/hash.h \
	crisscross/hashtable.cpp \
	crisscross/hashtable.h \
	crisscross/heapsort.h \
	crisscross/internal_mem.h \
	crisscross/llist.cpp \
	crisscross/llist.h \
	crisscross/md4.h \
	crisscross/md5.h \
	crisscross/mutex.h \
	crisscross/node.h \
	crisscross/platform_detect.h \
	crisscross/profiler.h \
	crisscross/quadtree.h \
	crisscross/quicksort.h \
	crisscross/rbtree.cpp \
	crisscross/rbtree.h \
	crisscross/sha1.h \
	crisscross/sha256.h \
	crisscross/sha512.h \
	crisscross/shellsort.h \
	crisscross/sort.h \
	crisscross/splaytree.cpp \
	crisscross/splaytree.h \
	crisscross/stltree.h \
	crisscross/stopwatch.h \
	crisscross/system.h \
	crisscross/tcpsocket.h \
	crisscross/tiger.h \
	crisscross/udpsocket.h \
	crisscross/universal_include.h \
	crisscross/vec2.h \
	crisscross/version.h \
	crisscross/zombiehunter.h

SOURCES = \
	console.cpp \
	core_io_reader.cpp \
	core_io_writer.cpp \
	core_network.cpp \
	core_socket.cpp \
	cpuid.cpp \
	debug.cpp \
	error.cpp \
	filereader.cpp \
	filesystem.cpp \
	filewriter.cpp \
	md4.cpp \
	md5.cpp \
	mutex.cpp \
	profiler.cpp \
	sha1.cpp \
	sha256.cpp \
	sha512.cpp \
	stopwatch.cpp \
	system.cpp \
	tcpsocket.cpp \
	tiger.cpp \
	udpsocket.cpp \
	universal_include.cpp \
	version.cpp \
	zombiehunter.cpp


HEADERS_INST := $(patsubst crisscross/%,$(includedir)/%,$(HEADERS))
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))

CFLAGS ?= -O2
CFLAGS += -I.

.PHONY: install

all: $(LIB)

$(includedir)/%.h: crisscross/%.h
	-@if [ ! -d $(includedir)  ]; then mkdir -p $(includedir); fi
	$(QUIET_INSTALL)cp $< $@
	@chmod 0644 $@

$(libdir)/%.a: %.a
	-@if [ ! -d $(libdir)  ]; then mkdir -p $(libdir); fi
	$(QUIET_INSTALL)cp $< $@
	@chmod 0644 $@

install: $(HEADERS_INST) $(libdir)/$(LIB)

clean:
	$(RM) $(OBJECTS) $(LIB) .cflags

distclean: clean

$(LIB): $(OBJECTS)
	$(QUIET_AR)$(AR) rcu $@ $^
	$(QUIET_RANLIB)$(RANLIB) $@

%.o: %.cpp .cflags
	$(QUIET_CXX)$(CXX) $(CFLAGS) -o $@ -c $<

crisscross/build_number.h:
	$(QUIET_GEN)../tools/gen_buildnum_h.pl crisscross/build_number.h

version.cpp: crisscross/build_number.h

TRACK_CFLAGS = $(subst ','\'',$(CXX) $(CFLAGS))

.cflags: .force-cflags
	@FLAGS='$(TRACK_CFLAGS)'; \
    if test x"$$FLAGS" != x"`cat .cflags 2>/dev/null`" ; then \
        echo "    * rebuilding libCrissCross: new build flags or prefix"; \
        echo "$$FLAGS" > .cflags; \
    fi

.PHONY: .force-cflags
