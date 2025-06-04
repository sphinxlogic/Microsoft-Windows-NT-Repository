SKEY, System_Mgmt, Secure one-time key login system for VAX and Alpha

S/Key is a login authentication method, using one-time key generation
in response to a "challenge".  The S/Key system is described in
RFC-1760 and RFC-1938.

The advantage of one-time keys is that such techniques as "packet
sniffing" cannot be used: each login requires a different key that is
derived in a secure fashion from a secret key that is never
transmitted during an authentication session.

Unlike Kerberos, S/Key does not require modification or special
versions of client programs such as Telnet, FTP, etc., and generally
it does not require connections to be made on special port numbers.

SKEY 0.9 for VMS provides a LGI$ callout to produce S/Key challenges,
and to verify logins.  S/Key authenticated logins are normally
optional, but can be user-configured to be default or manditory.  A
/[NO]SKEY qualifer on the login username can select or deselect S/Key
authentication when allowed by the configuration.

An SKEY command verb is provided for calculating S/Key responses as
well as allowing users to set up their S/Key logins.   Utility routines
are provided to aid in making application programs "S/Key capable".

Full source code is provided, along with object files compiled on VMS 6.2.
