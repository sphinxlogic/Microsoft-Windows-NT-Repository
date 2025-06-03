$! int_demo_benchmark.com - INTOUCH Demonstration System
$!
$! command procedure to run a benchmark program capturing the process accounting
$! information before and after the program runs
$!
$! Expects:
$!      p1 = benchmark procedure to run
$!      p2 = language
$!
$ envi = f$trnlnm("vms_environment")
$ if  envi .eqs. "AXP"
$ then
$   exe_type = ".EXE_AXP"
$ else
$   exe_type = ".EXE"
$ endif
$!
$ b1 = f$getjpi("", "bufio")
$ d1 = f$getjpi("", "dirio")
$ t1 = f$time()
$ c1 = f$getjpi("", "cputim")
$ if  p2 .eqs. "INTOUCH" then goto intouch
$ run 'p1''exe_type'
$ goto done
$ intouch:
$ $tti_run:intouch 'p1'/source
$ done:
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
$ open/write results sys$scratch:int_demo_benchmark_results.tmp 
$ write results "Bio=''b',DIO=''d',CPU=''c',ELAPSED=''e'"
$ close results
$ exit
