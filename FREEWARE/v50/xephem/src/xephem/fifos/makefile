# Make the fifos used by xephem, assuming the default configuration.
#
# on non-POSIX systems, you make fifos using mknod as follows:
#    mknod <file> p

all: xephem_loc_fifo xephem_db_fifo xephem_in_fifo xephem_out_fifo

xephem_loc_fifo:
	mkfifo $@

xephem_db_fifo:
	mkfifo $@

xephem_in_fifo:
	mkfifo $@

xephem_out_fifo:
	mkfifo $@

# For RCS Only -- Do Not Edit
# @(#) $RCSfile: Makefile,v $ $Date: 1996/08/26 19:40:47 $ $Revision: 1.1 $ $Name:  $
