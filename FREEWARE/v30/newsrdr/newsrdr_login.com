$!++
$! Sample NEWSRDR login command procedure for NEWSRDR
$! 26-Oct-1993
$!--
$!
$! The following assumes the NEWSRDR image is located in the
$! same directory as this file, and that the files are local:
$!
$ here = F$ENVIRONMENT("PROCEDURE")
$ here = F$PARSE(here,,,"DEVICE")+F$PARSE(here,,,"DIRECTORY")
$ NEWS*RDR :== $'here'NEWSRDR
$!
$!-----
$!
$! If you want mailed replies to your articles to be delivered somewhere
$! else, edit and uncomment the following:
$!
$! DEFINE NEWSRDR_REPLY_TO "username@hostname"
$!
$!-----
$!
$! If you want to put your NEWSRDR profile and/or key definitions file
$! someplace other than your login directory, edit and uncomment the
$! following:
$!
$! DEFINE NEWSRDR_PROFILE disk:[directory]NEWSRDR_PROFILE.NRPF
$! DEFINE NEWSRDR_KEYDEFS disk:[directory]NEWSRDR_KEYDEFS.NRKEY
$!
$!-----
$!
$! If you typically read newsgroups containing more than 128 new
$! articles per session, or you usually need to scan more than 128
$! articles per newsgroup at once, you may get increased performance
$! by increasing the header cache size (edit and uncomment):
$!
$! DEFINE NEWSRDR_HEADER_CACHE_SIZE 128 
$!
$ EXIT
