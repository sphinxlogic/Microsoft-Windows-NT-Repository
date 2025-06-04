$ in_verify = 'F$VERIFY(0)'
$!
$! This command file acts mostly as a reference for what can
$! be obtained from the F$GETSYI lexical function.  Whenever
$! I want to find out if I can get some bit of information on
$! the system I can invoke this command file and find out which
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
$ sym = F$GETSYI("HW_MODEL")
$ text = "            Hardware Model = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("HW_NAME")
$ text = "       Hardware Model Name = " + sym
$ WRITE outfil text
$!
$ sym = F$GETSYI("ARCHFLAG")
$ text = "        Architecture Flags = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("ACTIVECPU_CNT")
$ text = "          Active CPU Count = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("AVAILCPU_CNT")
$ text = "       Available CPU Count = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("BOOTTIME")
$ text = "                 Boot Time = " + sym
$ WRITE outfil text
$ sym = F$GETSYI("CONTIG_GBLPAGES")
$ text = " Free Contig. global pages = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("FREE_GBLPAGES")
$ text = "         Free Global pages = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("FREE_GBLSECTS")
$ text = "      Free Global sections = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("ERRORLOGBUFFERS")
$ text = "  System Pages errlog buff = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("CPU")
$ text = "                  CPU type = " + F$STRING(sym)
$ WRITE outfil text
$!
$ sym = F$GETSYI("CHARACTER_EMULATED")
$ text = " Character string emulated = " + sym
$ WRITE outfil text
$ sym = F$GETSYI("DECIMAL_EMULATED")
$ text = "   Decimal string emulated = " + sym
$ WRITE outfil text
$ sym = F$GETSYI("D_FLOAT_EMULATED")
$ text = "          D_Float emulated = " + sym
$ WRITE outfil text
$ sym = F$GETSYI("F_FLOAT_EMULATED")
$ text = "          F_Float emulated = " + sym
$ WRITE outfil text
$ sym = F$GETSYI("G_FLOAT_EMULATED")
$ text = "          G_Float emulated = " + sym
$ WRITE outfil text
$ sym = F$GETSYI("H_FLOAT_EMULATED")
$ text = "          H_Float emulated = " + sym
$ WRITE outfil text
$!
$ sym = F$GETSYI("PAGEFILE_FREE")
$ text = "            Page file free = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("PAGEFILE_PAGE")
$ text = "           Page file pages = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("SID")
$ text = "        System ID register = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("SWAPFILE_FREE")
$ text = "            Swap file free = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("SWAPFILE_PAGE")
$ text = "           Swap file pages = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("VERSION")
$ text = "               VMS version = " + sym
$ WRITE outfil text
$!
$ sym = F$GETSYI("CLUSTER_MEMBER")
$ text = "            Cluster member = " + sym
$ WRITE outfil text
$ IF sym .EQS. "FALSE" THEN $ GOTO not_cluster
$ sym = F$GETSYI("CLUSTER_FSYSID")
$ text = "      ID first node booted = " + sym
$ WRITE outfil text
$ sym = F$GETSYI("CLUSTER_FTIME")
$ text = "    Time first node booted = " + sym
$ WRITE outfil text
$ sym = F$GETSYI("CLUSTER_NODES")
$ text = "             Cluster nodes = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("CLUSTER_QUORUM")
$ text = "            Cluster Quorum = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("CLUSTER_VOTES")
$ text = "             Cluster votes = " + F$STRING(sym)
$ WRITE outfil text
$!
$ not_cluster:
$!
$ sym = F$GETSYI("NODENAME")
$ text = "                 Node Name = " + sym
$ WRITE outfil text
$ sym = F$GETSYI("NODE_AREA")
$ text = "               DECnet area = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("NODE_NUMBER")
$ text = "             DECnet number = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("NODE_CSID")
$ text = "              CSID of node = " + sym
$ WRITE outfil text
$ sym = F$GETSYI("NODE_HWTYPE")
$ text = "             Hardware type = " + sym
$ WRITE outfil text
$ sym = F$GETSYI("NODE_HWVERS")
$ text = "          Hardware version = " + sym
$ WRITE outfil text
$ sym = F$GETSYI("NODE_QUORUM")
$ text = "               Node quorum = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("NODE_SWINCARN")
$ text = "      Software incarnation = " + sym
$ WRITE outfil text
$ sym = F$GETSYI("NODE_SWTYPE")
$ text = "     Operating system type = " + sym
$ WRITE outfil text
$ sym = F$GETSYI("NODE_SWVERS")
$ text = "  Operating system version = " + sym
$ WRITE outfil text
$ sym = F$GETSYI("NODE_SYSTEMID")
$ text = "                 System ID = " + sym
$ WRITE outfil text
$ sym = F$GETSYI("NODE_VOTES")
$ text = "                Node votes = " + F$STRING(sym)
$ WRITE outfil text
$ sym = F$GETSYI("SCS_EXISTS")
$ text = "                SCS loaded = " + sym
$ WRITE outfil text
$!
$ CLOSE outfil
$!
$ EXIT 1 + 0*F$VERIFY(in_verify)
$!
$! B. Z. Lederman show all of the F$GETSYI lexical options
