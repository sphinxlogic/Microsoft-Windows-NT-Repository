CALLBACK, UTILITIES, VAX callback utilities for Hayes modems


  Written by Robert Eden   robert@cpvax.tu.com
             Texas Utilities
             Comanche Peak Steam Electric Station
             (817) 897-0491
 
 If you use this program, why not drop me a line and let me know!

 This program fakes a normal hayes modem connected to a terminal server
 into calling you back.

 For security reasons, auto answer is disabled at the start of the sequence.
 If a failure is detected, the program logs out the user, leaving the modem
 at a terminal server prompt (with auto-answer turned off).  The terminal
 server's inactivity timer should be set to eventually drop DTR and reset
 the modem to memory defaults.

 After the call is reconnected, the modem parameters are returned to normal.

 This program has been tested on Hayes 9600, and Hayes ULTRA modems.  Your
 mileage (and AT commands) may vary.

 The program should be set up as a foreign command.  I have a command 
 called "HOME.COM":

              $!This is home.com
              $ callback :== $sys$login:callback.exe
              $ callback 9w 555-1212
              $ @sys$login:login.com

 So a login of "username/login=home" will call me back and then run the
 normal login file.

