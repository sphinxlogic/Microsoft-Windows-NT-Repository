/*
 * Name - checkvm.h
 *
 * Version:	1.1
 *
 * ccsid:	@(#)checkvm.h	1.1 - 7/6/92 10:53:31
 * from: 	ccs/s.checkvm.h
 * date: 	8/12/92 14:15:37
 *
 * description:  a routine to check resource availability before attempting
 *               to display a stand-alone image
 */


char *CheckVM[] = {
  "/checkvm { %% bytesNeeded checkvm -",
  "  /needed exch store",
  "  /used  0 store",
  "  /max   0 store",
  "  vmstatus /max exch store /used exch store pop",
  "  /avail max used sub store  %% amount of vm available",
  "  avail needed lt {",
  "    /str 10 string store",
  "    gsave",
  "    0 inch 8.0 inch translate",
  "    1.0 1.0 scale",
  "    /Courier findfont 18 scalefont setfont",
  "    20 20 moveto",
  "    (There is not enough printer memory for this image.) show",
  "    20 -5 moveto",
  "    needed avail sub 10 str cvrs show",
  "    ( more bytes are needed.) show",
  "    showpage",
  "    grestore",
  "  } if",
  "} def",
  0
  };


