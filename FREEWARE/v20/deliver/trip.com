$ ! TRIP.COM - Send a reply saying this user is on vacation.
$ ! Written by Ned Freed, 19-Oct-1985
$ !
$ ! TRIP.COM sends the file SYS$LOGIN:TRIP.TXT to any user
$ ! who sends mail. Only one copy of TRIP.TXT is sent to a given
$ ! user no matter how many messages he or she sends.
$ !
$ ! Check for replies sent list's presence
$ if F$SEARCH("sys$login:trip.reply") .nes "" then goto search_list
$ open/write tr sys$login:trip.reply
$ goto add_list
$ !
$ ! Check the list for this sender
$ search_list:
$ search/nooutput sys$login:trip.reply "''QFROM'"
$ if $status .eq. 1 then exit
$ open/append tr sys$login:trip.reply
$ !
$ add_list:
$ write tr FROM
$ close tr
$ !
$ mail/noself/subject="Away on a trip" trip.txt "''F$ELEMENT(0," ",QFROM)'"
$ exit
