TELL, UTILITIES, Issues DCL commands on remote machines

 This command procedure will execute commands on a remote node.

 The command syntax for tell is :-
 @TELL[/output=filename] NODENAME[,...] DCL-COMMAND [&] [|...]

   P1 - NODENAME:
    More than one nodename can be specified, each being seperated by a comma.
    Nodename can also be a symbol and the double colon (::) after the nodename
     is optional.
    "0" can be used to specify the local nodename.
    If you do not have proxy access then you can specify access control 
      information as part of the nodename specification. 
    You can force a routing path by specifiy nodename of the from
      nodename::nodename::
    If two or more nodenames are specified then on completion TELL displays a 
      summary listing of unreachable nodes. This feature can be disabled by
      defining the logical name as follows:-
      $ DEFINE/PROCESS/NOLOG TELL_INHIBIT_SUMMARY TRUE

   P2 to P8 - DCL COMMAND:
    INTERACTIVE MODE:
      If no DCL command is specified then you will be in interactive mode.
      At that point you will have a DCL session in the context of the remote
        node. The standard EXIT command will return you to your local node.
    COMMAND MODE:
      Here the DCL commands are specified in P2 to P8 and are separated 
      by a vertical bar "|". Quotes are optional and are only required to
      reduce the number of P parameters.
    DETACHED MODE:
      Jobs can be run in the background as a detached process by 
      suffixing the & (unix background command) to any command.
      This can be done from either Interactive or Command Mode.

    Note that you cannot issue commands that require terminal input such as 
    EDIT etc. This is a restriction of the DECnet task-task software and not
    of TELL itself. Some of these restrictions can be overcome by defining 
    the verb as a foreign command. This allows many commands to be entered on 
    a single line.
        eg. $ AUTHORIZE :== $AUTHORIZE
            $ AUTHORIZE SHOW KINGSMILL
     
   QUALIFIERS:
    /Output can be used to direct the output to a filename.


   EXAMPLES:

   $ TELL :== @SYS$MANAGER:TELL.COM
        All the following examples assume that the tell symbol is defined.
        Place this in your login.com.

   $ TELL MVAX SHOW ERROR
        This command relies on proxy access to MVAX. 
        It does a remote $show error on node MVAX.

   $ TELL/OUTPUT=SYS$LOGIN:TELL.OUT MVAX SHOW ERROR
        This command relies on proxy access to MVAX. 
        It does a remote $show error on node MVAX with the output
           directed to the file sys$login:tell.out

   $ TELL MVAX"KINGSMILL PASSWORD":: SHOW ERROR
        this command supplies access control to accounts that do not have 
        proxy access

   $ TELL MVAX"KINGSMILL":: SHOW ERROR
        Under VMS V5.x you can have a number of different proxy users pointed
        to one account. This example show how to gain access to a non-default
        proxy account. Note that only the username needs to be specified.

   $ TELL MVAX,TVAX,SVAX SHOW ERROR
        this command will do a show error on MVAX, TVAX and SVAX.

   $ DECA_CLUSTER :== MVAX,TVAX,SVAX
   $ TELL 'DECA_CLUSTER' SHOW ERROR
        this command will do a show error on MVAX, TVAX and SVAX.

   $ TELL MVAX,TVAX "SHOW ERROR"|"SHOW NET"
        this command will do both a show error and a show net on both MVAX 
        and TVAX.

   $ TELL MVAX,TVAX "SET DISPLAY/CREATE/NODE=MAXPS1" | "CREATE/TERM &"
        this command will create a detached decterm on both MVAX and TVAX

   $ TELL MVAX
   MVAX_KINGSMILL>> SHOW ERROR
   MVAX_KINGSMILL>> SET PROC/PRIV=OPER
   MVAX_KINGSMILL>> MCR NCP SHOW EXEC
   MVAX_KINGSMILL>> EXIT
   $
        This is an example of an interactive session. The prompt indicates
        the remote nodename and username, whilst the ">>" is used to
        distinguish between a local and remote session. 
    
   $ TELL MVAX TELL TVAX SHOW ERROR
   $ TELL MVAX::TVAX:: SHOW ERROR
        Both of these commands perform the same function. 
        The first performs a nested TELL, ie. TELL node MVAX to TELL node
          TVAX to do a $ show error. Only command mode is valid with 
          this syntax and you must specify the command as TELL not @tell.
        The second example show how to do the same thing using Poor Mans
          Routing (PMR). This is far more efficent than the first examples
          and it supports both command and interactive modes.


 CONFIGURATION SETUPS:
 TELL.COM needs to exist on both the local and remote nodes and requires
 the user to either specify node access control or for the remote account 
 to be set up for proxy access. TELL can simply reside in sys$login on the
 remote node, which requires each user to have their own copy -or- TELL
 can be defined as a DECnet object, requiring each system to have only one 
 copy.

 To install TELL as a network object the system manager should enter the
 following commands:
  $ MCR NCP
  NCP> DEFINE OBJECT TELL NUMBER 0 FILE SYS$MANAGER:TELL.COM PROXY BOTH
  NCP> SET OBJECT TELL ALL
  NCP> EXIT
  $

 If TELL is not installed as a network object then by default no other setups
 are required. However if the DECnet object task has been modified to disable
 proxy access then tell will fail. In this case the system manager should 
 enter the following commands:
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
            UAF> ADD/PROXY MVAX::OLSENK KINGSMILL
            UAF> EXIT
            $
  This examples allows remote user KINGSMILL and OLSENK on node MVAX
  into the local KINGSMILL account. The default switch enables you to
  specify just the nodename when addressing this node:-
    eg. $ DIRECTORY TVAX::
  If Default access is not specified then you need to specify the 
  nodename and username when addressing this node:-
    eg. $ DIRECTORY TVAX"KINGSMILL"::
  If you do not have proxy access then you need to specify the
  nodename, username and password when addressing a remote node:-
    eg. $ DIRECTORY TVAX"KINGSMILL password"::

  The following logicals can be used to alter the default TELL behaviour.
  TELL_INHIBIT_SUMMARY to disable the statistics printout.

 TROUBLESHOOTING:
   The following logicals can be used to aid in troubleshooting.
   TELL_VERBOSE to display the progress of both local and remote events.
     This can also be enabled by setting the version prefix from "V" to "X"
   TELL_VERIFY to turn on verify and verbose.
   NETSERVER.LOG on the remote node may also aid in troubleshooting.
   TELL_'pid'*.COM temporary file on the remote node shows the created
     command file if detached qualifier was specified.
   TELL.LOG temporary file on on the remote node showing the output
     from tell_'pid'.com

