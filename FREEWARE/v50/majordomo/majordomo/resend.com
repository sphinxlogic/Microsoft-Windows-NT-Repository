$ DEFINE/USER SYS$INPUT 'P1
$ perl UCX$MAJORDOMO_HOME:RESEND. 'P2
$ WRITE SYS$OUTPUT "Perl is done now"
