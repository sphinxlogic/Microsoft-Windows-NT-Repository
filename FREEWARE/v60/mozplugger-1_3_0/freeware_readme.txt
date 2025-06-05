MOZPLUGGER,UTILITIES,plugin handler for Mozilla on VMS

    This is a port of mozplugger V1.3.0 for use with Mozilla on OpenVMS.
    It enables one to use standalone programs as if they were plugins for
    Mozilla.  For example, one can use XPDF to display PDF files and
    mozplugger will cause XPDF to be embedded in the browser window.  You
    can also use MPG123 to play streaming MP3 files and Timidity to play
    background midi files.

    Tools needed:

    UNZIP		(you must use the -V switch when unzipping)
    DEC C compiler
    Mozilla web browser (or CSWB)
    OpenVMS Porting Library (VMS_JACKETS.EXE) minimum version A8.

    After unzipping the mozplugger.zip file, set default to the
    mozplugger-1_3_0 directory and read README.VMS for build and
    installation instructions.  MOZPLUGGER.TXT documents the keywords in
    the mozpluggerrc. file.

    Mark Berryman

