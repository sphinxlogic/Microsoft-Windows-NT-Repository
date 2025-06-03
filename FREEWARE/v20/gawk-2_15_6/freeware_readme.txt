GAWK, TOOLS, GNU AWK for VMS

aug-1995 Sheldon Bishov, bishov@star.enet.dec.com

The source for files was Free Software Foundation, ftp from
anonymous@prep.ai.mit.edu

To build, set default to source directory and enter the command:

$ @vmsbuild {param}

where {param} is VAXC (for VAX only) or DECC (for VAX or Alpha).

To (optionally) load help you can use one of the following methods.
{directory} refers to location where the gawk.hlp and/or gawk.hlb files
are to be located; in the source kit, gawk.hlp is in the [.vms] 
subdirectory of the main source directory:

    - Load help entry into a VMS help library
      e.g., set default to source directory, then enter command:
        $ library/help sys$help:helplib {directory}gawk.hlp

    OR

    - Load help into user library.  Set default to top level source
      directory and enter the following commands:
        $ library/help/create {directory}gawk.hlb
        $ library  {directory}gawk.hlb/help {directory}gawk.hlp
        $ @install_help  {directory}gawk.hlb

For other build and installation details, see the *readme*.* files in 
the source kit.

The following changes were made in the code:

getopt.c: change names of variables optarg, optind, opterr, optopt to have
         prefix "vms" (e.g., vmsoptarg) to avoid conflict with externals
         either in VAXC or DECC.  Also, removed local definition of getopt
         routine.

getopt.h: change variable names, as in getopt.c

getopt1.c: change variable names, as in getopt.c

install_help.com: new procedure for optional installation of help file.

main.c: change variable names, as in getopt.c
