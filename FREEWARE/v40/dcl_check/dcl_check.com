$! File created by DCL_DIET at 21-SEP-1998 10:55:58.83 from
$! DISK$PEEK_USER:[HAMMOND.DCL_CHECK.SRC]DCL_CHECK.COM_SOURCE;
$goto start
$DECK
    **************************************************************
    Enter the command @DCL_CHECK HELP for help with this procedure.
    **************************************************************
$EOD
$start:
$set = "set"
$set symbol /scope=(nolocal,noglobal)
$sav_status = 1
$on control_y then goto y_exit
$on warning then goto err_exit
$pid = f$getjpi("","PID")
$say = "WRITE SYS$OUTPUT"
$quote[0,8]=34
$s_quote[0,8]=39
$s2_quote = s_quote+s_quote
$s2_fao = s2_quote + "F$FA"
$s2_upr = s2_quote + "F$"
$s2_lwr = s2_quote + "f$"
$exclaim[0,8]=33
$form_feed[0,8] = 12
$dcl_ck_vers = "V2.0"
$say ""
$say -
"-*- Charlie Hammond's unsupported DCL checker (Version ''dcl_ck_vers') -*-"
$suppress_BL = f$trnlnm("DCL_CHECK$SUPPRESS_BL")
$suppress_CCN = f$trnlnm("DCL_CHECK$SUPPRESS_DDN")
$suppress_CLD = f$trnlnm("DCL_CHECK$SUPPRESS_CLD")
$suppress_CLS = f$trnlnm("DCL_CHECK$SUPPRESS_CLS")
$suppress_CRE = f$trnlnm("DCL_CHECK$SUPPRESS_CRE")
$suppress_CRG = f$trnlnm("DCL_CHECK$SUPPRESS_CRG")
$suppress_DFB = f$trnlnm("DCL_CHECK$SUPPRESS_DFB")
$suppress_DL = f$trnlnm("DCL_CHECK$SUPPRESS_DL")
$suppress_DNA = f$trnlnm("DCL_CHECK$SUPPRESS_DNA")
$suppress_EFB = f$trnlnm("DCL_CHECK$SUPPRESS_EFB")
$suppress_EFN = f$trnlnm("DCL_CHECK$SUPPRESS_EFN")
$suppress_ENA = f$trnlnm("DCL_CHECK$SUPPRESS_ENA")
$suppress_ICO = f$trnlnm("DCL_CHECK$SUPPRESS_ICO")
$suppress_INT = f$trnlnm("DCL_CHECK$SUPPRESS_INT")
$suppress_LC = f$trnlnm("DCL_CHECK$SUPPRESS_LC")
$suppress_LFF = f$trnlnm("DCL_CHECK$SUPPRESS_LFF")
$suppress_LND = f$trnlnm("DCL_CHECK$SUPPRESS_LND")
$suppress_LNF = f$trnlnm("DCL_CHECK$SUPPRESS_LNF")
$suppress_LNR = f$trnlnm("DCL_CHECK$SUPPRESS_LNR")
$suppress_LOD = f$trnlnm("DCL_CHECK$SUPPRESS_LOD")
$suppress_MEC = f$trnlnm("DCL_CHECK$SUPPRESS_MCO")
$suppress_NCL = f$trnlnm("DCL_CHECK$SUPPRESS_NCL")
$suppress_NED = f$trnlnm("DCL_CHECK$SUPPRESS_NED")
$suppress_NED = f$trnlnm("DCL_CHECK$SUPPRESS_NED")
$suppress_PML = f$trnlnm("DCL_CHECK$SUPPRESS_PML")
$suppress_PRQ = f$trnlnm("DCL_CHECK$SUPPRESS_PRQ")
$suppress_PSQ = f$trnlnm("DCL_CHECK$SUPPRESS_PSQ")
$suppress_PTL = f$trnlnm("DCL_CHECK$SUPPRESS_PTL")
$suppress_RLI = f$trnlnm("DCL_CHECK$SUPPRESS_RLI")
$suppress_RLS = f$trnlnm("DCL_CHECK$SUPPRESS_RLS")
$suppress_RNA = f$trnlnm("DCL_CHECK$SUPPRESS_RNA")
$suppress_SNT = f$trnlnm("DCL_CHECK$SUPPRESS_SNT")
$suppress_TLS = f$trnlnm("DCL_CHECK$SUPPRESS_TLS")
$suppress_TML = f$trnlnm("DCL_CHECK$SUPPRESS_TML")
$suppress_TNA = f$trnlnm("DCL_CHECK$SUPPRESS_TNA")
$suppress_TRH = f$trnlnm("DCL_CHECK$SUPPRESS_TRH")
$suppress_UMP = f$trnlnm("DCL_CHECK$SUPPRESS_UMP")
$suppress_UPQ = f$trnlnm("DCL_CHECK$SUPPRESS_UPQ")
$suppress_WCT = f$trnlnm("DCL_CHECK$SUPPRESS_WCT")
$get_help:
$if f$type(p2) .eqs. "" then p2 = ""
$if f$edit(p1,"UPCASE") .nes. "HELP" then goto get_filename
$help_dir = f$parse(F$ENVIRONMENT("PROCEDURE"),,,"DEVICE") -
+ f$parse(F$ENVIRONMENT("PROCEDURE"),,,"DIRECTORY")
$define/user sys$input sys$output
$if p2 .eqs. ""
$then
$help/libr='help_dir'dcl_check/nouser/prompt DCL_CHECK
$else
$help/libr='help_dir'dcl_check/nouser/prompt -
'p2' 'p3' 'p4' 'p5' 'p6' 'p7' 'p8'
$endif
$p1 = ""
$p2 = ""
$p3 = ""
$p4 = ""
$p5 = ""
$p6 = ""
$p7 = ""
$p8 = ""
$goto help_end
$help_end:
$say ""
$say "-*- Charlie Hammond's unsupported DCL checker (Version ''dcl_ck_vers') -*-"
$get_filename:
$if p1 .eqs. ""
$then
$say ""
$say "You can follow the name of the file to be checked with a name"
$say "for the report file (default is SYS$OUTPUT). (blank separated)"
$say ""
$read sys$output /end= common_exit /err=get_filename in$ -
/prompt= -
"enter name of file (or HELP or EXIT): "
$in$ = f$edit(in$,"TRIM,COMPRESS")
$if f$element(0," ",in$) .gts. " " then p1 = f$element(0," ",in$)
$if f$element(1," ",in$) .gts. " " then p2 = f$element(1," ",in$)
$if f$element(2," ",in$) .gts. " " then p3 = f$element(2," ",in$)
$if f$element(3," ",in$) .gts. " " then p4 = f$element(2," ",in$)
$if f$element(4," ",in$) .gts. " " then p5 = f$element(2," ",in$)
$if f$element(5," ",in$) .gts. " " then p6 = f$element(2," ",in$)
$if f$element(6," ",in$) .gts. " " then p7 = f$element(2," ",in$)
$if f$element(7," ",in$) .gts. " " then p8 = f$element(2," ",in$)
$goto get_filename
$endif
$if f$edit(p1,"UPCASE") .eqs. "HELP" then goto get_help
$if f$edit(p1,"UPCASE") .eqs. "EXIT" then goto common_exit
$dcl$file = f$parse(p1,".COM")
$if f$search(dcl$file) .eqs. ""
$then
$say "*** Cannot find ''dcl$file'"
$p1 = ""
$goto get_filename
$endif
$if p2 .nes. ""
$then
$if f$parse(p2,".LIS") .eqs. ""
$then
$say "*** ""''p2'"" is not a valid report filename"
$if f$edit(f$extract(0,4,p2),"UPCASE") .eqs. "/OUT"
$then
$say "   (""/OUT="" is invalid)"
$endif
$p1 = ""
$p2 = ""
$goto get_filename
$endif
$endif
$dcl_ck_time = f$time()
$say "Checking file ''dcl$file'"
$say ""
$close/err=open_error1 err_file
$open_error1:
$if f$search("sys$scratch:dcl$error_''pid'.tmp") .nes. "" then -
delete /nolog sys$scratch:dcl$error_'pid'.tmp;*
$create sys$scratch:dcl$error_'pid'.tmp
$open /append err_file sys$scratch:dcl$error_'pid'.tmp
$close/err=open_label1 label_file
$open_label1:
$if f$search("sys$scratch:dcl$label_''pid'.idx") .nes. "" then -
delete /nolog sys$scratch:dcl$label_'pid'.idx;*
$create /fdl=sys$input sys$scratch:dcl$label_'pid'.idx
$DECK
FILE
        ORGANIZATION            indexed
RECORD
        CARRIAGE_CONTROL        carriage_return
        FORMAT                  variable
        SIZE                    261
AREA 0
        ALLOCATION              54
        BEST_TRY_CONTIGUOUS     yes
        BUCKET_SIZE             9
        EXTENSION               27
AREA 1
        ALLOCATION              9
        BEST_TRY_CONTIGUOUS     yes
        BUCKET_SIZE             9
        EXTENSION               9
KEY 0
        CHANGES                 no
        DATA_AREA               0
        DATA_FILL               100
        DATA_KEY_COMPRESSION    yes
        DATA_RECORD_COMPRESSION yes
        DUPLICATES              no
        INDEX_AREA              1
        INDEX_COMPRESSION       yes
        INDEX_FILL              100
        LEVEL1_INDEX_AREA       1
        PROLOG                  3
        SEG0_LENGTH             255
        SEG0_POSITION           6
        TYPE                    string
$EOD
$open /read /write label_file sys$scratch:dcl$label_'pid'.idx
$close/err=open_if1 if_file
$open_if1:
$if f$search("sys$scratch:dcl$if_''pid'.idx") .nes. "" then -
delete /nolog sys$scratch:dcl$if_'pid'.idx;*
$create /fdl=sys$input sys$scratch:dcl$if_'pid'.idx
$DECK
FILE
        ORGANIZATION            indexed
RECORD
        CARRIAGE_CONTROL        carriage_return
        FORMAT                  fixed
        SIZE                    11
AREA 0
        ALLOCATION              6
        BEST_TRY_CONTIGUOUS     yes
        BUCKET_SIZE             3
        EXTENSION               3
AREA 1
        ALLOCATION              3
        BEST_TRY_CONTIGUOUS     yes
        BUCKET_SIZE             3
        EXTENSION               3
KEY 0
        CHANGES                 no
        DATA_AREA               0
        DATA_FILL               100
        DATA_KEY_COMPRESSION    no
        DATA_RECORD_COMPRESSION no
        DUPLICATES              no
        INDEX_AREA              1
        INDEX_COMPRESSION       no
        INDEX_FILL              100
        LEVEL1_INDEX_AREA       1
        PROLOG                  3
        SEG0_LENGTH             5
        SEG0_POSITION           0
        TYPE                    dstring
$EOD
$open /read /write if_file sys$scratch:dcl$if_'pid'.idx
$close/err=open_work1 work_file
$open_work1:
$if f$search("sys$scratch:dcl$work_''pid'.tmp") .nes. "" then -
delete /nolog sys$scratch:dcl$work_'pid'.tmp;*
$create sys$scratch:dcl$work_'pid'.tmp
$open /append work_file sys$scratch:dcl$work_'pid'.tmp
$close/err=open_dcl1 dcl_file
$open_dcl1:
$open /read dcl_file 'dcl$file'
$exit_shadow = 0
$goto_shadow = 0
$line_number = 0
$total_lines = 0
$code_lines = 0
$commented_code_lines = 0
$continuation_lines = 0
$deck_lines = 0
$comment_lines = 0
$blank_lines = 0
$in_deck = 0
$deck_line = 0
$if_level = 0
$sr_level = 0
$if_lines = ""
$then_level = "000000000000000000000"
$else_level = "000000000000000000000"
$ifsr_level = "000000000000000000000"
$valid_lexicals = -
"F$CONTEXT,F$CSID,F$CVSI,F$CVTIME,F$CVUI,F$DEVICE,F$DIRECTORY,F$EDIT," + -
"F$ELEMENT,F$ENVIRONMENT'F$EXTRACT,F$FAO,F$FILE_ATTRIBUTES,F$GETENV," + -
"F$GETDVI,F$GETJPI,F$GETQUI,F$GETSYI,F$IDENTIFIER,F$INTEGER,F$LENGTH," + -
"F$LOCATE,F$LOGICAL,F$MESSAGE,F$MODE,F$PARSE,F$PID,F$PRIVILEGE," + -
"F$PROCESS,F$SEARCH,F$SETPRV,F$STRING,F$TIME,F$TRNLNM,F$TYPE,F$USER," + -
"F$VERIFY"
$length_valid_lexicals = f$length(valid_lexicals)
$Say "Starting Pass 1 -- ''f$time()' ..."
$line_disply_increment = 500
$line_to_display = line_disply_increment - 1
$read_dcl1:
$read /end=end_dcl1 dcl_file dcl_record
$line_number = line_number + 1
$saved_line = line_number
$if line_number .gt. line_to_display
$then
$say f$fao("...processing line number !UL...",line_number)
$line_to_display = line_to_display + line_disply_increment
$endif
$if in_deck
$then
$dcl_record = f$edit(dcl_record,"COMPRESS,UPCASE")
$deck_lines = deck_lines + 1
$else
$dcl_record = f$edit(dcl_record,"TRIM,COMPRESS,UPCASE")
$if dcl_record .eqs. "$"
$then
$if .not. suppress_LOD then write err_file f$fao( -
"!5UL  LOD  line contains only ""$"" (warning)", -
saved_line)
$goto read_dcl1
$endif
$if dcl_record .eqs. ""
$then
$if .not. suppress_BL then write err_file f$fao( -
"!5UL  BL   blank line (warning)", -
saved_line)
$blank_lines = blank_lines + 1
$goto read_dcl1
$endif
$if dcl_record .eqs. form_feed
$then
$if .not. suppress_LFF then write err_file f$fao( -
"!5UL  LFF  line contains only form-feed (<FF>)", -
saved_line)
$goto read_dcl1
$endif
$if ( (f$extract(0,2,dcl_record) .eqs. "$!") -
.or. (f$extract(0,3,dcl_record) .eqs. "$ !") )
$then
$if ( (dcl_record .eqs. "$!") -
.or. (dcl_record .eqs. "$ !") )
$then
$blank_lines = blank_lines + 1
$else
$comment_lines = comment_lines + 1
$endif
$goto read_dcl1
$endif
$endif
$t0 = f$element(0," ",dcl_record)
$t1 = f$element(1," ",dcl_record)
$if (t0 .eqs. "$DECK") .or. ( (t0 .eqs. "$") .and. (t1 .eqs. "DECK") )
$then
$if in_deck
$then
$if .not. suppress_NED then write err_file f$fao( -
"!5UL  NED  no $EOD for $DECK at !UL", -
saved_line,deck_line)
$if .not. suppress_DFB then write err_file f$fao( -
"!5UL  DFB  $DECK found between $DECK and $EOD", -
saved_line)
$else
$in_deck = 1
$deck_line = saved_line
$endif
$goto read_dcl1
$endif
$if (t0 .eqs. "$EOD") .or. ( (t0 .eqs. "$") .and. (t1 .eqs. "EOD") )
$then
$if in_deck
$then
$in_deck = 0
$deck_lines = deck_lines - 1
$else
$if .not. suppress_EFN then write err_file f$fao( -
"!5UL  EFN  $EOD found with no corresponding $DECK", -
saved_line)
$endif
$goto read_dcl1
$endif
$if in_deck then goto read_dcl1
$read_dcl1_c:
$x = f$locate(s2_lwr,dcl_record)
$if f$edit(f$extract(x,6,dcl_record),"UPCASE") .eqs. s2_fao -
then goto do_fao
$x = f$locate(s2_upr,dcl_record)
$if f$edit(f$extract(x,6,dcl_record),"UPCASE") .eqs. s2_fao -
then goto do_fao
$goto after_fao_check
$do_fao:
$x = x + 2
$work = f$extract (0,x,dcl_record) + "X$"
$x = x + 2
$q_count = 0
$next_char = f$extract(x,1,dcl_record)
$x = x + 1
$fao_loop:
$if q_count .ge. 2
$then
$work = work + f$extract(x,f$length(dcl_record)-x,dcl_record)
$dcl_record = work
$goto read_dcl1_c
$endif
$char = next_char
$next_char = f$extract(x,1,dcl_record)
$x = x + 1
$if char .eqs. quote
$then
$if next_char .eqs. quote
$then
$work = work + quote + quote
$next_char = f$extract(x,1,dcl_record)
$x = x + 1
$else
$work = work + quote
$q_count = q_count + 1
$endif
$else
$if char .nes. exclaim then work = work + char
$endif
$if x .ge. 1024 then q_count = q_count + 1
$goto fao_loop
$after_fao_check:
$work = f$edit(dcl_record,"UNCOMMENT,TRIM")
$if work .nes. dcl_record
$then
$commented_code_lines = commented_code_lines + 1
$dcl_record = work
$endif
$if f$extract(f$length(dcl_record)-1,1,dcl_record) .eqs. "-"
$then
$If f$extract(f$length(dcl_record)-2,2,dcl_record) .nes. " -"
$then
$if .not. suppress_CCN then write err_file f$fao( -
"!5UL  CCN  Continuation character (""-"") not preceded by space (warning)", -
line_number)
$endif
$dcl_record = f$extract(0,f$length(dcl_record)-1,dcl_record)
$read_continuation:
$read /end=end_dcl1_c dcl_file dcl_c_record
$line_number = line_number + 1
$continuation_lines = continuation_lines + 1
$work = f$edit(dcl_c_record,"TRIM,COMPRESS")
$if ( (f$extract(0,2,work) .eqs. "$!") -
.or. (f$extract(0,3,work) .eqs. "$ !") -
.or. (f$extract(0,1,work) .eqs. "!") )
$then
$if .not. suppress_CLS then write err_file f$fao( -
"!5UL  CLS  comment line separates continuation line (warning)", -
line_number)
$goto read_continuation
$endif
$if f$extract(0,1,dcl_c_record) .eqs. "$"
$then
$if .not. suppress_CLD then write err_file f$fao( -
"!5UL  CLD  continuation line starts with ""$""", -
line_number)
$endif
$dcl_record = dcl_record + dcl_c_record
$dcl_record = f$edit(dcl_record,"TRIM,COMPRESS,UPCASE")
$goto read_dcl1_c
$endif
$if f$extract(0,1,dcl_record) .nes. "$"
$then
$if .not. suppress_LND then write err_file f$fao( -
"!5UL  LND  line does not start with ""$""", -
saved_line)
$goto read_dcl1
$endif
$if f$extract(0,2,dcl_record) .eqs. "$ "
$then
$dcl_record = dcl_record - "$ "
$else
$if f$extract(0,1,dcl_record) .eqs. "$" then -
dcl_record = dcl_record - "$"
$endif
$label = f$element(0," ",dcl_record)
$If f$locate(":",label) .eq. (f$length(label)-1)
$then
$exit_shadow = 0
$goto_shadow = 0
$label = label - ":"
$read /err=add_the_label /key="''f$fao("!255AS",label)'" label_file x
$if .not. suppress_Dl then write err_file f$fao( -
"!5UL  DL   duplicate label ""!AS""", -
saved_line,label)
$endif
$goto label_found
$add_the_label:
$work_status = $status
$if work_status .eq. %X000182B2
$then
$x = f$fao("0!5UL!255AS",saved_line,label)
$write/symbol label_file x
$else
$goto err_exit_w_status
$endif
$label_found:
$l = f$locate("F$",dcl_record)
$if l .ge. f$length(dcl_record)
$then
$l = f$locate("f$",dcl_record)
$if l .ge. f$length(dcl_record) then goto after_lexical
$endif
$if l .gt. 0
$then
$prior$char = f$extract(l-1,1,dcl_record)
$else
$prior$char = " "
$endif
$work = f$extract(l,999,dcl_record)
$lexical_loop:
$work1 = f$edit(f$extract(0,2,work),"UPCASE")
$l = 2
$if prior$char .ges. "0" .and. prior$char .les. "9" then goto lexical_next
$if prior$char .ges. "a" .and. prior$char .les. "z" then goto lexical_next
$if prior$char .ges. "A" .and. prior$char .les. "Z" then goto lexical_next
$if prior$char .eqs. "$" then goto lexical_next
$if prior$char .eqs. "_" then goto lexical_next
$lexical_loop_2:
$char = f$edit(f$extract(l,1,work),"UPCASE")
$l = l + 1
$if char .ges. "a" .and. char .les. "z" then goto lexical_char
$if char .ges. "A" .and. char .les. "Z" then goto lexical_char
$goto lexical_check
$lexical_char:
$work1 = work1 + char
$goto lexical_loop_2
$lexical_check:
$work1 = f$edit(work1,"UPCASE")
$if l .lt. 6
$then
$if work1 .nes. "F$FAO" .and. work1 .nes. "F$PID"
$then
$if .not. suppress_PTL then write err_file f$fao( -
"!5UL  PTL  possible truncated lexical (!AS)", -
saved_line,work1)
$endif
$endif
$if f$locate(work1,valid_lexicals) .eq. length_valid_lexicals
$then
$if .not. suppress_PML then write err_file f$fao( -
"!5UL  PML  possible misspelled lexical (!AS)", -
saved_line,work1)
$endif
$lexical_next:
$work = f$extract(l-1,999,work)
$l = f$locate("F$",work)
$if l .ge. f$length(work) then goto after_lexical
$if l .gt. 0
$then
$prior$char = f$extract(l-1,1,work)
$else
$prior$char = " "
$endif
$work = f$extract(l,999,work)
$goto lexical_loop
$after_lexical:
$l = 0
$wct_loop:
$work = f$element(l," ",dcl_record)
$if work .eqs. " " then got after_wct
$if work .eqs. ".EQ." -
.or. work .eqs. ".GE." -
.or. work .eqs. ".GT." -
.or. work .eqs. ".LE." -
.or. work .eqs. ".LT." -
.or. work .eqs. ".NE."
$then
$if l .eq. 0
$then
$if .not. suppress_MEC then write err_file f$fao( -
"!5UL  MEC  missing expression for comparsion", -
saved_line)
$l = l + 1
$goto wct_loop
$endif
$if f$element(l+1," ",dcl_record) .eqs. " "
$then
$if .not. suppress_MEC then write err_file f$fao( -
"!5UL  MEC  missing expression for comparsion", -
saved_line)
$goto after_wct
$endif
$if ( ( (f$extract(0,1,f$element(l-1," ",dcl_record)) .eqs. quote ) -
.and. (f$extract(0,2,f$element(l-1," ",dcl_record)) .nes. """," ) -
.and. (f$extract(0,2,f$element(l-1," ",dcl_record)) .nes. """""") ) -
.or. ( (f$extract(0,1,f$element(l+1," ",dcl_record)) .eqs. quote ) -
.and. (f$extract(0,2,f$element(l+1," ",dcl_record)) .nes. """""") ) )
$then
$if .not. suppress_WCT then write err_file f$fao( -
"!5UL  WCT  wrong constant type for comparison", -
saved_line)
$endif
$endif
$if work .eqs. ".EQS." -
.or. work .eqs. ".GES." -
.or. work .eqs. ".GTS." -
.or. work .eqs. ".LES." -
.or. work .eqs. ".LTS." -
.or. work .eqs. ".NES."
$then
$if l .eq. 0
$then
$if .not. suppress_MEC then write err_file f$fao( -
"!5UL  MEC  missing expression for comparsion", -
saved_line)
$l = l + 1
$goto wct_loop
$endif
$if f$element(l+1," ",dcl_record) .eqs. " "
$then
$if .not. suppress_MEC then write err_file f$fao( -
"!5UL  MEC  missing expression for comparsion", -
saved_line)
$goto after_wct
$endif
$work1 = f$extract(0,1,f$element(l-1," ",dcl_record))
$work2 = f$extract(0,1,f$element(l+1," ",dcl_record))
$if (work1 .ges. "0" .and. work1 .les. "9") -
.or. (work2 .ges. "0" .and. work2 .les. "9")
$then
$if .not. suppress_WCT then write err_file f$fao( -
"!5UL  WCT  wrong constant type for comparison", -
saved_line)
$endif
$endif
$l = l + 1
$goto wct_loop
$after_wct:
$op_cnt = 0
$cp_cnt = 0
$l = 0
$count_o_parens:
$work = f$element(l,"(",dcl_record)
$if work .eqs. "(" then goto o_parens_counted
$l = l + 1
$goto count_o_parens
$o_parens_counted:
$op_cnt = l - 1
$l = 0
$count_c_parens:
$work = f$element(l,")",dcl_record)
$if work .eqs. ")" then goto c_parens_counted
$l = l + 1
$goto count_c_parens
$c_parens_counted:
$cp_cnt = l - 1
$if op_cnt .ne. cp_cnt
$then
$if .not. suppress_UMP then write err_file f$fao( -
"!5UL  UMP  unmatched parentheses", -
saved_line)
$endif
$l = 0
$count_quotes:
$work = f$element(l,quote,dcl_record)
$if work .eqs. quote then goto quotes_counted
$l = l + 1
$goto count_quotes
$quotes_counted:
$l = l - 1
$if l .ne. l/2*2
$then
$if .not. suppress_UPQ then write err_file f$fao( -
"!5UL  UPQ  unpaired quotation marks ("")", -
saved_line)
$endif
$work = ""
$dcl_record = dcl_record -
- "aren't" -
- "can't" -
- "couldn't" -
- "didn't" -
- "doesn't" -
- "don't" -
- "hadn't" -
- "hasn't" -
- "haven't" -
- "he'd" -
- "he'll" -
- "he's" -
- "I'd" -
- "I'll" -
- "I'm" -
- "I've" -
- "isn't" -
- "it's" -
- "she'd" -
- "she'll"
$dcl_record = dcl_record -
- "she's" -
- "shouldn't" -
- "that's" -
- "they'd" -
- "they'll" -
- "they're" -
- "wasn't" -
- "we'd" -
- "we'll" -
- "we're" -
- "we've" -
- "weren't" -
- "what's" -
- "who's" -
- "won't" -
- "wouldn't" -
- "you'd" -
- "you'll" -
- "you're" -
- "you've"
$dcl_record = dcl_record -
- "Aren't" -
- "Can't" -
- "Couldn't" -
- "Didn't" -
- "Doesn't" -
- "Don't" -
- "Hadn't" -
- "Hasn't" -
- "Haven't" -
- "He'd" -
- "He'll" -
- "He's" -
- "Isn't" -
- "It's" -
- "She'd" -
- "She'll"
$dcl_record = dcl_record -
- "She's" -
- "Shouldn't" -
- "That's" -
- "They'd" -
- "They'll" -
- "They're" -
- "Wasn't" -
- "We'd" -
- "We'll" -
- "We're" -
- "We've" -
- "Weren't" -
- "What's" -
- "Who's" -
- "Won't" -
- "Wouldn't" -
- "You'd" -
- "You'll" -
- "You're" -
- "You've"
$not_in_quote:
$temp = f$element(0,quote,dcl_record)
$if temp .eqs. quote then goto end_check_quotes
$work = work + temp
$dcl_record = dcl_record - temp - quote
$i = 0
$in_quote:
$not_in_symbol_subs:
$if f$extract(i,2,dcl_record) .eqs. s2_quote
$then
$i = i + 2
$goto in_symbol_subs
$endif
$char = f$extract(i,1,dcl_record)
$i = i + 1
$if char .eqs. "" then goto end_check_quotes
$if char .eqs. s_quote
$then
$if .not. suppress_PSQ then write err_file f$fao( -
"!5UL  PSQ  possible error using single-quote (') in quoted string", -
saved_line)
$goto end_check_quotes
$endif
$if char .eqs. quote
$then
$dcl_record = f$extract(i,9999,dcl_record)
$goto not_in_quote
$endif
$goto not_in_symbol_subs
$in_symbol_subs:
$char = f$extract(i,1,dcl_record)
$i = i + 1
$if char .eqs. ""
$then
$if .not. suppress_PSQ then write err_file f$fao( -
"!5UL  PSQ  possible error using single-quote (') in quoted string", -
saved_line)
$goto end_check_quotes
$endif
$if f$extract(i,1,dcl_record) .eqs. s_quote
$then
$i = i + 1
$goto not_in_symbol_subs
$endif
$goto in_symbol_subs
$end_check_quotes:
$dcl_record = work
$l = 0
$count_s_quotes:
$work = f$element(l,s_quote,dcl_record)
$if work .eqs. s_quote then goto s_quotes_counted
$l = l + 1
$goto count_s_quotes
$s_quotes_counted:
$l = l - 1
$if l .ne. l/2*2
$then
$if .not. suppress_PRQ then write err_file f$fao( -
"!5UL  PRQ  probable error using single-quote (')", -
saved_line)
$endif
$temp_rec = f$fao("!5UL!AS",saved_line,dcl_record)
$write /symbol work_file temp_rec
$token = f$edit(f$element(0," ",dcl_record),"UPCASE")
$If f$locate(":",token) .eq. (f$length(token)-1)
$then
$token = f$edit(f$element(1," ",dcl_record),"UPCASE")
$endif
$if token .eqs. "GOTO" then goto_shadow = saved_line
$if token .eqs. "EXIT" then exit_shadow = saved_line
$if_then_else:
$token = f$edit(f$element(0," ",dcl_record),"UPCASE")
$If f$locate(":",token) .eq. (f$length(token)-1)
$then
$token = f$edit(f$element(1," ",dcl_record),"UPCASE")
$endif
$if ( (token .nes. "THEN") -
.and. (f$extract(if_level,1,then_level) .eqs. "0") -
.and. (if_level .gt. 0) )
$then
$then_level[if_level,1] := 1
$if .not. suppress_TRH then write err_file f$fao( -
"!5UL  TRH  THEN statement required here", -
saved_line)
$endif
$if token .eqs. "IF"
$then
$if f$locate(" THEN ",dcl_record) .lt. f$length(dcl_record)
$then
$if f$locate("=",dcl_record) .lt. f$locate(" THEN ",dcl_record)
$then
$if .not. suppress_EFB then write err_file f$fao( -
"!5UL  EFB  ""="" found between IF and THEN", -
saved_line)
$endif
$else
$if if_level - sr_level .gt. 14
$then
$if .not. suppress_TML then write err_file f$fao( -
"!5UL  TML  too many levels of IF statements", -
saved_line)
$endif
$if_lines = f$fao("!UL/!AS",saved_line,if_lines)
$if_level = if_level + 1
$ifsr_level[if_level,1] := "I"
$endif
$endif
$if f$extract(0,4,token) .eqs. "SUBR"
$then
$if if_level .gt. 20
$then
$if .not. suppress_TMS then write err_file f$fao( -
"!5UL  TMS  too many levels of IF and SUBROUTINE statements", -
saved_line)
$endif
$if_lines = f$fao("!UL/!AS",saved_line,if_lines)
$if_level = if_level + 1
$sr_level = sr_level + 1
$ifsr_level[if_level,1] := "S"
$then_level[if_level,1] := 1
$else_level[if_level,1] := 1
$endif
$if token .eqs. "THEN"
$then
$if f$extract(if_level,1,then_level) .eqs. "1" -
.or. if_level .eq. 0
$then
$if .not. suppress_TNA then write err_file f$fao( -
"!5UL  TNA  THEN statement not allowed here", -
saved_line)
$else
$then_level[if_level,1] := 1
$endif
$dcl_record = dcl_record - "THEN"
$if f$extract(0,1,DCL_record) .eqs. " " then -
dcl_record = dcl_record - " "
$goto if_then_else
$endif
$if token .eqs. "ELSE"
$then
$goto_shadow = 0
$exit_shadow = 0
$if f$extract(if_level,1,else_level) .eqs. "1" -
.or. if_level .eq. 0
$then
$if .not. suppress_ENA then write err_file f$fao( -
"!5UL  ENA  ELSE statement not allowed here", -
saved_line)
$else
$else_level[if_level,1] := 1
$endif
$dcl_record = dcl_record - "ELSE"
$if f$extract(0,1,DCL_record) .eqs. " " then -
dcl_record = dcl_record - " "
$goto if_then_else
$endif
$if token .eqs. "ENDIF"
$then
$goto_shadow = 0
$exit_shadow = 0
$if if_level .eq. 0 -
.or. f$extract(if_level,1,ifsr_level) .nes. "I"
$then
$if .not. suppress_DNA then write err_file f$fao( -
"!5UL  DNA  ENDIF statement not allowed here", -
saved_line)
$else
$if_start = f$element(0,"/",if_lines)
$if_start_num = f$integer(if_start)
$write if_file f$fao("!5UL!5ULI",if_start_num,saved_line)
$then_level[if_level,1] := 0
$else_level[if_level,1] := 0
$ifsr_level[if_level,1] := 0
$if_level = if_level - 1
$if_lines = if_lines - if_start - "/"
$endif
$endif
$if f$extract(0,4,token) .eqs. "ENDS"
$then
$goto_shadow = 0
$exit_shadow = 0
$if if_level .eq. 0 -
.or. f$extract(if_level,1,ifsr_level) .nes. "S"
$then
$if .not. suppress_RNA then write err_file f$fao( -
"!5UL  RNA  ENDSUBROUTINE statement not allowed here", -
saved_line)
$else
$if_start = f$element(0,"/",if_lines)
$if_start_num = f$integer(if_start)
$write if_file f$fao("!5UL!5ULS",if_start_num,saved_line)
$then_level[if_level,1] := 0
$else_level[if_level,1] := 0
$ifsr_level[if_level,1] := 0
$if_level = if_level - 1
$sr_level = if_level - 1
$if_lines = if_lines - if_start - "/"
$endif
$endif
$if exit_shadow .gt. 0 .and. saved_line .gt. exit_shadow
$then
$if .not. suppress_CRE then write err_file f$fao( -
"!5UL  CRE  code cannot be reached due to EXIT at line !UL", -
saved_line,exit_shadow)
$exit_shadow = 0
$endif
$if goto_shadow .gt. 0 .and. saved_line .gt. goto_shadow
$then
$if .not. suppress_CRG then write err_file f$fao( -
"!5UL  CRG  code cannot be reached due to GOTO at line !UL", -
saved_line,goto_shadow)
$goto_shadow = 0
$endif
$goto read_dcl1
$end_dcl1_c:
$if .not. suppress_NCL then write err_file f$fao( -
"!5UL  NCL  no continuation line at EOF", -
line_number)
$end_dcl1:
$work2 = 0
$end_dcl1_a:
$if if_level .gt. 0
$then
$work = f$integer(f$element(work2,"/",if_lines))
$if f$extract(if_level,1,ifsr_level) .eqs. "I"
$then
$if .not. suppress_INT then write err_file f$fao( -
"!5UL  INT  IF statement not terminated", -
work)
$endif
$if f$extract(if_level,1,ifsr_level) .eqs. "S"
$then
$if .not. suppress_SNT then write err_file f$fao( -
"!5UL  SNT  SUBROUTINE statement not terminated", -
work)
$endif
$if_level = if_level - 1
$work2 = work2 + 1
$goto end_dcl1_a
$endif
$if in_deck
$then
$if .not. suppress_NED then write err_file f$fao( -
"!5UL  NED  no $EOD for $DECK at !UL", -
saved_line,deck_line)
$endif
$close work_file
$close dcl_file
$total_lines = line_number
$code_lines = total_lines - continuation_lines - deck_lines -
- comment_lines - blank_lines - deck_lines
$Say "Starting Pass 2 -- ''f$time()' ..."
$open /read work_file sys$scratch:dcl$work_'pid'.tmp
$line_to_display = line_disply_increment - 1
$read_dcl2:
$read /end=end_dcl2 work_file dcl_record
$line_number = f$integer(f$extract(0,5,dcl_record))
$dcl_record = f$extract(5,(f$length(dcl_record)-5),dcl_record)
$if line_number .gt. line_to_display
$then
$say f$fao("...processing line number !UL...",line_number)
$line_to_display = line_to_display + (line_disply_increment * 2)
$endif
$rescan = 0
$scan_for_labels_used:
$if f$locate("/END",dcl_record) .lt. f$length(dcl_record)
$then
$rescan = 1
$dcl_record = -
f$extract(f$locate("/END",dcl_record),999,dcl_record) - "/END"
$end_qual_loop:
$if f$extract(0,1,dcl_record) .nes. "="
$then
$dcl_record = f$extract(1,999,dcl_record)
$goto end_qual_loop
$endif
$dcl_record = f$extract(1,999,dcl_record)
$if f$extract(0,1,dcl_record) .eqs. " "
$then
$dcl_record = f$extract(1,999,dcl_record)
$endif
$goto check_for_label
$endif
$if f$locate("/ERR",dcl_record) .lt. f$length(dcl_record)
$then
$rescan = 1
$dcl_record = -
f$extract(f$locate("/ERR",dcl_record),999,dcl_record) - "/ERR"
$err_qual_loop:
$if f$extract(0,1,dcl_record) .nes. "="
$then
$dcl_record = f$extract(1,999,dcl_record)
$goto err_qual_loop
$endif
$dcl_record = f$extract(1,999,dcl_record)
$if f$extract(0,1,dcl_record) .eqs. " "
$then
$dcl_record = f$extract(1,999,dcl_record)
$endif
$goto check_for_label
$endif
$if f$locate("GOTO ",dcl_record) .lt. f$length(dcl_record)
$then
$dcl_record = -
f$extract(f$locate("GOTO",dcl_record),999,dcl_record) - "GOTO "
$goto_qual_loop:
$if f$extract(0,1,dcl_record) .eqs. " "
$then
$dcl_record = f$extract(1,999,dcl_record)
$goto goto_qual_loop
$endif
$goto check_for_label
$endif
$if f$locate("GOSUB ",dcl_record) .lt. f$length(dcl_record)
$then
$dcl_record = -
f$extract(f$locate("GOSUB",dcl_record),999,dcl_record) - "GOSUB "
$gosub_qual_loop:
$if f$extract(0,1,dcl_record) .eqs. " "
$then
$dcl_record = f$extract(1,999,dcl_record)
$goto gosub_qual_loop
$endif
$goto check_for_label
$endif
$if f$locate("CALL ",dcl_record) .lt. f$length(dcl_record)
$then
$dcl_record = -
f$extract(f$locate("CALL",dcl_record),999,dcl_record) - "CALL "
$call_qual_loop:
$if f$extract(0,1,dcl_record) .eqs. " "
$then
$dcl_record = f$extract(1,999,dcl_record)
$goto call_qual_loop
$endif
$goto check_for_label
$endif
$goto read_dcl2
$check_for_label:
$label = ""
$l = 0
$extract_label_loop:
$x = f$extract(l,1,dcl_record)
$if ( (x .eqs. " ") .or. (x .eqs. "/") .or. (x .eqs. "") ) then -
goto now_have_label
$label = label + x
$l = l + 1
$goto extract_label_loop
$now_have_label:
$if f$locate("'",label) .lt. f$length(label)
$then
$if .not. suppress_TLS then write err_file f$fao( -
"!5UL  TLS  target label ""!AS"" provided by symbol substitution (warning)", -
line_number,label)
$if rescan then goto scan_for_labels_used
$goto read_dcl2
$endif
$read /err=no_such_label /key="''f$fao("!255AS",label)'" label_file label_rec
$label_line = f$extract(1,5,label_rec)
$if f$extract(0,1,x) .nes. "!"
$then
$label_rec[0,1] := "1"
$write/symbol/update label_file label_rec
$endif
$read /err=endif_range -
/key="''label_line'" /match=gt if_file if_rec
$read_next_if:
$if_start = f$extract(0,5,if_rec)
$if_end = f$extract(5,5,if_rec)
$if_sr = f$extract(10,1,if_rec)
$if ( (label_line .gt. if_start) .and. (label_line .lt. if_end) )
$then
$if ( (line_number .lt. if_start) .or. (line_number .gt. if_end) )
$then
$if if_sr .eqs. "I"
$then
$if .not. suppress_RLI then write err_file f$fao( -
"!5UL  RLI  referenced label ""!AS"" is in if-group at lines !UL-!UL", -
line_number,label,f$integer(if_start),f$integer(if_end))
$endif
$if if_sr .eqs. "S"
$then
$if .not. suppress_RLS then write err_file f$fao( -
"!5UL  RLS  referenced label ""!AS"" is in subroutine at lines !UL-!UL", -
line_number,label,f$integer(if_start),f$integer(if_end))
$endif
$endif
$else
$read /err=endif_range if_file if_rec
$goto read_next_if
$endif
$endif_range:
$if rescan then goto scan_for_labels_used
$goto read_dcl2
$no_such_label:
$if .not. suppress_LNF then write err_file f$fao( -
"!5UL  LNF  label ""!AS"" not found", -
line_number,label)
$if rescan then goto scan_for_labels_used
$goto read_dcl2
$end_dcl2:
$close work_file
$close label_file
$close if_file
$open /read label_file sys$scratch:dcl$label_'pid'.idx
$read_label:
$read/end=end_label label_file x
$if f$extract(0,1,x) .eqs. "1" then goto read_label
$line_number = f$integer(f$extract(1,5,x))
$label = f$edit(f$extract(6,2555,x),"TRIM")
$if .not. suppress_LNR then write err_file f$fao( -
"!5UL  LNR  label ""!AS"" not referenced (warning)", -
line_number,label)
$goto read_label
$end_label:
$close label_file
$delete /nolog sys$scratch:dcl$label_'pid'.idx;*
$Say "Starting Pass 3 -- ''f$time()' ..."
$if f$search("sys$scratch:dcl$comp_op_''pid'.tmp") .nes. "" then -
delete /nolog sys$scratch:dcl$comp_op_'pid'.tmp;*
$if f$search("sys$scratch:dcl$spell_''pid'.tmp") .nes. "" then -
delete /nolog sys$scratch:dcl$spell_'pid'.tmp;*
$define /user sys$output nl:
$define /user sys$error nl:
$search sys$scratch:dcl$work_'pid'.tmp -
/out=sys$scratch:dcl$comp_op_'pid'.tmp -
" .eq ", " eq. ", " eq ", -
" .ge ", " ge. ", " ge ", -
" .gt ", " gt. ", " gt ", -
" .le ", " le. ", " le ", -
" .lt ", " lt. ", " lt ", -
" .ne ", " ne. ", " ne ", -
" .or ", " or. ", " or ", -
" .not "," not. "," not ", -
" .and "," and. "," and ", -
" .eqs "," eqs. "," eqs ", -
" .ges "," ges. "," ges ", -
" .gts "," gts. "," gts ", -
" .les "," les. "," les ", -
" .lts "," lts. "," lts ", -
" .nes "," nes. "," nes ", -
" .new "," new. "," .new. ", -
" .eas "," eas. "," .eas. ", -
" .gs. "," .ls. "," .es. "," .ns. ", -
">","<",">=","=>","<=","=<"," _ "
$work_status = $status
$if work_status .eq. %X08D78053
$then
$comp_op_found = 0
$else
$comp_op_found = 1
$open search_file sys$scratch:dcl$comp_op_'pid'.tmp
$read_search1:
$read /end=end_search1 search_file dcl_record
$line_number = f$integer(f$extract(0,5,dcl_record))
$if .not. suppress_ICO then write err_file f$fao( -
"!5UL  ICO  invalid comparison operator", -
line_number)
$goto read_search1
$end_search1:
$close search_file
$endif
$define /user sys$output nl:
$define /user sys$error nl:
$search sys$scratch:dcl$work_'pid'.tmp -
/out=sys$scratch:dcl$spell_'pid'.tmp -
"end_if","end if", -
"go_to","go to", -
"go_sub","go sub"
$work_status = $status
$if work_status .eq. %X08D78053
$then
$spell_found = 0
$else
$spell_found = 1
$open search_file sys$scratch:dcl$spell_'pid'.tmp
$read_search2:
$read /end=end_search2 search_file dcl_record
$line_number = f$integer(f$extract(0,5,dcl_record))
$if .not. suppress_LC then write err_file f$fao( -
"!5UL  LC   line contains END_IF, END IF, GO_TO, GO TO, GO_SUB or GO SUB", -
line_number)
$goto read_search2
$end_search2:
$close search_file
$endif
$close err_file
$sort /stable /key=(pos:1,size:5) -
sys$scratch:dcl$error_'pid'.tmp -
sys$scratch:dcl$error_'pid'.tmp
$dcl_end_time = f$time()
$if f$file_ATTRIBUTES("sys$scratch:dcl$error_''pid'.tmp","eof") .gt. 0
$then
$if ( (p2 .nes. "") .and. (p2 .nes. "$") )
$then
$report$file = f$parse(p2,".LIS")
$say ""
$say "Creating errors listing in ''report$file'"
$say "''dcl_end_time'"
$say ""
$close/err=open_report rep_file
$open_report:
$create 'report$file'
$open /append rep_file 'report$file'
$write rep_file -
"-*- Charlie Hammond's unsupported DCL checker (Version ''dcl_ck_vers' -*-)"
$write rep_file -
"Checking file ''dcl$file'"
$write rep_file -
"''dcl_ck_time'"
$write rep_file ""
$write rep_file f$fao( -
"Procedure contains:!7UL total lines",total_lines)
$write rep_file f$fao( -
"                   !7UL code lines (including !UL lines w/ comments)", -
code_lines, commented_code_lines)
$write rep_file f$fao( -
"                   !7UL additional continuation lines",continuation_lines)
$write rep_file f$fao( -
"                   !7UL lines w/i $DECK/$EOD pairs",deck_lines)
$write rep_file f$fao( -
"                   !7UL comment only lines",comment_lines)
$write rep_file f$fao( -
"                   !7UL blank lines",blank_lines)
$write rep_file ""
$write rep_file " LINE  CODE  --DIAGNOSTIC MESSAGE--"
$close rep_file
$append sys$scratch:dcl$error_'pid'.tmp,sys$input 'report$file'
$DECK

-*- END OF LISTING -*-
$EOD
$else
$say ""
$say f$fao( -
"Procedure contains:!7UL total lines",total_lines)
$say f$fao( -
"                   !7UL code lines (including !UL lines w/ comments)", -
code_lines, commented_code_lines)
$say f$fao( -
"                   !7UL additional continuation lines",continuation_lines)
$say f$fao( -
"                   !7UL lines w/i $DECK/$EOD pairs",deck_lines)
$say f$fao( -
"                   !7UL comment only lines",comment_lines)
$say f$fao( -
"                   !7UL blank lines",blank_lines)
$say ""
$say " LINE  CODE  --DIAGNOSTIC MESSAGE--"
$type sys$scratch:dcl$error_'pid'.tmp
$say "-*- END OF LISTING -*-   ''dcl_end_time'"
$say ""
$endif
$else
$say ""
$say f$fao( -
"Procedure contains:!7UL total lines",total_lines)
$say f$fao( -
"                   !7UL code lines (including !UL lines w/ comments)", -
code_lines, commented_code_lines)
$say f$fao( -
"                   !7UL additional continuation lines",continuation_lines)
$say f$fao( -
"                   !7UL lines w/i $DECK/$EOD pairs",deck_lines)
$say f$fao( -
"                   !7UL comment only lines",comment_lines)
$say f$fao( -
"                   !7UL blank lines",blank_lines)
$say ""
$say "-*- No errors found -*-   ''f$time()'"
$if ( (p2 .nes. "") .and. (p2 .nes. "$") ) then -
say "...listing file not created    ''dcl_end_time'"
$endif
$goto common_exit
$y_exit:
$write sys$output "Exiting due to Ctrl_y entry"
$goto 1_exit
$err_exit:
$sav_status = $status
$err_exit_w_status:
$write sys$output f$message(sav_status)
$if sav_status .lt. %x10000000 then sav_status = sav_status + %x10000000
$goto common_exit
$0_exit:
$write sys$output f$message(sav_status)
$sav_status = %x10000000
$goto common_exit
$1_exit:
$write sys$output f$message(sav_status)
$sav_status = 1
$goto common_exit
$2_exit:
$write sys$output f$message(sav_status)
$sav_status = %x10000002
$goto common_exit
$3_exit:
$write sys$output f$message(sav_status)
$sav_status = 3
$goto common_exit
$4_exit:
$%NONAME-F-NOMSG, Message number 00000004
$write sys$output "%FAC-I-MSG <informational message>"
$sav_status = %x10000004
$goto common_exit
$common_exit:
$on control_y then continue
$on warning then continue
$close/error=y_1 err_file
$y_1:
$close/error=y_2 label_file
$y_2:
$close/error=y_3 work_file
$y_3:
$close/error=y_4 dcl_file
$y_4:
$close/error=y_5 rep_file
$y_5:
$close/error=y_6 if_file
$y_6:
$if f$search("sys$scratch:dcl$error_''pid'.tmp") .nes. "" then -
delete /nolog sys$scratch:dcl$error_'pid'.tmp;*
$if f$search("sys$scratch:dcl$label_''pid'.idx") .nes. "" then -
delete /nolog sys$scratch:dcl$label_'pid'.idx;*
$if f$search("sys$scratch:dcl$work_''pid'.tmp") .nes. "" then -
delete /nolog sys$scratch:dcl$work_'pid'.tmp;*
$if f$search("sys$scratch:dcl$comp_op_''pid'.tmp") .nes. "" then -
delete /nolog sys$scratch:dcl$comp_op_'pid'.tmp;*
$if f$search("sys$scratch:dcl$spell_''pid'.tmp") .nes. "" then -
delete /nolog sys$scratch:dcl$spell_'pid'.tmp;*
$if f$search("sys$scratch:dcl$if_''pid'.idx") .nes. "" then -
delete /nolog sys$scratch:dcl$if_'pid'.idx;*
$exit sav_status
