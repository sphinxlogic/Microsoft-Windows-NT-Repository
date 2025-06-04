This directory tree contains a new VMS mail add-on called mailpipe. It provides
a feature similar to the one provided by the Unix alias file where a mail
message being delivered to an alias is piped to a program. For VMS it is
implemented as a VMS mail foreign transport - "PIPE%". It is required for
support of VMS Majordomo and was added to help allow a close Unix to VMS port of
Majordomo. The VMS mail SET FORWARD/USER command is used by VMS Majordomo in the
same way that the Unix alias file is used by Unix Majordomo.  VMS Majordomo uses
the VMS mail SET FORWARD /USER command to point an "alias" to a PIPE% command
string that funnels a mail message into a .COM file which in turn runs a
Majordomo Perl script. For more information on how VMS Majordomo uses PIPE% and
SET FORWARD /USER see the VMS Majordomo readme file.

Note that while the mailpipe add-on was coded for Majordomo and is required by
Majordomo it is not tied to Majordomo. It is a general purpose tool that can be
used for many other conceivable purposes.

How does it work?

The PIPE% foreign transport (aka. "mailpipe") converts the complex VMS mail
MAIL$PROTOCOL calling sequence into a stream of bytes which is comprised of the
VMS mail headers, a blank line and the body of the message. (In the case of an
SMTP message the "body" is in turn comprised of the RFC headers, a blank line
and the message body.) Mailpipe writes the stream of bytes to a file and passes
the name of the file as P1 to a .COM file running in a batch process which it
creates. 

The string that follows the "PIPE%" gives the parameters for mailpipe to use.
The syntax for the PIPE% string is:

   "file-name-to-run [user-name] ['-'param-string]"

where

   "file-name-to-run" is the file spec of a .COM file to run in the created
   process. It is required. The file must be a full filename although version
   number is not required.

|  "user-name" is the VMS username under which the batch process is created. If
|  the user-name isn't specified then the batch process is created in the same
|  username that the MAILPIPE code is piping from, ie. the username of the
|  process that is trying to send to PIPE%.

  "param-string" is a parameter string passed as P2 to the .COM file. Note that
   the required '-' at the beginning of the parameter string is passed along as
   part of P2. There is no default for this. If it is not there then P2 is
   undefined.

The process that mailpipe creates is, at present, a batch job running under the
specified username. (Ie. a "SUBMIT/USER=user-name file-name-to-run" is done.)
This implies that you must have batch queues running to use PIPE%. This part of
mailpipe may be changed in the future. A detached process may be used instead of
a batch process.

|PIPE Security Registry
|
|In order for PIPE to run you need to create a MAILPIPE Security Registry File.
|It is used to tell MAILPIPE what .COM files may be run by what users in what
|accounts. The syntax of the entries is
|
|   fromusername\tousername\filetorun
|
|For example the entry "SYSTEM\UCX$MAJORDOM\UCX$MAJORDOMO_HOME:DIGEST.COM" says
|that the user SYSTEM may pipe mail to the file UCX$MAJORDOMO_HOME:DIGEST.COM
|running as the user UCX$MAJORDOM.
|
|The check includes simple support for a '*' wildcard. Any field (or fields) may
|contain a single asterix ('*') to indicate a match on anything. For example the
|entry "*\JONES\USER$DISK:SOMEFILE.COM". Says that any user may pipe to
|USER$DISK:SOMEFILE.COM running in the JONES account.
|
|Partial wildcards are not supported. For example you can't do
|"ucx$*\someuser\file". The '%' wildcard is not supported either. If the first
|non whitespace character is an exclamation ('!') then the line is considered a comment. The
|exclamation point does not work as a comment after non-whitespace.For example
|you can't do "*\*\SOME:FILE.COM ! This type of comment doesn't work"
|
|The file must reside in SYS$SYSTEM and must be named PIPE_MAILSHR_REGISTRY.DAT.
|
|If it is desired to allow unprivileged users to use PIPE% to pipe *from* their
|own username *to* their own username then the PIPE_MAILSHR_REGISTRY.DAT.
|protections should be set to W:R.
|

How to install the images.

Mailpipe is contained entirely in one image - PIPE_MAILSHR.EXE which resides in
SYS$LIBRARY. The image for VAX is in [.VAX] and the image for AXP is in [.AXP].
The VAX image was built on VMS 5.5-2. The AXP image was built on V6.1. To
install the right image turn on your priv's and @ the INSTALL.COM in this
directory.

The com file will be smart enough (hopefully ;^) to do the right thing. In case
it screws up you need to copy the PIPE_MAILSHR.EXE image to SYS$COMMON:[SYSLIB].

After this you need to edit your VMS systartup file to do

   @SYS$MANAGER:MAILPIPE_STARTUP.COM.

The INSTALL.COM procedure copies MAILPIPE_STARTUP.COM to SYS$COMMON:[SYSMGR].

|Diagnostics:
|
|You can turn on diagnostics by doing
|
|  DEFINE/SYSTEM UCX$PIPE_MAILSHR_DEBUG 1
|  or
|  DEFINE/SYSTEM UCX$PIPE_MAILSHR_DEBUG 20
|
|When you turn on PIPE diagnostics:
|
|  1) PIPE_MAILSHR leaves a file named PIPE_DEBUG.TXT in the default directory
|  of the process *from* which the pipe is done, Ie. the process that sends to
|  PIPE%, not the batch job receiving the PIPE'd mail message. For example, say
|  the SMTP symbiont is delivering mail to a Majordomo PIPE% mailing list. The
|  PIPE_DEBUG.TXT file is left in SYS$SYSTEM because that is the SMTP symbiont's
|  default directory.
|
|  2) The MAILPIPE batch logs which are created every time PIPE submits a batch
|  job but which are usually deleted are not deleted. In the case of the SMTP
|  symbiont delivering mail to a Majordomo PIPE% mailing list the log file would
|  be in the UCX$MAJORDOM account's default directory and, since the .COM file
|  to receive mail to send to a list is named RESEND.COM, the log file would
|  be RESEND.LOG.
|
|At this time two values for the MAILPIPE diagnostic logical exist. This table
|describes them:
|
|Logical:	Effect:
|  1		Turns on MAILPIPE's diagnostics and causes PIPE_DEBUG.TXT to be
|		written. Also causes the PIPE% code not to delete the batch log
|               of the PIPE'd to batch job. (Ie. the job is submitted /KEEP.)
|  20		Everything that 1 does. In addition it tells PIPE not to delete
|		the .TMP files it makes.

Caveats:

The PIPE% code waits for the completion of the batch job before returning to the
caller. Majordomo owners need to be aware of this as it affects how the UCX SMTP
symbiont and the Majordomo scripts interact. See the Majordomo readme file
([-.MAJORDOMO]VMS_README.TXT) for more info.

Again, remember that you need to have a batch queue running to run mailpipe.
