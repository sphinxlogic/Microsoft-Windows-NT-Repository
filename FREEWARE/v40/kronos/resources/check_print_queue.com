$ !
$ !  Job to check all of the print queues for stalls
$ !
$ set default kronos_root:[resources]
$ problem = 0
$ !
$ !  Note: the print queue names could be put into a file, but that
$ !  would increase the job's I/O
$ !
$ q = "PRINTER1"
$ gosub check_it
$ q = "PRINTER2"
$ gosub check_it
$ q = "PRINTER3"
$ gosub check_it
$ !
$ if problem .ne. 0 
$   then 
$     close file
$     @message
$     delete message.com'vers'
$   endif
$ !
$ exit
$ !
$check_it: 
$ status = f$getqui ("DISPLAY_QUEUE", "QUEUE_STALLED", Q)
$ if status
$   then
$     gosub problem_is
$     write file "  Queue ''q' is stalled."
$   else
$     status = f$getqui ("DISPLAY_QUEUE", "QUEUE_STOPPED", Q)
$     if status
$       then
$         gosub problem_is
$         write file "  Queue ''q' is stopped."
$       else
$         status = f$getqui ("DISPLAY_QUEUE", "QUEUE_STOPPING", Q)
$         if status
$           then
$             gosub problem_is
$             write file "  Queue ''q' is stopping."
$           else
$             status = f$getqui ("DISPLAY_QUEUE", "QUEUE_PAUSED", Q)
$             if status
$               then
$                 gosub problem_is
$                 write file "  Queue ''q' is paused."
$               endif
$           endif
$       endif
$   endif
$ return
$ !
$problem_is: 
$ if problem .ne. 0 then return
$ problem = 1
$ open/write file message.com
$ file = f$search("message.com;")
$ vers = f$parse (file,,,"VERSION")
$ write file "$ SEND/anon system,operator"
$ write file "Message from Kronos:"
$ return
