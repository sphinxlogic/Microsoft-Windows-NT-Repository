UPTIME, SYSTEM_MANAGEMENT, Show system uptime and reboot time

UPTIME is a quick means of determining when a VMS system was
    last booted. It is designed to display uptime and reboot date
    on the current node, but could easily be modified to display
    the same info for each node in the cluster.

Output example:

$ uptime == "$disk:[dir1.dir2]uptime.xxx_exe"
$ uptime

        Uptime:     5 21:38:39.32
        Reboot:   Oct 30 14:55:52

The images in this directory were built on VMS 7.1 and DecC 5.6 but it
should build on any v5.5+ system, even using VaxC

Lyle W. West

