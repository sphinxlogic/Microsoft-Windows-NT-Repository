#ifndef _CGI_MAILTO_H_
#define _CGI_MAILTO_H_

/*
**++
**  FACILITY:  {@tbs@}
**
**  MODULE DESCRIPTION:
**
**      {@tbs@}
**
**  AUTHORS:
**
**      {@tbs@}
**
**  CREATION DATE:  {@tbs@}
**
**  DESIGN ISSUES:
**
**      {@tbs@}
**
**  [@optional module tags@]...
**
**  MODIFICATION HISTORY:
**
**      {@tbs@}...
**--
*/

/*
** Replace the SMTP with the appropriate mail transport for your system, e.g., IN,
** MX, SMTP, etc.  If an address doesn't have an embedded "@" then the mail
** transport will NOT be used.
*/

#define TO_ADDRESS "SMTP%%\"%s\""

#endif
