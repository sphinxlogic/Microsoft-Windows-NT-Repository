$ in_verify = 'F$VERIFY (0)'
$!
$! This command file acts mostly as a reference for what can
$! be obtained from the F$GETJPI lexical function.  Whenever
$! I want to find out if I can get some bit of information on
$! a process I can invoke this command file and find out which
$! item code (if any) will give me what I want.
$!
$! B. Z. Lederman	System Resources Corp. 	Burlington, MA
$!
$ IF P1 .EQS. ""
$  THEN
$    open/write outfil SYS$OUTPUT
$ ELSE
$    open/write outfil 'P1'
$ ENDIF
$!
$ value = F$GETJPI ("", "ACCOUNT")
$ text = "                           Account = " + value
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "APTCNT")
$ text = "           Active Page Table Count = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "ASTACT")
$ text = "     Access modes with active ASTs = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "ASTCNT")
$ text = "                 Remaing AST quota = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "ASTEN")
$ text = "     Acess modes with ASTs enabled = " + F$FAO ("!XB", value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "ASTLM")
$ text = "                   AST limit quota = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "AUTHPRI")
$ text = "                  Maximum priority = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "AUTHPRIV")
$ text = "             Authorized Privileges = " + value
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "BIOCNT")
$ text = "      Remaining buffered I/O quota = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "BIOLM")
$ text = "          Buffered I/O limit quota = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "BUFIO")
$ text = "           Buffered I/O operations = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "BYTCNT")
$ text = "Remaining buffered I/O count quota = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "BYTLM")
$ text = "     Buffered I/O byte count limit = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "CLASSIFICATION")
$ text = "                    Classification = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "CLINAME")
$ text = "                               CLI = " + value
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "CPULIM")
$ text = "                 Limit on CPU time = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "CPUTIM")
$ text = "                     CPU time used = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "CPU_ID")
$ text = "                            CPU ID = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "CREPRC_FLAGS")
$ text = "                      CREPRC_FLAGS = " + F$FAO ("!XL", value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "CURPRIV")
$ text = "                Current Privileges = " + value
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "CURRENT_AFFINITY_MASK")
$ text = "             CURRENT_AFFINITY_MASK = " + F$FAO ("!XL", value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "CURRENT_USERCAP_MASK")
$ text = "              CURRENT_USERCAP_MASK = " + F$FAO ("!XL", value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "DFPFC")
$ text = "        Default page fault cluster = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "DFWSCNT")
$ text = "          Default working set size = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "DIOCNT")
$ text = "        Remaining direct I/O quota = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "DIOLM")
$ text = "            Direct I/O limit quota = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "DIRIO")
$ text = "                       Direct I/Os = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "EFCS")
$ text = "                Event flags 0 - 31 = " + F$FAO ("!XL", value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "EFCU")
$ text = "               Event flags 32 - 63 = " + F$FAO ("!XL", value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "EFWM")
$ text = "              Event flag wait mask = " + F$FAO ("!XL", value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "ENQCNT")
$ text = "      Lock request quota remaining = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "ENQLM")
$ text = "          Lock request quota limit = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "EXCVEC")
$ text = "  Address of exception vector list = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "FAST_VP_SWITCH")
$ text = "                    FAST_VP_SWITCH = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "FILCNT")
$ text = "           Remaing open file quota = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "FILLM")
$ text = "                   Open file quota = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "FINALEXC")
$ text = "   Address of final except. vector = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "FREP0VA")
$ text = "      First free page at end of P0 = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "FREP1VA")
$ text = "      First free page at end of P1 = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "FREPTECNT")
$ text = "    Pages avail for virt mem expan = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "GPGCNT")
$ text = "  Global page count in working set = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "GRP")
$ text = "                       Group (UIC) = " + F$FAO ("!OW", value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "IMAGECOUNT")
$ text = "                   Images run down = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "IMAGNAME")
$ text = "                Current image name = " + value
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "IMAGPRIV")
$ text = "      Privileges for current image = " + value
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "IMAGE_RIGHTS")
$ text = "                      IMAGE_RIGHTS = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "JOBPRCCNT")
$ text = "      Number of subprocesses owned = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "JOBTYPE")
$ text = "                           JOBTYPE = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "KT_COUNT")
$ text = "                          KT_COUNT = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "LAST_LOGIN_I")
$ text = "                      LAST_LOGIN_I = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "LAST_LOGIN_N")
$ text = "                      LAST_LOGIN_N = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "LOGINTIM")
$ text = "     Process creation (login) time = " + value
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "LOGIN_FAILURES")
$ text = "                    LOGIN_FAILURES = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "LOGIN_FLAGS")
$ text = "                       LOGIN_FLAGS = " + F$FAO ("!XL", value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "MASTER_PID")
$ text = "                PID at top of tree = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "MAXDETACH")
$ text = "                         MAXDETACH = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "MAXJOBS")
$ text = "                           MAXJOBS = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "MEM")
$ text = "                      Member (UIC) = " + F$FAO ("!OW", value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "MODE")
$ text = "                              Mode = " + value
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "MSGMASK")
$ text = "              Default message mask = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "MULTITHREAD")
$ text = "                       MULTITHREAD = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "NODENAME")
$ text = "                          NODENAME = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "NODE_CSID")
$ text = "                         NODE_CSID = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "NODE_VERSION")
$ text = "                      NODE_VERSION = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "OWNER")
$ text = "                      PID of owner = " + value
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "P0_FIRST_FREE_VA_64")
$ text = "               P0_FIRST_FREE_VA_64 = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "P1_FIRST_FREE_VA_64")
$ text = "               P1_FIRST_FREE_VA_64 = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "P2_FIRST_FREE_VA_64")
$ text = "               P2_FIRST_FREE_VA_64 = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "PAGEFLTS")
$ text = "                       Page faults = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "PAGFILCNT")
$ text = "         Remaining page file quota = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "PAGFILLOC")
$ text = "                Page file location = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "PERMANENT_AFFINITY_MASK")
$ text = "           PERMANENT_AFFINITY_MASK = " + F$FAO ("!XL", value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "PERMANENT_USERCAP_MASK")
$ text = "            PERMANENT_USERCAP_MASK = " + F$FAO ("!XL", value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "PGFLQUOTA")
$ text = "                   Page file quota = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "PHDFLAGS")
$ text = "                        Flags word = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "PID")
$ text = "                        Process ID = " + value
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "PPGCNT")
$ text = "                Process page count = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "PRCCNT")
$ text = "                  Subprocess count = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "PRCLM")
$ text = "                  Subprocess quota = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "PRCNAM")
$ text = "                      Process name = " + value
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "PRI")
$ text = "                  Current priority = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "PRIB")
$ text = "                     Base priority = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "PROCPRIV")
$ text = "                Default privileges = " + value
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "PROCESS_RIGHTS")
$ text = "                    PROCESS_RIGHTS = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "PROC_INDEX")
$ text = "                        PROC_INDEX = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "RIGHTSLIST")
$ text = "                        RIGHTSLIST = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "RIGHTS_SIZE")
$ text = "                       RIGHTS_SIZE = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "RMS_DFMBC")
$ text = "                         RMS_DFMBC = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "RMS_DFMBFIDX")
$ text = "                      RMS_DFMBFIDX = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "RMS_DFMBFREL")
$ text = "                      RMS_DFMBFREL = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "RMS_DFMBFSDK")
$ text = "                      RMS_DFMBFSDK = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "RMS_DFMBFSMT")
$ text = "                      RMS_DFMBFSMT = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "RMS_DFMBFSUR")
$ text = "                      RMS_DFMBFSUR = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "RMS_DFNBC")
$ text = "                         RMS_DFNBC = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "RMS_EXTEND_SIZE")
$ text = "                   RMS_EXTEND_SIZE = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "RMS_FILEPROT")
$ text = "                      RMS_FILEPROT = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "RMS_PROLOGUE")
$ text = "                      RMS_PROLOGUE = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "SCHED_POLICY")
$ text = "                      SCHED_POLICY = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "SHRFILLM")
$ text = "                          SHRFILLM = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "SITESPEC")
$ text = "            Site specific longword = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "SLOW_VP_SWITCH")
$ text = "                    SLOW_VP_SWITCH = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "STATE")
$ text = "                     Process state = " + value
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "STS")
$ text = "                      Status flags = " + F$FAO ("!XL", value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "STS2")
$ text = "                              STS2 = " + F$FAO ("!XL", value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "SWPFILLOC")
$ text = "                Swap file location = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "SYSTEM_RIGHTS")
$ text = "                     SYSTEM_RIGHTS = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "TABLENAME")
$ text = "                         TABLENAME = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "TERMINAL")
$ text = "               Login terminal name = " + value
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "THREAD_INDEX")
$ text = "                      THREAD_INDEX = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "TMBU")
$ text = "          Termination mailbox unit = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "TQCNT")
$ text = " Remaining timer queue entry quota = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "TQLM")
$ text = "           Timer queue entry quota = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "TT_ACCPORNAM")
$ text = "                      TT_ACCPORNAM = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "TT_PHYDEVNAM")
$ text = "                      TT_PHYDEVNAM = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "UAF_FLAGS")
$ text = "                         UAF_FLAGS = " + F$FAO ("!XL", value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "UIC")
$ text = "                               UIC = " + value
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "USERNAME")
$ text = "                         User Name = " + value
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "VIRTPEAK")
$ text = "         Peak virtual address size = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "VOLUMES")
$ text = "         Currently mounted volumes = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "VP_CONSUMER")
$ text = "                       VP_CONSUMER = " + value
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "VP_CPUTIM")
$ text = "                         VP_CPUTIM = " + F$STRING (value)
$ WRITE outfil text + " ****"
$!
$ value = F$GETJPI ("", "WSAUTH")
$ text = "   Max authorized working set size = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "WSAUTHEXT")
$ text = " Max authorized working set extent = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "WSEXTENT")
$ text = "   Current working set size extent = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "WSPEAK")
$ text = "                  Working set peak = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "WSQUOTA")
$ text = "            Working set size quota = " + F$STRING (value)
$ WRITE outfil text
$!
$ value = F$GETJPI ("", "WSSIZE")
$ text = "          Current working set size = " + F$STRING (value)
$ WRITE outfil text
$!
$ CLOSE outfil
$!
$ EXIT 1 + 0*F$VERIFY (in_verify)
$!
$! B. Z. Lederman show all of the F$GETJPI lexical options
