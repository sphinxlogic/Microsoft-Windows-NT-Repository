$ set noon
$ say = "write sys$output"
$ osc[0,8] = 157
$ st[0,8]  = 156
$ icon_name = f$edit(P1,"compress,trim")
$ if (icon_name .eqs. "") then icon_name = "Win''f$getdvi("tt","unit")"
$ window_name = f$edit(P2,"compress,trim")
$ if (window_name .eqs. "") then window_name = icon_name
$ say "''osc'2L;''icon_name'''st'''osc'21;''window_name'''st'"
$ exitt 1
