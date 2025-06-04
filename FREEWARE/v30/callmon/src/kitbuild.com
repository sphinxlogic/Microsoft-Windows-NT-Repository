$!  CALLMON - A Call Monitor for OpenVMS Alpha
$!
$!  File:     KITBUILD.COM
$!  Author:   Thierry Lelegard
$!  Version:  1.0
$!  Date:     24-JUL-1996
$!
$!  Abstract: Build a zipped distribution kit and a DCL archive
$!            procedure for source files.
$!
$!
$!******************************************************************************
$!
$ version := 1.0
$ zipfile := [-]callmon_1_0.zip
$ dclfile := [-]callmon_1_0.com
$!
$!******************************************************************************
$!
$ text_files := -
 callmon.mms -
 callmon*.h -
 callmon*.c -
 callmon*.m64 -
 callmon*.msg -
 callmon*.com -
 callmon*.ada -
 callmon.ps -
 message_definition.c -
 example_*.ada -
 example_*.c -
 example_*.opt
$!
$ kit_files := -
 'text_files' -
 callmon.olb -
 message_definition.exe -
 example_*.exe
$!
$!******************************************************************************
$!
$!  Create the DCL archive procedure and write introduction sequence.
$!
$ on warning then goto exit
$!
$ dclname = f$parse(dclfile,,,"name") + f$parse(dclfile,,,"type")
$ text_files = f$edit(text_files,"trim,compress")
$ kit_files = f$edit(kit_files,"trim,compress")
$!
$ wrout := write sys$output
$ opdcl := open/append callmon$dclkit 'dclfile'
$ wrdcl := write callmon$dclkit
$ cldcl := close callmon$dclkit
$!
$ if f$trnlnm("callmon$dclkit").nes."" then close callmon$dclkit
$!
$ create/fdl=sys$input 'dclfile'
    file; organization sequential
    record; carriage_control carriage_return; format stream_lf
$!
$ opdcl
$ head = "CALLMON V''version' - A Call Monitor for OpenVMS Alpha"
$ wrdcl "$! ''head'"
$ wrdcl f$fao("$!! !#*-",f$length(head))
$ wrdcl "$!"
$ wrdcl "$! File ''dclname'"
$ wrdcl f$fao("$!! Automatically generated !%D",0)
$ wrdcl "$!"
$ wrdcl "$! Execute this command procedure to expand source and doc files"
$ wrdcl "$!"
$ wrdcl "$ ON WARNING THEN EXIT"
$ cldcl
$!
$!  Loop on all text files
$!
$ spec_count = 0
$ spec_loop:
$   spec = f$element(0,";",f$element(spec_count," ",text_files))
$   spec_count = spec_count + 1
$   if spec.eqs." " then goto end_spec_loop
$   previous_file = ""
$   file_loop:
$     file = f$element(0,";",f$search(spec))
$     if file.eqs."" .or. file.eqs.previous_file then goto end_file_loop
$     previous_file = file
$     name = f$parse(file,,,"name") + f$parse(file,,,"type")
$     ! Convert the text file into stream-lf format
$     if f$file(file,"rfm").nes."STMLF"
$     then
$       wrout "Converting ''file' to Strean-LF format..."
$       convert/fdl=sys$input: 'file' 'file'
            file; organization sequential
            record; carriage_control carriage_return; format stream_lf
$     endif
$     ! Append the file at the end of the DCL archive
$     wrout "Adding ''file' to DCL archive..."
$     opdcl
$     wrdcl "$!"
$     wrdcl f$fao("$!!!#*-",78)
$     wrdcl "$!"
$     wrdcl "$ CREATE ''name'"
$     wrdcl "$ DECK"
$     cldcl
$     append 'file' 'dclfile'
$     opdcl
$     wrdcl "$ EOD"
$     cldcl
$     goto file_loop
$   end_file_loop:
$   goto spec_loop
$ end_spec_loop:
$!
$!  Build the ZIP file.
$!
$ if f$search("''zipfile'").nes."" then delete/noconfirm/nolog 'zipfile';*
$ zip "-V" -9 'zipfile' 'kit_files'
$!
$!  Exit sequence
$!
$ exit:
$ if f$trnlnm("callmon$dclkit").nes."" then close callmon$dclkit
$ exit 1
