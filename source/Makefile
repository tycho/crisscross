.SECONDEXPANSION:
.SUFFIXES:

ifneq ($(findstring $(MAKEFLAGS),s),s)
ifndef V
        QUIET          = @
        QUIET_CXX      = @echo '   ' CXX $@;
        QUIET_GEN      = @echo '   ' GEN $@;
        QUIET_AR       = @echo '   ' AR $@;
        QUIET_RANLIB   = @echo '   ' RANLIB $@;
        QUIET_INSTALL  = @echo '   ' INSTALL $<;
        export V
endif
endif

LIB      = libCrissCross.a
AR      ?= ar
ARFLAGS ?= rc
CXX     ?= g++
RANLIB  ?= ranlib
RM      ?= rm -f

BUILD_DIR := build
BUILD_ID  ?= default-build-id
OBJ_DIR   := $(BUILD_DIR)/$(BUILD_ID)

ifeq (,$(BUILD_ID))
$(error BUILD_ID cannot be an empty string)
endif

uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

prefix ?= /usr/local
libdir := $(prefix)/lib
includedir := $(prefix)/include/crisscross

HEADERS = \
	crisscross/avltree.cpp \
	crisscross/avltree.h \
	crisscross/cc_attr.h \
	crisscross/combsort.h \
	crisscross/compare.h \
	crisscross/console.h \
	crisscross/core_io.h \
	crisscross/core_io_reader.h \
	crisscross/core_io_writer.h \
	crisscross/crisscross.h \
	crisscross/darray.cpp \
	crisscross/darray.h \
	crisscross/datatypes.h \
	crisscross/debug.h \
	crisscross/dstack.cpp \
	crisscross/dstack.h \
	crisscross/endian.h \
	crisscross/error.h \
	crisscross/filesystem.h \
	crisscross/hash.h \
	crisscross/hashtable.cpp \
	crisscross/hashtable.h \
	crisscross/heapsort.h \
	crisscross/internal_mem.h \
	crisscross/llist.cpp \
	crisscross/llist.h \
	crisscross/nasty_cast.h \
	crisscross/node.h \
	crisscross/platform_detect.h \
	crisscross/quadtree.h \
	crisscross/quicksort.h \
	crisscross/rbtree.cpp \
	crisscross/rbtree.h \
	crisscross/shellsort.h \
	crisscross/sort.h \
	crisscross/splaytree.cpp \
	crisscross/splaytree.h \
	crisscross/stltree.h \
	crisscross/stopwatch.h \
	crisscross/stree.cpp \
	crisscross/stree.h \
	crisscross/string_utils.h \
	crisscross/system.h \
	crisscross/vec2.h \
	crisscross/version.h \

SOURCES = \
	console.cpp \
	compare.cpp \
	core_io_reader.cpp \
	core_io_writer.cpp \
	debug.cpp \
	error.cpp \
	filesystem.cpp \
	stopwatch.cpp \
	string_utils.cpp \
	system.cpp \
	universal_include.cpp \
	version.cpp \

HEADERS_INST := $(patsubst crisscross/%,$(includedir)/%,$(HEADERS))
OBJECTS := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

CXXFLAGS ?= -O2
override CXXFLAGS += -std=c++17
override CXXFLAGS += -I.
override CXXFLAGS += -I$(OBJ_DIR)

ifeq ($(uname_S),FreeBSD)
override CXXFLAGS += -I/usr/local/include
endif

.PHONY: install

all: $(OBJ_DIR)/$(LIB)

$(includedir)/%.h: crisscross/%.h
	-@if [ ! -d $(includedir)  ]; then mkdir -p $(includedir); fi
	$(QUIET_INSTALL)cp $< $@
	@chmod 0644 $@

$(includedir)/%.cpp: crisscross/%.cpp
	-@if [ ! -d $(includedir)  ]; then mkdir -p $(includedir); fi
	$(QUIET_INSTALL)cp $< $@
	@chmod 0644 $@

$(libdir)/%.a: $(OBJ_DIR)/%.a
	-@if [ ! -d $(libdir)  ]; then mkdir -p $(libdir); fi
	$(QUIET_INSTALL)cp $< $@
	@chmod 0644 $@

install: $(HEADERS_INST) $(libdir)/$(LIB)

clean:
	$(RM) -r $(OBJ_DIR)

distclean: clean
	$(RM) -r $(BUILD_DIR)

$(OBJ_DIR)/$(LIB): $(OBJECTS) | $$(@D)/.
	$(QUIET_AR)$(AR) $(ARFLAGS) $@ $^
	$(QUIET_RANLIB)$(RANLIB) $@

$(OBJ_DIR)/%.o: %.cpp $(OBJ_DIR)/.cflags | $$(@D)/.
	$(QUIET_CXX)$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OBJ_DIR)/build_number.h: | $$(@D)/.
	$(QUIET_GEN)../tools/gen_buildnum_h.pl $(OBJ_DIR)/build_number.h "CC_LIB" "cc"

version.cpp: $(OBJ_DIR)/build_number.h

.PRECIOUS: $(OBJ_DIR)/. $(OBJ_DIR)%/.

$(OBJ_DIR)/.:
	$(QUIET)mkdir -p $@

$(OBJ_DIR)%/.:
	$(QUIET)mkdir -p $@

TRACK_CXXFLAGS = $(subst ','\'',$(CXX) $(CXXFLAGS))

$(OBJ_DIR)/.cflags: .force-cflags | $$(@D)/.
	@FLAGS='$(TRACK_CXXFLAGS)'; \
    if test x"$$FLAGS" != x"`cat $(OBJ_DIR)/.cflags 2>/dev/null`" ; then \
        echo "    * rebuilding libCrissCross: new build flags or prefix"; \
        echo "$$FLAGS" > $(OBJ_DIR)/.cflags; \
    fi

.PHONY: .force-cflags
