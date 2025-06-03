DCLCOMPLETE, UTILITIES, Add command completion and pipes to DCL

This version of DCLCOMPLETE adds the following (mis?)features
to the original.  

        <TAB>   will complete decnet filespecs and top level
                qualifiers now.

        "?"     will display a list of filenames, commands, or
                top level qualifiers that match the incomplete
                filespec.  See NOTE 2.

        " > "   will redirect sys$output to a file.

        " < "   will redirect sys$input from a file.

        " ; "   will execute multiple commands in the context
                of a subprocess.  See Note 1.

        " | "   will execute a series of commands in the context
                of subprocess where sys$output of commands to the
                left are redirected to sys$input of commands to the
                right.  See Note 1.

        """     double quotes are checked to see if the <tab> or "?"
                are part of a quoted string.  If they are then aren't
                used to complete or list files or commands.
        
        "\"     will quote a single instance of<TAB> or "?" 
                to allow these characters to be used on a DCL
                command line without quotes.  


Included with DCLCOMPLETE is a companion program called PIPE that is
needed to handle Piped and multiple commands.  To use DCLCOMPLETE
and PIPE do the following:

        $MACRO DCLCOMPLETE              !You might want to use /LIS 
        $MACRO PIPE                     !on these also
        $LINK/NOTRACE DCLCOMPLETE       !If you don't install don't
        $                               !need the notrace
        $LINK PIPE                      !You might also want /MAP
        $PIPE :== $yourdisk:[pipedirectory]PIPE.EXE
        $                               !remember to fill in the correct
        $                               !yourdisk and pipedirectory
        $RUN DCLCOMPLETE                !all set ready to use assuming
        $                               !you have CMKRNL privs
           when used in conjunction with DCLcomplete.  E.G. the string
                "<PF1 string> \?" 
           will be echoed
                "<PF1 string> \<PF1 string> ?" 
           ^R should restore the screen correctly.
        2. When using the PIPE program to pipe output to another program,
           if one of the subprocess terminates incorrectly the program
           will wait indefinitely for the final subprocess to terminate.
           You will need to ^Y and cleanup "stray" subprocesses.

Note 4:
        DCLCOMPLETE requires CMKRNL privilege inorder to work.
        Although the authors have made good faith attempts to see
        if there are any problems with the program CMKRNL programs
        are inherently dangerous.  Furthermore the tests have been
        carried out in limited environments.  In otherwords, the
        programs here seem to work for us and we don't know of any
        problems (other than those stated) but if it doesn't work
        for you...you're on your own.  Proceed with caution.
