$ set proc/prior=12
$ wait 02:00:00
$ set time = 01:00:02
$ copy sys$manager:winter.com sys$manager:daylight.com
$ @sys$manager:daylight.com
$ PURGE sys$manager:daylight.com
$ !
$ ! Run in the fall to return system to standard time
$ ! Approximately five seconds overhead
$ ! Note... run at 00:00 (midnight) in the fall.  This plus the WAIT
$ !  command prevent Kronos from submitting the job continously
