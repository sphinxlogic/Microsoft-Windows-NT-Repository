DCL_CHECK, UTILITIES, Charlie Hammond's unsupported DCL checker

    Note: Information about DCL_CHECK V3.0 is at the end of this file.

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

        When  you  execute  DCL_CHECK  with "HELP" as the first parameter,
        DCL_CHECK invokes the DCL Help utility.  If you provide additional
        parameters, they are passed to the Help utility.

        To exit the Help utility, press Return one or more times until the
        prompt "enter name of file:"  is displayed.


    Additional information available:

    Called_procs  DCL_CHECK       DCL_DIET    Diagnostics
    Format        Installation    Line_counts Modifications
    Output        Problems        Version     Examples

Topic? installation

INSTALLATION

        To install DCL_CHECK you must put the files

            DCL_CHECK.COM and
            DCL_CHECK.HLB

        into the same directory. You then execute DCL_CHECK as an indirect
        command procedure.  See "Format" for additional information.

        DCL_CHECK.COM  is provided in "dieted" form.  You may also wish to
        have the file DCL_CHECK.COM_SOURCE available; this  is  the  fully
        commented version of the procedure.

Topic? format

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
        DCL_CHECK.COM  and  .HLB files are in you login default directory,
        you might put the following in your LOGIN.COM:

            $ DCL_CHECK :== "@SYS$LOGIN:DCL_CHECK"

        Alternatively,  if  you  put  DCL_CHECK.COM  and  DCL_CHECK.HLB in
        SYS$SYSTEM, you might put the following in your SYS$SYLOGIN (which
        is normally SYS$MANAGER:SYLOGIN.COM):

            $ DCL_CHECK :== "@SYS$SYSTEM:DCL_CHECK"


  Additional information available:

  Parameters

FORMAT Subtopic?
Topic? modifications

MODIFICATIONS

        You  can  modify  or  "customize" DCL_CHECK by editing the command
        procedure, DCL_CHECK.COM. However, DCL_CHECK.COM is a "compressed"
        version  of  the  procedure.   To  save  disk  space  and  improve
        performance,  all  comments  and  unnecessary  spacing  have  been
        removed  from  DCL_CHECK.COM.  This makes it difficult to read and
        understand the procedure.

        It  should  be  much easier to edit the file DCL_CHECK.COM_SOURCE,
        which contains commands  and  is  formatted  to  facilitate  human
        reading.   You  can recreate a "compressed" version of the command
        procedure by using DCL_DIET.COM.


  Additional information available:

  Help

MODIFICATIONS Subtopic?
Topic? dcl_diet

DCL_DIET

        DCL_DIET  is  a  command  procedure  that  accepts  as input a DCL
        command procedure file.  It compresses, or "diets",  the  file  by
        removing  comments and unnecessary spaces.  The compressed version
        is created as output.

        The smaller size of the compressed file has two advantages:

            o It takes up less space on disk or tape.

            o It executes faster
                -- especially for larger command procedures.

        DCL_DIET is not part of DCL_CHECK.  It is provided separately.


---------------------------------------------------------

DCL_CHECK V3.0

The following comments from DCL_CHECK.COM_SOURCE outline the changes.

$!  D3.1    2003-AUG-21             Charlie Hammond
$!      Fixed some false instances of UMP and PSQ/PSR errors.
$!
$!  C3.1    2003-AUG-20             Charlie Hammond
$!      Avoid flagging possessives ("'s " in quoted string) as
$!      PSQ-W  possible error using single-quote (') in quoted string
$!
$!  B3.1    2003-AUG-20             Charlie Hammond
$!      Re-write code that finds the file-name logical-name on I/P verbs
$!      so that it can handle quoted strings and symbol substitution
$!      within quoted strings.
$!
$!  A3.1    2003-JUN-05             Charlie Hammond
$!      Improved checking for paired brackets
$!      by not checking in quoted strings


Version 3.0 of DCL_CHECK includes several new diagnostics related
to file checkin (OPEN, READ, WRITE and CLOSE commands).
It also contains a collection of incremental fixes and
improvements, and the new IFC diagnostic.

The following comments from DCL_CHECK.COM_SOURCE outline the changes.

$!  V3.0    2003-JUN-03             Charlie Hammond
$!      Add checking for paired brackets
$!
$!  H3.0    2003-APR-03             Charlie Hammond
$!      Added the contraction "They've".
$!      Added all contractions in all upper case.
$!      Added possessives for common accounting periods.
$!          "Day's", "Week's", "Month's", "Period's", "Quarter's",
$!          "Half's" and "Year's" (also in all lower and all upper case).
$!          Also names of weekdays and months
$!
$!  G3.0    2003-APR-01             Charlie Hammond
$!      Fixed various spelling errors/typos
$!      Fixed aproblme with ICO (Help from Norm Raphael @metso.com)
$!
$!  F3.0    30-Sep-2002             Charlie Hammond
$!      Don't do file checking on SYS$COMMAND, SYS$ERROR or SYS$PIPE
$!      (In addition to SYS$INPUT and SYS$OUTPUT)
$!      Fix a problem with double quotes in symbol used w/ READ/KEY
$!
$!  E3.0    16-Sep-2002             Charlie Hammond
$!      Avoid false diagnostics ICO-S for ">" and "<" in PIPEs
$!      Only check /END and /ERR labels on /END_OF_FILE and /ERROR
$!
$!  D3.0    04-Sep-2002             Charlie Hammond
$!      added " .ea ", " ea. " and " .ea. " to list of invalied
$!      comparison operators.
$!
$!  C3.0    18-Mar-2002             Charlie Hammond
$!      Fix problem getting file logical names when there is an
$!      /END or /ERR qualifier with spaces preceedign of following the "="
$!
$!  B3.0    13-Mar-2002             Charlie Hammond
$!      Fix problem with /ERROR on RUN statement
$!
$!  A3.0    18-Feb-2002             Charlie Hammond
$!      Add file verb checking
$!    NLN-S  An OPEN, READ, WRITE or CLOSE statement has no logical name
$!    ONC-E  A file that is opened has no close statement
$!    INR-A  file that is opened for read is not read
$!    ANR-E  A file that is opened for append is not written
$!    ONW-E  A file that is opened for write is not written
$!    RNR-S  A file that is not opened for read is being read
$!    WNW-S  A file that is not opened for write or append is being written
$!    UNU-S  A file that is not opened for read and write is being updated
$!    CNO-S  A file that is closed has no open statement
$!
$!  NOTE: There is no V2.2 --  A3.0 added sufficient functionality
$!        that the major version was bumped.
$!
$!  G2.2    20-Nov-2001             Charlie Hammond
$!          Make an "&" in a PIPE statement a warning.
$!          Re-word several ICF diagnostics.
$!          Include this in help.
$!
$!  F2.2    02-Aug-2001             Charlie Hammond
$!          Spelling corrections, mostly in comments, suggested
$!          by Norm.Raphael@jamesbury.com
$!
$!  E2.2    28-Jun-2001             Charlie Hammond
$!          Improve line number display in pass 2
$!          Display 1000, 2000, etc. instead of 500, 1500, etc.
$!          Also display line numbers in round 100's instead
$!          of actual numbers that could be off a bit.
$!
$!  D2.2    08-Jun-2001             Charlie Hammond
$!          Fix problem mistaking "CALL", "GOTO" and "GOSUB"
$!          at the end of a symbol name as a DCL command.
$!
$!  C2.2    31-May-2001             Charlie Hammond
$!          Fix problem with /END and /ERR on same line.
$!
$!  B2.2    22-Feb-2001             Charlie Hammond
$!          Fix problems with detecting various forms of $DECK and $EOD
$!
$!  A2.2    15-Aug-2000             Charlie Hammond
$!          Fix problem when first blank delimited token on line
$!          ends with ":" but is not a label.
$!
$!  V2.1    15-Aug-2000             Charlie Hammond
$!          Updated freeware release
$!
$!  H2.1    07-Aug-2000             Charlie Hammond
$!          Make ICF as warning for & and %
$!          Add information to ICF help.
$!
$!  G2.1    03-Aug-2000             Charlie Hammond
$!          Allow for a $ in a continued, "one-line" if statement.
$!          Avoid incorrect INT err that could happen if a PSQ error
$!              is found in a nested "one-line" if statement.
$!
$!  F2.1    02-May-2000             Charlie Hammond
$!          Add ICF invalid character found (#, %, ^ or &)
$!
$!  E2.1    24-Mar-2000             Charlie Hammond
$!          Improved EFB detection ("=" found between IF and THEN)
$!
$!  D2.1    13-Dec-1999             Charlie Hammond
$!          Improve detection of single quote errors
$!          Allow /OUT= on output file (P2)
$!
$!  B2.1    04-Dec-1999             Charlie Hammond
$!          Add LDS error -- Label defined by symbol substitution (warning)
$!          Fix single quote (') in definition of valid_lexicals
$!              It should be and now is a comma (,).
$!  A2.1    23-Oct-1999             Charlie Hammond
$!          Correct calculation of code_lines.
$!          We had been subtracting deck_lines twice.
$!
$!  V2.0    17-Sep-1999             Charlie Hammond
$!          For FREEWARE release
$!
$!  B2.0    31-Jul-1999             Charlie Hammond
$!          Correct handling of continuation comment that starts
$!              with only a "!" rather than "$!"
$!          Correct handling of SRT and INT errors.
$!              (SUBROUTINE/IF not terminated)
$!          "Beef up" a few help entries.
$!
$!  A2.0    28 July 1999            Charlie Hammond
$!          Improve handling for TNA/ENA/DNA
$!              (THEN/ELSE/ENDIF statement not allowed here)
$!          Make ENDSUBROUTINE cancel goto and exit shadows
$!          Handle SUBROUTINE and ENDSUBROUTINE
$!              IF/ENDIF and SUBR/ENDS may be disjoint or
$!              strictly nested -- they may not overlap
$!
$!  A1.0 - R1.0
$!          through October 1996    Charlie Hammond
$!          Many changes/additions
$!
$!  X-1     dd-mmm-1996             Charlie Hammond
$!          Original procedure created.
