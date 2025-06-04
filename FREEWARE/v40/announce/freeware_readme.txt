SYS$ANNOUNCE, SYSTEM_MANAGEMENT, Dynamic SYS$ANNOUNCE

Are you tired of looking at the same old boring message that OpenVMS displays
prior to the "Username:" prompt when you log in?  You know the one, "Welcome to
OpenVMS VAX V6.2" (or something similar).  

DIGITAL gives you the capability to change this message via the SYS$ANNOUNCE
logical name.  At first glance, it would appear that this logical name only
allows for a static message.

This article describes how to write a program that will automatically update
SYS$ANNOUNCE every time a user attempts to log in.  A sample program is
provided from which your own program can be constructed.
