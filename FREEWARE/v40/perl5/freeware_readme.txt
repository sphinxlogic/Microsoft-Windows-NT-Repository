OpenVMS PERL, Languages, Scripting Language

I. Summary:

    This is a port of PERL5 to VMS.  It contains all the information and files
that are required to build PERL5 on OpenVMS VAX and AXP systems.  The
readme.vms file in the [.perl5_000] directory that is unpacked explains how to
start building the tool.  It also provides pointers to the people who are
actively porting PERL5 to VMS, and you can get on their distribution list if
you desire.  The zip file also contains the freeware license statement as it
applies to Perl5.

II. Unpacking Perl once it is on your VMS system:

        1. Perl comes in a ZIP file.  Define a foreign symbol to
           point to your ZIP executable.

                uzip :== $ TOOLS$:[ZIP]vmsunzip.exe;


        2. Simply use the following command to unpack the zip file,
        
                $ uzip -x your_zip_file.zip

        3. TROUBLE SHOOTING the ZIP file

                a. Sometimes the unzip image that we have for VMS
                   will not understand a file format (or something).
                   The UNZIP simply stops with an error.  If this happens
                   with the freeware ZIP image, the following steps
                   will help you recover.
                   

                b. Note the last file 'Inflated', it was successful.
                   Now get a directory of the files using

                        uzip -v zip_file.zip

                c. Look for the filename of the last file that was 
                   successfully unpacked before the uzip failed.

                d. Now look at the very next file(s) in the listing
                   and manually uzip the next file (or several following files)
                   then retry the next command.

                e. Now you should be able to run uzip as follows:

                        uzip -xn your_zip_file.zip

                f. This will unpack your zip file and not overwrite
                   existing files.  Since you unpacked the problem
                   files, unzip should progress to the end of the file.
                   If not, manually unpack the next file after the last
                   successful file and try this command again.

                f1. Don't ask me why, but this even fails sometimes no matter
                    how many files you correctly unpack!  This sequence 
                    eventually got the whole thing unpacked after numerous 
                    errors.

                        uzip -u the_zip file
                        uzip -o the_zip_file
                        uzip -xno the_zip_file

                    Now, the -o produces a command line warning that it can not
                    be specified with  -n, BUT BUT BUT, the whole thing unpacked
                    without output statements.  I couldn't believe it!  Must be
                    a weird zip image that I have used.

                    Note that the zip image that I used may be old or buggy 
                    and if you simply make enough contortions when calling the
                    program to unzip your file, it will hiccup the right 
                    behavior. See the bizarre sequence above as example of a 
                    contortion!  Oh well...

                g. This is how to get through an unzip that has errors.
                   We probably need a later version of unzip that can 
                   read these newer file types and data, or one that is
                   compatible with the ZIP program used by the Perl folks.


