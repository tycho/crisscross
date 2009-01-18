#!/bin/bash

VERSTRING=$(git describe --tags)
OUT=$1

MAJOR=`echo $VERSTRING | cut -d'.' -f1`
MINOR=`echo $VERSTRING | cut -d'.' -f2`
REVIS=`echo $VERSTRING | cut -d'.' -f3 | cut -d'-' -f 1`
if [ $(echo $VERSTRING | grep "-") ]; then
	BUILD=`echo $VERSTRING | cut -d'-' -f2,3,4,5`
	TINYBUILD=`echo $VERSTRING | cut -d'-' -f2`
else
	BUILD=0
	TINYBUILD=0
fi

rm -f $OUT

cat >> $OUT << __eof__
#ifndef __included_cc_build_number_h
#define __included_cc_build_number_h

#define CC_LIB_VERSION_MAJOR $MAJOR
#define CC_LIB_VERSION_MINOR $MINOR
#define CC_LIB_VERSION_REVISION $REVIS
#define CC_LIB_VERSION_BUILD $TINYBUILD
#define CC_LIB_VERSION "$MAJOR.$MINOR.$REVIS"
#define CC_LIB_VERSION_STRING "$VERSTRING"

#define CC_RESOURCE_VERSION $MAJOR,$MINOR,$REVIS,$TINYBUILD
#define CC_RESOURCE_VERSION_STRING "$MAJOR, $MINOR, $REVIS, $TINYBUILD"

#endif

__eof__
