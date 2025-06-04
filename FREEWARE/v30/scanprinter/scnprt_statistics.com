$!**********************************************************************
$!                    SCNPRT_STATISTICS.COM        V2.6
$!**********************************************************************
$!
$! On settings
$ on error then goto error
$ on warning then goto error
$ on control_c then goto ctrl_c
$!
$! scan_table=f$trnlnm("SCNPRT_TABLE","LNM$SYSTEM_DIRECTORY",,,,"TABLE")
$!
$ say         := write sys$output
$ tell        := write sys$error
$ wcomm       := write sys$command
$ time 	       = f$time()
$ curnode = f$getsyi("nodename")        ! 3 lines for getting node name !!!
$ if curnode .eqs. "" then curnode = f$edit(f$getsyi("scsnode"),"collapse")
$ if curnode .eqs. "" then curnode = f$trnlnm("sys$node") - "::" - "-"
$ clusternode  = f$trnlnm("sys$cluster_node") -"::"  ! Getting cluster name
$ if clusternode .eqs. "" then clusternode=curnode
$ scan_version ="V2.6"
$ use_existing ="NO"
$!
$! Defining logical tests for testing opened files
$ stat_open :== -
   f$trnlnm("""temp_stat""",,,,,"""TABLE_NAME""").eqs."""LNM$PROCESS_TABLE"""
$!
$ say " SCNPRT_STATISTICS - ''scan_version' - Starting at ''time' on node ''curnode'"
$!
$! Check to see difference in version between SCNPRT on this node and this procedure
$!
$ temp = f$trnlnm("SCNPRT$VERSION","SCNPRT_TABLE")
$ temp = f$edit(temp,"TRIM,COLLAPSE,UPCASE")
$ if temp.nes.scan_version
$ then
$  if temp.nes."" 
$  then
$    req_message= -
     "%SCANPRT-W-VERMIS, SCAN PRINTERS version ''temp' and SCNPRT_STATISTICS version ''scan_version' mismatch"
$    tell "''req_message'"
$  endif
$ endif
$!
$ say ""
$!
$! Defining F$FAO strings parameters
$!
$ fao_pbstat= "%SCNPRTSTAT-E-!AS, Error !AS file !AS"
$ fao_stat  = "This queue was found !AS!_ !4UL time!0%C!1%C!%Es!%F"
$ fao_pbtime= "%SCNPRTSTAT-E-INVTIM, Invalid !AS time format !AS"
$!
$! Testing P1 parameter
$!
$ p1=f$edit(P1,"trim,collapse,upcase") 
$ if p1.eqs."" 
$ then 
$ 	scan_nocluster = f$trnlnm("SCNPRT$IGNORE_CLUSTER","SCNPRT_TABLE")
$ 	scan_nocluster = f$edit(f$extract(0,1,scan_nocluster),-
			 "UPCASE,TRIM,COLLAPSE")
$       scn_n_wild=0
$ 	if scan_nocluster.eqs."" then scan_nocluster = "Y"
$       if scan_nocluster.eqs."Y"
$	then 
$		scn_node=curnode
$	else
$		scn_node=clusternode
$	endif
$ else 
$       p1=f$extrcat(0,6,p1)
$	if f$extract(f$length(P1)-1,1,p1).eqs."*" 
$ 	then
$		scn_n_wild=1
$       	scn_node=f$extract(0,f$length(P1)-1,P1)
$               if scn_node.eqs."" then scn_node=" "
$ 	else
$       	scn_n_wild=0
$       	scn_node=p1
$ 	endif
$ endif
$ if p1.eqs."?" then goto help
$!
$! Testing P2 parameter
$!
$ p2=f$edit(P2,"trim,collapse,upcase") 
$ if f$extract(f$length(P2)-1,1,p2).eqs."*" .or. p2.eqs.""
$ then
$	scn_q_wild=1
$       scn_queue=f$extract(0,f$length(P2)-1,P2)
$ else
$       scn_q_wild=0
$       scn_queue=p2
$ endif
$!
$! Testing P3 parameter
$!
$ p3=f$edit(P3,"trim,collapse,upcase") 
$ if p3.nes.""
$ then
$   on error then continue
$   define/user sys$error nl:
$   define/user sys$output nl:
$   from_date = f$cvtime("''p3'","comparison")
$   status = $status
$   on error then goto error
$   deassign/user sys$output
$   deassign/user sys$error
$   if .not. status 
$   then 
$      tell f$fao(fao_pbtime,"beginning",P3)
$      goto end
$   endif
$!   from_length=f$length(p3)-1
$!   if from_length.gt.16 then from_length=16
$!   from_date = f$extract(0,from_length,from_date)
$ else
$!   from_length = 16
$   from_date = ""
$ endif
$ from_date = f$fao("!16AS",from_date)
$!
$! Testing P4 parameter
$!
$ p4=f$edit(P4,"trim,collapse,upcase") 
$ if p4.nes.""
$ then
$   on error then continue
$   define/user sys$error nl:
$   define/user sys$output nl:
$   to_date = f$cvtime("''p4'","comparison")
$   status = $status
$   on error then goto error
$   deassign/user sys$output
$   deassign/user sys$error
$   if .not. status 
$   then 
$      tell f$fao(fao_pbtime,"ending",P4)
$      goto end
$   endif
$!   to_length=f$length(p4)-1
$!   if to_length.gt.16 then to_length=16
$!   to_date = f$extract(0,to_length,to_date)
$ else
$!   to_length=16
$   to_date=""
$ endif
$ to_date = f$fao("!16AS",to_date)
$!
$! Display request
$!
$ if p2.eqs."" .or. p2.eqs."*" 
$ then
$	display_queue="any queues"
$ else
$       if scn_q_wild 
$	then
$        display_queue="queues " + p2
$       else
$        display_queue="queue " + scn_queue
$       endif
$ endif
$ if P1.eqs."*"
$ then
$	display_node="all nodes"
$ else
$	if scn_n_wild
$	then
$	 display_node="nodes " + p1
$	else
$        display_node="node " + scn_node
$	endif
$ endif
$ say ""
$ say "		Searching statistics for ''display_queue' 
$ say "		from ''display_node'"
$ range = ""
$ if p3.nes."" then range=range + "from " + from_date
$ if p3.nes."" .and. p4.nes."" then range = range + " and "
$ if p4.nes. ""  then range= range + "before " + to_date
$ say "		''f$edit(range,"trim")'"
$ say ""
$!
$! Initializing work directory and Statistics file name from logicals
$!
$ scan_statistics = f$trnlnm("SCNPRT$STATISTICS_FILE","SCNPRT_TABLE")
$ scan_wrkdir = f$trnlnm("SCNPRT$WORK_DIRECTORY","SCNPRT_TABLE")
$!
$! Test that stat. file exists
$!
$    scan_statistics=f$parse(scan_statistics,scan_wrkdir,-
                     "SYS$SCRATCH:SCAN_PRINTERS.STAT",,"SYNTAX_ONLY")
$    if f$search("''scan_statistics'").eqs."" 
$    then
$!
$! Stat. file does not exist
$!
$      req_message= f$fao(fao_pbstat,"FINDERR","finding",scan_statistics)
$      tell "''req_message'"
$      goto end
$    else
$!
$! Stat. file exist 
$!
$      scan_statistics_tmp=f$parse(scan_statistics,"SCAN_PRINTERS",-
                           ,"NAME","SYNTAX_ONLY")
$      scan_statistics_tmp=f$parse(scan_statistics_tmp,scan_wrkdir,-
                     "SYS$SCRATCH:SCAN_PRINTERS.WRK_''f$getjpi("","pid")'",,-
                     "SYNTAX_ONLY")
$!
$! A temporary file already exist, ask for re-using it !
$!
$      if f$search(scan_statistics_tmp).nes."" then gosub use_it
$      if use_existing.eqs."Y" .or. use_existing.eqs."O" .or. -
          use_existing.eqs."" then goto open_file
$!
$! Convert stat. file sorted by node and by date
$!
$      on error then goto conv_err
$      on warning then goto conv_err
$      convert/fdl=sys$input 'scan_statistics' 'scan_statistics_tmp'
TITLE	"SCNPRT_STATISTICS - V2.5 - Work file description - 02/1992"

SYSTEM
	SOURCE			"VAX/VMS"

FILE
	ORGANIZATION		indexed

RECORD
	CARRIAGE_CONTROL	carriage_return
	FORMAT			variable
	SIZE			255

AREA 0
	ALLOCATION		21
	BEST_TRY_CONTIGUOUS	yes
	BUCKET_SIZE		3
	EXTENSION		3

AREA 1
	ALLOCATION		3
	BEST_TRY_CONTIGUOUS	yes
	BUCKET_SIZE		3
	EXTENSION		3

AREA 2
	ALLOCATION		78
	BEST_TRY_CONTIGUOUS	yes
	BUCKET_SIZE		3
	EXTENSION		21

KEY 0
	CHANGES			no
	DATA_AREA		0
	DATA_FILL		100
	DATA_KEY_COMPRESSION	yes
	DATA_RECORD_COMPRESSION	yes
	DUPLICATES		yes
	INDEX_AREA		1
	INDEX_COMPRESSION	yes
	INDEX_FILL		100
	LEVEL1_INDEX_AREA	1
	NAME			"Node + Date"
	PROLOG			3
	SEG0_LENGTH		7
	SEG0_POSITION		17
	SEG1_LENGTH		16
	SEG1_POSITION		0
	TYPE			string

KEY 1
	CHANGES			no
	DATA_AREA		2
	DATA_FILL		100
	DATA_KEY_COMPRESSION	yes
	DUPLICATES		yes
	INDEX_AREA		2
	INDEX_COMPRESSION	yes
	INDEX_FILL		100
	LEVEL1_INDEX_AREA	2
	NAME			"Date + Node"
	SEG0_LENGTH		24
	SEG0_POSITION		0
	TYPE			string

$    endif
$!
$! Opening work file
$!
$open_file:
$    on error then goto error
$    on warning then goto error
$    open/share=read/error=stat_open_err temp_stat 'scan_statistics_tmp'
$!
$! Initializing parameters before reading loop
$!
$    scan_first_date="9999-DEC-31 23:59"
$    scan_last_date=""
$    nb_queue  = 0
$    nb_r_read = 0
$    nb_r_match= 0
$    nb_r_stat = 0
$!
$    if P3 .nes. "" .and. scn_n_wild    	! From date not null, wild node
$    then
$       index=1					! Key 1 (Date + Node)
$    	key=from_date + "/" + scn_node
$       if f$length(key).eq.23 .or. .not. scn_n_wild then key=key+"/"
$       
$    else			     		! From date null or node specified
$	index=0					! Key 0 (Node + Date)
$	key=scn_node
$	if f$length(key).eq.6 .or. .not. scn_n_wild then key=key+"/"
$    endif
$!
$! First keyed read on sorted file
$!
$    read/error=end_stat_file/end=end_stat_file/index='index'/key="''key'" -
         /match=ge temp_stat scan_enreg
$    nb_r_read=nb_r_read+1				! count read records
$!
$! Reading sorted file loop
$!
$read_loop:
$ !sh sym scan_enreg
$    scan_node=f$element(1,"/",scan_enreg)		! Recording node
$    scan_queue=f$element(2,"/",scan_enreg)             ! Queue name
$    scan_state=f$element(5,"/",scan_enreg)             ! Queue state
$    if scan_state.eqs."STAT" 
$    then 
$    	nb_r_stat=nb_r_stat+1				! Count but
$	goto read_record				! ignore Stat. record
$    endif
$    if scn_n_wild .and. p1.nes."*"
$    then						! Wild node but not *
$	if scn_node.nes.f$extract(0,f$length(scn_node),scan_node)
$	then 						! Not the right node
$		if index.eq.1 then goto read_record     ! Date search
$               if index.eq.0 then goto end_stat_file   ! Node search
$	endif
$    else
$	if scan_node.nes.scn_node .and. p1.nes."*"	! Not wild node
$       then 
$		if index.eq.1 then goto read_record     ! Date search
$		if index.eq.0 then goto end_stat_file   ! Node search
$	endif
$    endif
$    scan_date=f$element(0,"/",scan_enreg)              ! Recording date
$    if scan_date.lts.from_date .and. -
        p3.nes."" then goto read_record                 ! Before required date
$    if scan_date.gts.to_date .and. -
        P4.nes."" then goto read_record                 ! After required date
$    if scn_q_wild
$    then						! Wild queue
$	if scn_queue.nes.f$extract(0,f$length(scn_queue),scan_queue) .and. -
           scn_queue.nes."" then goto read_record	! Not right queue
$    else                                               ! Not wild queue
$       if scan_queue.nes.scn_queue then goto read_record
$    endif
$!
$!	Record can be processed
$!
$    nb_r_match = nb_r_match + 1                        ! Count matching records
$    scan_device=f$element(4,"/",scan_enreg)                  ! Full device
$    if f$element(3,"/",scan_enreg).nes."" then -             ! name for
     scan_device=f$element(3,"/",scan_enreg)+"::"+scan_device ! the queue
$   if f$locate("@",scan_device) .lt. f$length(scan_device) then gosub trans_aro
$!
$    if f$type('scan_queue'_'scan_state').eqs."" then - 
    'scan_queue'_'scan_state' = 0                       ! First time for this
$!                                                      ! queue state
$    'scan_queue'_'scan_state'='scan_queue'_'scan_state'+1 ! inc. queue state
$!
$    if f$type('scan_queue'_device).eqs."" 
$    then                                               ! First time for
$ 	'scan_queue'_device = scan_device		! that queue
$    else
$       if f$locate(scan_device,'scan_queue'_device).eqs. -
         f$length('scan_queue'_device) then -           ! Another device for
         'scan_queue'_device='scan_queue'_device+","+scan_device ! this queue
$    endif
$!
$    if f$type('scan_queue'_stat).eqs.""             
$    then 
$    	'scan_queue'_stat = 1                           ! First time for queue
$       nb_queue=nb_queue+1                             ! then inc. nb of queues
$       queue_name_'nb_queue'=scan_queue                ! and store queue name
$    endif
$!
$! Store begining and ending recording date
$!
$    if scan_date.lts.scan_first_date then scan_first_date=scan_date
$    if scan_date.gts.scan_last_date then scan_last_date=scan_date
$!
$read_record:
$    read/error=read_stat_err/end=end_stat_file temp_stat scan_enreg
$    nb_r_read=nb_r_read+1				! Count read records
$    goto read_loop
$!
$end_stat_file:
$!
$! Now displaying collected informations for this node and queues
$!
$    if nb_queue.gt.0 
$    then 			! We have some informations to display
$       say ""
$    	say "Statistics from ''display_node', between ''scan_first_date' and ''scan_last_date'"
$    	say ""
$       say "                      Records that have been read : ''nb_r_read'"
$       say "                      Number of STAT records read : ''nb_r_stat'"
$       say "                      Records matching criteria   : ''nb_r_match'"
$       say "                      
$      i=0                      ! Initializing queue counter
$stat_loop:
$       i=i+1                   ! Incrementing queue counter
$       if i .gt. nb_queue then goto end	! There's no more queue
$       scan_queue=queue_name_'i'               ! Getting queue name
$       scan_device='scan_queue'_device         ! Getting device name list
$    	say ""
$    	say "Statistics for queue ''scan_queue' on ''scan_device'"
$    	say ""
$    	if f$type('scan_queue'_stalled).eqs."" then 'scan_queue'_stalled = 0
$    	say f$fao(fao_stat,"Stalled",'scan_queue'_stalled)
$    	if f$type('scan_queue'_stopped).eqs."" then 'scan_queue'_stopped = 0
$    	say f$fao(fao_stat,"Stopped",'scan_queue'_stopped)
$    	if f$type('scan_queue'_paused).eqs."" then 'scan_queue'_paused = 0
$    	say f$fao(fao_stat,"Paused",'scan_queue'_paused)
$    	if f$type('scan_queue'_holding).eqs."" then 'scan_queue'_holding = 0
$    	say f$fao(fao_stat,"Holding",'scan_queue'_holding)
$       goto stat_loop
$    else			! We have no information to display
$    	say "No Statistic found for ''display_queue' from ''display_node'"
$       say "                      Records that have been read : ''nb_r_read'"
$    endif
$    say ""
$    goto end
$!
$use_it:
$ if f$getjpi("","mode").eqs."INTERACTIVE" .and. -
     f$getdvi("sys$command","trm") .and. -
     f$file(scan_statistics_tmp,"MRS").eq.255 .and. -    	
     f$file(scan_statistics_tmp,"NOK").eq.2 .and. -    	
     f$file(scan_statistics_tmp,"ORG").eqs."IDX" .and. -    	
     f$file(scan_statistics_tmp,"RAT").eqs."CR" .and. -    	
     f$file(scan_statistics_tmp,"RFM").eqs."VAR" 
$ then
$       wcomm "  It seems to have an existing version of our work file"
$       wcomm "  that was created at ''f$file(scan_statistics_tmp,"CDT")' with "+ -
            "owner ''f$file(scan_statistics_tmp,"uic")'"
$       wcomm "     (Last revision of statistics file ''f$file(scan_statistics,"RDT")')"
$ 	read- 
        /prompt="Do you want to use existing work file (Y/N) ? [Y] " -
	/end=end/error=end -
	sys$command use_existing
	use_existing=f$edit(f$extract(0,1,use_existing),-
                     "collapse,uncomment,upcase")
$ endif
$ return
$!
$end:
$   on error then continue
$   on control_c then continue
$   if 'stat_open' then close temp_stat		! Closing sorted file
$   if f$type(scan_statistics_tmp).nes.""
$   then
$   	scan_statistics_tmp=scan_statistics_tmp-";"
$   	if f$search(scan_statistics_tmp).nes."" then purge 'scan_statistics_tmp'
$   endif
$ exit
$!
$stat_open_err:
$      req_message= f$fao(fao_pbstat,"OPENERR","opening",scan_statistics_tmp)
$      tell "''req_message'"
$      goto error
$!
$read_stat_err:
$      req_message= f$fao(fao_pbstat,"READERR","reading",scan_statistics_tmp)
$      tell "''req_message'"
$      goto error
$!
$conv_err:
$      req_message= f$fao(fao_pbstat,"SORTERR","sorting",scan_statistics)
$      tell "''req_message'"
$      goto error
$!
$error:
$   tell -
    "%SCNPRTSTAT-F-STOPERR Procedure stopped on error"
$   goto end
$!
$ctrl_c:
$   tell -
    "%SCNPRTSTAT-F-INTERRUPT Procedure interrupted by [Ctrl] C"
$   goto end
$help:
$	type sys$input

		SCAN PRINTERS STATISTICS    V2.5

  Usage : @SCNPRT_STATISTICS <node> <queue> <from_date> <to_date>

	node      :  Node name from which we need statistics, default current 
                     node or current cluster name if SCNPRT$IGNORE_CLUSTER is NO
                     wildcard * in place or at end of node name possible
	queue     :  Queue name from which we need statistics, default all 
                     queues for <node>, wildcard * in place or at end of queue
                     name possible.
        from_date :  Beginning date in absolute format (DD-MMM-YYYY:HH:MM).
        to_date   :  Ending date in absolute format.

$ goto end
$!
$TRANS_ARO:
$TRANS_ARO_LOOP:
$ aro=f$locate("@",scan_device)
$ if aro.lt.f$length(scan_device)
$ then 
$ 	scan_device[aro*8,8]=%XE0			! à character
$ 	goto trans_aro_loop
$ endif
$ return
$!
$!
$!**********************************************************************
$! Creation      : H. MERCUSOT  - 10/91         V2.4  - Cap Sesa Exploitation
$!
$!		Cette procédure permet d'afficher des statistiques
$!              à partir du fichier de stat. de SCAN_PRINTERS V2.4
$!
$!	Syntaxe : @SCNPRT_STATISTICS <node> <queue> 
$!
$! Modifications : 
$!                 H. MERCUSOT  - 02/92        V2.5   - Cap Sesa Exploitation
$!
$!	Syntaxe : @SCNPRT_STATISTICS <node> <queue> <from_date> <to_date>
$!
$!               Changement de version de SCAN_PRINTERS.
$!               Modification dans la recherche du nom de n÷ud courant.
$!               Par défaut, le n÷ud de recherche est le n÷ud courant ou le
$!                 cluster courant suivant la valeur de SCNPRT$IGNORE_CLUSTER
$!               Changement dans la création du fichier de travail. Créé en
$!                 séquentiel indéxé avec clé primaire = N÷ud + Date et
$!                 clé secondaire = Date + N÷ud
$!               Possibilité de réutiliser un fichier de travail déjà existant
$!                 en mode interactif.
$!               Purge du fichier de travail en fin de procédure.
$!               Possibilité de terminer <queue> et <node> par * (wildcard)
$!               Adjonction d'une fourchette temporelle de recherche.
$!
$! Modifications : 
$!                 H. MERCUSOT  - 08/92        V2.6   - Cap Sesa Exploitation
$!
$!		Changement de version SCAN_PRINTERS.
$!		Changement des @ par à dans le nom du device (scan_device).
$!**********************************************************************
