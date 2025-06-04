$ on error then goto exit
$ on control_y then goto exit
$ set on
$ @make_bin_zip
$ @make_src_zip
$!
$ exit:
