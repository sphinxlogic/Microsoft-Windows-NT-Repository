$ set proc/prior=12
$ set time = 03:00:02
$ copy sys$manager:summer.com sys$manager:daylight.com
$ @sys$manager:daylight.com
$ PURGE sys$manager:daylight.com
$ !
$ !  Run me at 02:00 AM in the spring to set daylight savings time.
$ !  Approximately 3 seconds overhead in starting the job.
$ !
