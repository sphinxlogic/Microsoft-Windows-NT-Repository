$ vf = f$verify(1)
$ if f$location("DEB", p1) .lt. f$length(p1)
$ then
$   compile_flags = "/debug/noopt"
$   link_flags    = "/debug"
$ else
$   compile_flags = ""
$   link_flags    = ""
$ endif
$ cc 'compile_flags' ranalyzer.c
$ cc 'compile_flags' listfile.c
$ cc 'compile_flags' objects.c
$ cc 'compile_flags' reports.c
$ cc 'compile_flags' rpthtml.c
$ cc 'compile_flags' rptsdml.c
$ cc 'compile_flags' rpttext.c
$ cc 'compile_flags' txtparse.c
$ cc 'compile_flags' dclparse.c
$ cc 'compile_flags' cparser.c
$ cc 'compile_flags' bliparse.c
$ cc 'compile_flags' list.c
$ cc 'compile_flags' cmdopt.c
$ cc 'compile_flags' objalloc.c
$ @link_ranalyzer "''link_flags'"
$ vf = f$verify(vf)
