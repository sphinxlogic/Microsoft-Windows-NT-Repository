XMARK, GRAPHICS, The XMARK benchmark script, written as a C program
!
A favorite benchmark for X11 is the xmark.  Unfortunately, the xmark
benchmark was a Unix script, which makes it hard to use on OpenVMS.
This program implements the script for V1.15 of xmark as a C program.

You must have a valid x11perf data file, which can be created by:

$ define/user sys$output xperf.dat
$ x11perf -rop "GXxor" "GXcopy" -all

