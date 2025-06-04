DWREMTERM, SYSTEM_MGMT, Start X-Window displays on remote machines

This command procedure is designed to allow users flexability in using 
DECwindows across a large number of machines connected together by a 
Local Area Network. It allows users to create LAT, DECnet, Telnet and LDT
windows locally and connect to remote nodes or to create remote DECwindow
sessions using the RDT, RFV, RDW, DWRT and RSH commands. The remote
commands are actually network process's and therefore do not count as an
interactive login.

DWremterm can be run in two modes:
  MENU MODE: commands are entered from a Small Menu Window.
  COMMAND MODE: commands are entered from the DCL command line.

Valid command are:-
  HELP, DWHELP, LAT, DECNET, TELNET, LDR, RDT, RFV, RDW, RSH, DWRT and $.
Parameters listed are [] brackets is optional.
HELP: Gives details on how to receive Brief or Expanded Help
DWHELP: Provide detailed Help on DWremterm
LAT:    Creates a Local DECterm Window using a SET HOST/LAT command.
        LAT nodename
DECNET: Creates a Local DECterm Window using a SET HOST command.
        DECNET nodename
TELNET: Creates a Local DECterm Window using a TELNET command.
        TELNET nodename
LDT:    Creates a local decterm and then does an rlogin
        LDT nodename [init_state] [title]
RDT:    Creates a Remote Decterm Window.
        RDT nodename [init_state] [title] [dcl_command]
                     [display] [transport] [x:y]
RFV:    Creates a Remote Fileview Window.
        RFV nodename [init_state] [display] [transport]
RDW:    Runs a Remote Remote Decwindows Application.
        RDW nodename command [display] [transport]
RSH:    Runs RSH commands on a remote node.
        RSH nodename command
DWRT:   Starts a second DWremterm Window.
        DWRT nodename [init_state] [display] [transport]
$:      Executes a local DCL command

For further details refer to sys$library:dwremterm.hlb.

A unique window title and icon name is chosen by referencing the user's
process name. If the login sequence changes the process name then duplicate
window and icon names may occur.

Refer to SYS$HELP:DWREMTERM.HLP for further help by typing:-
@VUE$LIBRARY:DWREMTERM DWHELP.

DWremterm consists of two files:
SYS$COMMON:[VUE$LIBRARY.USER]DWREMTERM.COM 
SYS$COMMON:[SYSHLP]DWREMTERM.HLB

The RDT, RDW, DWRT and RFV commands require DWREMTERM.COM to exist on the
both the local and remote node and requires the user to either specify node
access control or for the remote account to be set up for proxy access.
DWremterm can simply reside in sys$login on the remote node, which requires
each user to have their own copy -or- DWremterm can be defined as a DECnet
object, requiring each system to have only one copy.

DECNET:
To install DWremterm as a network object the system manager should enter the
following commands:
DECnet/IV:
 $ MCR NCP
 NCP> DEFINE OBJECT DWREMTERM NUMBER 0 FILE -
      SYS$COMMON:[VUE$LIBRARY.USER]DWREMTERM.COM PROXY BOTH
 NCP> SET OBJECT DWREMTERM ALL
 NCP> EXIT
 $
DECnet/OSI:
 $@sys$startup:net$configure application_add dwremterm "" -
 "{name=dwremterm}||vue$library:dwremterm.com|true|true|true|true|true||||1"

If DWremterm is not installed as a network object then by default no other 
setups are required. However if the DECnet object task has been modified to 
disable proxy access then DWremterm will fail. In this case the system 
manager should enter the following commands:
 $ MCR NCP
 NCP> DEFINE OBJECT TASK PROXY BOTH
 NCP> SET OBJECT TASK ALL
 NCP> EXIT
 $

PROXY accounts allow one user direct access to another users account
 without having to specify username and password. The system manager
 can set up proxy account access from within AUTHORIZE. 
 Example:  $ MCR AUTHORIZE>
           UAF> ADD/PROXY MVAX::KINGSMILL KINGSMILL/DEFAULT
           UAF> ADD/PROXY MVAX::KINGSMILL OLSENK
           UAF> EXIT
           $
 This examples allows user KINGSMILL on node MVAX into the local KINGSMILL 
 and OLSENK account. The default switch enables you to specify just the 
 nodename when addressing this node:-
   eg. $ DIRECTORY TVAX::
 If Default access is not specified then you need to specify the 
 nodename and username when addressing this node:-
   eg. $ DIRECTORY TVAX"OLSENK"::
 If you do not have proxy access then you need to specify the
 nodename, username and password when addressing a remote node:-
   eg. $ DIRECTORY TVAX"SYSTEM password"::

LAT:
 VMS V5.5-2 is required for outgoing LAT. This is enabled as follows:-
 MCR LATCP> SET NODE/CONNECTION=BOTH.

TCPIP:
 To prevent the need to use passwords on the RSH command proxies can be
 set up to authorize remote user access. To set up remote user access
 on a UNIX machine, or TCPWARE/MULTINET under VMS you specify the remote
 node and username in the .rhosts file which resides in the users home
 directory. With UCX you use the "UCX> ADD PROXY command"
 In addition, if you don't want to specify a password on RSH and RLOGIN
 you may need to add the following:-
 UCX> set service rsh /flag=(proxy,case)
 UCX> set service rlogin /flag=(proxy,case)

DISPLAY SECURITY: 
 The Factory Defaults prohibit a remote users from displaying their
 output to your workstation. To authorize other users or remote
 nodes to use your workstation display you should:
   1. Choose Security... from Session Manager's Options menu.
   2. Type the nodename, the username, and the method of transport,
      of the user you want to authorize. Entering * * * will allow
      any user on any node with any transport to display output on
      your workstaton.
   3. Click on the Add button and the Clock OK.
 Refer to the DECwindows user guide for further information.
 
CONFIGURATION SETUPS:
  The following logicals can be used to alter default DWremterm behaviour.
  DWREMTERM_ENABLE_COPY gives the user the option of copying the latest
    version of DWREMTERM should there be a mismatch in versions between
    the local and remote node.
  The WINDOW POSITION and INITIAL STATE can be altered from the default
    by editing the resource file. Details on how to do this is given within 
    the following resource files. 
      Default DWremterm window:
      - decw$user_defaults:dwremterm$default_v*.dat 
      DWRT DWremterm window:
      - decw$user_defaults:dwremterm$dwrt_default_v*.dat 
        This is on the remote node/users account.
      You can also override the DWRT resource file window position by
      defining the logical names DWREMTERM_DWRT_X and DWREMTERM_DWRT_Y
      under your local user account. These values are then passed to
      the remote node/users account.
  LAT$DISCONNECT_CHARACTER defines the character that you can use to 
    disconnect from a remote session. Default character is Ctrl+\.
  SYS$SCRATCH - temporary files are written to sys$scratch. If you don't
    want the temporary files written to sys$login then define sys$scratch.

TROUBLESHOOTING HINTS:
o Ensure that Security has been set up to enable a remote node to
  display output on your workstation. See installation section above
  for further details.
o The following logicals can be used to aid in troubleshooting
  These can be defined before starting DWremterm or during a DWremterm 
  Session by using the $ command.
    DWREMTERM_VERBOSE to display the progress of both local and remote 
      events. If verbose is enabled then a full sized menu window will 
      be displayed. This can also be enabled by prefixing the version 
      with an "X".
    DWREMTERM_VERIFY to turn on verify and verbose.
o NETSERVER.LOG on the remote node may also aid in troubleshooting.
o DWREMTERM_'pid'*.COM temporary file on the remote node shows the created
    command file.
o DWREMTERM_'pid'_*.LOG temporary file on on the remote node showing the
    output from dwremterm_'pid'_*.TMP
o If you have problems creating DECterms then get the latest DECterm patch
    from your CSC.

TIPS:
o By default you cannot shutdown VMS from an RDT window when running
  DECnet/OSI or DECnet/Plus because the shutdown procedure stops DECnet,
  which aborts the DWremterm session and the shutdown procedure itself.
  You can workaround this by doing the following:-
  $ DEFINE NET$STARTUP_STATUS SHUTDOWN
  $ @SYS$SYSTEM:SHUTDOWN
