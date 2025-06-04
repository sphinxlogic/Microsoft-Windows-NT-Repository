REMDECW, SOFTWARE, Remote DECwindows Application launching

These command files allow you to add menu items to a FileView
or Session Manager Menu that allow access to remote systems.

The advantage of these routines is that you can use them on a
FileView session that is running on a different host than
your X-Server display to launch programs on a third or more
hosts.

This is quite useful when your X-11 Server is not running on an
OpenVMS host.

When they are configured as a Session Manager or FileView
menu option with the "[] ..." option, they will bring
up dialog boxes to prompt for information that they
need.  The last entered values will be displayed.

For best results, use with DECNET and TCPIP proxies.

For non proxied systems, they will cache the username and
password internally to the FileView/Session manager
session.

Caveats:

1. They use undocumented logical names for deciphering the
   return IP address.

2. They are using VUE$* dcl routines that I have not found
   documented anywhere.

3. They have only been used where UCX or TCPIP in use and
   probably will need modifications to use any other
   TCP/IP program.

4. They are normally used with DECNET so use with TCPIP
   versions prior to 5.1 may have some unknown bugs.

The author will probably eventually respond to mail left
at either wb8tyw@qsl.net or malmberg@eisner.decus.org
