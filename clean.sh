#!/bin/bash

# Quick sanity check.
if [ ! -d .svn ]; then
	if [ ! -d .git ]; then
		echo "This project doesn't seem to be under source control."
		exit 1
	fi
fi

echo
if [ -d .svn ]; then
	svn status --no-ignore | grep ^[I?] | sed 's/^[I?]//g' | sed 's/^[ ].//g'
else
	if [ -d .git ]; then
		git clean -nfxd
	fi
fi
echo

read -p "Remove the above listed files (Y/N)? "
echo
if [ "$REPLY" == "y" ]; then
	if [ -d .svn ]; then
		rm -rvf `svn status --no-ignore | grep ^[I?] | sed 's/^[I?]//g' | sed 's/^[ ].//g'`
	else
		git clean -fxd
	fi
else
	echo
	echo "No files removed."
	echo
fi
