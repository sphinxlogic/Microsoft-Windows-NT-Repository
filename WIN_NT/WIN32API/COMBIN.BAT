@echo off
echo Combining api32w.001 through api32w.010 to make api32wh.hlp
rem first make $$32.hlp, to avoid the command line length limitation.
if exist $$32.hlp goto error
copy/b api32w.001+api32w.002+api32w.003+api32w.004+api32w.005+api32w.006+api32w.007+api32w.008+api32w.009+api32w.010 $$32.hlp
ren $$32.hlp api32wh.hlp
goto done
:error
echo Error - $$32.hlp already exists.  combin.bat uses this as a
echo temporary file.  Please rename or edit combin.bat to use a 
echo different filename.
:done
