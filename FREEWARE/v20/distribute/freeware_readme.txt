DISTRIBUTE, TOOLS, Allows embedded distribution lists withn a document

   DISTRIBUTE allows you to embed a distribution list within the body  of
   a document that you want to mail.

   To run DISTRIBUTE, invoke it as follows:

           $ @SYS$SYSTEM:DISTRIBUTE
           Message file:   (type your filespec here)
           Subject:        (type subject of message)

   DISTRIBUTE.COM will read the message file,  looking  for  distribution
   information.  Addresses are recognised by being enclosed in "(*D:" and
   ")" brackets; any other text is ignored.  For example,  your  document
   might contain:

           To:     Ronnie          (*D: WHITEH::REAGAN)
                   Maggie          (*D: NMBA10::THATCHER)
                   Mike            (*D: KREMLN::GORBACHEV)
                                   (*D: ENDLIST)

   The "ENDLIST" entry is not strictly  necessary;  it  tells  DISTRIBUTE
   that  there  are  no  more addresses.  This can result in a worthwhile
   timesaving, since otherwise DISTRIBUTE would have to  read  until  the
   end of the document looking for further addresses.

   After having constructed the complete address  list,  DISTRIBUTE  will
   send  the  document to all intended recipients.  If Nmail is installed
   on your system then it will be used---you can  use  the  normal  Nmail
   commands to check on the progress of your message.


   [End]
