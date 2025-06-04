$ say := write sys$output
$ if f$search("SYS$SYSTEM:BLISS32.EXE") .eqs. "" then goto _Assemble_Em
$ say "Compiling BAT and HG$GET_INPUT..."
$ bliss bat,hg$get_input
$ goto _Continue
$ _Assemble_Em:
$ say "Assembling BAT and HG$GET_INPUT..."
$ macro bat,hg$get_input
$ _Continue:
$ say "Compiling BAT_MSG..."
$ message bat_msg
$ say "Compiling BAT_CLD..."
$ set comm/obj bat_cld
$ say "Linking BAT..."
$ link/notrace bat,hg$get_input,bat_msg,bat_cld
$ say "Building help file (.HLP)..."
$ runoff bat.rnh
$ say "Build completed"
$ exit
