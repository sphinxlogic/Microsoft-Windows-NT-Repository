$! File created by DCL_DIET at 21-SEP-1998 10:54:31.70 from
$! DISK$PEEK_USER:[HAMMOND.DCL_DIET.SRC]DCL_DIET.COM_SOURCE;
$goto start
$DECK
    **************************************************************

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
 
    **************************************************************
$EOD
$start:
$set = "set"
$set symbol /scope=(nolocal,noglobal)
$sav_status = 1
$on control_y then goto y_exit
$on warning then goto err_exit
$say = "write sys$output"
$in$deck = 0
$continuation$line = 0
$s_quote[0,8]=39
$s2_fao = s_quote + s_quote + "F$FA"
$s2_upr = s_quote + s_quote + "F$"
$s2_lwr = s_quote + s_quote + "f$"
$lines_in = 0
$lines_out = 0
$line_disply_increment = 500
$line_to_display = line_disply_increment - 1
$dcl_dt_vers = "V1.0"
$say ""
$say -
"-*- Charlie Hammond's unsupported DCL DIETer (Version ''dcl_dt_vers') -*-"
$say ""
$get_p1:
$if p1 .eqs. ""
$then
$read /error=get_p1 sys$output p1 -
/prompt="Enter name of file to be Dieted: "
$goto get_p1
$endif
$if f$search(p1) .eqs. ""
$then
$say "Cannot find ""''p1'"""
$p1 = ""
$goto get_p1
$endif
$diet$input = f$parse(p1)
$get_p2:
$if p2 .eqs. ""
$then
$say "The default for the ""dieted"" output file is"
$say """''diet$input'_DIET""" - ";"
$read /error=get_p2 sys$output p2 -
/prompt="Enter name of file for output: "
$if p2 .eqs. "" then p2 = "''diet$input'_DIET" - ";"
$endif
$diet$output = f$parse(p2,diet$input)
$say " Input file is: " + diet$input
$say "Output file is: " + diet$output
$say "Starting -- ''f$time()'"
$create 'diet$output'
$close /error=open_output diet$output
$open_output:
$open /append diet$output 'diet$output'
$write diet$output "$! File created by DCL_DIET at ''f$time()' from"
$write diet$output "$! ''diet$input'"
$close /error=open_input diet$input
$open_input:
$open /read diet$input 'diet$input'
$read_input:
$read /end=common_exit diet$input record_in
$lines_in = lines_in + 1
$if lines_in .gt. line_to_display
$then
$say f$fao("...processing line number !UL...",lines_in)
$line_to_display = line_to_display + line_disply_increment
$endif
$temp = f$edit(record_in,"TRIM,COMPRESS,UPCASE")
$if ( ( f$extract(0,2,temp) .eqs. "$!" ) -
.or. ( f$extract(0,3,temp) .eqs. "$ !" ) ) -
then goto read_input
$t0 = f$element(0," ",temp)
$t1 = f$element(2," ",temp)
$if in$deck
$then
$if (t0 .eqs. "$EOD") .or. ( (t0 .eqs. "$") .and. (t1 .eqs. "EOD") )
$then
$in$deck = 0
$write diet$output "$EOD"
$lines_out = lines_out + 1
$continuation$line = 0
$goto read_input
$endif
$else
$if (t0 .eqs. "$DECK") .or. ( (t0 .eqs. "$") .and. (t1 .eqs. "DECK") )
$then
$in$deck = 1
$write diet$output "$DECK"
$lines_out = lines_out + 1
$continuation$line = 0
$goto read_input
$endif
$endif
$if in$deck
$then
$write diet$output record_in
$lines_out = lines_out + 1
$continuation$line = 0
$goto read_input
$endif
$if ( ( f$extract(0,1,temp) .nes. "$" ) .and. ( .not. continuation$line ) )
$then
$write diet$output record_in
$lines_out = lines_out + 1
$continuation$line = 0
$goto read_input
$endif
$x = f$locate(s2_lwr,Record_in)
$if f$edit(f$extract(x,6,Record_in),"UPCASE") .eqs. s2_fao -
then goto do_fao
$x = f$locate(s2_upr,Record_in)
$if f$edit(f$extract(x,6,Record_in),"UPCASE") .eqs. s2_fao -
then goto do_fao
$goto after_fao
$do_fao:
$record_out = f$edit(record_in,"TRIM,COMPRESS")
$if f$extract(0,2,record_out) .eqs. "$ " then -
record_out = "$" + record_out - "$ "
$write diet$output record_out
$lines_out = lines_out + 1
$continuation$line = 0
$goto read_input
$after_fao:
$record_out = f$edit(record_in,"TRIM,COMPRESS,UNCOMMENT")
$continuation$line = 0
$if record_out .eqs. "$" then goto read_input
$if f$extract(0,2,record_out) .eqs. "$ " then -
record_out = "$" + record_out - "$ "
$write diet$output record_out
$lines_out = lines_out + 1
$if f$extract(f$length(record_out)-1,1,record_out) .eqs. "-" -
then continuation$line = 1
$goto read_input
$y_exit:
$write sys$output "Exiting due to Ctrl_y entry"
$goto 1_exit
$err_exit:
$sav_status = $status
$err_exit_w_status:
$write sys$output f$message(sav_status)
$if sav_status .lt. %x10000000 then sav_status = sav_status + %x10000000
$goto common_exit
$1_exit:
$write sys$output f$message(sav_status)
$sav_status = 1
$goto common_exit
$common_exit:
$on control_y then continue
$on warning then continue
$close /error=exit_output diet$output
$exit_output:
$close /error=exit_input diet$input
$exit_input:
$say "Finished -- ''f$time()'"
$say ""
$say f$fao("!7UL lines read",lines_in)
$say f$fao("!7UL lines written",lines_out)
$say ""
$exit sav_status
