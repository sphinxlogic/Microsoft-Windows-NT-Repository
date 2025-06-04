DCL_CHECK, UTILITIES, Charlie Hammond's unsupported DCL checker

The following information is extracted from DCL_CHECK's help file.
There is much more information in the help file.
The HELP utility can access the DCL_CHECK help file using the command

    $ HELP /LIBRARY=<device>:[<directory>]DCL_CHECK

or through DCL_CHECK with this command

    $ @<device>:[<directory>]dcl_check help

DCL_CHECK

        The DCL_CHECK procedure detects a variety of DCL coding errors and
        displays  diagnostic  messages  that  allow  you  to  correct  the
        problems. See "Diagnostics" for a list of the types of errors that
        DCL_CHECK can detect. See "Problems" for information on the limits
        of DCL_CHECK.

        When  you  execute  DCL_CHECK  with  "HELP" as the first parameter,
        DCL_CHECK invokes the DCL Help utility.  If you provide additional
        parameters, they are passed to the Help utility.

        To exit the Help utility, press Return one or more times until the
        prompt "enter name of file:"  is displayed.


    Additional information available:

    DCL_CHECK  Diagnostics           Examples   Format     Installation
    Line_counts           Modifications         Output     Problems   Version

Topic? Installation

INSTALLATION

        To install DCL_DIET you must put the files

            DCL_DIET.COM and
            DCL_DIET.HLB

        into the same directory.  You then execute DCL_DIET as an indirect
        command procedure.  See "Format" for additional information.
                                         
Topic? Format

FORMAT

        DCL_CHECK  is  a  DCL  command  procedure.   It is executed by the
        following DCL command:

            $ @[<disk>:][<directory>]DCL_CHECK.COM [<input> [<list>]]
            $ @[<disk>:][<directory>]DCL_CHECK.COM HELP [<topic>...]

        <disk>  and/or  <directory>  may  be omitted; the current defaults
        will be used.

        The   file   DCL_CHECK.HLB  must  be  in  the  same  directory  as
        DCL_CHECK.COM if you use the HELP function in DCL_CHECK. The files
        may be in any directory to which you have read access.

        If  you  use DCL_CHECK frequently, you may wish to assign a symbol
        in your LOGIN.COM file to execute DCL_CHECK.  For example, if  the
        DCL_CHECK.COM  and  .HLB files are in you LOGIN default directory,
        you might put the following in your LOGIN.COM:

            $ DCL_CHECK :== "@SYS$LOGIN:DCL_CHECK"

        Alternatively,  if  you  put  DCL_CHECK.COM  and  DCL_CHECK.HLB in
        SYS$SYSTEM, you might put the following in your SYS$SYLOGIN (which
        is normally SYS$MANAGER:SYLOGIN.COM):

            $ DCL_CHECK :== "@SYS$SYSTEM:DCL_CHECK"


  Additional information available:

  Parameters

FORMAT Subtopic?
Topic? Modifications

Modifications

        You  can  modify  or  "customize" DCL_CHECK by editing the command
        procedure, DCL_CHECK.COM. However, DCL_CHECK.COM is a "compressed"
        version  of  the  procedure.   To  save  disk  space  and  improve
        performance,  all  comments  and  unnecessary  spacing  have  been
        removed  from  DCL_CHECK.COM.  This makes it difficult to read and
        understand the procedure.

        It  should  be  much easier to edit the file DCL_CHECK.COM_SOURCE,
        which contains commands  and  is  formatted  to  facilitate  human
        reading.   You can re-create a "compressed" version of the command
        procedure by using DCL_DIET.COM.


  Additional information available:

  Help       DCL_DIET

Modifications Subtopic?
[Interrupt]

Exiting due to Ctrl_y entry
%SYSTEM-S-NORMAL, normal successful completion
 
