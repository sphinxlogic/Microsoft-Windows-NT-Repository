# Make the fifos used by xephem and gscdbd, assuming the default configuration.
# on some systems, you make fifos using mknod as follows "mknod <file> p"

all: xephem_loc_fifo xephem_db_fifo xephem_in_fifo xephem_out_fifo

xephem_loc_fifo:
	mkfifo $@

xephem_db_fifo:
	mkfifo $@

xephem_in_fifo:
	mkfifo $@

xephem_out_fifo:
	mkfifo $@
