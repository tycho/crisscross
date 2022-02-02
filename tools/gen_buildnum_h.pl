#!/usr/bin/env perl
$|=1;	# Flush writes as soon as print finishes.

use strict;
use warnings;

use File::Basename;

use Cwd;
my $cwd = cwd;

my $scriptpath = dirname($0);

my ($outfile, $prefix, $smprefix) = @ARGV;

die ("Output file not specified") unless(defined $outfile);
die ("Prefix not specified") unless(defined $prefix);
die ("Short prefix not specified") unless(defined $smprefix);

my $tag = "";
my $build = 0;
my $git_tag = "";

if((-e "$scriptpath/../.git") && `git`) {
	$tag = `cd $scriptpath && git describe --tags --abbrev=0 2> /dev/null`;
	if ($tag) {
		chomp($tag);
		$build = `cd $scriptpath && git rev-list --count $tag..HEAD 2> /dev/null`;
		chomp($build);

		$git_tag = `cd $scriptpath && git rev-parse --short HEAD`;
		chomp($git_tag);
		$git_tag = "${tag}-${build}-g${git_tag}";
	}
}

if (!$tag) {
	open RELEASEVER, "$scriptpath/release_ver" || die ("Can't open release_ver:$!");
	$tag = <RELEASEVER>;
	close RELEASEVER;
	chomp($tag);
	$git_tag="$tag";

	if (!$tag) {
		die "error: couldn't get the version information.\n";
	}
}

my @result=split(/[-.]+/, $tag);

my $major = $result[0];
my $minor = $result[1];
my $revis = $result[2];

if( substr($major, 0, 1) eq "v") {
	$major = substr($major, 1);
	$build = $result[3];
}

open FILE, '<', "$scriptpath/build_number.h.in" or die $!;
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

mkdir dirname($outfile);
unlink("$outfile.tmp");

if (-e $outfile) {
	open OUT, '>', "$outfile.tmp" or die $!;
	print OUT @lines;
	close(OUT) or die $!;

	use File::Compare;
	if(compare("$outfile.tmp", "$outfile")) {
		unlink($outfile);
		use File::Copy;
		move "$outfile.tmp", $outfile or die $!;
	} else {
		unlink("$outfile.tmp");
	}
} else {
	print("$outfile\n");
	open(OUT, '>', "$outfile") or die $!;
	print OUT @lines;
	close(OUT) or die $!;
}
