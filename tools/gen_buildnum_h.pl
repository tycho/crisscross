#!/usr/bin/perl

use strict;
use warnings;

use File::Basename;

use Cwd;
my $cwd = cwd;

my $in_git = 0;
my $scriptpath = $cwd . "/" . dirname($0);
my $outfile = $ARGV[0];

my $releasever;

if (open RELEASE, "<", "$scriptpath/release_ver") {
	$releasever = <RELEASE>;
	close RELEASE;
}

print "Is this project under Git? ";
if (-d "$scriptpath/../.git" ) {
	print "Yes\n";
	print "Is Git installed? ";
	if ( `which git` ) {
		print "Yes\n";
		$in_git = 1;
	} else {
		print "No\n";
		$in_git = 0;
	}
} else {
	print "No\n";
	$in_git = 0;
}

my $verstring = "";

if ($in_git == 0) {
	$verstring = $releasever;
} else {
	$verstring = `git describe --tags --long 2> /dev/null || git describe --tags`;
}

if (!$verstring) {
	die "couldn't get the version information\n";
}

chomp($verstring);

# This gets us:
#  $1.$2.$3.$4-$5-$6
my $component_pattern = "[v]?([0-9]+)[.]([0-9]+)[.]([0-9]+)(?:[.]([0-9]+))?(?:(?:-([a-zA-Z]+[0-9]+))?(?:-([0-9]+)?-g[a-fA-F0-9]+)?)?";

if ($verstring =~ $component_pattern) {
} else {
	die "Version string '$verstring' is malformed...\n";
}

my $major  = $1;
my $minor  = $2;
my $revis  = $3;
my $build  = $4;
my $commit = $6;

# Git didn't give us a --long format?
if ( !$commit ) {
	$commit = 0;
}

# This gets us just the tag:
my $tag_pattern = "([v]?[0-9]+[.][0-9]+[.][0-9]+(?:[.][0-9]+)?(?:(?:-[a-zA-Z]+[0-9]+)?))";

if ($verstring =~ $tag_pattern) {
} else {
	die "Version string '$verstring' is malformed...\n";
}

my $tag    = $1;

# We assume here that we must be using a different
# version number convention.
if ( !$build ) {
	$build = $commit;
}

# Old versions of git omit the commits-since-tag number,
# so we can try 'git rev-list' to get this instead.
if ( $commit == 0 && $in_git ) {
	$commit = `git rev-list $tag.. | wc -l`
}

if ( $commit == 0 ) {
	# If we're at the tag, don't make the long
	# version longer than necessary.
	$verstring = $tag;
}

unlink("$outfile.tmp");

my $prefix   = "CC_LIB";
my $smprefix = "cc";

open OUT, ">", "$outfile.tmp" or die $!;
print OUT <<__eof__;
#ifndef __included_${smprefix}_build_number_h
#define __included_${smprefix}_build_number_h

#define ${prefix}_VERSION_MAJOR ${major}
#define ${prefix}_VERSION_MINOR ${minor}
#define ${prefix}_VERSION_REVISION ${revis}
#define ${prefix}_VERSION_BUILD ${build}
#define ${prefix}_VERSION_TAG "${tag}"
#define ${prefix}_VERSION_LONG "${verstring}"

#define ${prefix}_RESOURCE_VERSION ${major},${minor},${revis},${build}
#define ${prefix}_RESOURCE_VERSION_STRING "${major}, ${minor}, ${revis}, ${build}"

#endif

__eof__
close OUT or die $!;

use Digest::MD5;

my $ctx = Digest::MD5->new;

my $md5old = ""; my $md5new = "";

if (-e $outfile) {
	open OUT, "$outfile" or die $!;
	$ctx->addfile(*OUT);
	$md5old = $ctx->hexdigest;
	close OUT
}

open OUT, "$outfile.tmp" or die $!;
$ctx->addfile(*OUT);
$md5new = $ctx->hexdigest;
close OUT;

use File::Copy;

if ($md5old ne $md5new) {
	if (-e $outfile) {
		unlink($outfile) or die $!;
	}
	move "$outfile.tmp", $outfile or die $!;
	print "$outfile updated.\n";
} else {
	unlink ("$outfile.tmp");
	print "$outfile is already up to date.\n";
}
