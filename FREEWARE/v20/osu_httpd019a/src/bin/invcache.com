$!
$! This script invalidates the server file cache.
$!
$    crlf = f$fao("!/")
$    write net_link "<DNETINVCACHE>"
$    write net_link "<DNETTEXT>"
$    copy sys$Input net_link:
200 Sending document
The HTTP server's cache has been invalidated.  Any updated files that
you haven't been seeing will now be visible.
$    write net_link "</DNETTEXT>"
$!
$ exit
