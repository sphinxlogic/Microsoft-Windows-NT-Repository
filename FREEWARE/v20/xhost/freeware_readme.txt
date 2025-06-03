XHOST, MOTIF TOYS, Server access control program for X

The xhost program is used to add and delete host names or user names to the
list allowed to make connections to the X server.  In the case of hosts, this
provides a rudimentary form of privacy control and security.  It is only
sufficient for a workstation (single user) environment, although it does limit
the worst abuses.  Environments which require more sophisticated measures
should implement the user-based mechanism or use the hooks in the protocol for
passing other authentication data to the server.
  
