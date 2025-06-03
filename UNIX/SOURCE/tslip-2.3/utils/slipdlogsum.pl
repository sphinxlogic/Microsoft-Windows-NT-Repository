#!/usr/bin/perl -w
# @(#)slipdlogsum.pl	1.1 (30 Sep 1993)
#.........................................................................
# Copyright 1993, Jim Avera.  All Rights Reserved.
# 
# You may use, modify, or distribute this work (or the portions owned by the 
# above copyright holder) ONLY as described in the file "COPYRIGHT" which 
# accompanies this file or program.
# 
# BECAUSE THIS SOFTWARE IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY,
# EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, TO
# THE EXTENT PERMITTED BY LAW.  THIS SOFTWARE IS PROVIDED "AS IS".
# IN NO EVENT UNLESS REQUIRED BY LAW WILL COPYRIGHT HOLDER BE LIABLE FOR 
# DAMAGES OF ANY NATURE ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.
#..........................................................................

#
# Summarize slipd log entries.
# Show one line per session and cumulative on-time, for each slip interface.
#

require "getopts.pl";

# Default log file (if no files listed on command line)
$deflog = "/var/adm/slip.log";

# Connect time to assume for connections with no OFF record (e.g., sys crashes)
$default_et_secs = 120;

$opt_v = $verbose = 0;
&Getopts('v') || &usage();
$verbose = 1 if $opt_v;

if (@ARGV==0) { @ARGV = ("$deflog"); }

umask(0022);

$ENV{'PATH'} = '/usr/bin:/bin:/usr/sbin:/sbin';
$ENV{'IFS'} = '';
$ENV{'SHELL'} = '/bin/sh';

foreach $log (@ARGV) {
    print "SUMMARY OF $log\n\n";
    open(LOG,$log) || die "$log: $!\n";

    ($d,$d,$d,$d,$d,$curr_yr) = localtime(time);

    $first_date = "";
    $last_date = "";
    while ($_ = <LOG>) {

	($verbose) && print "Input:$_";

	chop;

	# Insert year in date if not present
	s@^(slipd[-\s]\s*\d+\s+)(\d+)/(\d+)(\s+\d+:)@\1\2/\3/$curr_yr\4@;

	if    (m@^slipd[-\s]\s*\d+\s+(\d+)/(\d+)/(\d+)\s+(\d+):+(\d+):*(\d*)\s+ON\s+([-\w]+)\s+\(([^)]*)\)@) {
	    ($month,$day,$yr,$hr,$min,$sec,$ifc,$dialcmd) 
						= ($1,$2,$3,$4,$5,$6,$7,$8);
	    if (defined($active{$ifc}) && $active{$ifc}) {
		printf STDOUT "ERROR (system crash?): Missing OFF record, assuming connect time = %s\n", &hms($default_et_secs);
	       &process_OFF
	    	($month,$day,$yr,$hr,$min,$sec,$ifc,
	       				int($default_et_secs / 60),
	       				int($default_et_secs % 60));
	    }
	    $active{ $ifc } = 1;
	    $date = &fmt_date($yr,$month,$day,$hr,$min,$sec);
	    if (!$first_date) { $first_date = $date; }
	    undef $dialcmd; #avoid warning with -w
	}

	elsif (m@^slipd[-\s]\s*\d+\s+(\d+)/(\d+)/(\d+)\s+(\d+):+(\d+):*(\d*)\s+OFF\s+([-\w]+)\s+\(elapsed\s+(\d+):(\d+)\)@) {
	    ($month,$day,$yr,$hr,$min,$sec,$ifc,$et_min, $et_sec) 
						= ($1,$2,$3,$4,$5,$6,$7,$8,$9);
	    if ( !(defined($active{$ifc}) && $active{$ifc}) ) {
		print "ERROR: Missing ON record ($ifc)\n";
	    }

	   &process_OFF
	    	($month,$day,$yr,$hr,$min,$sec,$ifc,$et_min, $et_sec) ;
	}

	else {
		($verbose) && print "(DISCARDED)\n";
	}
    }
    print "\nTOTAL CONNECT TIME (during the period $first_date to $last_date)\n";
    print '-' x 77, "\n";
    foreach $ifc (sort(keys(%total_times))) {
	$et = $total_times{$ifc};
	printf STDOUT "%6s %s (%d seconds) in %d connections\n", 
		$ifc, &hms($et), $et,
		$connection_count{$ifc};
    
    }
}

exit(0);

# Process an OFF record (possibly faked if missing from log file)
# Print a summary line for the connection
sub process_OFF
{
	local($month,$day,$yr,$hr,$min,$sec,$ifc,$et_min, $et_sec) = @_;
	local($date);

	(!defined($total_times{$ifc})) && ($total_times{$ifc} = 0);
	(!defined($connection_count{$ifc})) && ($connection_count{$ifc} = 0);

	$total_times{ $ifc } += (($et_min * 60) + $et_sec);
	$connection_count{ $ifc } += 1;
	$date = &fmt_date($yr,$month,$day,$hr,$min,$sec);
	printf STDOUT "%6s: OFF at %s, connect time %8s\n", 
		      $ifc, $date, &hms($et_min*60 + $et_sec);
	$last_date = $date;
	$active{$ifc} = 0;
}

# Format date & time into fixed-width string
sub fmt_date
{
	local($yr,$mon,$day,$hr,$min,$sec,$rest) = @_;
	(defined($rest) || !defined($sec)) && die "fmt_date arg err\n";
	return sprintf("%2d/%02d/%02d %2d:%02d:%02d", 
			$mon, $day, $yr, $hr, $min, $sec);
}

# Format seconds into MM:SS or HH:MM:SS
sub hms
{
	local($totsecs) = @_;
	if ($totsecs >= 3600) {
		return sprintf("%2d:%02d:%02d",
			       int($totsecs / 3600),
			       int(int($totsecs / 60) % 60),
			       int($totsecs % 60));
	} else {
		return sprintf("%2d:%02d",
			       int($totsecs / 60),
			       int($totsecs % 60));
	}
}

sub usage
{
	print STDERR "USAGE: $0 [-v] [logfile-default is $deflog]\n";
	exit 1;
}
