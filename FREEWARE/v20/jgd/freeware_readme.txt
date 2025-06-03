JGdriver, SOFTWARE, intercept driver to retry errors on any VMS disk

JGdriver is an intercept driver which is used to retry errors on any
VMS disk. It is currently set up to build correctly on VAX systems.

It works by arranging, at start-IO time, that the disk start-io control
goes into it first, and a counter is initialized. Then when the I/O
completes, if it was unsuccessful, it is retried up to some number
of times (255 by default). If it was OK, the I/O is completed normally.

This means that a disk drive that gets some errors can be forced to
have them retried invisibly to the rest of VMS. Now and then some
disk errors will in fact occur which can be retried and which will
not appear all the time. This gives you a way to increase (sometimes
markedly) the reliability of the disk abstraction.

To build:

$ macro jgctl
$ macro jgdriver
$ link jgctl+sys$system:sys.stb/sel
$ link jgdriver+sys$system:sys.stb/sel+sys$input/opt
base=0
$ !
$ copy jgdriver.exe sys$common:[sysexe]
$ copy jgctl.exe sys$common:[sysexe]
$ copy jgctl.cld sys$common:[sysexe]
$ set command sys$system:jgctl
$ sysgen connect jga0:/noada/driver=jgdriver
$ sysgen connect jga1:/noada/driver=jgdriver
... etc....load one JG unit per disk...

$jgctl/verify jga0: dka0:
$jgctl/verify jga1: dka200:

...and so on. Use a different JGAn: unit for each disk.

(To deassign this facility use a command like

$jgctl/deas jga0: dka0:

)

You can do this at any time, and just continue to use the disks
as normal. There will be no observed effects except that transient
errors may disappear, and that fatal errors will take longer to
appear. In normal operation you don't see any change, but it
gives you a safety net. This was concocted for optical disks
originally where the author was seeing error log entries labelled
"recoverable ECC error" and the errors were not being retried in
dkdriver. It corrected the problem completely. 

The driver is also a decent example of a VMS intercept driver.

Glenn C. Everhart
