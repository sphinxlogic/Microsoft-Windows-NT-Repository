$ !!! ======================================================================
$ !!!  @VAX-VMS-shell-file{
$ !!!     filename        = "gopherprint.com",
$ !!!     version         = "1.01",
$ !!!     date            = "26 March 1993",
$ !!!     time            = "08:31:20 CST",
$ !!!     ISO-date        = "1993.03.26",
$ !!!     author          = "George D. Greenwade",
$ !!!     address         = "Department of Economics and Business Analysis
$ !!!                        College of Business Administration
$ !!!                        P. O. Box 2118
$ !!!                        Sam Houston State University
$ !!!                        Huntsville, Texas, USA 77341-2118",
$ !!!     email           = "bed_gdg@SHSU.edu (Internet)
$ !!!                        BED_GDG@SHSU     (BITNET)
$ !!!                        SHSU::BED_GDG    (THENET)",
$ !!!     telephone       = "(409) 294-1266",
$ !!!     FAX             = "(409) 294-3712",
$ !!!     supported       = "yes",
$ !!!     archived        = "Niord.SHSU.edu:[FILESERV.VMS-GOPHER.CLIENT-1_12]",
$ !!!     keywords        = "gopher, client, 1.12, print, VMS",
$ !!!     codetable       = "ISO/ASCII",
$ !!!     checksum        = "09398 163 1072 7880",
$ !!!     docstring       = "Command file driver to print files from within
$ !!!                        gopher sessions on VMS.  The default behavior
$ !!!                        of the version 1.12 gopher client for VMS may
$ !!!                        delete the file prior to the VMS Queue
$ !!!                        Manager's handling of the file.  We get around
$ !!!                        this by copying the file to SYS$SCRATCH, then
$ !!!                        using that file for the actual print command
$ !!!                        with a /DELETE option of the DCL PRINT command
$ !!!                        verb being imposed (i.e., no change to the
$ !!!                        distributed official code for the gopher client
$ !!!                        is required to use this).
$ !!!
$ !!!                        The gopher client option "O" (uppercase letter
$ !!!                        oh) on a gopher menu will allow the print
$ !!!                        command to be specified by user as:
$ !!!                          @DEVICE:[DIR]GOPHERPRINT.COM
$ !!!                        or
$ !!!                          @LOGICAL_DIRPATH:GOPHERPRINT.COM
$ !!!                        or whatever local definition is necessary.
$ !!!                        Alternately, the definition may be used in
$ !!!                        the CONF.H file of the official distribution
$ !!!                        prior to compiling the code to use this as a
$ !!!                        global default definition for your site.
$ !!!
$ !!!                        This command file relies on the logical
$ !!!                        GOPHERQUEUE, which is user-specificable.  If
$ !!!                        the logical is undefined, the implicit queue
$ !!!                        used is SYS$PRINT (which should exist on
$ !!!                        VAX/VMS systems; this may easily be edited
$ !!!                        for local needs).
$ !!!
$ !!!                        If GOPHERQUEUE is defined with additional
$ !!!                        parameters, it should be defined with quotes
$ !!!                        around the definition, such as:
$ !!!                          DEFINE GOPHERQUEUE "ECO$LASER/FORM=1LN3P6"
$ !!!                        or whatever is required for local queues, forms
$ !!!                        and other options availablevia the VMS Queue
$ !!!                        Manager.  One interesting twist is that users
$ !!!                        can use:
$ !!!                          DEFINE GOPHERQUEUE PORT
$ !!!                        to print to an attached printer (see code for
$ !!!                        how this is achieved).  It is assumed that
$ !!!                        users will place this definition within their
$ !!!                        individual LOGIN.COM files, although some -
$ !!!                        specification is system-wide parameters can
$ !!!                        easily be used.
$ !!!
$ !!!                        The checksum field above contains a CRC-16
$ !!!                        checksum as the first value, followed by the
$ !!!                        equivalent of the standard UNIX wc (word
$ !!!                        count) utility output of lines, words, and
$ !!!                        characters.  This is produced by Robert
$ !!!                        Solovay's checksum utility.",
$ !!!     revisions       = "Revised by GDG on 26 March 1993 to reflect:
$ !!!                        (1) redefined symbol GOPHERQUEUE (removed
$ !!!                            leading ':' in equivalence string as it
$ !!!                            caused the symbol to not be defined);
$ !!!                        (2) added /NONOTIFY to PRINT command default
$ !!!                            string (don't want end of printing
$ !!!                            notification to screw up menu);
$ !!!                        (3) added /NOCONFIRM to PRINT command default
$ !!!                            string (in case /CONFIRM is part of user's
$ !!!                            own definition of PRINT symbol -- can't
$ !!!                            be used in this context)."
$ !!! }
$ !!! ======================================================================
$ !!!
$ !!! No matter what, we want to ensure that we keep a clean trail, so
$ !!! define what to do in the event of Control/Y.
$ !!!
$ ON CONTROL_Y THEN GOTO ABORT
$ !!!
$ !!! First, generate a quasi-random number for the file name.
$ !!!
$ ANUM = F$EXTRACT(21,2,F$TIME())
$ !!!
$ !!! Second, copy the 'P1 filename passed by the client to SYS$SCRATCH:.
$ !!!
$ COPY/NOLOG/NOCONFIRM 'P1 SYS$SCRATCH:GOPHERPRINT'ANUM'.TMP
$ !!!
$ !!! Third, determine the queue name and associated parameters from
$ !!! the logical GOPHERQUEUE, if it exists.  If it does assign the symbol.
$ !!! MODIFIED: 26 March 1993 -- see "revisions" in header (GDG)
$ !!!
$ GOPHERQUEUE = F$EDIT(F$TRNLNM("GOPHERQUEUE"),"UPCASE")
$ !!!
$ !!! If doesn't, use SYS$PRINT and no further parameters for the symbol.
$ !!!
$ IF GOPHERQUEUE .EQS. "" THEN GOPHERQUEUE := SYS$PRINT
$ !!!
$ !!! Finally, present the possibility of printing to an attached printer
$ !!! with GOPHERQUEUE defined as PORT.
$ !!!
$ IF GOPHERQUEUE .NES. "PORT" THEN GOTO SYSTEM_QUEUE
$ !!!
$ !!! Change Control/Y handling is different since this is to PORT.
$ !!!
$ ON CONTROL_Y THEN GOTO ABORT_PORT
$ !!!
$ !!! Set the file up so it can be COPY'ed to TT: device (/dev/tty for
$ !!! the U*ix crowd is TT: for the VMS crowd)...
$ !!!
$ ESC[0,7] = 27
$ WRITE SYS$OUTPUT "''ESC'[5i"
$ !!!
$ !!! then copy it there (and neither ask nor log it)!
$ !!!
$ COPY/NOLOG/NOCONFIRM SYS$SCRATCH:GOPHERPRINT'ANUM'.TMP TT:
$ABORT_PORT:
$ !!!
$ !!! Ensure PORT is turned off, then finish up.
$ !!!
$ WRITE SYS$OUTPUT "''ESC'[4i"
$ GOTO ABORT
$ !!!
$SYSTEM_QUEUE:
$ !!!
$ !!! We didn't use PORT, so pass everything to the VMS Queue Manager for
$ !!! processing, being sure to clean up after ourselves.
$ !!! MODIFIED: 26 March 1993 -- see "revisions" in header (GDG)
$ !!!
$ PRINT/QUEUE='GOPHERQUEUE'/DELETE/NOCONFIRM/NONOTIFY -
     SYS$SCRATCH:GOPHERPRINT'ANUM'.TMP
$ GOTO EXIT
$ !!!
$ABORT:
$ !!!
$ !!! Be sure to clean up after ourselves.  If a system queue is successfully
$ !!! used via VMS Queue Manager, we don't want to see this; otherwise we do.
$ !!!
$ IF F$SEARCH("SYS$SCRATCH:GOPHERPRINT''ANUM'.TMP") .NES "" THEN -
      DELETE/NOLOG/NOCONFIRM SYS$SCRATCH:GOPHERPRINT'ANUM'.TMP.*
$ !!!
$EXIT:
$ !!!
$ !!! Finally, a graceful exit, providing a usable and seamless way to
$ !!! print files with the VMS Gopher client (1.12) without having to
$ !!! alter the distributed code.
$ !!!
$ EXIT
