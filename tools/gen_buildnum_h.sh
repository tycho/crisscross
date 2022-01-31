#!/bin/sh

gen_ver() {
	local output="$1"

	if [ -z "$output" ]; then
		echo "Error: missing output file"
		return
	fi

	local scriptPath=`dirname "$0"`

	local tag=
	local build=
	local git_tag=

	if [ -e "$scriptPath/../.git" ] && [ ! -z `which git` ]; then
		# parse the git pattern
		tag=`cd $scriptPath && git describe --tags --abbrev=0 2> /dev/null`
	fi

	if [ -z "$tag" ]; then
		echo "${scriptPath}/release_ver"
		tag=`cat ${scriptPath}/release_ver`
		if [ -z "$tag" ]; then return 1; fi
	else
		build=`cd $scriptPath && git rev-list --count $tag..HEAD 2> /dev/null`
		git_tag=`cd $scriptPath && git rev-parse --short HEAD`
		git_tag="$tag-$build-g$git_tag";
	fi

	local major=
	local minor=
	local revis=

	if [ "$tag" == 'v'* ]; then
		IFS=. read major minor revis build <<< $tag
		major="${major#v}"
	else
		IFS=. read major minor revis <<< $tag
	fi

	unlink "$output" || true

	cp "$scriptPath/../source/crisscross/build_number.h.in" "$output"

	sed -i "s/@SM_PREFIX@/cc/; s/@PREFIX@/CC_LIB/; s/@VERSION_MAJOR@/$major/; s/@VERSION_MINOR@/$minor/; s/@VERSION_REVISION@/$revis/; s/@VERSION_BUILD@/$build/; s/@GIT_TAG@/$git_tag/" "$output"
}

gen_ver "$@"
