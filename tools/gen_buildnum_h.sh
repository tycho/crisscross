#!/bin/bash

VERSTRING=$(git describe --tags --long)
OUT=$1

MAJOR=`echo $VERSTRING | cut -d'.' -f1`
MINOR=`echo $VERSTRING | cut -d'.' -f2`
REVIS=`echo $VERSTRING | cut -d'.' -f3 | cut -d'-' -f 1`
TINYBUILD=`echo $VERSTRING | cut -d'-' -f2`
RC=
if [ $(echo $TINYBUILD | grep rc) ]; then
	# We've got a release candidate. Reparse to get the build -number-.
	RC=-$TINYBUILD
	TINYBUILD=`echo $VERSTRING | cut -d'-' -f3`
fi

VERSTRING=$(git describe --tags)

rm -f $OUT

cat >> $OUT << __eof__
#ifndef __included_cc_build_number_h
#define __included_cc_build_number_h

#define CC_LIB_VERSION_MAJOR $MAJOR
#define CC_LIB_VERSION_MINOR $MINOR
#define CC_LIB_VERSION_REVISION $REVIS
#define CC_LIB_VERSION_BUILD $TINYBUILD
#define CC_LIB_VERSION "$MAJOR.$MINOR.$REVIS$RC"
#define CC_LIB_VERSION_STRING "$VERSTRING"

#define CC_RESOURCE_VERSION $MAJOR,$MINOR,$REVIS,$TINYBUILD
#define CC_RESOURCE_VERSION_STRING "$MAJOR, $MINOR, $REVIS, $TINYBUILD"

#endif

__eof__
