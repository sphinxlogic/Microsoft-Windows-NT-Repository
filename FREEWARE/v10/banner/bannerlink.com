$	if "''banner$debug'" .eqs. "" then banner$debug := "/NODEB/NOTRACE"
$	link'banner$debug'/exe=VMI$KWD:DECW$banner.exe VMI$KWD:BANNER.OPT /options 
