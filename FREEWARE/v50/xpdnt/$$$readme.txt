;++
; Copyright © 1993 - 1998  by Brian Schenkenberger and TMESIS.
; ALL RIGHTS RESERVED.
;                            Notice of Disclaimer
;                         -------------------------
; 
; This Software is provided "AS IS" and is supplied for informational purpose
; only.  No warranty is expressed or implied and no liability can be accepted
; for any direct, indirect or consequential damages or for any damage whatso-
; ever resulting in the loss of systems, data or profit from the use of this
; software or from any of the information contained herein.  The author makes
; no claims as to the suitablility or fitness of this Software or information
; contain herein for any particular purpose.  
;
;                            Title and Ownership
;                         -------------------------
; NO TITLE TO AND/OR OWNERSHIP OF THIS SOFTWARE IS HEREBY TRANSFERRED.  ANY
; MODIFICATION WITHOUT THE PRIOR WRITTEN CONSENT OF THE COPYRIGHT HOLDER IS
; PROHIBITED.  ANY USE, IN WHOLE OR PART, OF THIS SOFTWARE FOR A COMMERCIAL
; PRODUCT WITHOUT THE PRIOR WRITTEN CONSENT OF THE COPYRIGHT HOLDER IS ALSO
; PROHIBITED.  THE TECHNIQUES EMPLOYED IN THE SOFTWARE ARE THE INTELLECTUAL
; PROPERTY OF THE COPYRIGHT HOLDER.  THIS SOFTWARE CONTAINS "TRADE SECRETS"
; AND PROPRIETARY SOFTWARE TECHNIQUES.  REVERSE ENGINEERING OF THE SOFTWARE
; IS STRICTLY PROHIBITED.
;--
eXcellent
eXpedient Process Default Notification Tool
eXtensive

TMESIS xpdnt v1.1

This product offers a convenient mechanism for displaying the current pro-
cess default on OpenVMS systems.  It is shipped with three supporting stub
programs which will allow the process default to be displayed in:

  -- the DCL prompt (32 characters max.  Last 32 characters is longer.)
  -- the 25th "status line" of terminals supporting host writable status 
  -- the "Title Bar" of DECterm windows.

The procedure INSTALL_XPDNT.COM, found in the installation direcotry you
specify at installation time, must be invoked before xpdnt will function.
This procedure should be called during your system startup.

After installing xpdnt, the XPDNT command must be defined.  The ideal loc-
ation for the XPDNT command is in a user's LOGIN.COM file.  This allows a
user to elect to use XPDNT and, if elected, which display method.  The com-
mand itself can be defined in the SYLOGIN.COM or LOGIN.COM file as a for-
eign command:

$ XPDNT == "$TMESIS$XPDNT:XPDNT.EXE"

or as a DCL command:

$ SET COMMAND TMESIS$XPDNT:XPDNT

Once the command has been defined, XPDNT can be used to install the process
default display stub of choice.

To display the process default in the DCL prompt, use the following:

$ XPDNT/DCL_PROMPT

To display the process default in the 25th line, use the following:

$ XPDNT/STATUS_LINE

To display the process default in a DECterm title bar, use the following:

$ XPDNT/DECTERM_TITLE


xpdnt can only be invoked once per login session.

Once xpdnt has been installed, moving about on the system via $ SET DEFAULT
will reflect your default.  xpdnt can be used in conjunction with any other
procedures used for changing default on the system.

xpdnt also provides a "navigation" function.  After xpdnt had been used to
load the display stub, a <control><N> sequence at the DCL prompt will cause
the navigator to be launched.  The navigator is a SMG style menu-like tool
which will allow the user to easily move about the system based upon prior
defaults.

----------------------------------------------------------------------------

