This directory tree contains an image you'll need to allow the Majordomo Perl
scripts to pass mail messages to the UCX SMTP symbiont to deliver. The mechanism
is called "Send From File" or SFF for short. This mechanism takes as input a
pre-formatted RFC 822 mail message and converts it into a UCX SMTP control file
which it submits to the UCX SMTP symbiont.

A Majordomo Perl script writes a mail message it wants to send to a temporary
file and then invokes the SFF mechanism to send it. This feature is roughly
analogous to the Unix interface to sendmail.

The SFF mechanism requires one of SYSPRV, OPER or BYPASS.

Note that the UCX$SMTP_SFF.EXE image is only provided as a back door into a
routine in UCX$SMTP_MAILSHR.EXE (smtp_send_from_file) that does the real work.
The reason it is needed at all is because we haven't coded the "Perl extension"
which would allow the Majordomo Perl code to call the 'C' routine in
UCX$SMTP_MAILSHR.EXE directly. Once this is figured out we won't need to go
through SFF anymore. We'll just call UCX$SMTP_MAILSHR.EXE.


How does it work?

See the file SMTP_SFF.TXT for a detailed writeup on the mechanism. (You really
don't need to know this information. Just there FYI.)


How to install the image.

Only one image is installed, copied actually, to SYS$SYSTEM. It is
UCX$SMTP_SFF.EXE. The image for VAX is in [.VAX] and the image for AXP is in
[.AXP]. The VAX image was built on VMS 5.5-2. The AXP image was built on V6.1.
To install the right image turn on your priv's and @ the INSTALL.COM in this
directory.

After this you need to edit your SYLOGIN.COM file to do

$ if f$search("SYS$COMMON:[SYSEXE]UCX$SMTP_SFF.EXE") .nes. "" -
        then sff :== $SYS$COMMON:[SYSEXE]UCX$SMTP_SFF.EXE
