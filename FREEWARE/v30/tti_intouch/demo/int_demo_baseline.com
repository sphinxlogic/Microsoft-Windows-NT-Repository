$! int_demo_baseline.com - INTOUCH Demonstration System
$!
$! command procedure to run a series of baseline programs capturing the 
$! process accounting information before and after each program runs
$!
$ envi = f$trnlnm("vms_environment")
$ if  envi .eqs. "AXP"
$ then
$   exe_type = ".EXE_AXP"
$ else
$   exe_type = ".EXE"
$ endif
$!
$ open/write results sys$scratch:int_demo_baseline_results.tmp
$ b1 = f$getjpi("", "bufio")
$ d1 = f$getjpi("", "dirio")
$ t1 = f$time()
$ c1 = f$getjpi("", "cputim")
$ $tti_run:intouch tti_int_demo:int_demo_b_baseline_intouch/source
$ c2 = f$getjpi("", "cputim")
$ t2 = f$time()
$ b2 = f$getjpi("", "bufio")
$ d2 = f$getjpi("", "dirio")
$ b = b2 - b1
$ d = d2 - d1
$ c = c2 - c1
$ h1 = f$cvtime(t1,,"hour") * 3600
$ m1 = f$cvtime(t1,,"minute") * 60
$ s1 = f$cvtime(t1,,"second")
$ h2 = f$cvtime(t2,,"hour") * 3600
$ m2 = f$cvtime(t2,,"minute") * 60
$ s2 = f$cvtime(t2,,"second")
$ e = (h2 + m2 + s2) - (h1 + m1 + s1)
$ write results "INTOUCH,NON,Bio=''b',DIO=''d',CPU=''c',ELAPSED=''e'"
$ b1 = f$getjpi("", "bufio")
$ d1 = f$getjpi("", "dirio")
$ t1 = f$time()
$ c1 = f$getjpi("", "cputim")
$ $tti_run:intouch tti_int_demo:int_demo_b_io_baseline_intouch/source
$ c2 = f$getjpi("", "cputim")
$ t2 = f$time()
$ b2 = f$getjpi("", "bufio")
$ d2 = f$getjpi("", "dirio")
$ b = b2 - b1
$ d = d2 - d1
$ c = c2 - c1
$ h1 = f$cvtime(t1,,"hour") * 3600
$ m1 = f$cvtime(t1,,"minute") * 60
$ s1 = f$cvtime(t1,,"second")
$ h2 = f$cvtime(t2,,"hour") * 3600
$ m2 = f$cvtime(t2,,"minute") * 60
$ s2 = f$cvtime(t2,,"second")
$ e = (h2 + m2 + s2) - (h1 + m1 + s1)
$ write results "INTOUCH,IO,Bio=''b',DIO=''d',CPU=''c',ELAPSED=''e'"
$ b1 = f$getjpi("", "bufio")
$ d1 = f$getjpi("", "dirio")
$ t1 = f$time()
$ c1 = f$getjpi("", "cputim")
$ run tti_int_demo:int_demo_b_baseline_basic'exe_type'
$ c2 = f$getjpi("", "cputim")
$ t2 = f$time()
$ b2 = f$getjpi("", "bufio")
$ d2 = f$getjpi("", "dirio")
$ b = b2 - b1
$ d = d2 - d1
$ c = c2 - c1
$ h1 = f$cvtime(t1,,"hour") * 3600
$ m1 = f$cvtime(t1,,"minute") * 60
$ s1 = f$cvtime(t1,,"second")
$ h2 = f$cvtime(t2,,"hour") * 3600
$ m2 = f$cvtime(t2,,"minute") * 60
$ s2 = f$cvtime(t2,,"second")
$ e = (h2 + m2 + s2) - (h1 + m1 + s1)
$ write results "BASIC,NON,Bio=''b',DIO=''d',CPU=''c',ELAPSED=''e'"
$ b1 = f$getjpi("", "bufio")
$ d1 = f$getjpi("", "dirio")
$ t1 = f$time()
$ c1 = f$getjpi("", "cputim")
$ run tti_int_demo:int_demo_b_io_baseline_basic'exe_type'
$ c2 = f$getjpi("", "cputim")
$ t2 = f$time()
$ b2 = f$getjpi("", "bufio")
$ d2 = f$getjpi("", "dirio")
$ b = b2 - b1
$ d = d2 - d1
$ c = c2 - c1
$ h1 = f$cvtime(t1,,"hour") * 3600
$ m1 = f$cvtime(t1,,"minute") * 60
$ s1 = f$cvtime(t1,,"second")
$ h2 = f$cvtime(t2,,"hour") * 3600
$ m2 = f$cvtime(t2,,"minute") * 60
$ s2 = f$cvtime(t2,,"second")
$ e = (h2 + m2 + s2) - (h1 + m1 + s1)
$ write results "BASIC,IO,Bio=''b',DIO=''d',CPU=''c',ELAPSED=''e'"
$ b1 = f$getjpi("", "bufio")
$ d1 = f$getjpi("", "dirio")
$ t1 = f$time()
$ c1 = f$getjpi("", "cputim")
$ run tti_int_demo:int_demo_b_baseline_fortran'exe_type'
$ c2 = f$getjpi("", "cputim")
$ t2 = f$time()
$ b2 = f$getjpi("", "bufio")
$ d2 = f$getjpi("", "dirio")
$ b = b2 - b1
$ d = d2 - d1
$ c = c2 - c1
$ h1 = f$cvtime(t1,,"hour") * 3600
$ m1 = f$cvtime(t1,,"minute") * 60
$ s1 = f$cvtime(t1,,"second")
$ h2 = f$cvtime(t2,,"hour") * 3600
$ m2 = f$cvtime(t2,,"minute") * 60
$ s2 = f$cvtime(t2,,"second")
$ e = (h2 + m2 + s2) - (h1 + m1 + s1)
$ write results "FORTRAN,NON,Bio=''b',DIO=''d',CPU=''c',ELAPSED=''e'"
$ b1 = f$getjpi("", "bufio")
$ d1 = f$getjpi("", "dirio")
$ t1 = f$time()
$ c1 = f$getjpi("", "cputim")
$ run tti_int_demo:int_demo_b_io_baseline_fortran'exe_type'
$ c2 = f$getjpi("", "cputim")
$ t2 = f$time()
$ b2 = f$getjpi("", "bufio")
$ d2 = f$getjpi("", "dirio")
$ b = b2 - b1
$ d = d2 - d1
$ c = c2 - c1
$ h1 = f$cvtime(t1,,"hour") * 3600
$ m1 = f$cvtime(t1,,"minute") * 60
$ s1 = f$cvtime(t1,,"second")
$ h2 = f$cvtime(t2,,"hour") * 3600
$ m2 = f$cvtime(t2,,"minute") * 60
$ s2 = f$cvtime(t2,,"second")
$ e = (h2 + m2 + s2) - (h1 + m1 + s1)
$ write results "FORTRAN,IO,Bio=''b',DIO=''d',CPU=''c',ELAPSED=''e'"
$ b1 = f$getjpi("", "bufio")
$ d1 = f$getjpi("", "dirio")
$ t1 = f$time()
$ c1 = f$getjpi("", "cputim")
$ run tti_int_demo:int_demo_b_baseline_cobol'exe_type'
$ c2 = f$getjpi("", "cputim")
$ t2 = f$time()
$ b2 = f$getjpi("", "bufio")
$ d2 = f$getjpi("", "dirio")
$ b = b2 - b1
$ d = d2 - d1
$ c = c2 - c1
$ h1 = f$cvtime(t1,,"hour") * 3600
$ m1 = f$cvtime(t1,,"minute") * 60
$ s1 = f$cvtime(t1,,"second")
$ h2 = f$cvtime(t2,,"hour") * 3600
$ m2 = f$cvtime(t2,,"minute") * 60
$ s2 = f$cvtime(t2,,"second")
$ e = (h2 + m2 + s2) - (h1 + m1 + s1)
$ write results "COBOL,NON,Bio=''b',DIO=''d',CPU=''c',ELAPSED=''e'"
$ b1 = f$getjpi("", "bufio")
$ d1 = f$getjpi("", "dirio")
$ t1 = f$time()
$ c1 = f$getjpi("", "cputim")
$ run tti_int_demo:int_demo_b_io_baseline_cobol'exe_type'
$ c2 = f$getjpi("", "cputim")
$ t2 = f$time()
$ b2 = f$getjpi("", "bufio")
$ d2 = f$getjpi("", "dirio")
$ b = b2 - b1
$ d = d2 - d1
$ c = c2 - c1
$ h1 = f$cvtime(t1,,"hour") * 3600
$ m1 = f$cvtime(t1,,"minute") * 60
$ s1 = f$cvtime(t1,,"second")
$ h2 = f$cvtime(t2,,"hour") * 3600
$ m2 = f$cvtime(t2,,"minute") * 60
$ s2 = f$cvtime(t2,,"second")
$ e = (h2 + m2 + s2) - (h1 + m1 + s1)
$ write results "COBOL,IO,Bio=''b',DIO=''d',CPU=''c',ELAPSED=''e'"
$ b1 = f$getjpi("", "bufio")
$ d1 = f$getjpi("", "dirio")
$ t1 = f$time()
$ c1 = f$getjpi("", "cputim")
$ run tti_int_demo:int_demo_b_baseline_c'exe_type'
$ c2 = f$getjpi("", "cputim")
$ t2 = f$time()
$ b2 = f$getjpi("", "bufio")
$ d2 = f$getjpi("", "dirio")
$ b = b2 - b1
$ d = d2 - d1
$ c = c2 - c1
$ h1 = f$cvtime(t1,,"hour") * 3600
$ m1 = f$cvtime(t1,,"minute") * 60
$ s1 = f$cvtime(t1,,"second")
$ h2 = f$cvtime(t2,,"hour") * 3600
$ m2 = f$cvtime(t2,,"minute") * 60
$ s2 = f$cvtime(t2,,"second")
$ e = (h2 + m2 + s2) - (h1 + m1 + s1)
$ write results "C,NON,Bio=''b',DIO=''d',CPU=''c',ELAPSED=''e'"
$ b1 = f$getjpi("", "bufio")
$ d1 = f$getjpi("", "dirio")
$ t1 = f$time()
$ c1 = f$getjpi("", "cputim")
$ run tti_int_demo:int_demo_b_io_baseline_c'exe_type'
$ c2 = f$getjpi("", "cputim")
$ t2 = f$time()
$ b2 = f$getjpi("", "bufio")
$ d2 = f$getjpi("", "dirio")
$ b = b2 - b1
$ d = d2 - d1
$ c = c2 - c1
$ h1 = f$cvtime(t1,,"hour") * 3600
$ m1 = f$cvtime(t1,,"minute") * 60
$ s1 = f$cvtime(t1,,"second")
$ h2 = f$cvtime(t2,,"hour") * 3600
$ m2 = f$cvtime(t2,,"minute") * 60
$ s2 = f$cvtime(t2,,"second")
$ e = (h2 + m2 + s2) - (h1 + m1 + s1)
$ write results "C,IO,Bio=''b',DIO=''d',CPU=''c',ELAPSED=''e'"
$ close results
$ exit
