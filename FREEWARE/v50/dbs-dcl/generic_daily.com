$! File created by DCL_DIET at  8-FEB-1999 14:14:10.41 from
$! DBS0:[OLGA.COM]GENERIC_DAILY.COS;
$__vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$if (f$type('__vfy') .eqs. "") then __vfy = 0
$__vfy_saved = f$verify(&__vfy)
$procedure = f$element(0,";",f$environment("PROCEDURE"))
$procedure_name = f$parse(procedure,,,"NAME")
$location = f$parse(procedure,,,"DEVICE","NO_CONCEAL") -
+ f$parse(procedure,,,"DIRECTORY","NO_CONCEAL") - "]["
$set noon
$set message/facility/severity/identification/text
$on control_y then goto bail_out
$set process/privilege=all
$P1 = f$edit(P1,"COLLAPSE,UPCASE")
$P2 = f$edit(P2,"COLLAPSE,UPCASE")
$P3 = f$edit(P3,"COLLAPSE,UPCASE")
$test_only = ((f$trnlnm("GENERIC_DAILY_TEST") .nes. "") -
.or. (P1 .nes. "") .or. (P2 .nes. "") .or. (P3 .nes. ""))
$vax = (f$getsyi("HW_MODEL") .lt. 1024)
$axp = (f$getsyi("HW_MODEL") .ge. 1024)
$say = "write sys$output"
$err = "call reporter"
$log = "call reporter"
$facility = "BAGDAILY"
$err I STARTING "starting version V1-001 . . ."
$if (test_only) then err I TESTONLY "running in test only mode" C
$test_date = ""
$if (P2 .nes. "")
$then
$definee/nolog sys$output nla0:
$definee/nolog sys$error nla0:
$test_date = f$cvtime(P2,"ABSOLUTE","DATE")
$deassign sys$error
$deassign sys$output
$if (test_date .eqs. "")
$then
$err W BADTEST "test date ''P2' is invalid, using TODAY"
$test_date = "TODAY"
$endif
$endif
$id_only = (P3 .nes. "")
$public_holidays = "''location'PUBLIC.HOLIDAYS"
$school_holidays = "''location'SCHOOL.HOLIDAYS"
$scsnode = f$edit(f$getsyi("SCSNODE"),"COLLAPSE,UPCASE")
$run_date = f$cvtime(test_date,"ABSOLUTE","DATE")
$run_date_numeric = f$cvtime(run_date,"COMPARISON","DATE")
$day_of_week = f$edit(f$cvtime(run_date,,"WEEKDAY"),"UPCASE")
$run_day = f$cvtime(run_date,"COMPARISON","DAY")
$run_month = f$cvtime(run_date,"ABSOLUTE","MONTH")
$run_month_numeric = f$cvtime(run_date,"COMPARISON","MONTH")
$run_year = f$cvtime(run_date,"ABSOLUTE","YEAR")
$month_list = "1234567890AB"
$day_of_year = f$extract(run_month_numeric-1,1,month_list) + run_day
$today_is = "/ALL/EVE/''run_month'/M''run_month_numeric'/''day_of_year'"
$weekday3 = f$extract(0,3,day_of_week)
$weekday2 = f$extract(0,2,day_of_week)
$today_is = today_is + "/" + weekday3
$day_of_month = "D" + run_day
$today_is = today_is + "/" + day_of_month
$if ((weekday3 .eqs. "SAT") .or. (weekday3 .eqs. "SUN"))
$then
$weekend = 1
$weekday = 0
$type_of_day = "weekend"
$today_is = today_is + "/END"
$else
$weekend = 0
$weekday = 1
$type_of_day = "weekday"
$today_is = today_is + "/WEE"
$endif
$numeric_date = f$cvtime(run_date,"COMPARISON","DATE")
$todays_date = f$edit(run_date,"COLLAPSE,UPCASE")
$tomorrows_date = f$edit(f$cvtime("TOMORROW","ABSOLUTE","DATE") -
,"COLLAPSE,UPCASE")
$yesterdays_date = f$edit(f$cvtime("YESTERDAY","ABSOLUTE","DATE") -
,"COLLAPSE,UPCASE")
$nth_xxx_of_month = ""
$if (day_of_month .les. "D07") then -
nth_xxx_of_month = "1" + weekday2
$if ((day_of_month .ges. "D08") .and. (day_of_month .les. "D14")) then -
nth_xxx_of_month = "2" + weekday2
$if ((day_of_month .ges. "D15") .and. (day_of_month .les. "D21")) then -
nth_xxx_of_month = "3" + weekday2
$if ((day_of_month .ges. "D22") .and. (day_of_month .les. "D28")) then -
nth_xxx_of_month = "4" + weekday2
$if (nth_xxx_of_month .nes. "") then -
today_is = today_is + "/" + nth_xxx_of_month
$next_weeks_month = f$cvtime("''run_date'+7-00:00","ABSOLUTE","MONTH")
$if (run_month .nes. next_weeks_month) then -
today_is = today_is + "/L" + weekday2 + "/5" + weekday2
$first_of_month = "1-''run_month'-''run_year'"
$another_day = f$cvtime("''first_of_month'+31-00:00","ABSOLUTE","DATE")
$next_month = f$cvtime(another_day,"ABSOLUTE","MONTH")
$next_months_year = f$cvtime(another_day,"ABSOLUTE","YEAR")
$first_of_next_month = "1-''next_month'-''next_months_year'"
$last_day = f$cvtime("''first_of_next_month'-1-00:00","ABSOLUTE","DATE")
$second_last = f$cvtime("''first_of_next_month'-2-00:00","ABSOLUTE","DATE")
$third_last = f$cvtime("''first_of_next_month'-3-00:00","ABSOLUTE","DATE")
$fourth_last = f$cvtime("''first_of_next_month'-4-00:00","ABSOLUTE","DATE")
$fifth_last = f$cvtime("''first_of_next_month'-5-00:00","ABSOLUTE","DATE")
$sixth_last = f$cvtime("''first_of_next_month'-6-00:00","ABSOLUTE","DATE")
$seventh_last = f$cvtime("''first_of_next_month'-7-00:00","ABSOLUTE","DATE")
$if (run_date .eqs. last_day) then today_is = today_is + "/LAS/D99"
$if (run_date .eqs. second_last) then today_is = today_is + "/2LA/D98"
$if (run_date .eqs. third_last) then today_is = today_is + "/3LA/D97"
$if (run_date .eqs. fourth_last) then today_is = today_is + "/4LA/D96"
$if (run_date .eqs. fifth_last) then today_is = today_is + "/5LA/D95"
$if (run_date .eqs. sixth_last) then today_is = today_is + "/6LA/D94"
$if (run_date .eqs. seventh_last) then today_is = today_is + "/7LA/D93"
$holiday_today = 0
$holiday_yesterday = 0
$holiday_tomorrow = 0
$if (f$search(public_holidays) .eqs. "")
$then
$err W NOHOLIDAY "unable to determine public holiday status"
$err E MISSING "file ''public_holidays' is missing" C
$else
$err I PUBHOL "processing public holiday information"
$open/read/error=end_hloop date_file 'public_holidays'
$hloop:
$read/end_of_file=end_hloop/error=end_hloop date_file date_record
$date_record = f$edit(date_record,"COLLAPSE,UPCASE,UNCOMMENT")
$if (date_record .eqs. "") then goto hloop
$if (date_record .eqs. todays_date)
$then holiday_today = 1
$today_is = today_is + "/HOL/PUB"
$endif
$if (date_record .eqs. yesterdays_date)
$then holiday_yesterday = 1
$today_is = today_is + "/AFT"
$endif
$if (date_record .eqs. tomorrows_date)
$then holiday_tomorrow = 1
$today_is = today_is + "/BEF"
$endif
$goto hloop
$end_hloop:
$close/nolog date_file
$endif
$if (f$search(school_holidays) .eqs. "")
$then
$err I NOSCHOOL "no school holiday information available"
$else
$err I SCHOOL "processing school holiday information"
$school_day = 0
$open/read/error=end_shloop schoolfile 'school_holidays'
$shloop:
$read/end_of_file=end_shloop/error=end_shloop schoolfile school_record
$school_record = f$edit(school_record,"COLLAPSE,UPCASE,UNCOMMENT")
$if (school_record .eqs. "") then goto shloop
$term = f$element(0,",",school_record)
$term_start = f$element(1,",",school_record)
$term_end = f$element(2,",",school_record)
$if ((term .lts. "1") .or. (term .gts. "4"))
$then
$err W BADTERM "school holiday record <''school_record'> is invalid"
$else
$t_start = ""
$t_end = ""
$definee/nolog sys$output nla0:
$definee/nolog sys$error nla0:
$t_start = f$cvtime(term_start,"COMPARISON","DATE")
$t_end = f$cvtime(term_end,"COMPARISON","DATE")
$deassign sys$error
$deassign sys$output
$if ((t_start .eqs. "") .or. (t_end .eqs. ""))
$then
$err W BADDATE "school holiday record <''school_record'> is invalid"
$else
$if ((run_date_numeric .ges. t_start) -
.and. (run_date_numeric .les. t_end) -
.and. (.not. weekend) .and. (.not. holiday_today))
$then
$school_day = 1
$today_is = today_is + "/ST" + term
$endif
$endif
$endif
$goto shloop
$end_shloop:
$closee/nolog schoolfile
$if (school_day) then today_is = today_is + "/SCH"
$endif
$run_day_c = f$cvtime(run_date,"COMPARISON","DAY")
$run_month_c = f$cvtime(run_date,"COMPARISON","MONTH")
$run_year_c = f$cvtime(run_date,"COMPARISON","YEAR")
$if ((run_year_c .lt. 1901) .or. (run_year_c .gt. 2100)) then -
goto end_lunar_stuff
$lunar_offset = 4
$if (run_year_c .gt. 2000) then lunar_offset = 8
$if (run_month_c .eqs. "01") then run_month_c = "03"
$if (run_month_c .eqs. "02") then run_month_c = "04"
$run_yy = run_year_c - ((run_year_c/100)*100)
$number1 = run_yy - ((run_yy/19)*19)
$if (number1 .gt. 9)
$then negative = 1
$number1 = number1 - 19
$number2 = -number1*11
$else negative = 0
$number2 = number1*11
$endif
$number3 = number2 - ((number2/30)*30)
$if (negative) then number3 = -number3
$number4 = number3 + run_day_c + run_month_c - lunar_offset
$if (number4 .lt. 0)
$then negative = 1
$else negative = 0
$endif
$lunar_day = number4 - ((number4/30)*30)
$if (negative) then lunar_day = -lunar_day
$new_moon = 0
$full_moon = 0
$blue_moon = 0
$if (lunar_day .eq. 0)
$then
$new_moon = 1
$today_is = today_is + "/NEW"
$endif
$if (lunar_day .eq. 14)
$then
$full_moon = 1
$today_is = today_is + "/FUL"
$if (run_day_c .gt. 29)
$then
$blue_moon = 1
$today_is = today_is + "/BLU"
$endif
$endif
$if (lunar_day .lt. 10) then lunar_day = "0''lunar_day'"
$today_is = today_is + "/L''lunar_day'"
$end_lunar_stuff:
$today_is = today_is + "/"
$today_is_len = f$length(today_is)
$default_submit_options = "/KEEP/NOPRINT/LOG_FILE"
$default_procedure_spec = location + ".COM"
$bagdaily_run_date == "''run_date'"
$bagdaily_today_is == "''today_is'"
$if (.not. test_only)
$then
$definee/system/nolog BAGDAILY_RUN_DATE "''bagdaily_run_date'"
$definee/system/nolog BAGDAILY_TODAY_IS "''bagdaily_today_is'"
$endif
$err I TODAYIS "today is ''day_of_week' ''run_date'"
$err I POSSIBLE "today is one of ''today_is'" C
$if (new_moon) then err I NEWMOON "today is a new moon" C
$if (full_moon) then err I FULLMOON "today is a full moon" C
$if (blue_moon) then err I BLUEMOON "today is also a blue moon" C
$if (id_only) then goto bail_out
$common_jobs = "''location'COMMON_DAILY.JOBS"
$err I PROCCOM "processing ''common_jobs'"
$call process_file 'common_jobs'
$err I FINICOM "finished processing ''common_jobs'"
$user_jobs = "SYS$MANAGER:''scsnode'_DAILY.JOBS"
$err I PROCLOC "processing ''user_jobs'"
$call process_file 'user_jobs'
$err I FINILOC "finished processing ''user_jobs'"
$err I FINISHED "that's all folks . . ."
$bail_out:
$exitt 1+(0*'f$verify(__vfy_saved)')
$process_file: subroutine
$set noon
$job_file = f$edit(P1,"COLLAPSE,UPCASE")
$if (job_file .eqs. "") then goto exit_process_file
$job_filespec = f$search(job_file)
$if (job_filespec .eqs. "") then goto exit_process_file
$job_filespec = f$element(0,";",job_filespec)
$open/read/error=end_loop jobs 'job_file'
$loop:
$read/error=end_loop/end_of_file=end_loop jobs job_data
$job_data = f$edit(job_data,"COMPRESS,TRIM,UNCOMMENT,UPCASE")
$if (job_data .eqs. "") then goto loop
$job_day = f$edit(f$element(0," ",job_data),"COLLAPSE")
$job_inclusion = f$element(0,"-",job_day)
$inclusions = ""
$count = 0
$iloop:
$include = f$element(count,",",job_inclusion)
$count = count + 1
$if (include .eqs. "") then goto iloop
$if (include .eqs. ",") then goto end_iloop
$inclusions = inclusions + "," + f$extract(0,3,include)
$goto iloop
$end_iloop:
$job_inclusions = inclusions - ","
$job_exclusion = f$element(1,"-",job_day)
$if (job_exclusion .eqs. "-") then job_exclusion = ""
$exclusions = ""
$count = 0
$eloop:
$exclude = f$element(count,",",job_exclusion)
$count = count + 1
$if (exclude .eqs. "") then goto eloop
$if (exclude .eqs. ",") then goto end_eloop
$exclusions = exclusions + "," + f$extract(0,3,exclude)
$goto eloop
$end_eloop:
$job_exclusions = exclusions - ","
$job_username = f$edit(f$element(1," ",job_data),"COLLAPSE")
$job_procedure = f$edit(f$element(2," ",job_data),"COLLAPSE")
$job_queue = f$edit(f$element(3," ",job_data),"COLLAPSE")
$job_time = f$edit(f$element(4," ",job_data),"COLLAPSE")
$job_options = f$edit(f$element(5," ",job_data),"COLLAPSE")
$if (job_options .nes. "")
$then
$if (f$extract(0,1,job_options) .nes. "/")
$then
$job_options = "/" + job_options
$endif
$endif
$selected = 0
$count = 0
$sloop:
$item = f$element(count,",",job_inclusions)
$count = count + 1
$if (item .eqs. "") then goto sloop
$if (item .eqs. ",") then goto end_sloop
$if (f$locate("/''item'/",today_is) .lt. today_is_len) then selected = 1
$if (.not. selected) then goto sloop
$end_sloop:
$if (selected)
$then
$err I SELECT "selected ''job_procedure' for user ''job_username'"
$err I RUN_AT "to run at ''job_time' on queue ''job_queue'" C
$err I REASON "because of ''job_inclusions'" C
$rejected = 0
$count = 0
$rloop:
$item = f$element(count,",",job_exclusions)
$count = count + 1
$if (item .eqs. "") then goto rloop
$if (item .eqs. ",") then goto end_rloop
$if (f$locate("/''item'/",today_is) .lt. today_is_len) then rejected = 1
$if (.not. rejected) then goto rloop
$end_rloop:
$if (rejected)
$then
$err I REJECT "rejected because of ''job_exclusions'" C
$else
$call process_job 'job_username' 'job_procedure' -
'job_queue' 'job_time' 'job_options'
$endif
$endif
$goto loop
$end_loop:
$closee/nolog jobs
$exit_process_file:
$exitt 1
$endsubroutine
$process_job: subroutine
$set noon
$job_username = "''P1'"
$job_procedure = "''P2'"
$job_queue = "''P3'"
$job_time = "''P4'"
$job_options = "''P5'"
$job_procedure = f$element(0,";",f$parse(job_procedure,default_procedure_spec))
$if (f$search(job_procedure) .eqs. "")
$then
$err E NOPROC "can't find ''job_procedure'" C
$else
$cancel_it = f$getqui("CANCEL_OPERATION",,,)
$queue_name = f$getqui("DISPLAY_QUEUE","QUEUE_NAME",job_queue,)
$cancel_it = f$getqui("CANCEL_OPERATION",,,)
$if (queue_name .nes. job_queue)
$then
$err E NO_QUE "queue ''job_queue' does not exist" C
$else
$err I SUBMIT "submitting ''job_procedure'" C
$if (job_options .nes. "") then -
err I OPTIONS "with user options ''job_options'" C
$if (.not. test_only)
$then
$submitt'default_submit_options' -
/queue='job_queue'/user='job_username'/after="''job_time'" -
'job_procedure''job_options'
$endif
$endif
$endif
$exit_process_job:
$exitt 1
$endsubroutine
$reporter: subroutine
$set noon
$percent = "%"
$first = f$edit(P4,"COLLAPSE,UPCASE")
$if (first .nes. "") then percent = "-"
$say "''percent'''facility'-''P1'-''P2', ''P3'"
$exitt 1
$endsubroutine
