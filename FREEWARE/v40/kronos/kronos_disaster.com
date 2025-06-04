$ !
$ ! This file is submitted to run at 2:00 o'clock in the morning tomorrow.
$ ! If Kronos is running, it will delete the job at 1:00 o'clock in the
$ ! morning and then submit it to run tomorrow, again.  As a result, the
$ ! mail message will never be sent unless Kronos has failed by 1:00.
$ !
$ MAIL/SUBJECT="Major Disaster" KRONOS_DIR:DISASTER.DAT -
  IRENE,RAGOSTA,HOLST
