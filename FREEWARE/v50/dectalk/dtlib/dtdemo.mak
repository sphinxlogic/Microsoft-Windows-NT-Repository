#
# Build the sample program, run after building the library.
#
dtdemo:	dtdemo.o sample.o
	cc dtdemo.o sample.o dtlib.a -o dtdemo

dtdemo.o :	dtdemo.c dectlk.h
sample.o :	sample.c dectlk.h
