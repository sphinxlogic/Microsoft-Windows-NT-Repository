DCL_DIET, UTILITIES, Charlie Hammond's unsupported DCL dieter

    [The following also appears near the beginning of
     DCL_DIET.COM and DCL_DIET.COM_SOURCE.]

    **************************************************************

    This procedure "DIETs" a command procedure file -- compressing
    it by removing comments and unnecessary space. This saves file
    space and improves execution time.  It may also remove comments
    that you don't want users to see.

    To run this procedure, enter command

        $@DCL_DIET <input> <output>

        where   <input> is the input command procedure
               <output> is the "DIETed" output file

    (<output> may also be in the form "/OUTPUT=<filename>"

    If you use DCL_DIET frequently, you may wish to assign a symbol in
    your LOGIN.COM file to execute DCL_DIET.  For example, if the
    DCL_DIET.COM is in you LOGIN default directory, you might put the
    following in your LOGIN.COM:

        $ DCL_DIET :== "@SYS$LOGIN:DCL_DIET"

    Alternatively, if you put DCL_DIET.COM in SYS$SYSTEM, you might put
    the following in your SYS$SYLOGIN (which is normally
    SYS$MANAGER:SYLOGIN.COM):

        $ DCL_DIET :== "@SYS$SYSTEM:DCL_DIET"

    If the translation of the logical name DCL$DIET_DOTDOT is true, then
    this version adds a space a the beginning of a line that starts with ".".
    This is to overcome a problem with SMTP mail in Compaq TCP/IP services
    so that TCP/IP V5.3-18's SMTP won't double the "."

    **************************************************************
