$ ! Procedure:	IMAGE_IDENT.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ set noon
$ on control_y then goto bail_out
$ say = "write sys$output"
$ wildspec = f$edit(P1,"COLLAPSE,UPCASE")
$ if (wildspec .eqs. "") then wildspec = "*"
$ wildspec = f$parse (wildspec,"SYS$SYSTEM:.EXE",,,"SYNTAX_ONLY")
$ if (f$search(wildspec) .eqs. "")
$   then
$   say "%Image not found"
$ else
$ wildspec = wildspec + "*"
$loop:
$   filespec = f$search(wildspec,62533)
$   if (filespec .eqs. "") then goto end_loop
$   displayspec = f$parse(filespec,,,"NAME")
$   open/read/share/error=bad_one ident_file 'filespec'
$   read/error=bad_one/end_of_file=bad_one ident_file header
$   close ident_file
$   cdt = f$file_attributes(filespec,"CDT")
$   image_ident = "Unknown"
$   if (f$cvui(0,16,f$ext(0,2,header)) .lt. 48)
$     then
$     ptr = f$cvui(0,32,f$ext(24,4,header))
$     if (ptr .eq. 0) then goto no_id
$     offset = 56
$     tmp = f$cvui(0,32,f$ext(0,8,header))
$     if (tmp .eq. 2) then offset = 56
$     ptr = ptr + offset
$     fis = f$cvui(0,8,f$ext(ptr,1,header))
$     fi = "''f$ext(ptr+1,fis,header)'"
$   else
$   ptr = f$cvui(0,16,f$ext(6,2,header))
$   if (ptr .eq. 0) then goto no_id
$   offset = 40
$   tmp = "''f$ext(12,4,header)'"
$   if (tmp .eqs. "0204") then offset = 16
$   if (tmp .eqs. "0205") then offset = 40
$   ptr = ptr + offset
$   fis = f$cvui(0,8,f$ext(ptr,1,header))
$   fi = "''f$ext(ptr+1,fis,header)'"
$   endif
$   image_ident = f$edit(fi,"COMPRESS,TRIM")
$no_id:
$   say f$fao(" !46<!AS <!AS>!> created !AS", displayspec, image_ident, cdt)
$!$   say " ''displayspec' <''image_ident'>  created ''cdt'"
$bad_one:
$  goto loop
$end_loop:
$ endif !(f$search(filespec) .eqs. "")
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exitt 1
$ !+==========================================================================
$ !
$ ! Procedure:	IMAGE_IDENT.COM
$ !
$ ! Purpose:	To return the image identification for a given image(s).
$ !
$ ! Parameters:
$ !
$ ! History:
$ !		01-Aug-1997, DBS; Version V1-001
$ !	001 -	Original version.
$ !		09-Apr-1999, DBS; Version V1-002
$ !	002 -	Some cosmetic changes to the output.  Also added an "*" to the
$ !		filespec so the f$search() always works.
$ !-==========================================================================
