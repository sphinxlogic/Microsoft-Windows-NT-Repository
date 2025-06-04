# PERL implementation of Erik E. Fair's 'shlock' (from the NNTP distribution)
# Ported by Brent Chapman <Brent@GreatCircle.COM>
# Ported from Unix to VMS by Karol Zielonko
#
package shlock;

$EPERM = 1;
$ESRCH = 3;
$EEXIST = 17;

$my_system_name = &main'systemname;

$max_retries = 600;


#++
#   FUNCTION NAME:
#
#     main'shlock
#
#  FUNCTIONAL DESCRIPTION:
#
#      Tries to get a VMS style "shlock lock".
#
#  FORMAL PARAMETERS
#
#     $file	(I)	Name of file to make our lock file. This is not-unlike
#                       the VMS lock "resource name".
#     $SHLOCK_FH(I)	Filehandle of lock file. Caller needs to save this so it
#                       can close the file later. In Unix land the call to
#                       unlink the file will close it first but in VMS land if
#                       you try to unlink a file you still have open you'll get
#                       "file locked by another user".
#
#  IMPLICIT INPUTS:
#
#      none
#
#  IMPLICIT OUTPUTS:
#
#      none
#
#  SIDE EFFECTS:
#
#      none
#
#  RETURN VALUE:
#
#      none
#--
sub main'shlock	## Public
{
local($file) = shift;
local($SHLOCK_FH) = shift;
local($retcode) = 0;
local($done) = 0;
local($status) = 0;
local($tmp_string);

# force unqualified filehandles into callers' package
local($package) = caller;
$SHLOCK_FH =~ s/^[^']+$/$package'$&/;

$file = VMS::Filespec::vmsify($file);
$file .= ";1";
if ($main'majordomo_debug)
  {print STDERR "shlock trying lock \"$file\" \n";}

while (!$done)
  {
  $status  = open($SHLOCK_FH,">$file");
  if ($status)
    {
    if ($main'majordomo_debug)
      {
      print STDERR "shlock got lock. File is $file \n" if $main'majordomo_debug;
      printf(STDERR "PID: %X, System: %s, Package: %s, Calling script: %s\n",
           $$, $my_system_name, $package, $0);
      }
    printf($SHLOCK_FH "PID: %X, System: %s, Package: %s, Calling script: %s\n",
           $$, $my_system_name, $package, $0);

    # Flush to disk. More for debugging purposes than anything else
    select((select($SHLOCK_FH), $| = 1)[0]);
    $retcode = 1;
    $done = 1;
    }
  else
    {
    if ($! == $EEXIST)
      {
      print "shlock file $file exists\n" if $main'majordomo_debug;
      #
      # Try to unlink the file. If we can unlink it then we know noone else has
      # it open which means it's an invalid lock. If it's an invalid lock we try
      # to open the file again.
      $status = unlink($file);
      if (!$status)
        {
        # Possible enhancement???
        # If lock is ours already then return 1. Ie. grant the lock???
        if ($main'majordomo_debug)
          {
          print STDERR "Existing lock is valid. New lock not granted\n";
          }
        $done = 1;
        }
      elsif ($main'majordomo_debug)
        {print STDERR "Existing lock was invalid. Let's try again\n";}
      }
    else
      {
      $tmp_string = sprintf(
        "shlock got \"%s\" from opening temp file $file\n   (errno is %d)\n",
	$!, $!);
      warn($tmp_string);
      $done = 1;
      }
    }
  }

return($retcode);
}


#++
#   FUNCTION NAME:
#
#     make_lfh_symbol
#
#  FUNCTIONAL DESCRIPTION:
#
#      Make the string that we use for the lockfile file handle. This value
#      should uniquley identify the lockfile.
#
#  FORMAL PARAMETERS
#
#     $LFH	(I)	Passed to us as the full Unix style filename of the lock
#			file.
#     $base	(I)	A base prefix to append the $LFH to.
#
#  IMPLICIT INPUTS:
#
#      none
#
#  IMPLICIT OUTPUTS:
#
#      none
#
#  SIDE EFFECTS:
#
#      none
#
#  RETURN VALUE:
#
#      Value to use for lockfile's file handle.
#--
sub make_lfh_symbol
{
local($LFH) = shift || return(undef);
local($base) = shift || return(undef);

print "make_lfh_symbol entered.\n\$LFH is $LFH\n\$base is $base.\n" if $main'majordomo_debug;

#
# $LFH started as full lockfile name eg. "/a/b/c.d". Pick out the filename part
# of the lockfile name and save in $1. Eg. $1 = "c.d".
$LFH =~ m,([^/]*)$,;
print "\$LFH is $LFH\n" if $main'majordomo_debug;
#
# Take filename and append it to the base string plus a '_'
$LFH = "$base\_$1";
print "\$LFH is $LFH\n" if $main'majordomo_debug;
#
# Upcase it.
$LFH  =~ tr/a-z/A-Z/;
print "\$LFH is $LFH\n" if $main'majordomo_debug;
#
# Change the '.' (if any) to '_';
$LFH  =~ s/\./_/;
print "\$LFH is $LFH\n" if $main'majordomo_debug;
#
# Return result.
return($LFH);
}


#++
#   FUNCTION NAME:
#
#     main'shlock_with_retry
#
#  FUNCTIONAL DESCRIPTION:
#
#      Tries to get a VMS style "shlock lock". Retries until it
#      gets the lock or runs out of retries. To retry we call main'shlock
#      repeatedly.
#
#  FORMAL PARAMETERS
#
#     $lockfile	(I)	Name of file to make our lock file. This is not-unlike
#                       the VMS lock "resource name".
#
#  IMPLICIT INPUTS:
#
#      none
#
#  IMPLICIT OUTPUTS:
#
#      none
#
#  SIDE EFFECTS:
#
#      Affects contents of %lock_files associative array.
#
#  RETURN VALUE:
#
#      If the lock is granted then this routine returns the fileno of the
#      lockfile. This value should be passed to shlock_release when the
#      lock is to be released.
#--
sub main'shlock_with_retry
{
local($lockfile) = shift;
local($LFH);
local($retfileno) = 0;
local($done) = 0;
local($status) = 0;
local($num_retries) = 0;

if ($main'majordomo_debug)
  {print STDERR "shlock_with_retry entered with parameter $lockfile.\n";}

$LFH = &make_lfh_symbol($lockfile, "SHLOCKWRTR");
print "\$LFH is $LFH\n" if $main'majordomo_debug;

while (!$done)
  {
  $status = &main'shlock($lockfile,$LFH);
  if (!$status)
    {
    $num_retries++;
    if ($num_retries <= $max_retries)
      {
      if ($main'majordomo_debug)
        {print STDERR "shlock_with_retry waiting to try lock again.\n";}
      sleep(1);
      }
    else
      {
      if ($main'majordomo_debug)
        {print STDERR "shlock_with_retry exhausted all retries. Giving up.\n";}
      $done = 1;
      }
    }
  else
    {
    #
    # We have the lock. Save lockfile info in $lock_files associative array
    # so we can release the lock later. Use the fileno for the lock file itself
    # as the index into the lock_files array. This is different from lopen
    # which uses the fileno of the opened user file. However we don't have a
    # user file so we use the lockfile itself. This is fine. Lopen must store
    # the fileno of the user file because the user file is all the lopen caller
    # knows about. It doesn't know about the lock file. So, when the caller
    # calls lclose they pass the file handle of the user file. 
    $lock_files[fileno($LFH)] = join (',', $lockfile,$LFH);
    $done = 1;
    $retfileno = fileno($LFH);
    }
  }

if ($main'majordomo_debug)
  {print STDERR "shlock_with_retry returning $retfileno (any value other than 0 is success).\n";}
return($retfileno);
}


#++
#   FUNCTION NAME:
#
#     main'shlock_release
#
#  FUNCTIONAL DESCRIPTION:
#
#      Releases a shlock lock by closing the channel to the lock file and
#      deleting it.
#
#  FORMAL PARAMETERS
#
#     $index	(I)	Index into the lock_files associative array where the
#			info for the lock file is stored.
#
#  IMPLICIT INPUTS:
#
#      none
#
#  IMPLICIT OUTPUTS:
#
#      none
#
#  SIDE EFFECTS:
#
#      Affects contents of lock_files associative array.
#
#  RETURN VALUE:
#
#      none
#--
sub main'shlock_release
{
local($index) = shift || return(undef);
local($status) = 0;

local($lock,$LOCKFH) = split(/,/, $lock_files[$index]);

if ($main'majordomo_debug)
  {
  print STDERR "shlock_release entered.\n";
  print STDERR "\$index is $index\n";
  print STDERR "\$lock is $lock\n";
  print STDERR "\$LOCKFH is $LOCKFH\n";
  }

$status = close($LOCKFH);
if ($main'majordomo_debug)
  {print STDERR "After close of $LOCKFH, status is $status and \$! is $!\n";}

$status = unlink($lock);
if ($main'majordomo_debug)
  {print STDERR "After unlink $lock, status is $status and \$! is $!\n";}

if ($main'majordomo_debug)
  {print STDERR "shlock_release exiting.\n";}
1;
}


#++
#   FUNCTION NAME:
#
#     main'shlock_lockfile
#
#  FUNCTIONAL DESCRIPTION:
#
#      Given a file name returns the associated lockfile name.
#
#  FORMAL PARAMETERS
#
#     $file	(I)	Name of file.
#
#  IMPLICIT INPUTS:
#
#      none
#
#  IMPLICIT OUTPUTS:
#
#      none
#
#  SIDE EFFECTS:
#
#      none
#
#  RETURN VALUE:
#
#      Name of lockfile associated with $file input parameter
#--
sub main'shlock_lockfile_name{
local($lockfile) = shift || return(undef);

if ($main'majordomo_debug)
  {print "shlock_lockfile_name entered with parameter $lockfile\n";}
#
# Convert filename from real filename to lockfile name and return it.
$lockfile =~ s,([^/]*)$,L_$1,;
#
# Conceal ";" character in case filename part has version number.
$lockfile =~ s/\;/\_/g;
if ($main'majordomo_debug)
  {print "shlock_lockfile_name returning $lockfile\n";}
return($lockfile);
}


#++
#   FUNCTION NAME:
#
#     main'lopen
#
#  FUNCTIONAL DESCRIPTION:
#
#      Opens a file for exclusive access in given mode.
#
#  FORMAL PARAMETERS
#
#     $FH	(I/O)	Filehandle.
#     $mode	(I)	Perl mode in which to open the file.
#     $file	(I)	Name of file.
#
#  IMPLICIT INPUTS:
#
#      none
#
#  IMPLICIT OUTPUTS:
#
#      none
#
#  SIDE EFFECTS:
#
#      Affects contents of lock_files associative array.
#
#  RETURN VALUE:
#
#      none
#--
sub main'lopen {
print "lopen entered\n" if $main'majordomo_debug;
    local($FH) = shift;
    local($mode) = shift;
    local($file) = shift;
    # $fm is what will actually get passed to open()
    local($fm) = "$mode$file";
    local($LFH);
    local($status);
    local($tmp_status);
    local($tries);
# begin temporary
if ($main'majordomo_debug)
  {
  print "\$file is $file\n";
  print "\$mode is $mode\n";
  print "\$fm is $fm\n";
  }
# end temporary

    # create name for lock file
    local($lockfile) = &main'shlock_lockfile_name($file);

# begin temporary
print "\$lockfile is $lockfile\n" if $main'majordomo_debug;
# end temporary

    # force unqualified filehandles into callers' package
    local($package) = caller;
    local($caller_filename) = (caller)[1];
    local($caller_line) = (caller)[2];
    $FH =~ s/^[^']+$/$package'$&/;

    $LFH = &make_lfh_symbol($lockfile, "LOPEN");
    print "\$LFH is $LFH\n" if $main'majordomo_debug;

# begin temporary
print "lopen called by package $package, file $caller_filename at line $caller_line\n" if $main'majordomo_debug;
# end temporary

    for ($tries = 0 ; $tries < $max_retries ; $tries++) {
	# Try to obtain the lock $max_retries times, waiting 1 second after each try
	if (&main'shlock("$lockfile",$LFH)) {
	    # Got the lock; now try to open the file
	    if ($main'majordomo_debug)
              {
              print STDERR "B4 open $fm \n";
              print STDERR "\$status is $status \n";
              print STDERR "\$! is $! \n";
              }
	    $status = open($FH, $fm);
	    if (defined($status))
		{
		# File successfully opened; remember the lock file for deletion
		$lock_files[fileno($FH)] = join (',', $lockfile,$LFH);
		if ($main'majordomo_debug)
                  {
                  print STDERR "open $fm worked \n";
                  print STDERR "\$FH is $FH\n";
                  print STDERR "\$status is $status\n";
                  print STDERR "\$! is $! \n";
                  print STDERR "fileno is ", fileno($FH), "\n";
                  print STDERR "\$lock_files[n] is $lock_files[fileno($FH)]\n";
                  }
		}
	    else
		{
		# File wasn't successfully opened; delete the lock
		if ($main'majordomo_debug)
                  {print STDERR "open $fm failed.\n\$! is $! \nDeleting $lockfile \n";}
		close($LFH);
		$tmp_status = unlink("$lockfile");
		if ($main'majordomo_debug)
                  {
                  print STDERR "Status of unlink($lockfile) call is $tmp_status \n";
                  print STDERR "\$! is $! \n";
                  }
		}

	    # return the success or failure of the open
# begin temporary
print "Leaving lopen with status $status\n" if $main'majordomo_debug;
# end temporary
	    return($status);
	} else {
	    # didn't get the lock; wait 1 second and try again.
	    sleep(1);
	}
    }
    # If we get this far, we ran out of tries on the lock.
    return undef;
}


#++
#   FUNCTION NAME:
#
#     main'lclose
#
#  FUNCTIONAL DESCRIPTION:
#
#      Close a locked file and release lock.
#
#  FORMAL PARAMETERS
#
#     $FH	(I/O)	Filehandle of user's opened file. (ie. file opened with
#			lopen.)
#
#  IMPLICIT INPUTS:
#
#      none
#
#  IMPLICIT OUTPUTS:
#
#      none
#
#  SIDE EFFECTS:
#
#      Affects contents of lock_files associative array (because it calls
#      main'shlock_release).
#
#  RETURN VALUE:
#
#      none
#--
sub main'lclose {
local($FH) = shift;

local($status);
local($fileno);
local($package) = caller;

if ($main'majordomo_debug)
  {
  print STDERR "lclose entered \n";
  print STDERR "$FH is $FH\n";
  }

# force unqualified filehandles into callers' package
$FH =~ s/^[^']+$/$package'$&/;

$fileno = fileno($FH);

if ($main'majordomo_debug)
  {
  print STDERR "$FH is now $FH\n";
  print STDERR "fileno is $fileno\n";
  }

#
# Close the user's file.
close($FH);

#
# Release the lock.
$status = &main'shlock_release($fileno);
if ($main'majordomo_debug)
  {print STDERR "Status of call to shlock_release is $status and \$! is $!\n";}
1;
}

1;
