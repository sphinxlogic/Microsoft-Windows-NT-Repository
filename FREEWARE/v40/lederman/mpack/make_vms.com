$!  Make_vms.com for MPACK
$!  based loosely on the Unix makefile
$!  22-SEP-1995, David Mathog, Biology division, Caltech
$!
$!  03-May-1996 B. Z. Lederman.  Added prototypes and other
$!                               definitions to the code, so
$!                               there aren't so many warnings.
$!
$!  This one doesn't check dependencies - it just builds everything in
$!  a single pass.  Tested on OpenVMS 6.1 with DECC 4.1
$!
$ write sys$output "Expect about a billion implicit function warnings!"
$!
$! mycc :== cc/nolis/standard=ansi89/prefix=all/include=[]
$!
$ mycc :== cc /nolis /prefix=all /include=[]
$!
$! do it this way so that people who maintain Unix makefile can also guess
$! correctly what to put here.
$!
$! compile all files - if they get compiled by Makefile, there should be
$! a corresponding line here.  Everything BUT the two mains gets compiled
$! first.
$!
$ delete/nolog *.obj.*
$ mycc codes
$ mycc decode
$ mycc encode
$ mycc magic
$ mycc md5c
$ mycc part
$ mycc string
$ mycc vmsos
$ mycc uudecode
$ mycc xmalloc
$!
$ lib/create/object mplib.olb *.obj
$ purge/nolog mplib.olb
$!
$!  Now compile the two main() pieces
$!
$ mycc vmspk
$ mycc vmsunpk
$!
$! link the two main pieces
$!
$ link/nomap/exe=mpack vmspk,mplib/lib
$ link/nomap/exe=munpack vmsunpk,mplib/lib
$!
$! build the help library
$!
$ library/help/create mpack mpack
$!
$! make sure everything can be seen/used by anybody
$!
$ set file/prot=w:re *.exe,*.hlb
$!
$ type sys$Input

Build completed, you can now use:

  mpack.exe
  munpack.exe
  mpack.hlb

Set these up as is typical for your site, as in:

  $ mpack     :== $device:[directory.mpack]mpack
  $ munpack   :== $device:[directory.mpack]munpack
  $ mpackhelp :== help/library=device:[directory.mpack]mpack.hlb

Note that to send e-mail directly, or to post to a news server
you must also define:

  $ mmail     :== @device:[directory.mpack]mmail
  $ mnews     :== @device:[directory.mpack]mnews

These work with MULTINET - if you have something else you will likely
need to modify them.
