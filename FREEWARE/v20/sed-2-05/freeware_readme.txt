GNU SED, TOOLS, UNIX Stream Editor for VMS

aug-1995 Sheldon Bishov, bishov@star.enet.dec.com

The source for files was Free Software Foundation, ftp from
anonymous@prep.ai.mit.edu

To build, unzip the source kit, then set default to source directory and 
enter the command:

$ @vmsbuild {param}

where {param} is VAXC (for VAX only) or DECC (for VAX or Alpha).

To run, define a foreign command to sed, e.g., if .exe is in sys$system:
             sed :== $sys$system:sed

For other details, see file readme.; in the source kit.

The following changes were made in the code:

getopt.c: change names of variables optarg, optind, opterr, optopt to have
         prefix "vms" (e.g., vmsoptarg) to avoid conflict with externals
         either in VAXC or DECC.  Also, removed local definition of getopt
         routine.

getopt.h: change variable names, as in getopt.c

getopt1.c: change variable names, as in getopt.c

rx.c: syntax change; original shortcut notation not accepted by VAXC or DECC.

sed.c: syntax change; original shortcut notation not accepted by VAXC or DECC.
         Also change usage for errno (which assumed UNIX environment).

utils.c: switch from use of fwrite to fprintf so that output would be 
         written correctly with redirection (with fwrite would get one
         character per line).

vmsbuild.com: new procedure; adapted from vmsbuild.com for gawk from
         Free Software Foundation.
