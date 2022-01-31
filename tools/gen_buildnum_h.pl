#!/usr/bin/env perl
$|=1;	# Flush writes as soon as print finishes.

use strict;
use warnings;

use File::Basename;

use Cwd;
my $cwd = cwd;

my $in_git = 0;
my $scriptpath = dirname($0);
my $outfile = $ARGV[0];
# NOTE: ToFix: This breaks if the full path to the file contains a space character.

my $releasever;

my $Win32 = 0;

$Win32 = 1 if ($^O =~ /MSWin/i);
open RELEASEVER, "$scriptpath/release_ver" || die ("Can't open release_ver:$!");
$releasever = <RELEASEVER>;
close RELEASEVER;
mkdir dirname($outfile);
chomp($releasever);

if (-d "$scriptpath/../.git" || -f "$scriptpath/../.git") {
	if ( !$Win32 && `which git` ) {
		$in_git = 1;
	} else {
		$in_git = 0;
	}
} else {
	$in_git = 0;
}

my $tag = "";
my $build = 0;
my $git_tag = "";

if ($in_git == 0) {
	$tag = $releasever;
	$git_tag="$tag"
} else {
	$tag = `cd $scriptpath && git describe --tags --abbrev=0 2> /dev/null`;

	if (!$tag) {
		$tag = $releasever;
		$in_git = 0;
	} else {
		chomp($tag);
		$build = `cd $scriptpath && git rev-list --count $tag..HEAD 2> /dev/null`;
		chomp($build);

		$git_tag = `cd $scriptpath && git rev-parse --short HEAD`;
		chomp($git_tag);
		$git_tag = "${tag}-${build}-g${git_tag}";
	}

}

if (!$tag) {
	die "error: couldn't get the version information.\n";
}

my @result=split(/[-.]+/, $tag);

my $major = $result[0];
my $minor = $result[1];
my $revis = $result[2];

if( substr($major, 0, 1) eq "v") {
	$major = substr($major, 1);
	$build = $result[3];
}

unlink($outfile);

my $prefix   = "CC_LIB";
my $smprefix = "cc";

open FILE, "$scriptpath/../source/crisscross/build_number.h.in" or die $!;
my @lines = <FILE>;
close FILE or die $!;

foreach(@lines) {
   $_ =~ s/\@SM_PREFIX\@/cc/g;
   $_ =~ s/\@PREFIX\@/CC_LIB/g;
   $_ =~ s/\@VERSION_MAJOR\@/${major}/g;
   $_ =~ s/\@VERSION_MINOR\@/${minor}/g;
   $_ =~ s/\@VERSION_REVISION\@/${revis}/g;
   $_ =~ s/\@VERSION_BUILD\@/${build}/g;
   $_ =~ s/\@GIT_TAG\@/${git_tag}/g;
}

open OUT, ">", "$outfile" or die $!;
print OUT @lines;
close(OUT) or die $!;
