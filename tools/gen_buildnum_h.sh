#!/usr/bin/env bash

if [ -f ~/.bashrc ]; then
	source ~/.bashrc
fi

SCRIPTPATH="$(echo $0 | rev | cut -d '/' -f2,3,4,5,6,7,8,9,10,11,12,13 | rev)"
RELEASEVER="$(cat $SCRIPTPATH/release_ver 2> /dev/null)"
RELEASEVER_TAG="$(cat $SCRIPTPATH/release_ver 2> /dev/null | cut -d'-' -f1)"

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
	VERSTRING=$(git describe --tags --long 2> /dev/null || git describe --tags)

	# is this an rc, alpha, or beta?
	if [ "$(echo $VERSTRING | cut -d'-' -f 3 | grep "^[0-9]*$")" != "" ]; then
		FID=4
		IS_PRE=1
	else
		FID=3
		IS_PRE=0
	fi
	
	# is this an old version of git without --long support?
	REVCOUNT="$(echo $VERSTRING | cut -d'-' -f $FID)"
	BASE_VERSION="$(echo $VERSTRING | cut -d'-' -f 1)"
	if [ $IS_PRE -eq 1 ]; then
		BASE_VERSION="$BASE_VERSION-$(echo $VERSTRING | cut -d'-' -f 2)"
	fi

	if [ "$(echo $REVCOUNT | grep ^g)x" == "x" ]; then
		REVCOUNT="$(git rev-list $BASE_VERSION..HEAD | wc -l | sed 's/[ ]//g')"
		VERSTRING="$BASE_VERSION-$REVCOUNT-$(echo $VERSTRING | cut -d'-' -f $(($FID - 1)))"
	fi
fi
OUT=$1

MAJOR=`echo $VERSTRING | cut -d'.' -f1`
MINOR=`echo $VERSTRING | cut -d'.' -f2`
REVIS=`echo $VERSTRING | cut -d'.' -f3 | cut -d'-' -f 1`
TINYBUILD=`echo $VERSTRING | cut -d'-' -f2`
PRE=
if [ $IS_PRE == 1 ]; then
	# We've got a release candidate. Reparse to get the build -number-.
	PRE=-$TINYBUILD
	TINYBUILD=`echo $VERSTRING | cut -d'-' -f3`
fi

if [ $IN_GIT -eq 0 ]; then
	VERSTRING=$RELEASEVER_TAG
fi

rm -f $OUT.tmp

PREFIX=CC_LIB
SHORT_TAG=cc

cat >> $OUT.tmp << __eof__
#ifndef __included_$(echo $SHORT_TAG)_build_number_h
#define __included_$(echo $SHORT_TAG)_build_number_h

#define $(echo $PREFIX)_VERSION_MAJOR $MAJOR
#define $(echo $PREFIX)_VERSION_MINOR $MINOR
#define $(echo $PREFIX)_VERSION_REVISION $REVIS
#define $(echo $PREFIX)_VERSION_BUILD $TINYBUILD
#define $(echo $PREFIX)_VERSION "$MAJOR.$MINOR.$REVIS$PRE"
#define $(echo $PREFIX)_VERSION_STRING "$VERSTRING"

#define $(echo $PREFIX)_RESOURCE_VERSION $MAJOR,$MINOR,$REVIS,$TINYBUILD
#define $(echo $PREFIX)_RESOURCE_VERSION_STRING "$MAJOR, $MINOR, $REVIS, $TINYBUILD"

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

