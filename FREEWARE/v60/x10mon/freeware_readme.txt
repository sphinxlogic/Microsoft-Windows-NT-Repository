X10_MONITOR, SOFTWARE, Monitor and Control X-10 and Serial Interface Devices

X10_MONITOR and companion application X10_TIMER comprise a powerful, 
flexible system for monitoring and controlling X-10 and other types
of real-time control devices employing a serial port interface from
your OpenVMS system.  Functional diagram:

                  User Interface
                        |
  +------------+        V             +-------------+
  |            |-> Input mailbox  --> |             | <---> Terminal
  | X10_TIMER  |                      | X10_MONITOR |       Serial
  |  Process   |                      |   Process   |       Port
  | (optional) |<- Output mailbox <-- |             |      (Built-in
  +------------+                      +-------------+       or LAT)
        ^
        |           +----------+
        |           |  [Rdb]   |
        +---------->| X10MONDB |<-----> User Interface
                    | Database |      
                    |(optional)|
                    +----------+

Major components:

  - X10_MONITOR is a "message switch" application run in the context of 
    a detached process.  It handles the routing of commands and messages 
    between two mailbox devices and a user-defined terminal port (built-in 
    or LAT). The monitor can operate without the timer component and is
    easily customized for multiple applications.  

  - Optional component X10_TIMER also runs in the context of a detached 
    process.  At a user-defined interval it queries an Oracle Rdb rela-
    tional database for scheduled events.  Selected events are sent to
    X10_MONITOR's input mailbox for routing. The Timer's secondary func-
    tion is to log timestamped message activity to a database table for
    user interface querying and reporting.  Current status for each
    module is maintained based on controller message activity.

  - Supplemental programs for interacting with the monitor, defining the 
    database, populating database tables, generating scheduled events,
    testing, startup and shutdown, etc., are included in this package.

Modules are written in C, COBOL, DCL and SQL.  They are a good example of 
employing many OpenVMS Run-Time Library and System Service routines, ASTs, 
etc. to handle intermittent real-time monitoring and control.  A cross-
reference list can be found in the README.TXT file.  Refer also to the 
documentation file and program source code for additional information.

The author of all modules in the X10_MONITOR package is:

   Jason Brady
   Lynnwood, Washington USA
   jr_brady@earthlink.net

Programs were written and tested on an Alphaserver 300 4/266 running 
OpenVMS AXP 7.2, Compaq C 6.0-001, Compaq COBOL 5.5 and Oracle Rdb 7.0-4.
