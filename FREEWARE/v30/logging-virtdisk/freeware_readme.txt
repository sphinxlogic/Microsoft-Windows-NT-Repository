LOGGING-VD, Storage Management, Virtual disk that logs all writes to a logfile

This area contains my VAX driver (not touched for a few years) that
can (using logdata.ftn) log writes to the device to a logfile. The
idea was to give continuous backup. The logfile contains LBN, time
(to 64 bits), and PID of the writer, and the data written so that it
can be re-recorded to play forward from a physical backup up to any
time. The logger currently writes a block's worth at a time, though
that can easily be altered. FVdriver, LOGdata.ftn, and FVHST* are
the relevant routines.

Also an older version of my back/phys tape as disk utility sources are
present here in zoo files.

Creation dates are not all exact; these have been copied and renamed
a number of times. The files data from generally late 1991 to early
1992.

Glenn Everhart



(ps - check out Safetyspd.txt here...some useful things are described.)
