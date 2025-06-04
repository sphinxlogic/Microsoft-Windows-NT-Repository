$! File created by DCL_DIET at 15-APR-1999 07:56:04.99 from
$! DBS0:[OLGA.SYS]OPTIMIZE_RMS.COS;
$__vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$if (f$type('__vfy') .eqs. "") then __vfy = 0
$__vfy_saved = f$verify(&__vfy)
$set noon
$on control_y then goto bail_out
$say = "write sys$output"
$ask = "@olga$sys:sys_ask"
$__warning = %X10000000
$__success = %X10000001
$__error = %X10000002
$__information = %X10000003
$__fatal = %X10000004
$exit_status = __success
$temp_file = "''f$getjpi("","pid")'_playback.input"
$filename == f$edit(P1, "collapse,upcase")
$adds == f$edit(P2, "collapse,upcase")
$index_compression == f$edit(P3, "collapse,upcase")
$data_compression == f$edit(P4, "collapse,upcase")
$bucket_fill == f$edit(P5, "collapse,upcase")
$if ((filename .eqs. "?") .or. (filename .eqs. "HELP")) then goto give_help
$if (adds .eqs. "") then adds == "0"
$if (index_compression .eqs. "") then index_compression == "N"
$if (data_compression .eqs. "") then data_compression == "Y"
$if (bucket_fill .eqs. "") then bucket_fill == "75"
$if (filename .nes. "") then goto batch
$ask_file:
$ask filename "File to be optimized (^Z to exit)" "" DSZ^
$if ((sys__control_z) .or. (sys__backwards)) then goto bail_out
$filename == f$search(filename)
$got_name_interactive:
$name = f$parse(filename,,,"name")
$if (f$file_attribute(filename,"org") .nes. "IDX")
$then
$say "%OPTIMIZE_RMS-E-NOTIDX, file is not indexed"
$goto ask_file
$endif
$ask_adds:
$ask adds "Number of records to add after initial load" "0" DIZ^
$if (sys__control_z) then goto bail_out
$if (sys__backwards) then goto ask_file
$ask_idx:
$ask index_compression "Index & Key Compression" "No" BDZ^
$if (sys__control_z) then goto bail_out
$if (sys__backwards) then goto ask_adds
$ask_data:
$ask data_compression "Data Record Compression" "Yes" BDZ^
$if (sys__control_z) then goto bail_out
$if (sys__backwards) then goto ask_idx
$ask bucket_fill "Bucket fill percentage" "75" DIZ^
$if (sys__control_z) then goto bail_out
$if (sys__backwards) then goto ask_data
$batch:
$got_name:
$filename == f$search(filename)
$if (filename .eqs. "")
$then
$say "%OPTIMIZE_RMS-F-FNF, file not found"
$exit_status = %X18292
$goto bail_out
$endif
$name = f$parse(filename,,,"name")
$if (f$file_attribute(filename,"org") .nes. "IDX")
$then
$say "%OPTIMIZE_RMS-E-NOTIDX, file is not indexed"
$exit_status = %X10024
$goto bail_out
$endif
$if (adds .lt. 0)
$then
$say "%OPTIMIZE_RMS-F-BADPARAM, added records must be greater than 0"
$exit_status = %X38060
$goto bail_out
$endif
$if ((bucket_fill .lt. 50) .or. (bucket_fill .gt. 100))
$then
$bucket_fill == "75"
$say ""
$say "%OPTIMIZE_RMS-I-BKTFILL, using a bucket fill of 75%"
$endif
$number_of_keys == f$file_attribute(filename,"nok")
$fdl_name = f$parse(".fdl;0",filename)
$say ""
$open/write/error=open_error out 'temp_file'
$wo = "write out"
$wo "$ define/user/nolog sys$command sys$input"
$wo "$ analyze/rms/fdl/output=''fdl_name' ''filename'"
$wo "$ define/user/nolog sys$command sys$input"
$wo "$ define/user/nolog edf$$playback_input kludge"
$wo "$ editt/fdl/script=optimize/analyze=''fdl_name' ''fdl_name'"
$wo ""
$wo ""
$wo ""
$wo "Fast"
$wo "''adds'"
$if (adds .ne. 0)
$then
$wo ""
$wo ""
$endif
$wo "yes"
$wo "-"
$wo "BF"
$wo "''bucket_fill'"
$if (index_compression)
$then
$wo "IC"
$wo "YES"
$wo "KC"
$wo "YES"
$else
$wo "IC"
$wo "NO"
$wo "KC"
$wo "NO"
$endif
$if (data_compression)
$then
$wo "RC"
$wo "YES"
$else
$wo "RC"
$wo "NO"
$endif
$wo "FD"
$wo ""
$wo ""
$wo ""
$k_loop:
$if (number_of_keys .eq. 1) then goto end_k_loop
$wo ""
$wo ""
$wo "yes"
$wo "-"
$wo "BF"
$wo "''bucket_fill'"
$if (index_compression)
$then
$wo "IC"
$wo "YES"
$wo "KC"
$wo "YES"
$else
$wo "IC"
$wo "NO"
$wo "KC"
$wo "NO"
$endif
$wo "FD"
$wo ""
$wo ""
$number_of_keys == number_of_keys - 1
$goto k_loop
$end_k_loop:
$wo "E"
$wo "$ exitt 1"
$close/nolog out
$@'temp_file'
$goto bail_out
$open_error:
$say "OPTIMIZE_RMS-F unable to open ''temp_file'"
$exit_status = __fatal
$goto bail_out
$give_help:
$say ""
$say " OPTIMIZE_RMS  V1-003"
$say ""
$say " This procedure will analyze an RMS indexed file, generate an FDL,"
$say " then optimize it using the parameters specified."
$say ""
$say " Parameters:"
$say "   P1   Filename.  No default.  (? or HELP will give this text.)"
$say "  [P2]  Additional records to add after initial load, defaults to 0."
$say "  [P3]  Index & Key compression, defaults to No."
$say "  [P4]  Data record compression, defaults to Yes."
$say "  [P5]  Bucket fill percentage, defaults to 75."
$say ""
$goto bail_out
$bail_out:
$if (f$search(temp_file) .nes. "") then deletee 'temp_file';*
$exitt 'exit_status'
