X11PERF12, MOTIF_TOYS, X11 server performance test program

The x11perf program runs one or more performance tests and reports timings of
the operations.

Many graphics benchmarks assume that the graphics device is used to display the
output of a single fancy graphics application, and that the user gets his work
done on some other device, like a terminal.  Such benchmarks usually measure
drawing speed for lines, polygons, text, etc.

Since workstations are not used as standalone graphics engines, but as
super-terminals,  x11perf measures window management performance as well as
traditional graphics performace. x11perf includes benchmarks for the time it
takes to create and map windows (as when you start up an application); to map a
pre-existing set of windows onto the screen (as when you deiconify an
application or pop up a menu); and to rearrange windows (as when you slosh
windows to and fro trying to find the one you want).

