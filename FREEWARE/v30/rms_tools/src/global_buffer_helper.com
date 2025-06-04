$!Hein van den Heuvel, Sophia Antipolis, June 1990
$!		Oct-1991, throw in control block pages.
$
$! This command file accepts a wildcard file specification as input
$! and calculates the relevant global buffers parameters found.
$
$IF p1.EQS."" THEN INQUIRE P1 "(Wildcard) Filespec"
$IF p1.EQS."" THEN EXIT
$say = "WRITE sys$output "
$files = 0
$pages = 0
$buffers = 0
$sections = 0
$loop:
$file = F$SEARCH(p1)
$IF file.EQS."" THEN GOTO done
$files = files + 1
$gbc = F$FILE(file,"GBC")
$org = F$FILE(file,"ORG")
$IF gbc.EQ.0 THEN GOTO loop
$IF sections.EQ.0 
$  THEN
$    say ""
$    say "GBC	BKS	ORG	NAME"
$    say "---------------------------------------------------------------------"
$ENDIF
$IF org.EQS."SEQ" 
$  THEN bks = f$getsy("RMS_DFMBC")
$  ELSE bks = F$FILE(file,"BKS")
$ENDIF
$sections = sections + 1
$overhead_pages = (88 + (gbc * 48) + 511)/512   ! RND # pages for GBH and GBD's
$pages = pages + ( bks * gbc ) + overhead_pages + 1 	!Stopper page
$buffers = buffers + gbc
$say gbc,"	",bks,"	",org,"	",file
$GOTO loop
$
$done:
$IF files.EQ.0 
$ THEN say "No files found using : ",p1
$ ELSE 
$   IF sections.EQ.0
$   THEN say "No files with global buffers found out of ",files, " Files tried."
$   ELSE 
$	say ""
$	say buffers, " Global Buffers for ",sections, -
			  " Files, requiring ", pages, " Global Pages."
$	say "Please make sure the SYSGEN params allow for:
$	say ""
$	say "	RMS_GBLBUFQUO = ", buffers
$	say "	GBLSECTIONS   = ", sections
$	say "	GBLPAGES      = ", pages
$	say "	GBLPAGFIL     = ", pages
$   ENDIF
$ENDIF
