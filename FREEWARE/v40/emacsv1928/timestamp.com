$! TIMESTAMP.COM
$! Generates a timestamp in vms-date.el (template is vms-date.el_in)
$! when a distributon is created.
$
$ month_01="January"
$ month_02="February"
$ month_03="March"
$ month_04="April"
$ month_05="May"
$ month_06="June"
$ month_07="July"
$ month_08="August"
$ month_09="September"
$ month_10="October"
$ month_11="November"
$ month_12="December"
$ now=f$time()
$ weekday=f$cvt(now,,"weekday")
$ month=month_'f$cvt(now,,"month")'
$ day=f$str('f$cvt(now,,"day")')
$ year=f$str('f$cvt(now,,"year")')
$ date=weekday+", "+month+" "+day+" "+year
$
$ infile:=[.lisp]vms-date.el_in
$ outfile:=[.lisp]vms-date.el
$
$ create sys$scratch:timestamp.tmp_tpu
procedure TPU_substitute(pre_pat,
                         pat,
                         post_pat,
                         replacement)
	local r,r0,re;
	position (beginning_of (main_buffer));
	loop
		r := search_quietly (pre_pat + 
                                ((pat+"")@r0) + 
                                post_pat, 
                                FORWARD);
		EXITIF r = 0;
		if beginning_of(r0) <> end_of(r0)
		then
			erase (r0);
			position (r0);
			copy_text (replacement);
		endif;
		position (end_of (r0));
!		position (line_begin);
!		move_vertical (1);
	endloop;
endprocedure;

input_file:=GET_INFO(COMMAND_LINE, "file_name");
main_buffer := CREATE_BUFFER ("main", input_file);

$ open/append foo sys$scratch:timestamp.tmp_tpu
$ write foo "TPU_substitute(""@",name,"@"",.........................
$ edit/tpu/nosect/command=sys$input:/output='outfile' 'infile'
$
$ exit
