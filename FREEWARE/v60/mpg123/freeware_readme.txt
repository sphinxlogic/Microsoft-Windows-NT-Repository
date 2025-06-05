MPG123,UTILITIES,Plays MP3 files from local files or URLs

    This is a modifed version of mpg123 v0.59s.  This program was last
    modified by the author in October, 2000 to revision mh4.  I have made
    the following changes:

      o fixed the http code so that the ability to read an MP3 file
        directly from a URL now works on VMS.  You can now issue the
        command mpg123 http://somewhere/something.mp3 and it will read
        the file directly from the web for you.

      o Rewrote the VMS sound support.  The file included with the kit
        from the author's web site (AUDIO_DEC.C) works by constantly
        allocating and freeing buffers from shared memory space.  There
        appears to be an error in the MMOV libraries that handle this
        work since, if you play a playlist of several songs length, this
        program will eventually start getting memory allocation failures.  
        I have changed the code to only allocate a buffer once and then 
        reuse it over and over.  Also, the supplied code did not
        return all of the sound formats supported by the various audio
        cards.  This has also been corrected.  My version of the driver
        is called AUDIO_VMS.C.  You are free to use whichever you prefer.

    Tools needed:

    UNZIP		(You must use the -V switch when unzipping)
    DEC C compiler
    Multimedia Services for OpenVMS Alpha V2.2 (Development kit)

    Build Instructions:

    $ unzip mpg123-0_59s-mh4.zip
    $ set default [.mpg123-0_59s-mh4]
    $ @build

    I have included an executable in this distribution.  It should work
    on any recent version of VMS.  Since it is built against a patched
    version of MMOV.OLB that has not yet been released to the public, I
    recommend using it, rather than rebuilding, if you can.

    Mark Berryman

