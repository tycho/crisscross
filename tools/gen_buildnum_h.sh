#!/bin/bash

if [ -f ~/.bashrc ]; then
	source ~/.bashrc
fi

SCRIPTPATH="$(echo $0 | rev | cut -d '/' -f2,3,4,5,6,7,8,9,10,11,12,13 | rev)"
RELEASEVER="$(cat $SCRIPTPATH/release_ver &> /dev/null)"
RELEASEVER_TAG="$(cat $SCRIPTPATH/release_ver &> /dev/null | cut -d'-' -f1)"

echo -n "Is CrissCross under version control? "
if [ -x "$SCRIPTPATH/../.git" ]; then
	echo "Yes"
	echo -n "Is git installed? "
	if [ "" != "$(which git)" ]; then
		echo "Yes"
		IN_GIT=1
	else
		echo "No"
		IN_GIT=0
	fi
else
	echo "No"
	IN_GIT=0
fi

if [ $IN_GIT -eq 0 ]; then
	VERSTRING=$RELEASEVER
else
	VERSTRING=$(git describe --tags --long)
fi
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

if [ $IN_GIT -eq 0 ]; then
	VERSTRING=$RELEASEVER_TAG
else
	VERSTRING=$(git describe --tags)
fi

rm -f $OUT.tmp

cat >> $OUT.tmp << __eof__
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

if [ -f $OUT ]; then
	if [ -x /sbin/md5 ]; then
		MD5OLD=`/sbin/md5 $OUT | cut -d' ' -f4`
	else
		MD5OLD=`md5sum $OUT | cut -d' ' -f1`
	fi
else
	MD5OLD=
fi

if [ -x /sbin/md5 ]; then
	MD5NEW=`/sbin/md5 $OUT.tmp | cut -d' ' -f4`
else
	MD5NEW=`md5sum $OUT.tmp | cut -d' ' -f1`
fi

if [ "$MD5NEW" == "$MD5OLD" ]; then
        echo "$OUT is already up to date."
        rm -f $OUT.tmp
else
        echo "$OUT updated."
        mv $OUT.tmp $OUT
fi

