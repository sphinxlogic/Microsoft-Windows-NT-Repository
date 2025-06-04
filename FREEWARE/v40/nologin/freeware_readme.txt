NOLOGIN, SYSTEM_MANAGEMENT, Display users not logged in for x days

Utility tools to display SYSUAF info about inactive users. Output
is readily alterable by using built-in qualifiers.

Define a foreign symbol to point to NOLOGIN.xxx_EXE file. Note that
this is not an option; it will not work with the RUN command.

Example:
$   ZZZ == "$device:[dir1.dir2]nologin.vax_exe"
$   zzz /help

The images in this directory were built with V7.1, and DecC 5.6
With a small amount of tweaking, this will build on 5.5-2 & VaxC.
Your mileage may vary.

Lyle W. West

