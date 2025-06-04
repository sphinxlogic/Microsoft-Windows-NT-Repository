DELIVER, Utilities, Handle incoming mail messages automatically 

DELIVER is an adjunct to VMS MAIL which makes it possible for incoming
mail  messages  to  be  handled  and  processed automatically based on
information provided in a user-supplied file.  Different  actions  can
be  taken  based  on  a message's address, subject or contents.  These
actions include delivering the message, forwarding the message or even
invoking  a DCL command script to perform some complex operation.  Any
actions taken occur immediately upon receipt of the message; the  user
does not need to log in for DELIVER to operate.

DELIVER is modelled after the MAILDELIVERY facility of the  MMDF  mail
system.   DELIVER  is,  however, completely distinct from MMDF and the
formats of .MAILDELIVERY files for MMDF and  MAIL.DELIVERY  files  for
DELIVER are dissimiliar.

Full documentation is available in DELIVER.MEM
