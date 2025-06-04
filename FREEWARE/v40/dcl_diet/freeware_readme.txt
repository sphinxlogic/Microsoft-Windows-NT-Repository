DCL_DIET, UTILITIES, Charlie Hammond's unsupported DCL DIETer

The following information is extracted from DCL_DIET.COM.

    This procedure "DIETs" a command procedure file -- compressing
    it by removing comments and unnecessary space. This saves file
    space and improves execution time.

    To run this procedure, enter command 

        $@DCL_DIET <input> <output>
    
        where   <input> is the input command procedure
               <output> is the "DIETed" out put file

    If you use DCL_DIET frequently, you may wish to assign a symbol in
    your LOGIN.COM file to execute DCL_DIET.  For example, if the
    DCL_DIET.COM is in you LOGIN default directory, you might put the
    following in your LOGIN.COM:

        $ DCL_DIET :== "@SYS$LOGIN:DCL_DIET"

    Alternatively, if you put DCL_DIET.COM in SYS$SYSTEM, you might put
    the following in your SYS$SYLOGIN (which is normally
    SYS$MANAGER:SYLOGIN.COM):

        $ DCL_DIET :== "@SYS$SYSTEM:DCL_DIET"
