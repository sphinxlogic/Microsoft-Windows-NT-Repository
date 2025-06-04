/*
** CLIDEFS.H
**
**  Definitions for use with the CLI$ utility routines.
**
**  COPYRIGHT © 1992, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
*/

#ifdef __GNUC__
extern int cli$_absent();
#define CLI$_ABSENT ((unsigned int) cli$_absent)
extern int cli$_present();
#define CLI$_PRESENT ((unsigned int) cli$_present)
extern int cli$_negated();
#define CLI$_NEGATED ((unsigned int) cli$_negated)
extern int cli$_defaulted();
#define CLI$_DEFAULTED ((unsigned int) cli$_defaulted)
#else
#pragma nostandard
globalvalue unsigned int CLI$_ABSENT, CLI$_PRESENT, CLI$_NEGATED, CLI$_DEFAULTED;
#pragma standard
#endif

extern unsigned int cli$present(), cli$get_value(), cli$dcl_parse();
