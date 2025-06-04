MadGoat SEND V1.0, UTILITIES, Send interactive messages to users
                                       
   [23-JUN-1994]
   
   MadGoat SEND lets users send interactive messages to other users on
   the same system, clusterwide, and via DECnet to remote notes. MadGoat
   SEND is patterned after the Jnet SEND command for BITNET sites. For
   example, a message could be sent to user BRUCE on node BATMAN using:

  $ send batman::bruce "Hello, are you there?"
  $ send batman::bruce
         (BATMAN)BRUCE: These are multi-line broadcasts

   
   
   The message would be broadcast on BRUCE's terminal like:

  (CAVE)ROBIN - Hello, are you there?

   
   
   SEND supports a number of qualifiers to control the broadcasts of the
   messages.
   
   The BLISS sources are distributed with the software.

     _________________________________________________________________
   
  CONTACTING THE AUTHOR
  
   MadGoat SEND was written by Darrell Burkhead. To report bugs, please
   send mail to MadGoat-Bugs@WKUVX1.WKU.EDU. Additional MadGoat mailing
   lists are available as well.
