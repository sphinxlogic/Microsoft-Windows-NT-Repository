$! This command file acts mostly as a reference for what can
$! be obtained from the F$ENVIRONMENT lexical function.  Whenever
$! I want to find out if I can get some bit of information
$! I can invoke this command file and find out which
$! item code (if any) will give me what I want.
$!
$! B. Z. Lederman	System Resources Corp. 	Burlington, MA
$!
$ in_verify = 'F$VERIFY(0)'
$!
$ IF P1 .EQS. ""
$  THEN
$    open/write outfil SYS$OUTPUT
$ ELSE
$    open/write outfil 'P1'
$ ENDIF
$!
$ sym = F$ENVIRONMENT("CAPTIVE")
$ text = "         Captive = " + sym
$ WRITE outfil text
$ sym = F$ENVIRONMENT("CONTROL")
$ text = "         Control = " + sym
$ WRITE outfil text
$ sym = F$ENVIRONMENT("DEFAULT")
$ text = "         Default = " + sym
$ WRITE outfil text
$ sym = F$ENVIRONMENT("DEPTH")
$ text = "           Depth = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$ENVIRONMENT("DISIMAGE")
$ text = "        Disimage = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$ENVIRONMENT("INTERACTIVE")
$ text = "     Interactive = " + sym
$ WRITE outfil text
$ sym = F$ENVIRONMENT("KEY_STATE")
$ text = "    Keypad State = " + sym
$ WRITE outfil text
$ sym = F$ENVIRONMENT("MAX_DEPTH")
$ text = "       Max Depth = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$ENVIRONMENT("MESSAGE")
$ text = "         Message = " + sym
$ WRITE outfil text
$ sym = F$ENVIRONMENT("NOCONTROL")
$ text = "      No Control = " + sym
$ WRITE outfil text
$ sym = F$ENVIRONMENT("ON_CONTROL_Y")
$ text = "    ON CONTROL_Y = " + sym
$ WRITE outfil text
$ sym = F$ENVIRONMENT("ON_SEVERITY")
$ text = "     ON SEVERITY = " + sym
$ WRITE outfil text
$ sym = F$ENVIRONMENT("OUTPUT_RATE")
$ text = "     Output Rate = " + sym
$ WRITE outfil text
$ sym = F$ENVIRONMENT("PROCEDURE")
$ text = "       Procedure = " + sym
$ WRITE outfil text
$ sym = F$ENVIRONMENT("PROMPT")
$ text = "          Prompt = " + sym
$ WRITE outfil text
$ sym = F$ENVIRONMENT("PROMPT_CONTROL")
$ text = "  Prompt Control = " + sym
$ WRITE outfil text
$ sym = F$ENVIRONMENT("PROTECTION")
$ text = "      Protection = " + sym
$ WRITE outfil text
$ sym = F$ENVIRONMENT("RESTRICTED")
$ text = "      Restricted = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$ENVIRONMENT("SYMBOL_SCOPE")
$ text = "    Symbol Scope = " + sym
$ WRITE outfil text
$ sym = F$ENVIRONMENT("VERB_SCOPE")
$ text = "      Verb Scope = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$ENVIRONMENT("VERIFY_IMAGE")
$ text = "    Verify Image = " + sym
$ WRITE outfil text
$ sym = F$ENVIRONMENT("VERIFY_PROCEDURE")
$ text = "Verify Procedure = " + sym
$ WRITE outfil text
$!
$ CLOSE outfil
$!
$ EXIT 1 + 0*F$VERIFY(in_verify)
$!
$! B. Z. Lederman show all of the F$ENVIRONMENT lexical options
$!
$!	19-Nov-1990	Add V5.4 DISIMAGE, RESTRICTED and VERB_SCOPE
