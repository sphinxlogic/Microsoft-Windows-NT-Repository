$!
$! The extra level of indirection is provided so that if we provide
$! code that may not be changed by the user it resides here. Since
$! The values of the majordomo logicals defined in MAJORDOMO_SYSTARTUP
$! are configurable they belong in a systartup type file.
$ if f$search("SYS$STARTUP:UCX$MAJORDOMO_SYSTARTUP.COM") .nes. "" then -
        @SYS$STARTUP:UCX$MAJORDOMO_SYSTARTUP
