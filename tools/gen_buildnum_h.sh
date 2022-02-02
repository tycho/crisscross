#!/bin/sh

gen_ver() {
	local output="$1"
	local prefix="$2"
	local smprefix="$3"

	if [ -z "$output" ]; then
		echo "Error: missing output file"
		return
	fi

	if [ -z "$prefix" ]; then
		echo "Error: missing prefix"
		return
	fi

	if [ -z "$smprefix" ]; then
		echo "Error: missing short prefix"
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

	if [ -e "${output}" ]; then
		cp "$scriptPath/build_number.h.in" "${output}.tmp"

		sed -i "s/@SM_PREFIX@/$smprefix/; s/@PREFIX@/$prefix/; s/@VERSION_MAJOR@/$major/; s/@VERSION_MINOR@/$minor/; s/@VERSION_REVISION@/$revis/; s/@VERSION_BUILD@/$build/; s/@GIT_TAG@/$git_tag/" "${output}.tmp"

		if cmp -s "${output}.tmp" "${output}"; then
			unlink "${output}.tmp" || true
		else
			unlink "${output}" || true
			mv "${output}.tmp" "${output}"
		fi
	else
		cp "$scriptPath/build_number.h.in" "${output}"

		sed -i "s/@SM_PREFIX@/$smprefix/; s/@PREFIX@/$prefix/; s/@VERSION_MAJOR@/$major/; s/@VERSION_MINOR@/$minor/; s/@VERSION_REVISION@/$revis/; s/@VERSION_BUILD@/$build/; s/@GIT_TAG@/$git_tag/" "${output}"
	fi
}

gen_ver "$@"
