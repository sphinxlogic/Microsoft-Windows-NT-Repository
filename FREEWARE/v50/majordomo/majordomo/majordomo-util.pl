package Majordomo_Util;

$tmp = "/SYS\$SCRATCH/MJD-UTIL_$$";


sub main'my_exit
{
local($status) = shift;
chdir($main'defdir);
exit($status);
}


sub main'my_die
{
local($string) = shift;
chdir($main'defdir);
print STDERR "$string\n";
$SS_ABORT = 44;
exit($SS_ABORT);
}


# Mails UCX$MAJORDOMO_HOME:VMS-LIST-OWNER-INFO.TXT to the list owner.
sub main'mail_list_info
{
local($list) = shift;
local($passwd) = shift;
local($digest_name) = shift;
local($digest_passwd) = shift;
local($list_owner) = "$list\-owner";

if ($main'majordomo_debug) {print "mail_list_info entered. list is $list. passwd is $passwd \n";}

# who do we send the body to if we step on a landmine?
&main'set_abort_addr($main'whoami_owner);

if ($main'majordomo_debug)
  {
  print "mail_list_info calling sendmail with these header parameters: \n";
  print "  Return-Path: $main'whoami_owner \n";
  print "  To: $list_owner  \n";
  print "  From: $main'whoami \n";
  print "  Subject: You have been made list owner of Majordomo list $list \n";
  }

print "Sending list owner information in a mail message to $list_owner...\n";

# Open the sendmail process to send the list owner info
&main'sendmail(LISTINFO,
  $list_owner,		# To:
  "You have been made list owner of Majordomo list $list",	# Subject
  $main'whoami,		# From:
  $main'whoami_owner);	# Return-Path:

# Flush to disk. Not strictly needed.
select((select(LISTINFO), $| = 1)[0]);

# Fill out particulars for this list at top of text
print LISTINFO "Majordomo address:		$main'whoami\n";
print LISTINFO "Majordomo-Owner address:	$main'whoami_owner\n";
print LISTINFO "List Name:			$list\n";
print LISTINFO "List posting address:		$list\@$main'whereami\n";
print LISTINFO "List request address:		$list\-Request\@$main'whereami\n";
print LISTINFO "List password:			$passwd\n";

if ($digest_name)
  {
  print LISTINFO "Digest list name:		$digest_name\n";
  if ($digest_passwd)
    {print LISTINFO "Digest list password:		$digest_passwd\n";}
  else
    {print LISTINFO "Digest list password:		???\n";}
  }

print LISTINFO "\n";

# Flush to disk. Not strictly needed.
select((select(LISTINFO), $| = 1)[0]);

# And put away text of VMS-LIST-OWNER-INFO.TXT
open(ININFO, "UCX\$MAJORDOMO_HOME:VMS-LIST-OWNER-INFO.TXT");
while (<ININFO>) {print LISTINFO $_;}
close(ININFO);

# close (and thereby send) the message
&main'close_and_send(LISTINFO);

if ($main'majordomo_debug < 20)
  {&main'delete_tmpfiles;}

return 1;
}


sub main'execute_file
{
local($fn) = shift;
local($fn_vms);
local($cmd_string);
local($status_str);
#
# Convert to VMS syntax
$fn_vms = VMS::Filespec::vmsify($fn);
if ($main'majordomo_debug) {print "execute_file converted unix filename to $fn_vms\n";}
$cmd_string = "\@$fn_vms";
$status_str = `$cmd_string`;
if (defined($status_str))
  {
  if (($status_str =~ /-W-/) ||
      ($status_str =~ /-E-/) ||
      ($status_str =~ /-F-/))
    {
    print STDERR "DANGER! DANGER! An error occured while \@'ing $fn_vms.\n";
    print STDERR "\output is:\n$status_str\n";
    return undef;
    }
  }
return 1;
}


sub main'passwd_legal_chars
{
    local($taint_passwd) = shift;
    local($save_passwd) = $taint_passwd;
    local($clean_passwd);

$taint_passwd =~ tr/A-Z/a-z/;
$clean_passwd = $taint_passwd;
$taint_passwd =~ s/\s//;
if ($taint_passwd ne $clean_passwd)
  {
  print STDERR "Password $save_passwd contains white space. That's a no-no.\n";
  return "";
  }
$taint_passwd =~ s/[\x00-\x1F]//;
if ($taint_passwd ne $clean_passwd)
  {
  print STDERR "Password contains a control character. That's a no-no.\n";
  return "";
  }
$taint_passwd =~ s/[\x7F-\xFF]//;
if ($taint_passwd ne $clean_passwd)
  {
  print STDERR "Password contains an ASCII character > 126. That's a no-no.\n";
  return "";
  }
return $save_passwd;
}


sub main'make_passwd_file
{
local($listdir) = shift;
local($list) = shift;
local($passwd) = shift;
local($FH) = "PASSWD";
open($FH, ">$listdir/$list.passwd") || return 0;
print $FH $passwd;
close($FH) || return 0;
return 1;
}


sub main'spot_ucx_smtp_list
{
local($list) = shift;
local($oldlist) = ();
local(@oldlist) = ("/UCX\$SMTP_COMMON/$list.DIS",
                  "/UCX\$SMTP_DIS_DIRECTORY/$list.DIS",
                  "/SYS\$SPECIFIC/UCX_SMTP/$list.DIS");

#
# Look for list in one of three places:
#    -  UCX$SMTP_COMMON
#    -  UCX$SMTP_DIS_DIRECTORY (Obsolete but should still support it.)
#    -  SYS$SPECIFIC:[UCX_SMTP]
# 
foreach $oldlist (@oldlist)
  {
  if ($main'majordomo_debug) {print "Checking for existing UCX SMTP list file $oldlist\n";}
  if (-e "$oldlist")
    {
    if ($main'majordomo_debug) {print "Found file $oldlist.\n";}
    return($oldlist);
    }
  }

return undef;
}


sub main'make_old_filename
{
local($fn) = shift;
local($old_fn) = $fn;
    if ($old_fn =~ /\./) {
	$old_fn =~ s,([^\.]*)$,,;
        $old_fn .= $1;
        $old_fn .= "_OLD";
    } else {
	$old_fn .= ".OLD";
    }
return $old_fn;
}


sub main'make_empty_list_file
{
local($listdir) = shift;
local($list) = shift;
local($FH) = "EMPTY";
open($FH, ">$listdir/$list.") || return 0;
close($FH) || return 0;
return 1;
}


sub main'copy_list_file
{
local($listdir) = shift;
local($list) = shift;
local($old_listname) = shift;

if ($main'majordomo_debug)
  {
  print "copy_list_file copying $old_listname to $listdir/$list.\n";
  }
open(OLDLIST, "$old_listname") || return 0;
open(NEWLIST, ">$listdir/$list.") || return 0;

while ($line = <OLDLIST>)
  {print NEWLIST "$line" || return 0;}

close(OLDLIST) || return 0;
close(NEWLIST) || return 0;
return 1;
}


sub main'make_list_file
{
local($listdir) = shift;
local($list) = shift;
local($digest_name) = shift;
local($old_listname) = shift;
local($oldlist_disposition) = shift;
local($fn_vms) = ();
local($rename_fn) = ();

if ($main'majordomo_debug)
  {
  print "make_list_file entered \n";
  print "\$listdir is $listdir\n";
  print "\$list is $list\n";
  print "\$digest_name is $digest_name\n";
  print "\$old_listname is $old_listname\n";
  print "\$oldlist_disposition is $oldlist_disposition\n";
  }
if (!$old_listname)
  {
  &main'make_empty_list_file($listdir,$list) || &main'my_die("Error while creating empty list file\n");
  }
else
  {
  &main'copy_list_file($listdir,$list,$old_listname) ||
    &main'my_die("Error while copying old list file to new directory.\n");
  if ($oldlist_disposition eq "DELETE")
    {
    print "Deleting $old_listname\n";

    #
    # Deleting this way rather than with unlink because unlink won't work if
    # directory part of filename is search list and file is not in directory
    # specified by first equivalence name of search list logical.
    $fn_vms = VMS::Filespec::vmsify($old_listname);
    $fn_vms .= ";*";
    `delete $fn_vms`;
    }
  else
    {
    $rename_fn = &main'make_old_filename($old_listname);
    print "Renaming $old_listname to $rename_fn\n";

    rename("$old_listname","$rename_fn") ||
      warn("Error renaming $old_listname to $rename_fn: $!\n");
    }
  #
  # Just in the off case that they had copies of the old list file in
  # more than one directory delete them all.
  while ($old_listname = main'spot_ucx_smtp_list($list))
    {
    print "Found another UCX SMTP version of the list ($old_listname). Deleting it.\n";
    $fn_vms = VMS::Filespec::vmsify($old_listname);
    $fn_vms .= ";*";
    `delete $fn_vms`;
    }
  }
#
# If list has a digest then cause all mail sent to list to go to digest by
# "subscribing" the listname-DIGEST address to the list.
if ($digest_name)
  {
  if ($main'majordomo_debug)
    {print "Adding $digest_name\@$main'whereami to file $listdir/$list.\n";}
  open(LIST, ">>$listdir/$list.")
    || &main'my_die("Can't open $listdir/$list. for append: $!\n");
  print LIST "$digest_name\@$main'whereami\n";
  close(LIST)
    || &main'my_die("Can't close $listdir/$list.: $!\n");
  }

return 1;
}


sub main'ask_list_digest
{
local($list) = shift;
local($answer) = ();
local($done) = 0;

if ($main'majordomo_debug) {print "ask_list_digest entered\n";}

while (!$done)
  {
  print "Do you want to set up a digest (\"?\" for help) for list $list Y/N (N)? ";
  $answer = (<STDIN>);

  # This handles Ctrl-Z (Ctrl-Z not allowed at this point.)
  if (!$answer)
    {
    print "Sorry. You can't exit from here.\n";
    next;
    }

  chop($answer);
  if (($answer eq "") || (!$answer)) {return undef;}
  if ($answer =~ /^y/i) {return 1;}

  # If "?" character anywhere in response assume they want help.
  elsif ($answer =~ /\?/)
    {
    print "
You may optionally choose to have a digest set up for this list.
If you do so this script will set up all the necessary forwarding entries and
files. See the Majordomo documentation for more info.
";
    }
  else
    {return undef;}
  }

return undef;
}


sub main'ask_delete_digest
{
local($list) = shift;
local($answer) = ();
local($done) = 0;
local($tmp_digest_name) = main'make_digest_name($list);

if ($main'majordomo_debug) {print "ask_delete_digest entered\n";}

while (!$done)
  {
  print "Do you want to delete the digest (\"?\" for help) for list $list Y/N (N)? ";
  $answer = (<STDIN>);

  # This handles Ctrl-Z (Ctrl-Z not allowed at this point.)
  if (!$answer)
    {
    print "Sorry. You can't exit from here.\n";
    next;
    }

  chop($answer);
  if (($answer eq "") || (!$answer)) {return undef;}
  if ($answer =~ /^y/i) {return 1;}

  # If "?" character anywhere in response assume they want help.
  elsif ($answer =~ /\?/)
    {
    print "
You may optionally choose to delete the digest for this list. If you do so this
script will create the forwarding entry for \"$list\" without 
\"-d $tmp_digest_name\". This will effectively delete the digest list
since submissions to \"$list\" will no longer be sent to the digest. However you
must still manually delete the digest list itself by running the delete-list
Perl script on the list \"$tmp_digest_name\".

See the Majordomo documentation for more info.
";
    }
  else
    {return undef;}
  }

return undef;
}


sub main'is_digest_name
{
local($list) = shift;
if (!$list) {return undef};

if ($list_digest =~ /-digest$/i)
  {return 1;}
else
  {return undef;}
}


sub main'list_has_digest
{
local($listdir) = shift;
local($list) = shift;
if (!$listdir) {return undef};
if (!$list) {return undef};

local($tmp_digest_name) = main'make_digest_name($list);

return main'list_exists($listdir, $tmp_digest_name);
}


sub main'make_digest_name
{
local($list) = shift;
if (!$list) {return undef};
return("$list-digest");
}


sub main'make_digest_owner
{
local($list) = shift;
if (!$list) {return undef};
return("$list-owner");
}


sub get_reply_to
{
local($list) = shift;
local($reply_to) = ();
local($tmp_reply_to) = ();
local($done) = 0;

if ($main'majordomo_debug) {print "get_reply_to entered\n";}

while (!$done)
  {
  print "Enter Reply-To (<CR> for no Reply-To, \"?\" for help): ";
  $reply_to = (<STDIN>);

  # This handles Ctrl-Z (Ctrl-Z not allowed at this point.)
  if (!$reply_to)
    {
    print "Sorry. You can't exit from here.\n";
    next;
    }

  chop($reply_to);
  if (($reply_to eq "") || (!$reply_to))
    {
    return undef;
    }
  if ($main'majordomo_debug) {print "You entered: $reply_to\n";}
  if ($reply_to =~ /^LIST$/i)
    {
    $reply_to = "$list\@$main'whereami";
    $done = 1;
    }

  # If "?" character anywhere in response assume they want help. This
  # is because stupid Perl address parsing routine will parse "? " as a legal
  # address.
  elsif ($reply_to =~ /\?/)
    {
    print "
If you want a Reply-To: header sent with each message you can configure it here.
You may do one of three things:

   1) If you do not want a Reply-To: header at all or wish to configure it via
   the list configuration file (ie. UCX\$MAJORDOMO_LISTS:$list.config) then
   enter a blank <CR> at the prompt below. Eg:

        Enter Reply-To (<CR> for no Reply-To, \"?\" for help): <CR>

   2) If you want the Reply-To: header to be set automatically to

         Reply-to: $list\@$main'whereami

      then enter \"LIST\" (Not case sensitive. ie. \"list\" is same as \"LiSt\"). Eg:

        Enter Reply-To (<CR> for no Reply-To, \"?\" for help): LIST

   3) Enter the exact string you want for the Reply-To: field.
      Eg:

        Enter Reply-To (<CR> for no Reply-To, \"?\" for help): Mygreatauntfanny\@some.where
";
    }
  else
    {
    $tmp_reply_to = main'valid_addr($reply_to);
    if ($main'majordomo_debug) {print "address back from valid_addr is $tmp_reply_to\n";}
    if ($tmp_reply_to)
      {
      $reply_to = $tmp_reply_to;
      $done = 1;
      }
    else
      {
      print "String $reply_to doesn't parse as legal address\n";
      }
    }
  }

if ($main'majordomo_debug) {print "get_reply_to returning $reply_to\n";}
return($reply_to);
}


#
# Certain versions of VMS have support for nested double quotes in the SET
# FORWARD address and older ones don't. The problem is that a simple version
# check would be too difficult to implement since with some versions it depends
# on whether or not you've added this or that patch. (Eg. You can't just say if
# we're on 7.1 then assume nested double quotes because the first version of 7.1
# didn't have it. Only a later, patched version had it.) So anyway this
# routine makes a test SET FORWARD and then does a SHOW FORWARD to determine
# which VERSION of the mail SET FORWARD command we have on this system. If it
# detects that the old style address parsing is being done on SET FORWARD (ie.
# that we need to use triple double quotes to get a single double quote in the
# result) then a 1 is returned. Otherwise a 0 is returned.
sub main'set_forward_triple_doubles
{
local($comfn) = $tmp  . "_" . $my_system_name . ".COM";
local($logfn) = $tmp  . "_" . $my_system_name . ".LOG";
local($logfn_vms) = VMS::Filespec::vmsify($logfn);
local($line);
local($result);

if ($main'majordomo_debug) {print "set_forward_triple_doubles entered \n";}

# Just being suresure. open_temp opens for append so need to clean up first.
1 while (unlink($comfn));

&main'open_temp(TMPTST, $comfn) || &main'my_die("Can't open $comfn: $!\n");

#
# Create a com file to set forwardfor a test and sho the result. Point
# SYS$OUTPUT to a log where we will examine the results.
print TMPTST "\$ DEFINE/USER SYS\$OUTPUT $logfn_vms\n";
print TMPTST "\$ MAIL\n";
print TMPTST "SET FORWARD/USER=ABCDEFGFEDCBA SMTP%\"\"\"HELP_ME_SPOCK\"\"\"\n";
print TMPTST "SHOW FORWARD/USER=ABCDEFGFEDCBA\n";
print TMPTST "SET  NOFORWARD/USER=ABCDEFGFEDCBA\n";
print TMPTST "EXIT\n";
print TMPTST "\$ EXIT\n";

close(TMPTST)                || &main'my_die("Can't close $comfn: $!\n");
&main'execute_file($comfn)   || &main'my_die("Error while executing $comfn: $!\n");

#
# Look through log to find results of SHOW FORWARD. Find a line
# that contains the string "ABCDEFGFEDCBA has mail forwarded to ". Everything
# after this string to the end of the line is the result we're looking for.
open(TMPTST, $logfn);
while ($line = <TMPTST>)
  {
  chop($line);
  if ($line =~ /^ABCDEFGFEDCBA has mail forwarded to /i)
    {
    $result = $';
    last;
    }
  }
close(TMPTST);

unlink($comfn);
unlink($logfn);

if ($main'majordomo_debug) {print "\$result is $result \n";}

#
# If the result looks like this:
#
#    SMTP%"HELP_ME_SPOCK"
#
# then our system is using old style SET FORWARD parsing. Ie. we used the
# old style triple double quotes to get the address in and the result is
# single double quotes which is what we want.
#
if ($result =~ /SMTP%\"HELP_ME_SPOCK\"$/i)
  {
  if ($main'majordomo_debug)
    {print "Old style triple double quote parsing of SET FORWARD entries is in use.\n";}
  return 1;
  }
else
  {
  if ($main'majordomo_debug)
    {print "New style parsing of SET FORWARD entries is in use. (ie. no triple double quotes needed.)\n";}
  return 0;
  }

return 0; # should never get here.
}


sub list_cmds_to_file
{
local($FH) = shift;
local($clean_list) = shift;
local($list_owner) = shift;
#local($digest_name) = shift;
local($tmp_string) = ();
local($reply_to) = ();
local($whereami) = $main'whereami;
local($homedir_vms) = $main'homedir_vms;
local($majordomo_debug) = $main'majordomo_debug;
local($use_triple_doubles) = &main'set_forward_triple_doubles;

print $FH "\$ MAIL\n";
print $FH "SET FORWARD/USER=OWNER-$clean_list            $clean_list-OWNER\n";
print $FH "SET FORWARD/USER=OWNER-$clean_list-OUTGOING   $clean_list-OWNER\n";
print $FH "SET FORWARD/USER=OWNER-$clean_list-REQUEST    $clean_list-OWNER\n";
print $FH "SET FORWARD/USER=$clean_list-APPROVAL         $clean_list-OWNER\n";
print $FH "SET FORWARD/USER=$clean_list-OWNER            $list_owner\n";

if ($use_triple_doubles)
  {$tmp_string = "SMTP%\"\"\"$clean_list\@$whereami\"\"\"";}
else
  {$tmp_string = "SMTP%\"$clean_list\@$whereami\"";}

print $FH "SET FORWARD/USER=$clean_list-LIST             $tmp_string\n";

if ($use_triple_doubles)
  {$tmp_string = "PIPE%\"\"\"UCX\$MAJORDOMO_HOME:RESEND.COM UCX\$MAJORDOM ";}
else
  {$tmp_string = "PIPE%\"UCX\$MAJORDOMO_HOME:RESEND.COM UCX\$MAJORDOM ";}

#if ($digest_name)
#  {$tmp_string .= "-d $digest_name ";}
$tmp_string .= "-l $clean_list ";

# changed resend so -h is not mandatory. If not there then it uses $whereami
# which allows $whereami to be change in one place without having to change it
# in all the forwarding commands. If the user wants to change the SET FORWARD
# for the list to have a -h they can do so but we won't put it in by default.
#$tmp_string .= "-h $whereami ";
$reply_to = &get_reply_to($clean_list);
if ($reply_to)
  {
  if ($majordomo_debug)
    {print "Setting Reply-To: for $clean_list to $reply_to\n";}
  $tmp_string .= "-r $reply_to ";
  }

if ($use_triple_doubles)
  {$tmp_string .= "$clean_list-OUTGOING\"\"\"";}
else
  {$tmp_string .= "$clean_list-OUTGOING\"";}

print $FH "SET FORWARD/USER=$clean_list $tmp_string\n";

if ($use_triple_doubles)
  {$tmp_string = "SMTP%\"\"\"¢UCX\$MAJORDOMO_LIST:$clean_list\.¢\@$whereami\"\"\"";}
else
  {$tmp_string = "SMTP%\"¢UCX\$MAJORDOMO_LIST:$clean_list\.¢\@$whereami\"";}
print $FH "SET FORWARD/USER=$clean_list-OUTGOING   $tmp_string\n";

if ($use_triple_doubles)
  {
  $tmp_string = "PIPE%\"\"\"UCX\$MAJORDOMO_HOME:REQUEST-ANSWER.COM UCX\$MAJORDOM ";
  $tmp_string .= "$clean_list\"\"\"";
  }
else
  {
  $tmp_string = "PIPE%\"UCX\$MAJORDOMO_HOME:REQUEST-ANSWER.COM UCX\$MAJORDOM ";
  $tmp_string .= "$clean_list\"";
  }

print $FH "SET FORWARD/USER=$clean_list-REQUEST    $tmp_string\n";
print $FH "EXIT\n";
print $FH "\$ EXIT\n";
}


sub digest_cmds_to_file
{
local($FH) = shift;
local($digest_name) = shift;
local($digest_owner) = shift;
local($tmp_string) = ();
local($whereami) = $main'whereami;
local($homedir_vms) = $main'homedir_vms;
local($majordomo_debug) = $main'majordomo_debug;
local($use_triple_doubles) = &main'set_forward_triple_doubles;

print $FH "\$ MAIL\n";
print $FH "SET FORWARD/USER=OWNER-$digest_name            $digest_name-OWNER\n";
print $FH "SET FORWARD/USER=OWNER-$digest_name-OUTGOING   $digest_name-OWNER\n";
print $FH "SET FORWARD/USER=OWNER-$digest_name-REQUEST    $digest_name-OWNER\n";
print $FH "SET FORWARD/USER=$digest_name-APPROVAL         $digest_name-OWNER\n";
print $FH "SET FORWARD/USER=$digest_name-OWNER            $digest_owner\n";

if ($use_triple_doubles)
  {$tmp_string = "SMTP%\"\"\"$digest_name\@$whereami\"\"\"";}
else
  {$tmp_string = "SMTP%\"$digest_name\@$whereami\"";}

print $FH "SET FORWARD/USER=$digest_name-LIST             $tmp_string\n";

if ($use_triple_doubles)
  {$tmp_string = "PIPE%\"\"\"UCX\$MAJORDOMO_HOME:DIGEST.COM UCX\$MAJORDOM ";}
else
  {$tmp_string = "PIPE%\"UCX\$MAJORDOMO_HOME:DIGEST.COM UCX\$MAJORDOM ";}

$tmp_string .= "-r ¢-C¢ -l $digest_name ";

if ($use_triple_doubles)
  {$tmp_string .= "$digest_name-OUTGOING\"\"\"";}
else
  {$tmp_string .= "$digest_name-OUTGOING\"";}

print $FH "SET FORWARD/USER=$digest_name $tmp_string\n";

if ($use_triple_doubles)
  {$tmp_string = "SMTP%\"\"\"¢UCX\$MAJORDOMO_LIST:$digest_name\.¢\@$whereami\"\"\"";}
else
  {$tmp_string = "SMTP%\"¢UCX\$MAJORDOMO_LIST:$digest_name\.¢\@$whereami\"";}

print $FH "SET FORWARD/USER=$digest_name-OUTGOING   $tmp_string\n";

if ($use_triple_doubles)
  {
  $tmp_string = "PIPE%\"\"\"UCX\$MAJORDOMO_HOME:REQUEST-ANSWER.COM UCX\$MAJORDOM ";
  $tmp_string .= "$digest_name\"\"\"";
  }
else
  {
  $tmp_string = "PIPE%\"UCX\$MAJORDOMO_HOME:REQUEST-ANSWER.COM UCX\$MAJORDOM ";
  $tmp_string .= "$digest_name\"";
  }

print $FH "SET FORWARD/USER=$digest_name-REQUEST    $tmp_string\n";
print $FH "EXIT\n";
print $FH "\$ EXIT\n";
}


sub main'set_list_fwd
{
local($fn) = shift;
local($list_name) = shift;
local($list_owner) = shift;
#local($digest_name) = shift;
if (!$fn) {return undef;}

# Just being suresure. open_temp opens for append so need to clean up first.
unlink($fn);
$! + 1;

&main'open_temp(TMP, $fn) || &main'my_die("Can't open $fn: $!\n");

#&list_cmds_to_file(TMP,$list_name,$list_owner,$digest_name)
#  || &main'my_die("Error writing commands to $fn: $!\n");

&list_cmds_to_file(TMP,$list_name,$list_owner) || &main'my_die("Error writing commands to $fn: $!\n");

close(TMP)                || &main'my_die("Can't close $fn: $!\n");
#
if (!(&main'execute_file($fn)))
  {
  warn "Error while executing $fn: $!\nTrying to continue...\n";
  print STDERR "
An error of \"MAIL-E-INVITMLEN, invalid item length\" most likely means that
the \"alias\" value in the \"SET FORWARD/USER=alias fwd-string\" command
was longer than the VMS imposed limit of 32 characters. Often the list can
continue to operate without the alias defined. To see exactly which
SET FORWARD commands failed SET VERIFY and run $fn

";
  }
if ($main'majordomo_debug < 20)
  {unlink($fn)               || warn "Error while deleting $fn: $!\n";}
}


sub main'set_digest_fwd
{
local($fn) = shift;
local($digest_name) = shift;
local($digest_owner) = shift;
if (!$fn) {return undef;}

# Just being suresure. open_temp opens for append so need to clean up first.
unlink($fn);
$! + 1;

&main'open_temp(TMP, $fn) || &main'my_die("Can't open $fn: $!\n");
&digest_cmds_to_file(TMP,$digest_name,$digest_owner)
  || &main'my_die("Error writing commands to $fn: $!\n");
close(TMP)                || &main'my_die("Can't close $fn: $!\n");
#
if (!(&main'execute_file($fn)))
  {
  warn "Error while executing $fn: $!\nTrying to continue...\n";
  print STDERR "
An error of \"MAIL-E-INVITMLEN, invalid item length\" most likely means that
the \"alias\" value in the \"SET FORWARD/USER=alias fwd-string\" command
was longer than the VMS imposed limit of 32 characters. Often the list can
continue to operate without the alias defined. To see exactly which
SET FORWARD commands failed SET VERIFY and run $fn

";
  }
if ($main'majordomo_debug < 20)
  {unlink($fn)               || warn "Error while deleting $fn: $!\n";}
}


sub set_forward_triple_doubles_help
{
local($user_string) = shift;
print
"
You must now enter the name or email address as a string suitable for passing to
the VMS mail command:

   MAIL> SET FORWARD/USER=$user_string your-string-here

because that is how ownership is set up. Here are some examples:

   1) A simple VMS username. For example, enter:

         jones

   2) A simple SMTP address. For example, enter:

         SMTP%\"\"\"jones\@acme.com\"\"\"

      Note that with this version of VMS mail you *MUST* enter the triple
      double-quotes!

   3) A multiple user SMTP address. For example, enter:

         SMTP%\"\"\"jones\@acme.com, smith\@acme.com\"\"\"

      or, for local addresses enter:

         SMTP%\"\"\"jones, smith\"\"\"

      Note that with this version of VMS mail you *MUST* enter the triple
      double-quotes!

   4) A DECnet address. For example, enter:

         COOKIE::MONSTER

   5) A string for which a SET FORWARD/USER has been done. For example, enter:

         Chuck.Jones

      This assumes that a

         MAIL> SET FORWARD/USER=\"Chuck.Jones\" somewhere

      has been done

";
}


sub set_forward_notriple_doubles_help
{
local($user_string) = shift;
print
"
You must now enter the name or email address as a string suitable for passing to
the VMS mail command:

   MAIL> SET FORWARD/USER=$user_string your-string-here

because that is how ownership is set up. Here are some examples:

   1) A simple VMS username. For example, enter:

         jones

   2) A simple SMTP address. For example, enter:

         SMTP%\"jones\@acme.com\"

      Note that with this version of VMS mail you must *NOT* enter the triple
      double-quotes!

   3) A multiple user SMTP address. For example, enter:

         SMTP%\"jones\@acme.com, smith\@acme.com\"

      or, for local addresses enter:

         SMTP%\"jones, smith\"

      Note that with this version of VMS mail you must *NOT* enter the triple
      double-quotes!

   4) A DECnet address. For example, enter:

         COOKIE::MONSTER

   5) A string for which a SET FORWARD/USER has been done. For example, enter:

         Chuck.Jones

      This assumes that a

         MAIL> SET FORWARD/USER=\"Chuck.Jones\" somewhere

      has been done

";
}


sub main'print_set_forward_help
{
local($user_string) = shift;
local($use_triple_doubles) = &main'set_forward_triple_doubles;

if ($use_triple_doubles)
  {set_forward_triple_doubles_help($user_string);}
else
  {set_forward_notriple_doubles_help($user_string);}
}


#
# Do a rudimentary check to see if the use of double quotes seems to coincide
# with the version of VMS mail we're running.
sub main'set_forward_valid
{
local($owner_string) = shift;
local($use_triple_doubles) = &main'set_forward_triple_doubles;

if ($main'majordomo_debug)
  {print "set_forward_valid entered with parameter $owner_string\n";}

#
# Does the string contain any double quotes at all?
if (!($owner_string =~ /"/))
  {
  if ($main'majordomo_debug)
    {print "No double quotes found in $owner_string\n";}
  return(1);
  }

#
# OK. It's got at least one double quote. However if it doesn't contain a '%'
# then don't make any other assumptions about the string. It's not anything
# we're trying to parse for here. (For example it could be an MTS address which
# would contain a double quote but no '%'.)
if (!($owner_string =~ /%/))
  {
  if ($main'majordomo_debug) {print "No '\%' found in $owner_string\n";}
  return(1);
  }

if ($main'majordomo_debug)
  {print "double quotes and '\%' found in $owner_string\n";}
#
# Are we supposed to use triple doubles?
if ($use_triple_doubles)
  {
  #
  # Yes. Parse for anything%"""anything"""
  if ($owner_string =~ /.+%""".+"""$/)
    {
    if ($main'majordomo_debug)
      {print "$owner_string parses fine.\n";}
    return(1);
    }
  else
    {
    print "
The string $owner_string doesn't parse right for this version of VMS mail to
yield a correct forwarding entry. It should look like:

      SMTP%\"\"\"jones\@acme.com\"\"\"

(See the section entitled \"A note about triple double quotes and VMS mail\"
VMS_README.TXT file for VMS Majordomo for more information.)

";
    return(undef);
    }
  }
else
  {
  #
  # Do not use triple doubles. Parse for anything%"anything"
  if ($owner_string =~ /.+%"[^"]+"$/)
    {
    if ($main'majordomo_debug)
      {print "$owner_string parses fine.\n";}
    return(1);
    }
  else
    {
    print "
The string $owner_string doesn't parse right for this version of VMS mail to
yield a correct forwarding entry. It should look like:

      SMTP%\"jones\@acme.com\"

";
    if ($owner_string =~ /"""/)
      {
      print "
Hint: There are triple double quotes (ie. `\"\"\"') in the text. If you are
      using the old style syntax for double quotes in the SET FORWARD command
      try replacing the triple double quotes with single double quotes.
";
      }

    print "
(See the section entitled \"A note about triple double quotes and VMS mail\"
VMS_README.TXT file for VMS Majordomo for more information.)

";
    return(undef);
    }
  }

#
# Just in case. Should never get here.
return(1);
}


sub main'get_list_owner
{
local($list) = shift;
local($list_owner) = ();
local($tmp_list_owner) = ();
local($done) = 0;

if ($main'majordomo_debug) {print "get_list_owner entered\n";}

while (!$done)
  {
  print "Enter list owner or \"?\" for help (help recomended for first-time VMS Majordomo users): ";
  $list_owner = (<STDIN>);

  # This handles Ctrl-Z
  if (!$list_owner) {return undef;}

  chop($list_owner);

  # This handles blank CR
  if (($list_owner eq "") || (!$list_owner)) {return undef;}

  if ($main'majordomo_debug) {print "You entered: $list_owner\n";}

  # If "?" character anywhere in response assume they want help.
  if ($list_owner =~ /\?/)
    {&main'print_set_forward_help("$list-owner");}
  else
    {
    if (&main'set_forward_valid($list_owner))
      {$done = 1;}
    }
  }

if ($main'majordomo_debug) {print "get_list_owner returning $list_owner\n";}
return($list_owner);
}


sub main'log_cf_stuff
{
local($i);
print "\$main'whereami is $main'whereami \n";
print "\$main'whoami is $main'whoami \n";
print "\$main'whoami_owner is $main'whoami_owner \n";
print "\$main'homedir is $main'homedir \n";
print "\$main'homedir_vms is $main'homedir_vms \n";
print "\$main'listdir is $main'listdir \n";
print "\$main'listdir_vms is $main'listdir_vms \n";
print "\$main'digest_work_dir is $main'digest_work_dir \n";
print "\$main'digest_work_dir_vms is $main'digest_work_dir_vms \n";
print "\$main'log is $main'log \n";
print "\$main'mailer is $main'mailer \n";
print "\$main'filedir is $main'filedir \n";
print "\$main'filedir_suffix is $main'filedir_suffix \n";
print "\$main'index_command is $main'index_command \n";
print "\$main'return_subject is $main'return_subject \n";
print "\$main'majordomo_request is $main'majordomo_request \n";
$i = 0;
while ($main'archive_dirs[$i])
  {
  print "\$main'archive_dirs[$i] is $main'archive_dirs[$i]\n";
  $i++;
  }
$i = 0;
while ($main'archive_dirs_vms[$i])
  {
  print "\$main'archive_dirs_vms[$i] is $main'archive_dirs_vms[$i]\n";
  $i++;
  }
print "\$main'analyze_slash_in_address is $main'analyze_slash_in_address \n";
print "\$main'no_x400at is $main'no_x400at\n";
print "\$main'no_true_x400 is $main'no_true_x400\n";
print "\$tmp 'no_true_x400 is $main'tmp\n";
}

1;
