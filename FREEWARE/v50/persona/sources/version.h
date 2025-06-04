/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\
 *                %% \___________________________________%% \
 *                %% |                                   %%  \
 *                %% |              PERSONA              %%   \
 *                %% |          version.h  c2001         %%    \
 *                %% |            Lyle W. West           %%    |
 *                %% |                                   %%    |
 *                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    |
 *                \                                        \   |
 *                 \                                        \  |
 *                  \                                        \ |
 *                   \________________________________________\|
 *
 *
 *
 *  Copyright (C) 2001 Lyle W. West, All Rights Reserved.
 *  Permission is granted to copy and use this program so long as [1] this
 *  copyright notice is preserved, and [2] no financial gain is involved
 *  in copying the program.  This program may not be sold as "shareware"
 *  or "public domain" software without the express, written permission
 *  of the author.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define VERSION "V1.1-8"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *    V1.1-8 17-Jan-2001 Lyle West - Added SYS$SETPRV early in code to assure
 *                       image has privs if not installed. 
 *
 *    V1.1-7 11-Jan-2001 Lyle West - Created common, platform independant
 *                       build proceures. 
 *
 *    V1.1-6 23-Nov-2000 Lyle West - Cleaned up some subtle errors in the
 *                       function CheckIdent, and enhanced related error
 *                       messages displayed to user (DecC 6.2, VMS 7.2-1).
 *
 *    V1.1-5 04-Oct-2000 Lyle West - Added logfile which is create/written
 *                       if logical PERSONA_LOG is defined. Entry and exit
 *                       info is written to file PERSONA_LOG points to.
 *
 *    V1.1-4 21-Oct-1999 Lyle West - Modified so created detached (persona)
 *                       process name is same as invoking pid which permits
 *                       multiple persona instances to same username.
 *
 *    V1.1-3 01-Apr-1999 Lyle West - Modified persona.cld to indicate user
 *                       can ignore identifier requirement if they enter
 *                       the system manager's password.
 *
 *    V1.1-2 22-Mar-1999 Lyle West - Modified source build procedures to
 *                       use common source base where possible. That is,
 *                       .MMS, .OPT, .COM are pre-suffixed as -VAX or -AXP
 *                        as applicable and images are .VAX_EXE or .AXP_EXE
 *
 *    V1.1-1 21-Mar-1999 Lyle West - added code to bypass identifer
 *                       requirment if user enters correct system manager
 *                       password
 *
 *    V1.0-9 15-MAR-1999 Lyle West - Initial Release, converted from 
 *                        Macro to Dec C (C - 5.6, VMS 7.1)
 *    
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
