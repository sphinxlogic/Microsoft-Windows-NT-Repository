#!/usr/bin/taintperl -w
# @(#)slipcmd.pl	1.2 (13 Oct 1993)
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
# slipcmd -- run or schedule commands which use a slip link
#
# USAGE:
#	slipcmd [-s /dev/tslip] [-w] [-q] [-r] [-v] ifcname [cmd args...]
#
# Slipcmd runs or queues a command depending on whether a dial-up slip link 
# is currently established.
#
# -w Initiates a connection (if necessary), waits for it to be active,
#    and then runs the command.  This may be used to avoid timeout errors in 
#    programs which give up too quickly to wait for dial-out.
#
# -q causes the command to be deferred if the link is not currently established 
#    (i.e., no modem connection exists, even though IP believes the network
#    interface is up).  If the same command is attempted several times, only 
#    a single entry will be made in the deferred-work queue.
#
# -r causes any queued commands to be executed as with -w, regardless of 
#    initial link status.
#
# -v causes logging to stderr.
#
# Typically, work like polling a mail server is performed very frequently
# from cron using slipcmd -q, but occasionally using slipcmd -r (forcing 
# dial-out if necessary).
#
# To run deferred work as soon as the link is established for some other 
# reason, specify slipcmd -r as a "background command" in the parameter
# string given to slattach.  The slip dialing daemon (slipd) will then execute
# it whenever the link is established.
#
# EXIT VALUE:
#   0 if the command ran immediately (regardless of it's result).
#   1 if the command was queued (with -q) or discarded.
#   >1 if some error occurred.
# 
# Author: Jim Avera (jima@netcom.com)
#

require "getopts.pl";
require "shellwords.pl";
require "flush.pl";

$slipdpath = "@SBINDIR@/slipd";	#this is edited by Makefile

# Maximum time to wait for dialout
$linkwait_timeout_secs = 60;

# Maximum time a "meta" lock directory should exist (after which it is broken)
$max_meta_lock_age = 30;  #seconds

$ifcname = "sl0";
$qfiledir = "@SLIPCMD_SPOOLDIR@";

$verbose = 0;
$queue = 0;
$forcerun = 0;

$progname = "slipcmd";	# suid scripts don't know their name

$opt_v = $opt_q = $opt_r = $opt_w = 0;
$opt_s = $slippath = @SLIP_DEV_NAME_STR@;	# e.g., "/dev/tslip"
&Getopts('s:vqrw') || &usage();
$verbose = 1 	   if $opt_v;
$queue = 1 	   if $opt_q;
$forcerun = 1 	   if $opt_r;
$linkwait = 1   if $opt_w;
$slippath = $opt_s;

umask(0022);

$ENV{'PATH'} = '/usr/bin:/bin:/usr/sbin:/sbin';
$ENV{'IFS'} = '';
$ENV{'SHELL'} = '/bin/sh';

if (@ARGV < 1) { &usage(); }
&untaint( $ifcname = shift(@ARGV) );

$inuse = &check_inuse();
	
if (@ARGV) {
    # Form a single string for the queue file, containing:
    #   user/uid group/gid commandpath quoted-arg0 quoted-arg1 ...
    (($username)  = getpwuid($<)) || ($username = $<);
    (($groupname) = getgrgid($()) || ($groupname = $();
    $cmd = "$username $groupname $ARGV[0]";
    $ARGV[0] =~ s@.*/@@;
    while ($_ = shift @ARGV) {
        if (/\W/ && !/^\s*(['"]).*\1$/) {
    	$cmd .= " '$_'";
        } else {
    	$cmd .= " $_";
        }
    }

    if ($forcerun || $linkwait) {
    	# do it as though part of the queue we are about to run
        &runcmd($cmd);
    }
    elsif ($inuse) {
    	# The link is active - run it now
        &runcmd($cmd);
    } else {
        if ($queue) {
    	    # Put command on the deferred-work queue
    	    &queuecmd($ifcname, $cmd);
        } else {
    	    # Forget it completely
    	    print STDERR "$progname: $ifcname not active - cmd was not run\n"
    	        unless !$verbose;
        }
    }
}

if ($forcerun) { &runqueue($ifcname); }

exit(0) if $inuse;
exit 1;
# --- END OF MAIN PROGRAM ---


# Put a command string on the deferred-work queue, if not there already.
sub 
queuecmd {
	local($ifcn, $cmd) = @_;
	local($_, $qfilepath, *QFILE);
		
	&untaint( $ifcn );
	$qfilepath = "$qfiledir/$ifcn";

	&getlock("$qfilepath.L");
        if (open(QFILE,$qfilepath)) {
	    while (<QFILE>) {
		chop;
		if ($_ eq $cmd) {
		    print STDERR "$progname: cmd already on queue ($cmd)\n"
			unless (!$verbose);
		    close(QFILE);
		    &freelock("$qfilepath.L");
		    return;
		}
	    }
	}
	mkdir($qfiledir, 0755);
        open(QFILE,">>$qfilepath") || die "Can't create $qfilepath:$!";
	print(QFILE $cmd, "\n");
	close(QFILE);
	&freelock("$qfilepath.L");

	print STDERR "$progname: cmd deferred ($cmd)\n" unless (!$verbose);
}

# Run the stored queue
sub runqueue {
	local($ifcname) = @_;
	local($_,$qfilepath,*QFILE);
		
	$qfilepath = "$qfiledir/$ifcname";

	# Read the command strings into an associative array, thereby
	# eliminating any redundancies (created by processes which
	# did not use the proper locking protocol).

	&getlock("$qfilepath.L");
        if (open(QFILE,$qfilepath)) {
	    unlink $qfilepath;
	    &freelock("$qfilepath.L");
	    while (<QFILE>) {
		&runcmd($_);
	    }
	    close(QFILE);
	} else {
	    &freelock("$qfilepath.L");
	}
}

# Execute a command, after bringing up the connection (if necessary).
# The input string is encoded as user/uid group/gid progpath av0 cmdstring...
sub 
runcmd {
	local($cmd) = @_;
	local(@F,$uid,$gid,$path,$pid);
	&force_call();
	@F = &shellwords($cmd);
	if (@F < 4) {
	    printf STDERR "$progname: CORRUPT QUEUE RECORD:$cmd\n";
	    return 0;
	}
	$uid = shift @F;
	if ($uid !~ /^\d+$/) {
	    ($d,$d,$uid) = getpwnam($uid);
	    (defined($uid)) || die "Unknown user \"$uid\"\n";
	}
	$gid = shift @F;
	if ($gid !~ /^\d+$/) {
	    ($d,$d,$gid) = getgrnam($gid);
	    (defined($gid)) || die "Unknown user \"$gid\"\n";
	}
	$path = shift @F;
	print STDERR "$progname: RUN (u=$uid,g=$gid) $path @F\n" unless !$verbose;
	if ($pid = fork) {
	    # parent
	    (wait == $pid) || die("unexpected pid from wait\n");
	} else {
	    # child -- become the user and run the program
	    $) = $gid;
	    $> = $uid;
	    exec $path $F[0],@F[1..$#F];
	    die "Cant exec $path:$!\n";
	}
}

# If the link is not currently in use, ping the server host to start it up,
# and wait for the link to be established.  
# If it never comes up after a time out, terminate the program.
sub force_call
{
	local($_, $i, $cmd, $s, $server_ipaddr);

	# If currently active (determined in main), just return.
	($inuse) && return;

	# Find out the IP address of the other end of the slip link
	# The ifconfig command emits something like:
	#  sl0: flags=71<UP,POINTOPOINT,NOTRAILERS,RUNNING>
	#     inet 192.187.158.13 --> 192.187.158.254 netmask ffffff00 
	# where the second IP address is our idea of the server's addr.
	$cmd = "ifconfig $ifcname";
	print STDERR "...$cmd\n" unless (!$verbose);
	($s = `$cmd`) =~ s/\n/ /g;
	($? != 0) && die "Can't run ifconfig (not in ", $ENV{'PATH'}, " ?)\n";
	($server_ipaddr = $s) =~ s/.*\D(\d+\.\d+\.\d+\.\d+)\b.*/\1/;

	# Ping the server IP address, but don't wait for it to complete 
	# because this IP addr may not really exist (especially if the server
	# dynamically assigns it's end to an unused address).  Instead,
	# periodically check to see if the link is up.  If it doesn't come up
	# after $linkwait_timeout_secs, return anyway.
	$cmd = "ping $server_ipaddr >/dev/null 2>&1 &";  #background
	print STDERR "...$cmd\n" unless (!$verbose);
	system($cmd);

	for ($i=0; $i < $linkwait_timeout_secs; $i++) {
		if (&check_inuse()) {
			print STDERR "$ifcname has become active.\n" 
				unless (!$verbose);
			$inuse = 1;
			return;
		}
		sleep(1);
	}
	print STDERR "$ifcname could not be activated (busy or dialer err?)\n";
	exit(1);
}

# Check to see if the link is active.  Returns non-zero iff so.
sub check_inuse
{
	local($n);
	$n = system( "$slipdpath -s $slippath -c $ifcname" );
	if    ($n==0) 	     { return 1; }
	elsif ($n==(1 << 8)) { return 0; }
	else { die "$progname: $slipdpath failed ($n), exiting\n"; }
}

sub usage {
	print STDERR "USAGE:
  $progname -w ifcname cmd args...   
	(initiate a dial-out if necessary, wait for connection, then run cmd)

  $progname -q ifcname cmd args...
	(queue cmd if there is no active connection, else run immediately)

  $progname -r ifcname
	(run any queued commands as with -w, dialing out if necessary)

  Other options: -v (verbose),  -s slippath [default $slippath]
 
";
	exit(2);
}

sub
untaint {
	$_[0] =~ m/(.*)/;
	$_[0] = $1;
	return $1;
}

sub
getlock {
	local($path) = @_;
	local(*F, $lockpid, $n);
	# Perl doesn't support O_EXCL so we can't create a lock file only
	# if it doesn't already exist.  Therefore we create a directory
	# as a meta-lockfile, just to protect us while we write our pid
	# into the regular lock file.
	TRY: for (;;) {
	    &_meta_lock($path);
	    if (open(F, "+<$path")) {
		# File exists -- check to see if pid is valid
	        seek(F,0,0);
	        &untaint( $lockpid = <F> );
	        if ($lockpid =~ /^\s*\d+\s*$/) {
		    if (kill(0,$lockpid) || ($! =~ /owner|perm/)) {
		        # sent nop signal or got permission error
		        &_meta_unlock($path);
		        print STDERR "BUSY(pid $lockpid): $path\n"
			    unless !$verbose;
		        sleep 1;
		        next TRY;
		    } else {
		        print STDERR "STALE LOCK(pid $lockpid): $path\n"
			    unless !$verbose;
		    }
	        }
	        seek(F,0,0);
	    } else {
	        open(F, ">$path") 
	          || (&_meta_unlock($path), die "Can't create $path:$!\n");
	    }
	    printf F $$;
	    close(F);
	    &_meta_unlock($path);
	    # print STDERR "$progname: LOCKED $path\n" unless !$verbose;
	    last;
	}
}

sub
freelock {
	local($path) = @_;
	local($n);
	&_meta_lock($path);
	$n = unlink($path);
	&_meta_unlock($path);
	(!$n) && die "Can't unlink $path:$!\n";
	# print STDERR "$progname: UNLOCKED $path\n" unless !$verbose;
}

sub _meta_lock_sighand {
	$got_sig = $_[0];
	print STDERR "Got SIG$got_sig\n" unless !$verbose;
}
sub _meta_lock {
	local($lockpath) = @_;
	local($lockdir) = $lockpath . ".d";
	local($_,%sigs,@stat_result);

	$got_sig = 0;
	foreach $_ ('HUP', 'INT', 'QUIT', 'TERM', 'TSTP') {
	    (!defined $SIG{$_}) && next;
	    $sigs{$_} = $SIG{$_};
	    $SIG{$_} = '_meta_lock_sighand';
	}
	while (!mkdir($lockdir,0600)) {
	    ($! !~ m/exist/) && die "$lockdir:$!\n";
	    # If it's too old, break the lock
	    (@stat_result = stat($lockdir)) || next;
	    if ((time - $stat_result[9]) > $max_meta_lock_age) {
	        print STDERR "STALE META-LOCK: rmdir $lockdir ...\n" unless (!$verbose);
		rmdir $lockdir || die "$progname: Can't rmdir $lockdir: $!\n";
		next;
	    }
	    sleep 1;
	    if ($got_sig) {
	        foreach $_ (keys %sigs) { $SIG{$_} = $sigs{$_}; }
	        kill($$, $got_sig);
		print STDERR "$progname: SIG$got_sig did nothing! Aborting.\n";
		exit(1);
	    }
	}
	foreach $_ (keys %sigs) { $SIG{$_} = $sigs{$_}; }
}
sub _meta_unlock {
	local($lockpath) = @_;
	local($lockdir) = $lockpath . ".d";
	rmdir($lockdir) || die "rmdir $lockdir:$!\n";
}
