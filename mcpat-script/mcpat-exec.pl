#!/usr/bin/perl

use strict;
use POSIX;

my ($mcpatxml) = @ARGV;

my $time = 0;
open(WORKING,"<$mcpatxml") or die "Failed to open XML file\n";
while(my $line = <WORKING>) {
    if ($line =~ m!<stat name="runtime_sec" value="([\d\.]+)"/>!) {
	$time = $1;
	last;
    }
}
close(WORKING);
die "Parse error (couldn't find runtime_sec in XML file) \n" unless ($time != 0);

my $r = `./mcpat -infile $mcpatxml -printlevel 0 2>/dev/null`;

die "Parse error $r\n" unless ($r =~ /Total Leakage = ([\d\.]+) W/);
my $leakage = $1;
die "Parse error $r\n" unless ($r =~ /Runtime Dynamic = ([\d\.]+) W/);
my $runtime = $1;

my $j = ($leakage+$runtime)*$time;

print "$r\n$j J in $time sec\n";
