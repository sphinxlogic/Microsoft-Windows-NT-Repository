$ in_verify = F$VERIFY(0)
$!
$! This command file acts mostly as a reference for what can
$! be obtained from the F$GETDVI lexical function.  Whenever
$! I want to find out if I can get some bit of information on
$! a device I can invoke this command file and find out which
$! item code (if any) will give me what I want.
$!
$! Invoke with a device name as the first parameter.
$!
$! B. Z. Lederman	System Resources Corp. 	Burlington, MA
$!
$!  2-Jun-1992	New for VMS 5.5: DEVDEPEND4, TT_CHARSET, TT_CS_HANGUL,
$!		TT_CS_HANYU, TT_CS_HANZI, TT_CS_KANA, TT_CS_KANJI, TT_CS_THAI
$!
$ ON CONTROL_Y THEN GOTO com_exit
$!
$ IF P2 .EQS. ""
$  THEN
$    open/write outfil SYS$OUTPUT
$ ELSE
$    open/write outfil 'P2'
$ ENDIF
$!
$ WRITE outfil "                 P1 "+ P1
$ value = F$GETDVI (P1, "EXISTS")
$ WRITE outfil "      Device Exists "+ value
$ IF .NOT. value THEN $ GOTO com_exit
$!
$ value = F$GETDVI (P1, "ACPPID")
$ WRITE outfil "    ACP process PID "+ value
$ value = F$GETDVI (P1, "ACPTYPE")
$ WRITE outfil "           ACP type "+ value
$ value = F$GETDVI (P1, "ALL")
$ WRITE outfil "          Allocated "+ value
$ value = F$GETDVI (P1, "ALLDEVNAM")
$ WRITE outfil "  Alloc. Dev. Class "+ value
$ value = F$GETDVI (P1, "ALLOCLASS")
$ WRITE outfil "   Allocation Class "+ F$STRING (value)
$ value = F$GETDVI (P1, "ALT_HOST_AVAIL")
$ WRITE outfil "     ALT_HOST_AVAIL "+ value
$!
$ IF .NOT. value THEN $ GOTO no_alt_host
$!
$ value = F$GETDVI (P1, "ALT_HOST_NAME")
$ WRITE outfil "      ALT_HOST_NAME "+ value
$ value = F$GETDVI (P1, "ALT_HOST_TYPE")
$ WRITE outfil "      ALT_HOST_TYPE "+ value
$!
$ no_alt_host:
$!
$ value = F$GETDVI (P1, "AVL")
$ WRITE outfil "   Device Available "+ value
$ value = F$GETDVI (P1, "CCL")
$ WRITE outfil "   Carriage Control "+ value
$ value = F$GETDVI (P1, "CLUSTER")
$ WRITE outfil "       Cluster Size "+ F$STRING (value)
$ value = F$GETDVI (P1, "CONCEALED")
$ WRITE outfil "          CONCEALED "+ value
$ value = F$GETDVI (P1, "CYLINDERS")
$ WRITE outfil "          CYLINDERS "+ F$STRING (value)
$ value = F$GETDVI (P1, "DEVBUFSIZ")
$ WRITE outfil " Device Buffer Size "+ F$STRING (value)
$ value = F$GETDVI (P1, "DEVCHAR")
$ WRITE outfil "            DEVCHAR "+ F$FAO ("!XL", value)
$ value = F$GETDVI (P1, "DEVCHAR2")
$ WRITE outfil "           DEVCHAR2 "+ F$FAO ("!XL", value)
$ value = F$GETDVI (P1, "DEVCLASS")
$ WRITE outfil "       Device Class "+ F$STRING (value)
$ value = F$GETDVI (P1, "DEVDEPEND")
$ WRITE outfil "          DEVDEPEND "+ F$FAO ("!XL", value)
$ value = F$GETDVI (P1, "DEVDEPEND2")
$ WRITE outfil "         DEVDEPEND2 "+ F$FAO ("!XL", value)
$ value = F$GETDVI (P1, "DEVDEPEND4")
$ WRITE outfil "         DEVDEPEND4 "+ F$FAO ("!XL", value)
$!
$! lock name moved to the end
$!
$ value = F$GETDVI (P1, "DEVNAM")
$ WRITE outfil "        Device Name "+ value
$ value = F$GETDVI (P1, "DEVSTS")
$ WRITE outfil " Device Dep. Status "+ F$FAO ("!XL", value)
$ value = F$GETDVI (P1, "DEVTYPE")
$ WRITE outfil "        Device Type "+ F$STRING (value)
$ value = F$GETDVI (P1, "DIR")
$ WRITE outfil "  Directory Struct. "+ value
$ value = F$GETDVI (P1, "DISPLAY_DEVNAM")		! New for V5
$ WRITE outfil "Display device name "+ value
$ value = F$GETDVI (P1, "DMT")
$ WRITE outfil "  Marked for Dismnt "+ value
$ value = F$GETDVI (P1, "DUA")
$ WRITE outfil "  Generic Device DU "+ value
$ value = F$GETDVI (P1, "ELG")
$ WRITE outfil " Error Logging Ena. "+ value
$ value = F$GETDVI (P1, "ERRCNT")
$ WRITE outfil "        Error Count "+ F$STRING (value)
$ value = F$GETDVI (P1, "FOD")
$ WRITE outfil "     Files Oriented "+ value
$ value = F$GETDVI (P1, "FOR")
$ WRITE outfil "    Mounted Foreign "+ value
$ value = F$GETDVI (P1, "FREEBLOCKS")
$ WRITE outfil "        Free Blocks "+ F$STRING (value)
$ value = F$GETDVI (P1, "FULLDEVNAM")
$ WRITE outfil "   Full Device Name "+ value
$ value = F$GETDVI (P1, "GEN")
$ WRITE outfil "     Generic Device "+ value
$ value = F$GETDVI (P1, "HOST_AVAIL")
$ WRITE outfil "     Host available "+ value
$ value = F$GETDVI (P1, "HOST_COUNT")
$ WRITE outfil "         Host Count "+ F$STRING (value)
$ value = F$GETDVI (P1, "HOST_NAME")
$ WRITE outfil "          Host Name "+ value
$ value = F$GETDVI (P1, "HOST_TYPE")
$ WRITE outfil "          Host Type "+ value
$ value = F$GETDVI (P1, "IDV")
$ WRITE outfil " Dev provides Input "+ value
$ value = F$GETDVI (P1, "LOCKID")
$ WRITE outfil "            Lock ID "+ F$STRING (value)
$ value = F$GETDVI (P1, "LOGVOLNAM")
$ WRITE outfil "  Logical Vol. Name "+ value
$ value = F$GETDVI (P1, "MAXBLOCK")
$ WRITE outfil " Max Logical Blocks "+ F$STRING (value)
$ value = F$GETDVI (P1, "MAXFILES")
$ WRITE outfil "          Max Files "+ F$STRING (value)
$ value = F$GETDVI (P1, "MBX")
$ WRITE outfil "  Device is Mailbox "+ value
$ value = F$GETDVI (P1, "MEDIA_ID")
$ WRITE outfil "Nondecoded media ID "+ F$STRING (value)
$ value = F$GETDVI (P1, "MEDIA_NAME")
$ WRITE outfil "  Disk or Tape type "+ value
$ value = F$GETDVI (P1, "MEDIA_TYPE")
$ WRITE outfil " Device name prefix "+ value
$ value = F$GETDVI (P1, "MNT")
$ WRITE outfil "     Device Mounted "+ value
$ value = F$GETDVI (P1, "MOUNTCNT")
$ WRITE outfil "        Mount count "+ F$STRING (value)
$ value = F$GETDVI (P1, "NET")
$ WRITE outfil "     Network Device "+ value
$ value = F$GETDVI (P1, "NEXTDEVNAM")
$ WRITE outfil "   Next vol. in set "+ value
$ value = F$GETDVI (P1, "ODV")
$ WRITE outfil "Dev provides output "+ value
$ value = F$GETDVI (P1, "OPCNT")
$ WRITE outfil "    Operation Count "+ F$STRING (value)
$ value = F$GETDVI (P1, "OPR")
$ WRITE outfil " Device is Operator "+ value
$ value = F$GETDVI (P1, "OWNUIC")
$ WRITE outfil "       UIC of owner "+ value
$ value = F$GETDVI (P1, "PID")
$ WRITE outfil "       PID of owner "+ F$STRING (value)
$ value = F$GETDVI (P1, "RCK")
$ WRITE outfil " Read Check Enabled "+ value
$ value = F$GETDVI (P1, "RCT")
$ WRITE outfil "  Disk contains RCT "+ value
$ value = F$GETDVI (P1, "REC")
$ WRITE outfil "    Record Oriented "+ value
$ value = F$GETDVI (P1, "RECSIZ")
$ WRITE outfil "Blocked Record Size "+ F$STRING (value)
$ value = F$GETDVI (P1, "REFCNT")
$ WRITE outfil "    Reference Count "+ F$STRING (value)
$ value = F$GETDVI (P1, "REMOTE_DEVICE")
$ WRITE outfil "      Remote Device "+ value
$ value = F$GETDVI (P1, "RND")
$ WRITE outfil " Random Access Dev. "+ value
$ value = F$GETDVI (P1, "ROOTDEVNAM")
$ WRITE outfil " Root Volume in set "+ value
$ value = F$GETDVI (P1, "RTM")
$ WRITE outfil "   Real-time Device "+ value
$ value = F$GETDVI (P1, "SDI")
$ WRITE outfil "   Single Directory "+ value
$ value = F$GETDVI (P1, "SECTORS")
$ WRITE outfil "  Sectors per track "+ F$STRING (value)
$ value = F$GETDVI (P1, "SERIALNUM")
$ WRITE outfil " Volume Serial Nbr. "+ F$STRING (value)
$ value = F$GETDVI (P1, "SERVED_DEVICE")
$ WRITE outfil "      Served Device "+ value
$ value = F$GETDVI (P1, "SHDW_MEMBER")
$ WRITE outfil "  Shadow Set Member "+ value
$!
$ IF .NOT. value THEN $ GOTO not_shadow
$!
$ value = F$GETDVI (P1, "SHDW_CATCHUP_COPYING")
$ WRITE outfil "    CATCHUP_COPYING "+ value
$ value = F$GETDVI (P1, "SHDW_FAILED_MEMBER")
$ WRITE outfil "      FAILED_MEMBER "+ value
$ value = F$GETDVI (P1, "SHDW_MASTER")
$ WRITE outfil "        SHDW_MASTER "+ value
$ value = F$GETDVI (P1, "SHDW_MASTER_NAME")
$ WRITE outfil "        MASTER_NAME "+ value
$ value = F$GETDVI (P1, "SHDW_MERGE_COPYING")
$ WRITE outfil "      MERGE_COPYING "+ value
$ value = F$GETDVI (P1, "SHDW_NEXT_MBR_NAME")
$ WRITE outfil "      NEXT_MBR_NAME "+ value
$!
$ not_shadow:
$!
$ value = F$GETDVI (P1, "SHR")
$ WRITE outfil "          Shareable "+ value
$ value = F$GETDVI (P1, "SPL")
$ WRITE outfil "            Spooled "+ value
$ value = F$GETDVI (P1, "SPLDEVNAM")
$ WRITE outfil " Dev. being Spooled "+ value
$ value = F$GETDVI (P1, "SQD")
$ WRITE outfil "  Seq. Block (Tape) "+ value
$ value = F$GETDVI (P1, "STS")
$ WRITE outfil "             Status "+ F$FAO ("!XL", value)
$ value = F$GETDVI (P1, "SWL")
$ WRITE outfil " Sftware Wrt Locked "+ value
$ value = F$GETDVI (P1, "TRACKS")
$ WRITE outfil "             Tracks "+ F$STRING (value)
$ value = F$GETDVI (P1, "TRANSCNT")
$ WRITE outfil " Volume Transaction "+ F$STRING (value)
$ value = F$GETDVI (P1, "TRM")
$ WRITE outfil "           Terminal "+ value
$!
$ IF .NOT. value THEN $ GOTO not_terminal
$!
$ value = F$GETDVI (P1, "TT_ACCPORNAM")
$ WRITE outfil "       TT_ACCPORNAM "+ value
$ value = F$GETDVI (P1, "TT_ALTYPEAHD")
$ WRITE outfil "       TT_ALTYPEAHD "+ value
$ value = F$GETDVI (P1, "TT_ANSICRT")
$ WRITE outfil "         TT_ANSICRT "+ value
$ value = F$GETDVI (P1, "TT_APP_KEYPAD")
$ WRITE outfil "      TT_APP_KEYPAD "+ value
$ value = F$GETDVI (P1, "TT_AUTOBAUD")
$ WRITE outfil "        TT_AUTOBAUD "+ value
$ value = F$GETDVI (P1, "TT_AVO")
$ WRITE outfil "             TT_AVO "+ value
$ value = F$GETDVI (P1, "TT_BLOCK")
$ WRITE outfil "           TT_BLOCK "+ value
$ value = F$GETDVI (P1, "TT_BRDCSTMBX")
$ WRITE outfil "       TT_BRDCSTMBX "+ value
$ value = F$GETDVI (P1, "TT_CHARSET")
$ WRITE outfil "         TT_CHARSET "+ F$STRING (value)
$ value = F$GETDVI (P1, "TT_CS_HANGUL")
$ WRITE outfil "       TT_CS_HANGUL "+ value
$ value = F$GETDVI (P1, "TT_CS_HANYU")
$ WRITE outfil "        TT_CS_HANYU "+ value
$ value = F$GETDVI (P1, "TT_CS_HANZI")
$ WRITE outfil "        TT_CS_HANZI "+ value
$ value = F$GETDVI (P1, "TT_CS_KANA")
$ WRITE outfil "         TT_CS_KANA "+ value
$ value = F$GETDVI (P1, "TT_CS_KANJI")
$ WRITE outfil "        TT_CS_KANJI "+ value
$ value = F$GETDVI (P1, "TT_CS_THAI")
$ WRITE outfil "         TT_CS_THAI "+ value
$ value = F$GETDVI (P1, "TT_CRFILL")
$ WRITE outfil "          TT_CRFILL "+ value
$ value = F$GETDVI (P1, "TT_DECCRT")
$ WRITE outfil "          TT_DECCRT "+ value
$ value = F$GETDVI (P1, "TT_DIALUP")
$ WRITE outfil "          TT_DIALUP "+ value
$ value = F$GETDVI (P1, "TT_DISCONNECT")
$ WRITE outfil "      TT_DISCONNECT "+ value
$ value = F$GETDVI (P1, "TT_DMA")
$ WRITE outfil "             TT_DMA "+ value
$ value = F$GETDVI (P1, "TT_DRCS")
$ WRITE outfil "            TT_DRCS "+ value
$ value = F$GETDVI (P1, "TT_EDIT")
$ WRITE outfil "            TT_EDIT "+ value
$ value = F$GETDVI (P1, "TT_EDITING")
$ WRITE outfil "         TT_EDITING "+ value
$ value = F$GETDVI (P1, "TT_EIGHTBIT")
$ WRITE outfil "        TT_EIGHTBIT "+ value
$ value = F$GETDVI (P1, "TT_ESCAPE")
$ WRITE outfil "          TT_ESCAPE "+ value
$ value = F$GETDVI (P1, "TT_FALLBACK")
$ WRITE outfil "        TT_FALLBACK "+ value
$ value = F$GETDVI (P1, "TT_HALFDUP")
$ WRITE outfil "         TT_HALFDUP "+ value
$ value = F$GETDVI (P1, "TT_HANGUP")
$ WRITE outfil "          TT_HANGUP "+ value
$ value = F$GETDVI (P1, "TT_HOSTSYNC")
$ WRITE outfil "        TT_HOSTSYNC "+ value
$ value = F$GETDVI (P1, "TT_INSERT")
$ WRITE outfil "          TT_INSERT "+ value
$ value = F$GETDVI (P1, "TT_LFFILL")
$ WRITE outfil "          TT_LFFILL "+ value
$ value = F$GETDVI (P1, "TT_LOCALECHO")
$ WRITE outfil "       TT_LOCALECHO "+ value
$ value = F$GETDVI (P1, "TT_LOWER")
$ WRITE outfil "           TT_LOWER "+ value
$ value = F$GETDVI (P1, "TT_MBXDSABL")
$ WRITE outfil "        TT_MBXDSABL "+ value
$ value = F$GETDVI (P1, "TT_MECHFORM")
$ WRITE outfil "        TT_MECHFORM "+ value
$ value = F$GETDVI (P1, "TT_MECHTAB")
$ WRITE outfil "         TT_MECHTAB "+ value
$ value = F$GETDVI (P1, "TT_MODEM")
$ WRITE outfil "           TT_MODEM "+ value
$ value = F$GETDVI (P1, "TT_MODHANGUP")
$ WRITE outfil "       TT_MODHANGUP "+ value
$ value = F$GETDVI (P1, "TT_NOBRDCST")
$ WRITE outfil "        TT_NOBRDCST "+ value
$ value = F$GETDVI (P1, "TT_NOECHO")
$ WRITE outfil "          TT_NOECHO "+ value
$ value = F$GETDVI (P1, "TT_NOTYPEAHD")
$ WRITE outfil "       TT_NOTYPEAHD "+ value
$ value = F$GETDVI (P1, "TT_OPER")
$ WRITE outfil "            TT_OPER "+ value
$ value = F$GETDVI (P1, "TT_PAGE")
$ WRITE outfil "            TT_PAGE "+ F$STRING (value)
$ value = F$GETDVI (P1, "TT_PASTHRU")
$ WRITE outfil "         TT_PASTHRU "+ value
$ value = F$GETDVI (P1, "TT_PHYDEVNAM")
$ WRITE outfil "       TT_PHYDEVNAM "+ value
$ value = F$GETDVI (P1, "TT_PRINTER")
$ WRITE outfil "         TT_PRINTER "+ value
$ value = F$GETDVI (P1, "TT_READSYNC")
$ WRITE outfil "        TT_READSYNC "+ value
$ value = F$GETDVI (P1, "TT_REGIS")
$ WRITE outfil "           TT_REGIS "+ value
$ value = F$GETDVI (P1, "TT_REMOTE")
$ WRITE outfil "          TT_REMOTE "+ value
$ value = F$GETDVI (P1, "TT_SCOPE")
$ WRITE outfil "           TT_SCOPE "+ value
$ value = F$GETDVI (P1, "TT_SECURE")
$ WRITE outfil "          TT_SECURE "+ value
$ value = F$GETDVI (P1, "TT_SETSPEED")
$ WRITE outfil "        TT_SETSPEED "+ value
$ value = F$GETDVI (P1, "TT_SIXEL")
$ WRITE outfil "           TT_SIXEL "+ value
$ value = F$GETDVI (P1, "TT_TTSYNC")
$ WRITE outfil "          TT_TTSYNC "+ value
$ value = F$GETDVI (P1, "TT_WRAP")
$ WRITE outfil "            TT_WRAP "+ value
$!
$ not_terminal:
$!
$ value = F$GETDVI (P1, "UNIT")
$ WRITE outfil "        Unit Number "+ F$STRING (value)
$ value = F$GETDVI (P1, "VOLCOUNT")
$ WRITE outfil "     Volumes in Set "+ F$STRING (value)
$ value = F$GETDVI (P1, "VOLNAM")
$ WRITE outfil "        Volume Name "+ value
$ value = F$GETDVI (P1, "VOLNUMBER")
$ WRITE outfil "Nbr of Current Vol. "+ F$STRING (value)
$ value = F$GETDVI (P1, "VOLSETMEM")
$ WRITE outfil "Memb. of Volume Set "+ value
$ value = F$GETDVI (P1, "VPROT")
$ WRITE outfil "  Volume Protection "+ value
$ value = F$GETDVI (P1, "WCK")
$ WRITE outfil " Write Check Enable "+ value
$!
$ value = F$GETDVI (P1, "DEVLOCKNAM")
$ WRITE outfil "   Device Lock Name "+ value
$ str = ""
$ parse_loop:
$ length = F$LENGTH(value)
$ IF length .LE. 0 THEN GOTO no_more
$ hex = "%X"+F$EXTRACT(0, 2, value)
$ length = length - 2
$ value = F$EXTRACT(2, length, value)
$ b[0,8]='hex
$ IF b .LTS. " " THEN b = "."
$ IF b .GTS. "~" THEN b = "."
$ str = b+str
$ GOTO parse_loop
$!
$ no_more:
$!
$ WRITE outfil "                    "+ str
$!
$ com_exit:
$ CLOSE outfil
$ IF in_verify .EQ. 1 THEN SET VERIFY
$ EXIT
$!
$! B. Z. Lederman	Investigate and display all lexical device information
