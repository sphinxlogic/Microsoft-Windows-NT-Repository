BYACC, UTILITIES, Parser generator
Yacc reads the grammar specification in the file filename and generates an
LR(1) parser for it.  The parsers consist of a set of LALR(1) parsing tables
and a driver routine written in the C programming language.  Yacc normally
writes the parse tables and the driver routine to the file y_tab.c

This is a port of Berkeley yacc (BYACC) to OpenVMS.  The code has been changed
from K&R form to ANSI C, and generates ANSI C.

Author: Thomas E. Dickey (dickey@herndon4.his.com)
