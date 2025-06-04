# General subroutines for Majordomo

# $Source: /sources/cvsrepos/majordomo/majordomo.pl,v $
# $Revision: 1.12.2.2.2.4 $
# $Date: 1995/01/07 17:34:27 $
# $Author: rouilj $
# $State: Exp $
# 
# $Header: /sources/cvsrepos/majordomo/majordomo.pl,v 1.12.2.2.2.4 1995/01/07 17:34:27 rouilj Exp $
# 
# $Locker:  $
# 
# updated to 1.21 with patch for LIST file handle.

package Majordomo;
$EPERM = 1;
$ENOENT = 2;
$ESRCH = 3;
$EEXIST = 17;
$EINVAL = 22;


#  Mail header hacking routines for Majordomo
#
#  Derived from:
#  Routines to parse out an RFC 822 mailheader
#     E. H. Spafford,  last mod: 11/91
#  
#  ParseMailHeader breaks out the header into an % array
#    indexed by a lower-cased keyword, e.g.
#       &ParseMailHeader(STDIN, *Array);
#	use $Array{'subject'}
#
#    Note that some duplicate lines (like "Received:") will get joined
#     into a single entry in %Array; use @Array if you want them separate
#    $Array will contain the unprocessed header, with embedded
#     newlines
#    @Array will contain the header, one line per entry
#
#  RetMailAddr tries to pull out the "preferred" return address
#    based on the presence or absence of various return-reply fields


#  Call as &ParseMailHeader(FileHandle, *array)

sub main'ParseMailHeader  ## Public
{
    local($save1, $save2) = ($*, $/);
    local($FH, *array) =  @_;
    local ($keyw, $val);

    %array = ();
# begin temporary
if ($main'majordomo_debug) {print "Parse Mail Header entered\n";}
# end temporary
    # force unqualified filehandles into callers' package
    local($package) = caller;
    $FH =~ s/^[^']+$/$package'$&/;

    ($*, $/) = (1, '');
    $array = $_ = <$FH>;
    s/\n\s+/ /g;
       
    @array = split('\n');
    foreach $_ (@array)
    {
	($keyw, $val) = m/^([^:]+):\s*(.*\S)\s*$/g;
# begin temporary
if ($main'majordomo_debug) {print "\$keyw =$keyw , \$val = $val\n";}
# end temporary
	$keyw =~ y/A-Z/a-z/;
	if (defined($array{$keyw})) {
	    $array{$keyw} .= ", $val";
	} else {
	    $array{$keyw} = $val;
	}
    }
    ($*, $/) = ($save1, $save2); 
}


#  VMS Mail header hacking routines for Majordomo
#
#  Derived from:
#  Routines to parse out an RFC 822 mailheader
#     Karol Zielonko   cloned from ParseMailHeader for port to VMS
#  
#  ParseVMSMailHeader breaks out the VMS mailheaders into an % array
#    indexed by a lower-cased keyword, e.g.
#       &ParseVMSMailHeader(STDIN, *Array);
#	use $Array{'subject'}
#
#    $Array will contain the unprocessed header, with embedded
#     newlines
#    @Array will contain the header, one line per entry
#
#  RetMailAddr tries to pull out the "preferred" return address
#    based on the presence or absence of various return-reply fields


#  Call as &ParseVMSMailHeader(FileHandle, *array)

sub main'ParseVMSMailHeader  ## Public
{
    local($save1, $save2) = ($*, $/);
    local($FH, *array) =  @_;
    local ($keyw, $val);

    %array = ();
# begin temporary
if ($main'majordomo_debug)
  {
  print "ParseVMSMailHeader entered\n";
  print "\$FH is $FH\n";
  }
# end temporary
    # force unqualified filehandles into callers' package
    local($package) = caller;
    $FH =~ s/^[^']+$/$package'$&/;

# begin temporary
if ($main'majordomo_debug) {print "\$FH after munging is $FH\n";}
# end temporary
    ($*, $/) = (1, '');
    $array = $_ = <$FH>;
    s/\n\s+/ /g;
       
    @array = split('\n');
    foreach $_ (@array)
    {
# begin temporary
if ($main'majordomo_debug) {print "\$_ = $_\n";}
# end temporary
	($keyw, $val) = m/^([^:]+):\s*(.*\S)\s*$/g;
# begin temporary
if ($main'majordomo_debug) {print "\$keyw >>>$keyw<<<\$val >>>$val<<<\n";}
# end temporary
        #
        # Handle header with blank value. eg a blank CC: header.
        # VMS is different from Unix. If the CC: header is blank
        # You'll get a CC: line with a Tab after the "CC:" but no
        # value. The existing code can't parse this. I'm guessing the reg exp
        # above that assigns $key and $val could be changed to accomodate this
        # but I am too green at Perl at this point to do it. So hack away...
        if (!$keyw)
          {
          ($keyw) = m/(^[^:]+)/;
          if ($main'majordomo_debug) {print "\$keyw is >>>$keyw<<<\n";}
          }
	$keyw =~ y/A-Z/a-z/;
	if (defined($array{$keyw})) {
	    $array{$keyw} .= ", $val";
	} else {
	    $array{$keyw} = $val;
	}
    }
    ($*, $/) = ($save1, $save2); 
}


#  Call as $addr = &RetMailAddr(*array)
#    This assumes that the header is in RFC 822 format

sub main'RetMailAddr  ## Public
{
    local(*array) = @_;

    local($ReplyTo) = defined($array{'reply-to'}) ?
		$array{'reply-to'} : $array{'from'};

    $ReplyTo = $array{'apparently-from'} unless $ReplyTo;
    $ReplyTo = $array{'return-path'} unless $ReplyTo;

    join(", ", &main'ParseAddrs($ReplyTo)) if $ReplyTo;
    $ReplyTo;
}


# @addrs = &ParseAddrs($addr_list)
sub main'ParseAddrs {
    local($_) = shift;
    if ($main'majordomo_debug) {print "ParseAddrs entered with addr $_\n";}
    1 while s/\([^\(\)]*\)//g; 		# strip comments
    if ($main'majordomo_debug) {print "ParseAddrs addr after first comment strip is $_\n";}

#
# HACK ALERT!!!
# For non-SMTP addressing compatability check for quoted local part which will
# tip us off that the address has come from a domain oustide SMTP and been
################################################################################
# "SMTPfied". If this is the case we return the address at this point. It would
# be better to call UCX$SMTP_PARSE_ADDRESS here somehow but can't do
# Perl extensions yet.
#
# PS. This could very well turn out to be the wrong way/place to do this.
# If you think you know a better way don't be afraid to change it. I wanted to
# put this hack in the lowest level routine possible so every time we want
# to parse an address we get the "benefit".
#
    local($quoted_local_part) = $_;
    $quoted_local_part =~ /\"(.*)\"\@/;
    $quoted_local_part = $1;

    if (($main'majordomo_debug) && ($quoted_local_part))
      {print "ParseAddrs found quoted local part $quoted_local_part\n"}

    #
    # Any quoted local part will be passed automatically.
    if ($quoted_local_part)
      {
      if ($main'majordomo_debug)
        {
        print "ParseAddrs found quoted local part $quoted_local_part\n";
        print "ParseAddrs assuming address containing quoted local part is valid without parsing further\n";
        }
      #
      # Strip out leading and trailing white space.
      s/^\s+//;
      s/\s+$//;
      @_ = $_;
      return @_;
      }

    1 while s/"[^"]*"//g;		# strip comments
    if ($main'majordomo_debug) {print "ParseAddrs addr after second comment strip is $_\n";}
    split(/,/);				# split into parts
    if ($main'majordomo_debug) {print "ParseAddrs addr after split is $_\n";}

    foreach (@_) {
	1 while s/.*<(.*)>.*/\1/;
	s/^\s+//;
	s/\s+$//;
    }

    @_;
}


# Check to see if a list is valid.  If it is, return the validated list
# name; if it's not, return ""
sub main'valid_list {
    local($listdir) = shift;
    # start with a space-separated list of the rest of the arguments
    local($taint_list) = join(" ", @_);
if ($main'majordomo_debug)
  {
  print "valid_list: entered. \n";
  print "valid_list: \$listdir is $listdir \n";
  print "valid_list: \$taint_list is $taint_list \n";
  }
    # strip harmless matched leading and trailing angle brackets off the list
    1 while $taint_list =~ s/^<(.*)>$/\1/;
#if ($main'majordomo_debug) {print "valid_list: \$taint_list is $taint_list \n";}
    # strip harmless trailing "@.*" off the list
    $taint_list =~ s/@.*$//;
#if ($main'majordomo_debug) {print "valid_list: \$taint_list is $taint_list \n";}
    # anything else funny with $taint_list probably isn't harmless; let's check
    # start with $clean_list the same as $taint_list
    local($clean_list) = $taint_list;
#if ($main'majordomo_debug) {print "valid_list: \$clean_list is $clean_list \n";}
    # clean up $clean_list
    $clean_list =~ s/[^-_0-9a-zA-Z]*//g;
#if ($main'majordomo_debug) {print "valid_list: \$clean_list is $clean_list \n";}
    # if $clean_list no longer equals $taint_list, something's wrong
    if ($clean_list ne $taint_list) {
	return ""; 
    } 
    # convert to all-lower-case
    $clean_list =~ tr/A-Z/a-z/;
#if ($main'majordomo_debug) {print "valid_list: \$clean_list is $clean_list \n";}
    # check to see that $listdir/$clean_list exists
    if (! -e "$listdir/$clean_list") {
	return "";
    }
    return $clean_list;
}


#
# New routine for VMS port
# Check to see if a list name is valid.  If it is, return the validated list
# name; if it's not, return ""
sub main'line_is_comment {
local($line) = shift;
local($tmp_line) = $line;

# strip leading space.
$tmp_line =~ s/^\s+//;

# If it starts with a "!" then it's a comment. Otherwise no.
if ($tmp_line =~ /^\!/)
  {return 1;}
else
  {return 0;}
}


#
# New routine for VMS port
# Check to see if a list name is valid.  If it is, return the validated list
# name; if it's not, return ""
sub main'valid_list_name {
    # start with a space-separated list of the rest of the arguments
    local($taint_list) = join(" ", @_);
if ($main'majordomo_debug)
  {
  print "valid_list_name: entered. \n";
  print "valid_list_name: \$taint_list is $taint_list \n";
  }
    # strip harmless matched leading and trailing angle brackets off the list
    1 while $taint_list =~ s/^<(.*)>$/\1/;
if ($main'majordomo_debug) {print "valid_list_name: \$taint_list is $taint_list \n";}
    # strip harmless trailing "@.*" off the list
    $taint_list =~ s/@.*$//;
if ($main'majordomo_debug) {print "valid_list_name: \$taint_list is $taint_list \n";}
    # anything else funny with $taint_list probably isn't harmless; let's check
    # start with $clean_list the same as $taint_list
    local($clean_list) = $taint_list;
if ($main'majordomo_debug) {print "valid_list_name: \$clean_list is $clean_list \n";}
    # clean up $clean_list
    $clean_list =~ s/[^-_0-9a-zA-Z]*//g;
if ($main'majordomo_debug) {print "valid_list_name: \$clean_list is $clean_list \n";}
    # if $clean_list no longer equals $taint_list, something's wrong
    if ($clean_list ne $taint_list) {
	return ""; 
    } 
    # convert to all-lower-case
    $clean_list =~ tr/A-Z/a-z/;
if ($main'majordomo_debug) {print "valid_list_name: \$clean_list is $clean_list \n";}
    return $clean_list;
}


#
# New routine for VMS port
# Check to see if a list already exists. Assume that input listname has been
# validated by valid_list_name. We return 1 if list exists and 0 if not.
sub main'list_exists {
    local($listdir) = shift;
    local($clean_list) = shift;

if (-e "$listdir/$clean_list")
  {
  if ($main'majordomo_debug) {print "List $clean_list in directory $listdir exists\n";}
  return 1;
  }
else
  {
  if ($main'majordomo_debug) {print "List $clean_list in directory $listdir does not exist\n";}
  return 0;
  }
}


# compare two email address to see if they "match" by converting  to all
# lower case, then stripping off comments and comparing what's left.  If
# a optional third argument is specified and it's not undefined, then
# partial matches (where the second argument is a substring of the first
# argument) should return true as well as exact matches.
#
# if optional third argument is 2, then compare the two addresses looking
# to see if the addresses are of the form user@dom.ain.com and user@ain.com
# if that is the format of the two addresses, then return true.
sub main'addr_match {
    local($a1) = &main'chop_nl(shift);
    local($a2) = &main'chop_nl(shift);
    local($partial) = shift;	# may be "undef"

    if ($partial == 1) {
	$a1 =~ tr/A-Z/a-z/;
	$a2 =~ tr/A-Z/a-z/;
	if (index($a1, $a2) >= $[) {
	    return(1);
	} else {
	    return(undef);
	}
    }
	
    local(@a1, @a2);

    $a1 =~ tr/A-Z/a-z/;
    $a2 =~ tr/A-Z/a-z/;

    @a1 = &main'ParseAddrs($a1);
    @a2 = &main'ParseAddrs($a2);
    if (($#a1 != 0) || ($#a2 != 0)) {
	# Can't match, because at least one of them has either zero or
	# multiple addresses
	return(undef);
    }

    if ($partial == 2) { # see if addresses are foo@baz.bax.edu, foo@bax.edu
       local(@addr1,@addr2);
	  @addr1 = split(/@/, $a1[0]);
	  @addr2 = split(/@/, $a2[0]);
	  if ( $#addr1 == $#addr2 && $#addr1 == 1 && 
               $addr1[0] eq $addr2[0] && (index($addr1[1], $addr2[1]) >= $[))
	  {
	    return(1);
	  } else {
	    return(undef);
	  }
       }

    return($a1[0] eq $a2[0]);
}


# These are package globals referenced by &setabortaddr and &abort

$abort_addr = "Postmaster";

sub main'set_abort_addr {
    $abort_addr = shift unless ($#_ < $[);
}


# Abort the process, for the reason stated as the argument
sub main'abort {
    # log the reason for the abort, if possible
    &main'log("ABORT", join(" ", @_), "\n");
    # print the reason for the abort to stderr; maybe someone will see it
    print STDERR join(" ", @_), "\n";
    # send a message to the Majordomo owner, if possible
    if (defined($abort_addr)) {
	&main'sendmail(ABORT, $abort_addr, "MAJORDOMO ABORT");
	print ABORT <<"EOM";

MAJORDOMO ABORT

@_

EOM
	&main'close_and_send(ABORT);
    }
    die("ABORT ", @_);
}


# These are package globals referenced by &setlogfile and &log
$log_file = "/tmp/log.$$";
$log_host = "UNKNOWN";
$log_program = "UNKNOWN";
$log_session = "UNKNOWN";

# set the log file
sub main'set_log {
    $log_file = shift unless ($#_ < $[);
    $log_host = shift unless ($#_ < $[);
    $log_program = shift unless ($#_ < $[);
    $log_session = shift unless ($#_ < $[);
if ($main'majordomo_debug)
  {
  print "\$log_file is $log_file\n";
  print "\$log_host is $log_host\n";
  print "\$log_program is $log_program\n";
  print "\$log_session is $log_session\n";
  }
}


# Log a message to the log
sub main'log {
    local($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime;
    if (&main'lopen(LOG, ">>", $log_file)) {
	# if the log is open, write to the log
	printf LOG "%s %02d %02d:%02d:%02d %s %s[%d] {%s} ",
	    $ctime'MoY[$mon], $mday, $hour, $min, $sec,
	    $log_host, $log_program, $$, $log_session;
	print LOG join(" ", @_), "\n";
	&main'lclose(LOG);
    } else {
	# otherwise, write to stderr
	printf STDERR "%s[%d] {%s} ", $log_program, $$, $log_session;
	print STDERR join(" ", @_), "\n";
    }
}

# Globals referenced by &set_mail* and &sendmail
$tmp = "/SYS\$SCRATCH/MAJORDOMO_$$";
$mail_prog = "/usr/lib/sendmail -f\$sender -t";
$mail_from = "Majordomo";
$mail_sender = "Majordomo-Owner";
%mail_tmpfile_names = ();
$my_system_name = &main'systemname;

# set the mailer
sub main'set_mailer {
     $mail_prog = shift;
}


# set the default from address
sub main'set_mail_from {
    $mail_from = shift;
}


# set the default sender address
sub main'set_mail_sender {
    $mail_sender = shift;
}


sub main'hostname {
local($hostname);
$hostname = $ENV{'UCX$INET_HOST'} . "." . $ENV{'UCX$INET_DOMAIN'};
$hostname =~ tr/A-Z/a-z/;
return($hostname);
}


sub main'systemname {
local($systemname);
$systemname = $ENV{'UCX$INET_HOST'};
$systemname =~ tr/A-Z/a-z/;
return($systemname);
}


# 
# If DECnet not installed returns inet name. This is a hack.
sub main'decnet_nodename {
local($decnet_nodename);
$decnet_nodename = `write sys\$output f\$getsyi("NODENAME")`;
chop($decnet_nodename);

if ((!$decnet_nodename) ||
    ($decnet_nodename =~ m/-W-/i) ||
    ($decnet_nodename =~ m/-E-/i) ||
    ($decnet_nodename =~ m/-F-/i))
  {$decnet_nodename = &main'systemname;}

$decnet_nodename =~ tr/a-z/A-Z/;
return($decnet_nodename);
}

sub main'dump_tmpfile_array {
local($mykey);
local($myval);
print "Dump of mail_tmpfile_names associative array:\n";
while (($mykey,$myval) = each(%mail_tmpfile_names))
  {
  print "key is $mykey value is $myval\n";
  }
}


# Open a sendmail process
sub main'sendmail {
    local($MAIL) = shift;
    local($to) = shift;
    local($subject) = shift;
    local($from) = $mail_from;
    local($sender) = $mail_sender;
# begin temporary
if ($main'majordomo_debug)
  {
  print "\$sender is $sender\n";
  print "\$MAIL is $MAIL\n";
  }
# end temporary

    if ($#_ >= $[) { $from = shift; }
    if ($#_ >= $[) { $sender = shift; }
# begin temporary
if ($main'majordomo_debug) {print "\$sender is $sender\n";}
# end temporary

    # force unqualified filehandles into callers' package
    local($package) = caller;
    local($save_mail) = $MAIL;
    $MAIL =~ s/^[^']+$/$package'$&/;
    local($tmpfile_name);
# begin temporary
if ($main'majordomo_debug) {print "\$MAIL is $MAIL\n";}
if ($main'majordomo_debug) {print "\$tmp is $tmp\n";}
# end temporary

    # clean up the addresses, for use on the sendmail command line
    local(@to) = &main'ParseAddrs($to);
    for (@to) {
	$_ = join(", ", &main'ParseAddrs($_));
    }
    $to = join(", ", @to);

    # open the process
    # generate the header.  Note the line beginning with "-"; this keeps
    # this message from being reprocessed by Majordomo if some misbegotten
    # mailer out there bounces it back.
    $tmpfile_name = $tmp  . "_" . $my_system_name . "_" . $save_mail . ".TMP";
    $mail_tmpfile_names{$save_mail} = $tmpfile_name;
# begin temporary
if ($main'majordomo_debug)
  {
  print "Opening $tmpfile_name for write access.\n";
  &main'dump_tmpfile_array;
  }
# end temporary
    open($MAIL, ">$tmpfile_name");
#
# Before we write this need to add in MAIL FROM, RCPT TO and DATA lines
# What about date:, and message-id headers? Maybe unix puts them in
# automatically in which case we're going to need to enhance sff to do the
# same.
################################################################################
    print $MAIL "MAIL FROM:<$sender>\n";
    print $MAIL "RCPT TO:<$to>\n";
    print $MAIL "DATA\n";
    print $MAIL 
"To: $to
From: $from
Subject: $subject
Reply-To: $from

--

";
    
    return;
}


#
# New routine for VMS port
sub main'close_and_send {
    local($MAIL) = shift;
    # force unqualified filehandles into callers' package
    local($package) = caller;
    local($save_mail) = $MAIL;
    $MAIL =~ s/^[^']+$/$package'$&/;
    local($tmpfile_name) = $mail_tmpfile_names{$save_mail};

# begin temporary
if ($main'majordomo_debug)
  {
  print "\$save_mail is $save_mail\n";
  print "Value in associative array is $mail_tmpfile_names{$save_mail}\n";
  print "Closing $tmpfile_name\n";
  }
# end temporary
    close($MAIL);

    # Delete element for this file from associative array.
    delete $mail_tmpfile_names{$save_mail};

if ($main'majordomo_debug) {    &main'dump_tmpfile_array;}

    ($fid) = (stat(_))[1];

# begin temporary
if ($main'majordomo_debug) {print "BTW \$fid for temp file was $fid\n";}
# end temporary
    # Pass temp msg file to symbiont to be delivered.
    &main'pass_to_symbiont($tmpfile_name);

    # Delete all temp files
# begin temporary comment out until this mechanism is working
#    &main'delete_tmpfiles;
# end temporary

#    &main'log("Failed to exec mailer \"@_\": $!");
#    die("Failed to exec mailer \"@_\": $!");
}


#
# New routine for VMS port
# Pass the mail to the symbiont using the SFF foreign command in a subprocess.
# It would be better to do ths with Perl extension I think.
sub main'pass_to_symbiont {
    local($mail_tmpfile_name) = shift;
local($status_str);
if ($main'majordomo_debug) {print "Passing temp file $mail_tmpfile_name to symbiont with SFF and backticks.\n";}

if ($main'majordomo_debug >= 10)
  {
  $status_str = `SFF $mail_tmpfile_name -logfile majordomo_sff.log -loglevel 1`;
  }
else
  {
  $status_str = `SFF $mail_tmpfile_name`;
  }
if ($main'majordomo_debug)
  {
  if (defined($status_str))
    {
    if ($status_str eq "")
      {print "\$status_str is empty string.\n";}
    else
      {print "\$status_str is >>>$status_str<<<\n";}
    }
  else
    {print "\$status_str is not defined.\n";}
  }
#
# Need to check for errors here.
#    &main'log("Failed to exec mailer \"@_\": $!");
#    die("Failed to exec mailer \"@_\": $!");
}


#
# New routine for VMS port
sub main'make_version_one
{
local($filespec) = shift;
local($thisfile);
local($status) = 1;
$filespec = VMS::Filespec::vmsify($filespec);

$! = ();
if ($main'majordomo_debug) {print "vmsified file spec in make_version_one is $filespec\n";}

#
# Input filespec must not contain version number already
$_ = $filespec;
if (/;/)
  {
  $! = $EINVAL;		# "invalid argument"
  warn "make_version_one: input param $filespec contains ';'";
  return(0);
  }

#
# Must not be more than one version of this file. If there is then renaming it
# to version one could make an older version of the file version ;0. If they
# really want to do this they can with a straight rename call but that is not
# the purpose of this routine.
local($filespec_wild) = $filespec . ";*";
local($filecnt) = 0;
if ($main'majordomo_debug) {print "wildcard file spec in make_version_one is $filespec_wild\n";}
while ($thisfile = <${filespec_wild}>)
  {
  $filecnt++;
  if ($main'majordomo_debug) {print "file count is $filecnt\n";}
  }

if ($filecnt != 1)
  {
  if ($main'majordomo_debug) {print "file count is $filecnt\n";}
  if ($filecnt == 0)
    {$! = $ENOENT;}	# No such file or directory
  else
    {$! = $EINVAL;}	# invalid argument
  warn "make_version_one: must be one and only one version of $filespec to rename ;0 to ;1";
  return(0);
  }

local($filespec_topver) = $filespec . ";0";
local($filespec_ver1) = $filespec . ";1";
$status = rename($filespec_topver, $filespec_ver1);
if (!$status)
  {
  warn "Couldn't rename $filespec_topver to $filespec_ver1";
  return(0);
  }
1;
}


#
# New routine for VMS port
sub main'purge {
local($filespec) = shift;
local($first_time) = 1;
local($thisfile);
local($status) = 1;
$filespec = VMS::Filespec::vmsify($filespec);

$filespec_wild = $filespec . ";*";

$! = ();
if ($main'majordomo_debug) {print "purging filespec after munge is $filespec_wild\n";}
while ($thisfile = <${filespec_wild}>)
  {
  if ($main'majordomo_debug) {print "purge processing file $thisfile\n";}
  if (!$first_time)
    {
    if ($main'majordomo_debug) {print "deleting file $thisfile\n";}
    unlink($thisfile) || return(0);
    }
  else
    {
    if ($main'majordomo_debug) {print "leaving file $thisfile\n";}
    }

  $first_time = 0;
  }

$status = &main'make_version_one("$filespec");
if (!$status)
  {
  warn "Couldn't set $filespec version number back to ;1\n";
  return(0);
  }
1;
}


#
# New routine for VMS port
sub main'delete_tmpfiles {
local($fileglob) = $tmp . "*";
local(@fileglob) = ($fileglob);
local($filespec);
local($thisfile);
local($status) = 0;

if ($main'majordomo_debug)
  {
  print "delete_tmpfiles entered.\n";
  print "\$fileglob is $fileglob\n";
  }

foreach $filespec (@fileglob)
  {
  if ($main'majordomo_debug) {print "\$filespec is $filespec\n";}
  while ($thisfile = <${filespec}>)
    {
    if ($main'majordomo_debug) {print "Processing file $thisfile\n";}
    $! = "";
    ($status = unlink($thisfile)) || warn "Can't delete $thisfile\n";
    #
    # If the unlink didn't work print reason (in $! special symbol).
    if (!$status)
      {print STDERR "$!\n";}
    }
  }
if ($main'majordomo_debug) {print "delete_tmpfiles exiting.\n";}
}


# check the password for a list
sub main'valid_passwd {
    local($listdir) = shift;
    local($list) = shift;
    local($passwd) = shift;
    # is it a valid list?
    local($clean_list) = &main'valid_list($listdir, $list);
    if ($clean_list ne "") {
	# it's a valid list check config passwd first 
        if (defined($main'config_opts{$clean_list,"admin_passwd"}) &&
            $passwd eq $main'config_opts{$clean_list,"admin_passwd"} ) 
	      {	return 1; }

	# read the password from the file in any case
	if (&main'lopen(PASSWD, "", "$listdir/$clean_list.passwd")) {
	    local($file_passwd) = <PASSWD>;
	    &main'lclose(PASSWD);
	    $file_passwd = &main'chop_nl($file_passwd);
	    # got the password; now compare it to what the user sent
	    if ($passwd eq $file_passwd) {
		return 1;
	    } else {
		return 0;
	    }
	} else {
	    return 0;
	}
    } else {
	return 0;
    }
}


# get the password for a list
sub main'get_passwd {

local($listdir) = shift;
local($list) = shift;

# is it a valid list?
local($clean_list) = &main'valid_list($listdir, $list);
if ($clean_list ne "")
  {
  # it's a valid list check config passwd first. First
  # parse its config file if needed.
  if (!&main'cf_ck_bool($clean_list, '', 1))
    {&main'get_config($listdir, $clean_list);}

  if (defined($main'config_opts{$clean_list,"admin_passwd"}))
    {return $main'config_opts{$clean_list,"admin_passwd"};}

  # read the password from the file in any case
  if (&main'lopen(PASSWD, "", "$listdir/$clean_list.passwd"))
    {
    local($file_passwd) = <PASSWD>;
    &main'lclose(PASSWD);
    $file_passwd = &main'chop_nl($file_passwd);
    return $file_passwd;
    }
  else
    {return undef;}
  }
else
  {return undef;}
}


# Check to see that this is a valid address. 
# A valid address is a single address with 
# no "/" or "|" in the address part.

sub main'valid_addr {
    local($addr) = shift;
    local(@addrs);

    if ($main'majordomo_debug) {print "valid_addr entered with addr $addr\n";}

    # Parse the address out into parts
    @addrs = &main'ParseAddrs($addr);

    if ($main'majordomo_debug)
      {
      foreach (@addrs)
        {print "addr back from ParseAddrs is $_\n";}
      }

    # if there's not exactly 1 part, it's no good
    if ($#addrs != 0) {
	return undef;
    }

    local($_) = @addrs;

    if ( $main'analyze_slash_in_address ) {
       # if there's a "|" in it, it's hostile
       if ( /\|/ ) {
           &main'abort("HOSTILE ADDRESS $addr pipe in address"); #'
           return undef;
       }
 
       # if the is a / in it, it may be an attempt to write to a file.
       # or it may be an X.400, HP Openmail or some other dain bramaged
       # address 8-(. We check this by breaking the address on '/'s
       # and checking to see if the first component of the address
       # exists in the filesystem. If it does we bounce it as a hostile
       # address.
 
       if ( m#/# ) {
          local(@components) = split( /\//, $_);
 
          # strip null element for / at beginning of address
	  # since the "/" directory always exists.
	  shift(@components) if ( $components[0] eq "" );

          &main'abort("HOSTILE ADDRESS $addr /$components[0] exists") 
                                    if (-e "/$components[0]"); #'
          &main'abort("HOSTILE ADDRESS $addr $components[0] exists")
                                    if (-e "$components[0]"); #'
 
          # then as an extra check that can be turned off in the majordomo.cf
          # file we make sure that the last component of the address has an
          # @ sign on it for an X.400->smtp gateway translation.
 
          if (!$main'no_x400at) {  
              print "$components[$#components]\n"
	      if ( "$components[$#components]" !~ /@/);  #'
              &main'abort("HOSTILE ADDRESS $addr no @ in last component")
	      if ( "$components[$#components]" !~ /@/);  #'
          } #'
 
          # check to see that the c= and a[dm]= parts exist in the X.400
	  # address
          if (!$main'no_true_x400) { 
             &main'abort("HOSTILE ADDRESS $addr no /c=") if ($_ !~ m#/c=#i); #'
             &main'abort("HOSTILE ADDRESS $addr no /a[dm]=") 
                            if ($_ !~ m#/a[dm]=#i); #'
	  } #'
	}
    } else {
        # if there's a "|" or a "/" in it, it's hostile
        if (tr/|\//|\// != 0) {
	    &main'abort("HOSTILE ADDRESS $addr"); #'
	    return undef;
        }
    }

    # if there is a - sign at the front of the address, it may be an attempt
    # to pass a flag to the MTA
    if ( /^-/ ) {
        &main'abort("HOSTILE ADDRESS $addr"); # '
        return undef;
    }

    return $_;
}


# is this a valid filename?
sub main'oldvalid_filename {
    local($directory) = shift;
    local($list) = shift;
    local($suffix) = shift;
    local($taint_filename) = shift;
    local($clean_filename);

    if ($main'majordomo_debug)
      {
      print "valid_filename entered \n";
      print "\$directory is $directory \n";
      print "\$list is $list \n";
      print "\$suffix is $suffix \n";
      print "\$taint_filename is $taint_filename \n";
      }

    # Safety check the filename.
    if ($taint_filename =~ /^[\/.]|\.\.|[^-_0-9a-zA-Z.\/] /) {
        if ($main'majordomo_debug) { print "valid_filename returning undef. Filename tainted.\n";}
	return undef;
    } else {
	$clean_filename = $taint_filename;
    }

    if ($main'majordomo_debug) { print "valid_filename after filter is $clean_filename \n";}
    $clean_filename = "$directory/$list$suffix/$clean_filename";
    if (! -e "$clean_filename") {
        if ($main'majordomo_debug) { print "valid_filename returning undef. File \"$clean_filename\" doesn't exist.\n";}
	return undef;
    }
    if ($main'majordomo_debug) { print "valid_filename returning $clean_filename \n";}
    return "$clean_filename";
}


# is this a valid VMS filename?
sub main'valid_filename {
    local($directory) = shift;
    local($list) = shift;
    local($suffix) = shift;
    local($taint_filename) = shift;
    local($clean_filename);

if ($main'majordomo_debug)
  {
  print "valid_filename entered \n";
  print "\$directory is $directory \n";
  print "\$list is $list \n";
  print "\$suffix is $suffix \n";
  print "\$taint_filename is $taint_filename \n";
  }

# Safety check the filename.
# Filename includes a '/'. (Just in case the next check doesn't spot every
# case.)
if ($taint_filename =~ m,/,)
  {
  if ($main'majordomo_debug) { print "valid_filename returning undef. Filename tainted. Looks like Unix filename\n";}
  return(undef);
  }

$clean_filename = $taint_filename;

if ($main'majordomo_debug) { print "valid_filename after Unix filters is $clean_filename \n";}

return($clean_filename);
}


# Chop any trailing newlines off of a string, and return the string
sub main'chop_nl {
    if ($#_ >= $[) {
	local($x) = shift;
	$x =~ s/\n+$//;
	return($x);
    } else {
	return(undef);
    }
}

sub main'is_list_member {
    local($subscriber) = shift;
    local($listdir) = shift;
    local($clean_list) = shift;
    local($matches);
    local(*LIST);
    local($_);
# begin temporary
if ($main'majordomo_debug) {print "is_list_member entered\n";}
# end temporary
    open(LIST, "$listdir/$clean_list")
	|| &main'abort("Can't read $listdir/$clean_list: $!");
    while (<LIST>) {
        # skip comment lines for compatability with UCX SMTP comments
        if (&main'line_is_comment($_)) {next;}
	if (&main'addr_match($subscriber, $_, 
	   (&main'cf_ck_bool($clean_list,"mungedomain") ? 2 : undef))) {
	    $matches++;
	}
    }
    close(LIST);
# begin temporary
if ($main'majordomo_debug)
  {
  if ($matches)
    {print "$subscriber is member of list $clean_list $matches times\n";}
  else
    {print "$subscriber is not member of list $clean_list\n";}
  }
# end temporary
    return($matches);
}


sub main'open_temp {
    local($FH_name, $filename) = @_;
    local($inode1, $inode2, $dev1, $dev2) = ();

if ($main'majordomo_debug) {print "open_temp entered. filename $filename \n";}

    # force unqualified filehandles into callers' package
    local($package) = caller;
    $FH_name =~ s/^[^']+$/$package'$&/;

    if ( -e $filename ) {
	&main'abort("Failed to open temp file $filename, it exists");
    }
if ($main'majordomo_debug) {print "OK. filename doesn't exist.\n";}

    open($FH_name, ">> $filename") || 
	&main'abort("open of temp file $filename failed\n $!");

if ($main'majordomo_debug) {print "OK. open worked.\n";}
1;
}

1;
