;;; File:    check.awk
;;; Author:  Julian Smart
;;; Date:    24th April 1994
;;; Purpose: AWK script to flag some common errors in input to Tex2RTF.
;;;          Users are welcome to add to this -- please mail me your changes!
;;;          J.Smart@ed.ac.uk
;;; Usage:   You need AWK (e.g. GNU AWK). Sources may be ftp'ed from many sites.
;;;          A DOS executable is included with Tex2RTF. Use the batch script
;;;          tcheck.bat, or awk -f check.awk filename.tex

; Initialisation
BEGIN { IGNORECASE = 1;
        FS = " ";
        INTABULAR = 0;
      }

; Check for missing document style
/documentstyle\[.*\]$/{
       print "Error at line " NR ": missing major document style. Add e.g. {report}";
     }

; Labels should immediately follow section headings or captions
/^\\label/{
       print "Error at line " NR ": label should DIRECTLY follow section or caption.";
     }

; Common bracket confusion
/\{\\[a-zA-Z]*{/{
       print "Error at line " NR ": braces will confuse Tex2RTF. Try a space after the command.";
     }

; Middle of line
/[^\{]\\bf[^A-Za-z\{]/{
       print "Error at line " NR ": no brace before or after \\bf.";
     }
; Start of line
/^\\bf[^A-Za-z\{]/{
       print "Error at line " NR ": no brace before or after \\bf.";
     }
; On its own
/^\\bf$/{
       print "Error at line " NR ": no brace before or after \\bf.";
     }

; Middle of line
/[^\{]\\it[A-Za-z^\{]/{
       print "Error at line " NR ": no brace before or after \\it.";
     }
; Start of line
/^\\it[^A-Za-z\{]/{
       print "Error at line " NR ": no brace before or after \\it.";
     }
; On its own
/^\\it$/{
       print "Error at line " NR ": no brace before or after \\it.";
     }

; Middle of line
/[^\{]\\rm[^A-Za-z\{]/{
       print "Error at line " NR ": no brace before or after \\rm.";
     }
; Start of line
/^\\rm[^A-Za-z\{]/{
       print "Error at line " NR ": no brace before or after \\rm.";
     }
; On its own
/^\\rm$/{
       print "Error at line " NR ": no brace before or after \\rm.";
     }

; Middle of line
/[^\{]\\small[^A-Za-z\{]/{
       print "Error at line " NR ": no brace before or after \\small.";
     }
; Start of line
/^\\small[^A-Za-z\{]/{
       print "Error at line " NR ": no brace before or after \\small.";
     }
; On line of its own
/^\\small$/{
       print "Error at line " NR ": no brace before or after \\small.";
     }

; Middle of line
/[^\{]\\large[^A-Za-z\{]/{
       print "Error at line " NR ": no brace before or after \\large.";
     }
; Start of line
/^\\large[^A-Za-z\{]/{
       print "Error at line " NR ": no brace before or after \\large.";
     }
; On its own
/^\\large$/{
       print "Error at line " NR ": no brace before or after \\large.";
     }

; Middle of line
/[^\{]\\Large[^A-Za-z\{]/{
       print "Error at line " NR ": no brace before or after \\Large.";
     }
; Start of line
/^\\Large[^A-Za-z\{]/{
       print "Error at line " NR ": no brace before or after \\Large.";
     }
; On its own
/^\\Large$/{
       print "Error at line " NR ": no brace before or after \\Large.";
     }

; Middle of line
/[^\{]\\LARGE[^A-Za-z\{]/{
       print "Error at line " NR ": no brace before or after \\LARGE.";
     }
; Start of line
/^\\LARGE[^A-Za-z\{]/{
       print "Error at line " NR ": no brace before or after \\LARGE.";
     }
; On its own
/^\\LARGE$/{
       print "Error at line " NR ": no brace before or after \\LARGE.";
     }

; Middle of line
/[^\{]\\normal[^A-Za-z\{]/{
       print "Error at line " NR ": no brace before or after \\normal.";
     }
; Start of line
/^\\normal[^A-Za-z\{]/{
       print "Error at line " NR ": no brace before or after \\normal.";
     }
; On its own
/^\\normal$/{
       print "Error at line " NR ": no brace before or after \\normal.";
     }

; Opening brace at start of line
/^\{[^\\]/ {
       print "Warning at line " NR ": possible split command. If so, remove intervening newline.";
     }

; Start tabular environment
/begin{tabular}/{
       INTABULAR = 1;
     }
; End tabular environment
/end{tabular}/{
       INTABULAR = 0;
     }
; Check that we don't have \\ on its own within tabular environment.
; It will crash Tex2RTF.
/^\\\\/ {
     if (INTABULAR == 1)
     {
       print "Error at line " NR ": insert appropriate number of ampersands before end of row marker.";
     }
  }
