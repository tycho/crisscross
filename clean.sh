#!/bin/bash

echo
echo "The following files will be purged:"
if [ -d .svn ]; then
	svn status --no-ignore | grep ^[I?] | sed 's/^[I?]//g' | sed 's/^[ ].//g'
else
	git clean -n -f -x -d
fi

echo
read -p "Continue (Y/N)? "
if [ "$REPLY" == "y" ]; then
	if [ -d .svn ]; then
		rm -rvf `svn status --no-ignore | grep ^[I?] | sed 's/^[I?]//g' | sed 's/^[ ].//g'`
	else
		git clean -f -x -d
	fi
else
	echo
	echo "No files removed."
	echo
fi
