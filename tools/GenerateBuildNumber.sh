#!/bin/bash

echo "Generating build_number.h..."

if [ "$#" -lt "1" ]; then
	echo "No input path specified."
	exit 1
fi

if [ "$#" -lt "2" ]; then
	echo "No output path specified."
	exit 1
fi

cd $1

BUILD=`svn --xml info | tr -d '\r' | tr -d '\n' | sed -e 's/.*<entry.*revision="\([0-9]*\)".*<url>.*/\1/'`

mkdir -p $2
cd $2

if [[ "${#BUILD}" -gt "5" || "${#BUILD}" -lt "1" ]]; then
	echo "Build number couldn't be detected, defaulting to 1!"
	BUILD=1
else
	echo "Build number ${BUILD} detected, creating header..."
fi

PREINCLUDED="__included_build_number_h"

echo "#ifndef ${PREINCLUDED}" > build_number.h.tmp
echo "#define ${PREINCLUDED}" >> build_number.h.tmp
echo "" >> build_number.h.tmp
echo "#define BUILD_NUMBER ${BUILD}" >> build_number.h.tmp
echo "" >> build_number.h.tmp
echo "#endif" >> build_number.h.tmp
echo "" >> build_number.h.tmp

touch build_number.h

if [ "$(md5 -s test 2> /dev/null)" == "" ]; then
	TEMPHEADERMD5=`md5sum build_number.h.tmp | cut -d' ' -f 1`
	HEADERMD5=`md5sum build_number.h | cut -d' ' -f 1`
else
	TEMPHEADERMD5=`md5 -q build_number.h.tmp`
	HEADERMD5=`md5 -q build_number.h`
fi

if [ "${TEMPHEADERMD5}" != "${HEADERMD5}" ]; then
	echo "Header updated."
	mv build_number.h.tmp build_number.h
else
	echo "Header is already up to date. Skipping update..."
	rm build_number.h.tmp
fi
