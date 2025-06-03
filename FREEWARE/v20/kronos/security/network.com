$ set default KRONOS_ROOT:[security]
$ acc/since=-0007-00:00:00/output=network.acct/full/type=(logfail,process) -
   /process=network
$ run network
$ if $status .eq. 3 then -
  mail/subject="This week on MRL" network.out system,operator
$ delete network.acct;*
$ delete network.out;*
