$! This utility saves the RMS attributes of a file and then GZIPs it.
$!
$! Newsgroups: comp.os.vms
$! Subject: GZIP companion:  SQUEEZE.COM
$! Date: 30 Jan 2001 18:18:28 GMT
$! Message-ID: <9570hk$9i6$1@top.mitre.org>
$!
$! --Keith Lewis              klewis_no_spam@mitre.org
$! The above may not (yet) represent the opinions of my employer.
$!
$!
$!  Squeeze.Com
$!
$!     ENK  -  21-Feb-00
$!     ENK  -  13-Mar-00  save security settings onto com file
$!
$  say :== write sys$output
$  say " "
$  say "---------------------------------------------------------------------"
$  say " Squeeze.Com uses GZIP for compression; however, the input file's "
$  say "  attributes must be changed to ensure a reliable compression.  GZIP " 
$  say "  replaces the input file with a compressed file having the same name "
$  say "  but with -GZ appended to the file type."
$  say " "
$  say " Squeeze.Com saves the input's attributes in a -UNSQUEEZE com file in "
$  say "  the same directory as the compressed file.  This com file could be "
$  say "  used to decompress the file and restore the attributes at a later date"
$  say "  but it can not delete itself afterwards.  A better solution is to use"
$  say "  UnSqueeze.Com which executes the -UNSQUEEZE file and then deletes it."
$  say " "
$  say "  The input file(s) can be specified with wild cards to allow multiple "
$  say "  versions and even whole directory trees."
$  say " "
$  say "  To allow compression of multiple versions of the same file, both the "
$  say "   UNSQUEEZE suffix and the GZ suffix will have -Vxx added to their "
$  say "   file type - where xx is the version number."
$  say " "
$  say " NOTE:  Only one version can be decompressed back into a directory - see"
$  say "        UnSqueeze.Com for details."
$  say "---------------------------------------------------------------------"
$  say " "
$  wait 00:00:01
$!
$  firstfile = "yes"
$ask_file:
$  if p1 .eqs. ""
$   then
$    if f$environment("interactive")
$     then
$      inquire p1 "Please enter the full name of input file "
$      say "   "
$      goto ask_file
$     else
$      say "No input file specified _ exiting"
$      exit
$    endif
$  endif
$!
$file_loop:
$  target = f$search(p1)
$!
$  if target .eqs. ""
$   then
$    if firstfile .eqs. "yes"
$     then
$      firstfile = "no"
$      say "  "
$      say " ****** Can not locate any files with ''p1'"
$      say "  "
$      exit
$     else
$      exit
$    endif
$  endif
$!
$  firstfile = "no"
$!
$  filetype = f$parse(target,,,"type")
$  if filetype .eqs. ".DIR" then goto file_loop
$!
$  typlen = f$length(filetype)
$  if f$locate("-GZ-V",filetype) .lt. typlen then goto file_loop
$  if f$locate("-UNSQUEEZE-V",filetype) .lt. typlen then goto file_loop
$!
$  filename = f$parse(target,,,"name")
$  filevers = f$parse(target,,,"version")
$  novers = target - filevers
$  filevers = f$integer(filevers-";")
$  archive  = novers + "-GZ-V''filevers'"
$  zip   = novers + "-GZ"
$!
$  say " "
$  say " "
$  say " Input file = ''target' "
$  say " Output file = ''archive' "
$  say " "
$  say " "
$!
$  cfile = filename + filetype
$!
$  archive_loc = f$parse(target,,,"device") + f$parse(target,,,"directory")
$!
$  attrib_file = archive_loc + cfile + "-UNSQUEEZE-V''filevers'"
$!
$  lrl = f$file_attributes("''target'","lrl")
$  mrs = f$file_attributes("''target'","mrs")
$  rfm = f$file_attributes("''target'","rfm")
$  rat = f$file_attributes("''target'","rat")
$  if rat .eqs. "" then rat = "NONE"
$!
$  open/write attrib 'attrib_file'
$   write attrib "$!  "
$   write attrib "$!  ",target 
$   write attrib "$!  ",f$time() 
$   write attrib "$!  "
$   write attrib "$! say :== write sys$output "
$   write attrib "$  say "" "" "
$   write attrib "$  say ""-------------------------------------------------"" "
$   write attrib "$  say "" "" "
$   write attrib "$!  "
$   write attrib "$  here = f$environment(""default"") "
$   write attrib "$!  "
$   write attrib "$  there = f$environment(""procedure"") "
$   write attrib "$  device = f$parse(there,,,""device"") "
$   write attrib "$  direct = f$parse(there,,,""directory"") "
$   write attrib "$  procdir = device + direct "
$   write attrib "$  set default '","procdir'"
$   write attrib "$!  "
$   write attrib "$  type = f$parse(there,,,""type"") "
$   write attrib "$  typlen = f$length(type)  "
$   write attrib "$  addpoint = f$locate(""-UNSQUEEZE-V"",type)  "
$   write attrib "$  filetype = f$extract(0,addpoint,type)  "
$   write attrib "$  filevers = f$extract(addpoint+12,typlen-addpoint,type)  "
$   write attrib "$  filename = f$parse(there,,,""name"") "
$   write attrib "$!  "
$   write attrib "$  base = filename + filetype  "
$   write attrib "$!  "
$   write attrib "$  test = f$search(base)"
$   write attrib "$  if test .nes. """" " 
$   write attrib "$   then
$   write attrib "$    say "" "" "
$   write attrib "$    say test,"" already exists"""
$   write attrib "$    say "" "" "
$   write attrib "$    exit
$   write attrib "$  endif
$   write attrib "$!  "
$   write attrib "$  say "" "" "
$   write attrib "$  say ""Decompressing "",base
$   write attrib "$  say "" "" "
$   write attrib "$!  "
$   write attrib "$  from = base + ""-GZ-V"" + filevers "
$   write attrib "$  to = base + ""-GZ""  "
$   write attrib "$  rename '","from'","  ","'","to'"
$   write attrib "$  on ERROR then rename '","to'","  ","'","from'"
$   write attrib "$!  "
$   write attrib "$  say "" "" "
$   write attrib "$  say "" Start "",f$time() "
$   write attrib "$  say "" "" "
$   write attrib "$!  "
$   write attrib "$  gunzip -N9 '","base'"
$   write attrib "$!  "
$   write attrib "$  say "" "" "
$   write attrib "$  say "" End "",f$time() "
$   write attrib "$  say "" "" "
$   write attrib "$!  "
$   write attrib "$  rename '","base'",";1","  ;'","filevers'"
$   write attrib "$!  "
$   write attrib "$  set file/attr=lrl:''lrl' ","'","base'"
$   write attrib "$  set file/attr=mrs:''mrs' ","'","base'"
$   write attrib "$  set file/attr=rfm:''rfm' ","'","base'"
$   write attrib "$  set file/log/attr=rat:''rat' ","'","base'"
$   write attrib "$  say "" "" "
$   write attrib "$!  "
$   write attrib "$  set default '","here'"
$   write attrib "$!  "
$   write attrib "$  exit
$  close attrib
$  set security/class=file/like=name='target' 'attrib_file'
$  say " Created ''attrib_file' "
$  say " "
$!
$  set file/log/attr=rfm:stmlf 'target'
$  say " "
$  say " Starting compression ''f$time()' - please wait "
$  say " "
$!
$  gzip -N9 'target'
$  rename 'zip' 'archive'
$!
$  say " "
$  say " Compression finished at ''f$time()'"
$  say " "
$  say "---------------------------------------------------------------------"
$  say " "
$  say " "
$  goto file_loop
