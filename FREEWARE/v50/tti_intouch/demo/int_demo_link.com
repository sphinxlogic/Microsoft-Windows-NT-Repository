$! INT_DEMO_LINK.COM - Link all .OBJ modules
$
$ write sys$output "    Linking INTOUCH_DEMO 3GL benchmark modules..."
$!
$ if  f$getsyi("HW_MODEL") .gt. 1024
$ then
$   environ  = "AXP"
$   exe_type = ".EXE_AXP"
$ else
$   environ  = "VAX"
$   exe_type = ".EXE"
$ endif
$!
$ lib_file = "INT_DEMO_''environ'.OLB"
$!
$ link/exe=int_demo_b_baseline_basic'exe_type' -
    'lib_file'/inc=int_demo_b_baseline_basic$main
$ if  environ .eqs. "VAX"  then  define/user lnk$library sys$share:vaxcrtl
$ link/exe=int_demo_b_baseline_c'exe_type' -
    'lib_file'/inc=int_demo_b_baseline_c
$ link/exe=int_demo_b_baseline_cobol'exe_type' -
    'lib_file'/inc=prog1
$ link/exe=int_demo_b_baseline_fortran'exe_type' -
    'lib_file'/inc=int_demo_b_baseline_fortra$main
$ link/exe=int_demo_b_io_baseline_basic'exe_type' -
    'lib_file'/inc=int_demo_b_io_baseline_bas$main
$ if  environ .eqs. "VAX"  then  define/user lnk$library sys$share:vaxcrtl
$ link/exe=int_demo_b_io_baseline_c'exe_type' -
    'lib_file'/inc=int_demo_b_io_baseline_c
$ link/exe=int_demo_b_io_baseline_cobol'exe_type' -
    'lib_file'/inc=prog2
$ link/exe=int_demo_b_io_baseline_fortran'exe_type' -
    'lib_file'/inc=for2
$ link/exe=int_demo_b_read_basic'exe_type' -
    'lib_file'/inc=int_demo_b_read_basic$main
$ link/exe=int_demo_b_read_cobol'exe_type' -
    'lib_file'/inc=prog5
$ link/exe=int_demo_b_read_fortran'exe_type' -
    'lib_file'/inc=for5
$ link/exe=int_demo_b_redim_basic'exe_type' -
    'lib_file'/inc=int_demo_b_redim_basic$main
$ link/exe=int_demo_b_screen_io_basic'exe_type' -
    'lib_file'/inc=int_demo_b_screen_io_basic$main
$ if  environ .eqs. "VAX"  then  define/user lnk$library sys$share:vaxcrtl
$ link/exe=int_demo_b_screen_io_c'exe_type' -
    'lib_file'/inc=int_demo_b_screen_io_c
$ link/exe=int_demo_b_screen_io_cobol'exe_type' -
    'lib_file'/inc=prog4
$ link/exe=int_demo_b_screen_io_fortran'exe_type' -
    'lib_file'/inc=int_demo_b_screen_io_fortran
$ link/exe=int_demo_b_update_basic'exe_type' -
    'lib_file'/inc=int_demo_b_update_basic$main
$ link/exe=int_demo_b_update_cobol'exe_type' -
    'lib_file'/inc=prog6
$ link/exe=int_demo_b_write_basic'exe_type' -
    'lib_file'/inc=int_demo_b_write_basic$main
$ link/exe=int_demo_b_write_cobol'exe_type' -
    'lib_file'/inc=prog3
$ link/exe=int_demo_b_write_fortran'exe_type' -
    'lib_file'/inc=int_demo_b_write_fortran
$!
$ exit
