#!/usr/bin/perl

use strict;
use POSIX;

my ($statstxt,$configini,$mcpatxml) = @ARGV;

my $stats = &loadStats($statstxt);
my $config = &loadConfig($configini);
my $mcpatxml = &loadxml($mcpatxml);

$mcpatxml =~ s/value="{(.*?)}"/'value="'.&subst($1).'"'/ge;

print $mcpatxml;

sub subst() {
    my ($e) = @_;
    my $f = $e;
    $e =~ s/stats.([\w\d\.:]+)/$stats->{$1}/g;
    $e =~ s/config.([\w\d\.:]+)/$config->{$1}/g;
    my $r = eval $e;
    if ($r eq "") {
	print STDERR "Warning: empty result for $f\n";
	$e = 0;
    }
    return eval $e;
}

sub output() {
    print join("\t",@_),"\n";
}

sub loadxml() {
    my ($file) = @_;
    my $result = "";
    open(WORKING,"<$file") or die "Failed to open xml file $file\n";
    while(my $line = <WORKING>) {
	$result .= $line;
    }
    close(WORKING);
    return $result;
}


sub loadStats() {
    my ($result,$file) = ({},@_);
    open(WORKING,"<$file") or die "Failed to open stats file $file\n";
    while(my $line = <WORKING>) {
	chomp($line);
	if ($line =~ /^(\S+)\s+([\d\.\-]+|nan|inf|no_value)\s/) {
	    $result->{$1} = $2;
	}
	elsif ($line =~ /(?:Begin|End) Simulation Statistics/) {}
	elsif ($line =~ /^\s*$/) {}
	elsif ($line =~ /((\S+)(\s+))+/) {}
	else {
	    die "Failed to parse stats $line\n";
	}
    }
    close(WORKING);
    return $result;
}

sub loadConfig() {
    my ($result,$current,$file) = ({},"",@_);
    open(WORKING,"<$file") or die "Failed to open config file $file\n";
    while(my $line = <WORKING>) {
	chomp($line);
	if ($line =~ /\[(.*)\]/) {
	    $current = $1;
	}
	elsif ($line =~ /(.*)=(.*)/) {
	    $result->{$current.".".$1} = $2;
	}
	elsif ($line =~ /^\s*$/) {}
	else {
	    die "Failed to parse config $line\n";
	}
    }
    close(WORKING);
    return $result;
}
