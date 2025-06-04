$ !
$ ! ALIGN.COM
$ !
$ ! (See README.TXT for instructions)
$ !
$ run align$tools:read_entry_points
$ run align$tools:resolve_addresses
$ spawn/nowait/input=nl:/output=alignment.log/notify/process=align_fault -
	run align$tools:sys_align_faults
$ exit
