      function isystem (ip1, isstring)
c*
c*  Evaluate the SYSTEM function item codes.  Use GETSYI.
c*
      parameter (nums=368)
      common /syidefs/ i_syi(nums), c_syi(nums), t_syi(nums)
      character *18 c_syi, sname
      integer*2 t_syi, i_syi
      character *64 temp
      logical isstring
      integer itmlst(4)
      integer*2 items(8)
      equivalence (items(1),itmlst(1))
c
      logical match, ambig
c
c --- find the getsyi item code based on the Kronos keyword
c
      call strcpy (sname, ip1)
      call search (c_syi, nums, sname, k, match, ambig)
      if (.not. match  .or.  ambig) then
         call error (3, 'Illegal system item code.', 0)
         return
      endif
c
c --- build item list for GETSYI
c
      items(2) = i_syi(k)
      itmlst(3) = %loc(ltemp)
      if (t_syi(k) .eq. 0) then
         items(1) = 4
         itmlst(2) = %loc (isystem)
         isstring = .false.
      else
         items(1) = t_syi(k)
         itmlst(2) = %loc (temp)
         isstring = .true.
      endif
      istat = sys$getsyiw (,,, itmlst,,,)
c
c --- if the return value is a string, save it to the heap.  If its an
c ---  integer, its already in isystem
c
      if (isstring) isystem = isave (temp(1:ltemp), .true.)
      return
      end
c
c---end isystem
c
      function idevice (ip1, ip2, isstring)
c*
c*   Evaluate the function item codes using GETDVI.
c*
      parameter (numd=150)
      common /dvidefs/ i_dvi(numd), c_dvi(numd), t_dvi(numd)
      integer *2 i_dvi, t_dvi
      character *16 c_dvi, dname
      character *80 temp
      logical match, ambig, isstring
c
      integer itmlst(4)
      integer*2 items(8)
      equivalence (items(1),itmlst(1))
c
c
c --- find the getdvi item code based on the Kronos keyword
c
      call strcpy (dname, ip2)
      call search (c_dvi, numd, dname, k, match, ambig)
      if (.not. match  .or.  ambig) then
         call error (3, 'Illegal device item code.', 0)
         return
      endif
c
      items(2) = i_dvi(k)
      itmlst(3) = %loc (ltemp)
      if (t_dvi(k) .eq. 0) then
         items(1) = 4
         itmlst(2) = %loc (idevice)
         isstring = .false.
      else
         items(1) = t_dvi(k)
         itmlst(2) = %loc (temp)
         isstring = .true.
      endif
      call strcpy (dname, ip1)
      istat = sys$getdviw (,,dname(1:length(dname)), itmlst,,,,)
c
c --- if the return value is a string, save it to the heap.  If its an
c ---  integer, its already in idevice
c
      if (isstring) idevice = isave (temp(1:ltemp), .true.)
      return
      end
c
c---end idevice
c
      function iprocess (ip1, ip2, isstring)
c*
c*  Determine whether or not a process exists or a user is active
c*   note: user code doesn't check to make sure its interactive
c*
      include '($JPIDEF)'
      include '($SSDEF)'
      logical isstring
      character *15 p1, p2
      integer itmlst(4), sys$getjpiw
      integer*2 items(8), iosb(4)
      equivalence (items(1),itmlst(1))
c
      isstring = .false.
      call strcpy (p1, ip1)      ! process name or user name
      call strcpy (p2, ip2)      ! "PROCESS" or "USER"
      itmlst(2) = %loc(p2)
      itmlst(3) = 0
      itmlst(4) = 0
      ipid = -1
c
      if (p2(1:4) .eq. 'USER') then
         items(1) = 12
         items(2) = jpi$_username
      else
         items(1) = 15
         items(2) = jpi$_prcnam
      endif
c
c --- check all current processes for this one
c
10    istat = sys$getjpiw (,ipid,,itmlst,iosb,,)
      if (istat .eq. ss$_normal) then
         call caps(p2)
         if (p2(1:length(p2)) .eq. p1(1:length(p1))) then
            iprocess = 1
            return
         endif
      endif
      if (istat .ne. ss$_nomoreproc) go to 10
c
c --- fallthrough means process not found
c
      iprocess = 0 
      return
      end
c
c---end iprocess
c
      function iprint (ip, isstring)
      logical isstring
c
      iprint = 0
      return
      end
c
c---end iprint
c
      function isecurity (ip, isstring)
      logical isstring
c
      isecurity = 0
      return
      end
c
c---end isecurity
c
      block data dvi_data
      parameter (numd=150)
      common /dvidefs/ i_dvi(numd), c_dvi(numd), t_dvi(numd)
      character *16 c_dvi
      integer *2 i_dvi, t_dvi
c
c --- text name for the valid device item codes
c
      data c_dvi /
     $ 'ACPPID',          'ACPTYPE',         'ALL',
     $ 'ALLDEVNAM',       'ALLOCLASS',       'ALT_HOST_AVAIL',
     $ 'ALT_HOST_NAME',   'ALT_HOST_TYPE',   'AVL',
     $ 'CCL',             'CLUSTER',         'CONCEALED',
     $ 'CYLINDERS',       'DEVBUFSIZ',       'DEVCHAR',
     $ 'DEVCHAR2',        'DEVCLASS',        'DEVDEPEND',
     $ 'DEVDEPEND2',      'DEVDEPEND3',      'DEVLOCKNAM',
     $ 'DEVNAM',          'DEVSTS',          'DEVTYPE',
     $ 'DFS_ACCESS',      'DIR',             'DISPLAY_DEVNAM',
     $ 'DMT',             'DUA',             'ELG',
     $ 'ERRCNT',          'FOD',             'FOR',
     $ 'FREEBLOCKS',      'FULLDEVNAM',      'GEN',
     $ 'HOST_AVAIL',      'HOST_COUNT',      'HOST_NAME',
     $ 'HOST_TYPE',       'IDV',             'LOCKID',
     $ 'LOGVOLNAM',       'MAXBLOCK',        'MAXFILES',
     $ 'MBX',             'MEDIA_ID',        'MEDIA_NAME',
     $ 'MEDIA_TYPE',      'MNT',             'MOUNTCNT',
     $ 'MSCP_UNIT_NUMBER','NET',             'NEXTDEVNAM',
     $ 'ODV',             'OPCNT',           'OPR',
     $ 'OWNUIC',          'PID',             'RCK',
     $ 'RCT',             'REC',             'RECSIZ',
     $ 'REFCNT',          'REMOTE_DEVICE',   'RND',
     $ 'ROOTDEVNAM',      'RTM',             'SDI',
     $ 'SECTORS',         'SERIALNUM',       'SERVED_DEVICE',
     $ 'SET_HOST_TERMINA','SHDW_CATCHUP_COP','SHDW_FAILED_MEMBER',
     $ 'SHDW_MASTER',     'SHDW_MASTER_NAME','SHDW_MEMBER',
     $ 'SHDW_MERGE_COPYI','SHDW_NEXT_MBR_NA','SHR',
     $ 'SPL',             'SQD',             'STS',
     $ 'SWL',             'TRACKS',          'TRANSCNT',
     $ 'TRM',             'TT_ACCPORNAM',    'TT_ALTYPEAHD',
     $ 'TT_ANSICRT',      'TT_APP_KEYPAD',   'TT_AUTOBAUD',
     $ 'TT_AVO',          'TT_BLOCK',        'TT_BRDCSTMBX',
     $ 'TT_CRFILL',       'TT_DCL_MAILBX',   'TT_DECCRT',
     $ 'TT_DECCRT2',      'TT_DECCRT3',      'TT_DIALUP',
     $ 'TT_DISCONNECT',   'TT_DMA',          'TT_DRCS',
     $ 'TT_EDIT',         'TT_EDITING',      'TT_EIGHTBIT',
     $ 'TT_ESCAPE',       'TT_FALLBACK',     'TT_HALFDUP',
     $ 'TT_HANGUP',       'TT_HOSTSYNC',     'TT_INSERT',
     $ 'TT_LFFILL',       'TT_LOCALECHO',    'TT_LOWER',
     $ 'TT_MBXDSABL',     'TT_MECHFORM',     'TT_MECHTAB',
     $ 'TT_MODEM',        'TT_MODHANGUP',    'TT_MULTISESSION',
     $ 'TT_NOBRDCST',     'TT_NOECHO',       'TT_NOTYPEAHD',
     $ 'TT_OPER',         'TT_PAGE',         'TT_PASSALL',
     $ 'TT_PASTHRU',      'TT_PHYDEVNAM',    'TT_PRINTER',
     $ 'TT_READSYNC',     'TT_REGIS',        'TT_REMOTE',
     $ 'TT_SCOPE',        'TT_SCRIPT',       'TT_SECURE',
     $ 'TT_SETSPEED',     'TT_SIXEL',        'TT_SYSPWD',
     $ 'TT_TTSYNC',       'TT_WRAP',         'UNIT',
     $ 'VOLCOUNT',        'VOLNAM',          'VOLNUMBER',
     $ 'VOLSETMEM',       'VPROT',           'WCK'/
c
c --- value for the item code from FORSYSDEF($DVIDEF)
c
      data i_dvi /
     $ '00000040'X,       '00000042'X,       '0000006C'X,
     $ '000000EC'X,       '000000F2'X,       '000000F4'X,
     $ '000000F6'X,       '000000F8'X,       '00000062'X,
     $ '00000048'X,       '0000003A'X,       '00000044'X,
     $ '00000028'X,       '00000008'X,       '00000002'X,
     $ '000000E6'X,       '00000004'X,       '0000000A'X,
     $ '0000001C'X,       '00000124'X,       '000000F0'X,
     $ '00000020'X,       '000000E4'X,       '00000006'X,
     $ '0000012C'X,       '0000004C'X,       '00000120'X,
     $ '00000068'X,       '0000005C'X,       '0000006A'X,
     $ '00000014'X,       '0000005A'X,       '0000006E'X,
     $ '0000002A'X,       '000000E8'X,       '00000060'X,
     $ '000000FA'X,       '000000FC'X,       '000000FE'X,
     $ '00000100'X,       '00000072'X,       '000000EA'X,
     $ '0000002C'X,       '0000001A'X,       '0000003C'X,
     $ '00000066'X,       '0000011A'X,       '00000116'X,
     $ '00000118'X,       '00000064'X,       '00000038'X,
     $ '0000011E'X,       '00000058'X,       '00000034'X,
     $ '00000074'X,       '00000016'X,       '00000054'X,
     $ '00000010'X,       '0000000E'X,       '0000007A'X,
     $ '00000056'X,       '00000046'X,       '00000018'X,
     $ '0000001E'X,       '00000102'X,       '00000076'X,
     $ '00000032'X,       '00000078'X,       '0000004E'X,
     $ '00000024'X,       '0000003E'X,       '00000104'X,
     $ '0000012A'X,       '00000106'X,       '0000011C'X,
     $ '00000108'X,       '0000010A'X,       '0000010C'X,
     $ '0000010E'X,       '00000110'X,       '0000005E'X,
     $ '00000052'X,       '00000050'X,       '000000E2'X,
     $ '00000070'X,       '00000026'X,       '00000036'X,
     $ '0000004A'X,       '00000122'X,       '000000B8'X,
     $ '000000D6'X,       '000000D2'X,       '000000AE'X,
     $ '000000DC'X,       '000000DA'X,       '000000B4'X,
     $ '00000092'X,       '000000BC'X,       '000000E0'X,
     $ '00000114'X,       '00000128'X,       '000000C4'X,
     $ '000000C8'X,       '000000B6'X,       '000000CE'X,
     $ '000000DE'X,       '000000BE'X,       '0000009A'X,
     $ '00000084'X,       '000000C2'X,       '000000A4'X,
     $ '000000B0'X,       '00000086'X,       '000000C0'X,
     $ '00000094'X,       '000000AC'X,       '0000008C'X,
     $ '0000009C'X,       '000000A2'X,       '0000008E'X,
     $ '000000A6'X,       '000000B2'X,       '00000126'X,
     $ '0000009E'X,       '00000080'X,       '00000082'X,
     $ '000000A8'X,       '000000AA'X,       '0000007E'X,
     $ '000000CA'X,       '00000112'X,       '000000D0'X,
     $ '000000A0'X,       '000000D8'X,       '00000098'X,
     $ '00000096'X,       '0000008A'X,       '000000C6'X,
     $ '000000BA'X,       '000000CC'X,       '000000D4'X,
     $ '00000088'X,       '00000090'X,       '0000000C'X,
     $ '00000030'X,       '00000022'X,       '0000002E'X,
     $ '000000EE'X,       '00000012'X,       '0000007C'X/
c
c --- size of the item in bytes (0 for longword)
c
      data t_dvi/
     $ 0, 0, 0, 64, 0, 0,       ! alt host avail
     $ 64, 4, 0, 0, 0, 0,       ! concealed
     $ 0, 0, 0, 0, 0, 0,        ! devdepend
     $ 0, 0, 64, 64, 0, 0,      ! devtype
     $ 0, 0, 256, 0, 0, 0,      ! elg
     $ 0, 0, 0, 0, 64, 0,       ! gen
     $ 0, 0, 64, 4, 0, 0,       ! lockid
     $ 64, 0, 0, 0, 0, 64,      ! media name
     $ 64, 0, 0, 0, 0, 64,      ! next devnam
     $ 0, 0, 0, 0, 0, 0,        ! rck
     $ 0, 0, 0, 0, 0, 0,        ! rnd
     $ 64, 0, 0, 0, 0, 0,       ! served device
     $ 0, 0, 0, 0, 0, 0,        ! shdw member
     $ 0, 0, 0, 0, 0, 0,        ! sts
     $ 0, 0, 0, 0, 64, 0,       ! tt altypeahd
     $ 0, 0, 0, 0, 0, 0,        ! tt brdcstmbx
     $ 0, 0, 0, 0, 0, 0,        ! tt dialup
     $ 0, 0, 0, 0, 0, 0,        ! tt eightbit
     $ 0, 0, 0, 0, 0, 0,        ! tt insert
     $ 0, 0, 0, 0, 0, 0,        ! tt mechtab
     $ 0, 0, 0, 0, 0, 0,        ! tt notypahd
     $ 0, 0, 0, 0, 64, 0,       ! tt printer
     $ 0, 0, 0, 0, 0, 0,        ! tt secure
     $ 0, 0, 0, 0, 0, 0,        ! unit
     $ 0, 12, 0, 0, 0, 0/       ! wck
      end
c
      block data syi_data
      common /syidefs/ i_syi(368), c_syi(368), t_syi(368)
      character *18 c_syi
      integer *2 i_syi, t_syi
      data (c_syi(i),i=1,204) /
     $  'ACP_BASEPRIO',    'ACP_DATACHECK',     'ACP_DINDXCACHE',
     $  'ACP_DIRCACHE',    'ACP_EXTCACHE',      'ACP_EXTLIMIT',
     $  'ACP_FIDCACHE',    'ACP_HDRCACHE',      'ACP_MAPCACHE',
     $  'ACP_MAXREAD',     'ACP_MULTIPLE',      'ACP_QUOCACHE',
     $  'ACP_REBLDSYSD',   'ACP_SHARE',         'ACP_SWAPFLGS',
     $  'ACP_SYSACC',      'ACP_WINDOW',        'ACP_WORKSET',
     $  'ACP_WRITEBACK',   'ACP_XQP_RES',       'ACTIVECPU_CNT',
     $  'AFFINITY_SKIP',   'AFFINITY_TIME',     'ALLOCLASS',
     $  'ARCHFLAG',        'AUTOCONFIG_ALGO',   'AVAILCPU_CNT',
     $  'AWSMIN',          'AWSTIME',           'BALSETCNT',
     $  'BJOBLIM',         'BOOTTIME',          'BORROWLIM',
     $  'BREAKPOINTS',     'BUGCHECKFATAL',     'BUGREBOOT',
     $  'CHANNELCNT',      'CHARACTER_EMULATED','CJFLOAD',
     $  'CJFSYSRUJ',       'CLASS_PROT',        'CLISYMTBL',
     $  'CLOCK_INTERVAL',  'CLUSTER_EVOTES',    'CLUSTER_FSYSID',
     $  'CLUSTER_FTIME',   'CLUSTER_MEMBER',    'CLUSTER_NODES',
     $  'CLUSTER_QUORUM',  'CLUSTER_VOTES',     'CONCEAL_DEVICES',
     $  'CONTIG_GBLPAGES', 'CPU',               'CRDENABLE',
     $  'CTLIMGLIM',       'CTLPAGES',          'DEADLOCK_WAIT',
     $  'DECIMAL_EMULATED','DEFMBXBUFQUO',      'DEFMBXMXMSG',
     $  'DEFMBXNUMMSG',    'DEFPRI',            'DEFQUEPRI',
     $  'DISK_QUORUM',     'DISMOUMSG',         'DLCKEXTRASTK',
     $  'DORMANTWAIT',     'DUMPBUG',           'DUMPSTYLE',
     $  'D_FLOAT_EMULATED','ERLBUFFERPAGES',    'ERRORLOGBUFFERS',
     $  'EXPECTED_VOTES',  'EXTRACPU',          'EXUSRSTK',
     $  'FALLBACK_MODE',   'FREEGOAL',          'FREELIM',
     $  'FREE_GBLPAGES',   'FREE_GBLSECTS',     'F_FLOAT_EMULATED',
     $  'GBLPAGES',        'GBLPAGFIL',         'GBLSECTIONS',
     $  'GROWLIM',         'G_FLOAT_EMULATED',  'HW_MODEL',
     $  'HW_NAME',         'H_FLOAT_EMULATED',  'IJOBLIM',
     $  'IMGIOCNT',        'INTSTKPAGES',       'IOTA',
     $  'IRPCOUNT',        'IRPCOUNTV',         'JOBCTLD',
     $  'KFILSTCNT',       'LAMAPREGS',         'LASTEXE',
     $  'LASTHDW',         'LASTSFW',           'LGI_BRK_DISUSER',
     $  'LGI_BRK_LIM',     'LGI_BRK_TERM',      'LGI_BRK_TMO',
     $  'LGI_HID_TIM',     'LGI_PWD_TMO',       'LGI_RETRY_LIM',
     $  'LGI_RETRY_TMO',   'LNMPHASHTBL',       'LNMSHASHTBL',
     $  'LOADCHKPRT',      'LOADERAPT',         'LOADMTACCESS',
     $  'LOAD_SYS_IMAGES', 'LOCKDIRWT',         'LOCKIDTBL',
     $  'LOCKIDTBL_MAX',   'LOCKRETRY',         'LONGWAIT',
     $  'LRPCOUNT',        'LRPCOUNTV',         'LRPMIN',
     $  'LRPSIZE',         'MAXATTACHPRI',      'MAXBUF',
     $  'MAXCLASSPRI',     'MAXPROCESSCNT',     'MAXQUEPRI',
     $  'MAXSYSGROUP',     'MINCLASSPRI',       'MINPRPRI',
     $  'MINWSCNT',        'MOUNTMSG',          'MPW_HILIMIT',
     $  'MPW_IOLIMIT',     'MPW_LOLIMIT',       'MPW_LOWAITLIMIT',
     $  'MPW_PRIO',        'MPW_THRESH',        'MPW_WAITLIMIT',
     $  'MPW_WRTCLUSTER',  'MSCP_BUFFER',       'MSCP_CREDITS',
     $  'MSCP_LOAD',       'MSCP_SERVE_ALL',    'MULTIPROCESSING',
     $  'MVTIMEOUT',       'NISCS_CONV_BOOT',   'NISCS_LOAD_PEA0',
     $  'NISCS_PORT_SERV', 'NJOBLIM',           'NOAUTOCONFIG',
     $  'NOCLOCK',         'NOCLUSTER',         'NODENAME',
     $  'NODE_AREA',       'NODE_CSID',         'NODE_EVOTES',
     $  'NODE_HWTYPE',     'NODE_HWVERS',       'NODE_NUMBER',
     $  'NODE_QUORUM',     'NODE_SWINCARN',     'NODE_SWTYPE',
     $  'NODE_SWVERS',     'NODE_SYSTEMID',     'NODE_VOTES',
     $  'NOPGFLSWP',       'NPAGEDYN',          'NPAGEVIR',
     $  'OLDCPU',          'OLDSID',            'OLDVERSION',
     $  'PAGEDYN',         'PAGEFILE_FREE',     'PAGEFILE_PAGE',
     $  'PAGFILCNT',       'PAGTBLPFC',         'PAMAXPORT',
     $  'PANOPOLL',        'PANUMPOLL',         'PAPOLLINTERVAL',
     $  'PAPOOLINTERVAL',  'PASANITY',          'PASTDGBUF',
     $  'PASTIMOUT',       'PE1',               'PE2',
     $  'PE3',             'PE4',               'PE5',
     $  'PE6',             'PFCDEFAULT',        'PFRATH',
     $  'PFRATL',          'PFRATS',            'PHYSICALPAGES',
     $  'PIOPAGES',        'PIXSCAN',           'POOLCHECK',
     $  'POOLPAGING',      'PQL_DASTLM',        'PQL_DBIOLM'/
      data (c_syi(i),i=205,368) /
     $  'PQL_DBYTLM',      'PQL_DCPULM',        'PQL_DDIOLM',
     $  'PQL_DENQLM',      'PQL_DFILLM',        'PQL_DJTQUOTA',
     $  'PQL_DPGFLQUOTA',  'PQL_DPRCLM',        'PQL_DTQELM',
     $  'PQL_DWSDEFAULT',  'PQL_DWSEXTENT',     'PQL_DWSQUOTA',
     $  'PQL_MASTLM',      'PQL_MBIOLM',        'PQL_MBYTLM',
     $  'PQL_MCPULM',      'PQL_MDIOLM',        'PQL_MENQLM',
     $  'PQL_MFILLM',      'PQL_MJTQUOTA',      'PQL_MPGFLQUOTA',
     $  'PQL_MPRCLM',      'PQL_MTQELM',        'PQL_MWSDEFAULT',
     $  'PQL_MWSEXTENT',   'PQL_MWSQUOTA',      'PRCPOLINTERVAL',
     $  'PROCSECTCNT',     'PSEUDOLOA',         'PU_OPTIONS',
     $  'QBUS_MULT_INTR',  'QDSKINTERVAL',      'QDSKVOTES',
     $  'QUANTUM',         'QUORUM',            'REALTIME_SPTS',
     $  'RECNXINTERVAL',   'RESALLOC',          'RESHASHTBL',
     $  'RIGHTSLIST',      'RJOBLIM',           'RMS_DFMBC',
     $  'RMS_DFMBFHSH',    'RMS_DFMBFIDX',      'RMS_DFMBFREL',
     $  'RMS_DFMBFSDK',    'RMS_DFMBFSMT',      'RMS_DFMBFSUR',
     $  'RMS_DFNBC',       'RMS_EXTEND_SIZE',   'RMS_FILEPROT',
     $  'RMS_GBLBUFQUO',   'RMS_PROLOGUE',      'RSRVPAGCNT',
     $  'S0_PAGING',       'SAVEDUMP',          'SA_APP',
     $  'SBIERRENABLE',    'SCH_CTLFLAGS',      'SCSBUFFCNT',
     $  'SCSCONNCNT',      'SCSFLOWCUSH',       'SCSMAXDG',
     $  'SCSMAXMSG',       'SCSNODE',           'SCSRESPCNT',
     $  'SCSSYSTEMID',     'SCSSYSTEMIDH',      'SCS_EXISTS',
     $  'SETTIME',         'SHADOWING',         'SID',
     $  'SMP_CPUS',        'SMP_CPUSH',         'SMP_LNGSPINWAIT',
     $  'SMP_SANITY_CNT',  'SMP_SPINWAIT',      'SMP_TICK_CNT',
     $  'SPTREQ',          'SRPCOUNT',          'SRPCOUNTV',
     $  'SRPMIN',          'SRPSIZE',           'SSINHIBIT',
     $  'STARTUP_P1',      'STARTUP_P2',        'STARTUP_P3',
     $  'STARTUP_P4',      'STARTUP_P5',        'STARTUP_P6',
     $  'STARTUP_P7',      'STARTUP_P8',        'SWAPFILE_FREE',
     $  'SWAPFILE_PAGE',   'SWPALLOCINC',       'SWPFAIL',
     $  'SWPFILCNT',       'SWPOUTPGCNT',       'SWPRATE',
     $  'SWP_PRIO',        'SYSMWCNT',          'SYSPAGING',
     $  'SYSPFC',          'TAILORED',          'TAPE_ALLOCLASS',
     $  'TAPE_MVTIMEOUT',  'TBSKIPWSL',         'TIMEPROMPTWAIT',
     $  'TIME_CONTROL',    'TTY_ALTALARM',      'TTY_ALTYPAHD',
     $  'TTY_AUTOCHAR',    'TTY_BUF',           'TTY_CLASSNAME',
     $  'TTY_DEFCHAR',     'TTY_DEFCHAR2',      'TTY_DEFPORT',
     $  'TTY_DIALTYPE',    'TTY_DMASIZE',       'TTY_OWNER',
     $  'TTY_PARITY',      'TTY_PROT',          'TTY_RSPEED',
     $  'TTY_SCANDELTA',   'TTY_SILOTIME',      'TTY_SPEED',
     $  'TTY_TIMEOUT',     'TTY_TYPAHDSZ',      'UAFALTERNATE',
     $  'UDABURSTRATE',    'USED_GBLPAGCNT',    'USED_GBLPAGMAX',
     $  'USED_GBLSECTCNT', 'USED_GBLSECTMAX',   'USER3',
     $  'USER4',           'USERD1',            'USERD2',
     $  'VAXCLUSTER',      'VECTOR_MARGIN',     'VECTOR_PROC',
     $  'VERSION',         'VIRTUALPAGECNT',    'VMS5',
     $  'VMS6',            'VMS7',              'VMS8',
     $  'VMSD1',           'VMSD2',             'VMSD3',
     $  'VMSD4',           'VOTES',             'WINDOW_SYSTEM',
     $  'WPRE_SIZE',       'WPTTE_SIZE',        'WRITABLESYS',
     $  'WRITESYSPARAMS',  'WSDEC',             'WSINC',
     $  'WSMAX',           'WS_OPA0',           'XCPU',
     $  'XFMAXRATE',       'XSID'/
c
      data (i_syi(i),i=1,204) /
     $  '000010B5'X,       '000010B4'X,         '00001101'X,
     $  '000010AA'X,       '000010AD'X,         '000010AE'X,
     $  '000010AC'X,       '000010A9'X,         '000010A8'X,
     $  '000010B1'X,       '00002005'X,         '000010AF'X,
     $  '00002029'X,       '0000200C'X,         '000010B6'X,
     $  '000010B0'X,       '000010B2'X,         '000010AB'X,
     $  '000010B3'X,       '00002025'X,         '0000111E'X,
     $  '00001137'X,       '00001138'X,         '000010E5'X,
     $  '000010DA'X,       '0000112A'X,         '0000111D'X,
     $  '00001038'X,       '00001039'X,         '00001012'X,
     $  '000010B9'X,       '000010BF'X,         '00001056'X,
     $  '00001130'X,       '00002004'X,         '00002001'X,
     $  '00001024'X,       '0000201E'X,         '00002019'X,
     $  '0000201A'X,       '0000201D'X,         '0000105B'X,
     $  '00001131'X,       '0000110D'X,         '000010CD'X,
     $  '000010CE'X,       '000010CF'X,         '000010CA'X,
     $  '000010CC'X,       '000010CB'X,         '00002012'X,
     $  '00001110'X,       '00002000'X,         '00002002'X,
     $  '00001027'X,       '00001026'X,         '0000105E'X,
     $  '0000201F'X,       '00001050'X,         '00001051'X,
     $  '00001052'X,       '000010B7'X,         '000010E2'X,
     $  '000010DC'X,       '00002015'X,         '00001011'X,
     $  '000010F1'X,       '00002003'X,         '00001132'X,
     $  '00002020'X,       '00001140'X,         '0000112B'X,
     $  '00001112'X,       '0000104C'X,         '0000101B'X,
     $  '00001105'X,       '00001054'X,         '00001053'X,
     $  '0000110F'X,       '00001111'X,         '00002021'X,
     $  '00001007'X,       '00001008'X,         '00001006'X,
     $  '00001055'X,       '00002022'X,         '00001109'X,
     $  '0000110A'X,       '00002023'X,         '000010B8'X,
     $  '00001028'X,       '00001010'X,         '0000103D'X,
     $  '00001013'X,       '00001014'X,         '0000112C'X,
     $  '00001005'X,       '00001059'X,         '00001142'X,
     $  '00000202'X,       '00000101'X,         '00002028'X,
     $  '000010E8'X,       '00002027'X,         '000010E9'X,
     $  '000010EA'X,       '000010EB'X,         '000010E6'X,
     $  '000010E7'X,       '00001072'X,         '00001071'X,
     $  '00002017'X,       '00002016'X,         '00002024'X,
     $  '0000202D'X,       '000010EF'X,         '0000105C'X,
     $  '000010C0'X,       '00001057'X,         '0000103E'X,
     $  '0000101C'X,       '0000101D'X,         '0000101F'X,
     $  '0000101E'X,       '00001102'X,         '0000104F'X,
     $  '00001128'X,       '00001009'X,         '000010E3'X,
     $  '0000104D'X,       '00001127'X,         '00001129'X,
     $  '0000100C'X,       '00002014'X,         '0000102B'X,
     $  '00001107'X,       '0000102C'X,         '00001106'X,
     $  '0000102D'X,       '0000102F'X,         '00001030'X,
     $  '0000102A'X,       '0000112E'X,         '0000112F'X,
     $  '00001122'X,       '00001123'X,         '0000110E'X,
     $  '0000104E'X,       '0000202E'X,         '0000202F'X,
     $  '00001133'X,       '000010BA'X,         '00002006'X,
     $  '00002007'X,       '00002008'X,         '000010D9'X,
     $  '0000201B'X,       '000010D0'X,         '0000110C'X,
     $  '000010D7'X,       '000010D8'X,         '0000201C'X,
     $  '000010D2'X,       '000010D4'X,         '000010D5'X,
     $  '000010D6'X,       '000010D3'X,         '000010D1'X,
     $  '0000202B'X,       '00001016'X,         '00001017'X,
     $  '00000200'X,       '00000201'X,         '00000100'X,
     $  '00001018'X,       '000010F4'X,         '000010F2'X,
     $  '0000100D'X,       '00001003'X,         '000010E0'X,
     $  '000010F8'X,       '0000106C'X,         '0000106D'X,
     $  '0000106E'X,       '000010E1'X,         '0000106B'X,
     $  '0000106A'X,       '000010F9'X,         '000010FA'X,
     $  '000010FB'X,       '000010FC'X,         '000010FD'X,
     $  '000010FE'X,       '00001002'X,         '00001034'X,
     $  '00001033'X,       '00001035'X,         '00001032'X,
     $  '00001025'X,       '0000100A'X,         '00001124'X,
     $  '00002009'X,       '0000108E'X,         '00001090'X/
      data (i_syi(i),i=205,368) /
     $  '00001092'X,       '00001094'X,         '00001096'X,
     $  '000010A6'X,       '00001098'X,         '000010EC'X,
     $  '0000109A'X,       '0000109C'X,         '0000109E'X,
     $  '000010A0'X,       '000010A4'X,         '000010A2'X,
     $  '0000108F'X,       '00001091'X,         '00001093'X,
     $  '00001095'X,       '00001097'X,         '000010A7'X,
     $  '00001099'X,       '000010ED'X,         '0000109B'X,
     $  '0000109D'X,       '0000109F'X,         '000010A1'X,
     $  '000010A5'X,       '000010A3'X,         '00001069'X,
     $  '0000100B'X,       '00001126'X,         '00001113'X,
     $  '00001118'X,       '000010E4'X,         '000010F0'X,
     $  '00001029'X,       '000010BC'X,         '0000105A'X,
     $  '000010BE'X,       '00002010'X,         '0000105D'X,
     $  '00001119'X,       '000010BB'X,         '00001084'X,
     $  '0000108A'X,       '00001089'X,         '00001088'X,
     $  '00001085'X,       '00001086'X,         '00001087'X,
     $  '00001100'X,       '0000108C'X,         '0000108D'X,
     $  '000010FF'X,       '0000108B'X,         '00001134'X,
     $  '00001108'X,       '00002013'X,         '00002030'X,
     $  '0000200A'X,       '0000110B'X,         '0000105F'X,
     $  '00001060'X,       '00001064'X,         '00001062'X,
     $  '00001063'X,       '00001067'X,         '00001061'X,
     $  '00001065'X,       '00001066'X,         '000010DB'X,
     $  '0000200B'X,       '0000202C'X,         '00001001'X,
     $  '00001103'X,       '00001104'X,         '0000111B'X,
     $  '00001116'X,       '0000111A'X,         '00001117'X,
     $  '0000101A'X,       '00001020'X,         '00001021'X,
     $  '00001023'X,       '00001022'X,         '00002011'X,
     $  '000010C2'X,       '000010C3'X,         '000010C4'X,
     $  '000010C5'X,       '000010C6'X,         '000010C7'X,
     $  '000010C8'X,       '000010C9'X,         '000010F5'X,
     $  '000010F3'X,       '0000103C'X,         '0000103F'X,
     $  '0000100E'X,       '0000103B'X,         '0000103A'X,
     $  '0000102E'X,       '0000100F'X,         '0000200D'X,
     $  '00001004'X,       '000010C1'X,         '00001141'X,
     $  '00001125'X,       '00001031'X,         '0000106F'X,
     $  '0000111C'X,       '0000107D'X,         '0000107C'X,
     $  '000010F7'X,       '00001078'X,         '00001081'X,
     $  '00001079'X,       '0000107A'X,         '00001083'X,
     $  '00001074'X,       '0000107E'X,         '00001080'X,
     $  '00001077'X,       '0000107F'X,         '00001076'X,
     $  '00001073'X,       '00001082'X,         '00001075'X,
     $  '000010F6'X,       '0000107B'X,         '0000200E'X,
     $  '00001070'X,       '0000113C'X,         '0000113D'X,
     $  '0000113E'X,       '0000113F'X,         '0000104A'X,
     $  '0000104B'X,       '00001048'X,         '00001049'X,
     $  '000010EE'X,       '00001136'X,         '00001135'X,
     $  '00001000'X,       '00001019'X,         '00001044'X,
     $  '00001045'X,       '00001046'X,         '00001047'X,
     $  '00001040'X,       '00001041'X,         '00001042'X,
     $  '00001043'X,       '000010BD'X,         '0000112D'X,
     $  '00001115'X,       '00001114'X,         '0000200F'X,
     $  '00002026'X,       '00001037'X,         '00001036'X,
     $  '00001015'X,       '0000202A'X,         '00002018'X,
     $  '00001058'X,       '000010DD'X/
      data (t_syi(i),i=1,204) /
     $  0, 0, 0, 0, 0, 0,       ! acp extlimit
     $  0, 0, 0, 0, 0, 0,       ! acp quocache
     $  0, 0, 0, 0, 0, 0,       ! acp workset
     $  0, 0, 0, 0, 0, 0,       ! alloclass
     $  0, 0, 0, 0, 0, 0,       ! balsecnt
     $  0, 0, 0, 0, 0, 0,       ! bugreboot
     $  0, 0, 0, 0, 0, 0,       ! clisymtbl
     $  0, 0, 6, 8, 0, 0,       ! cluster nodes
     $  0, 0, 0, 0, 0, 0,       ! crdenable
     $  0, 0, 0, 0, 0, 0,       ! defmbxmxmsg
     $  0, 0, 0, 16, 0, 0,      ! dlckextrask
     $  0, 0, 0, 0, 0, 0,       ! errorlogbuffers
     $  0, 0, 0, 0, 0, 0,       ! freelim
     $  0, 0, 0, 0, 0, 0,       ! gblsections
     $  0, 0, 0, 31, 0, 0,      ! ijoblim
     $  0, 0, 0, 0, 0, 0,       ! jobctld
     $  0, 0, 0, 0, 0, 0,       ! lgi brk disuser
     $  0, 0, 0, 0, 0, 0,       ! lgi retry lim
     $  0, 0, 0, 0, 0, 0,       ! loadmtaccess
     $  0, 0, 0, 0, 0, 0,       ! longwait
     $  0, 0, 0, 0, 0, 0,       ! maxbuf
     $  0, 0, 0, 0, 0, 0,       ! minprpri
     $  0, 0, 0, 0, 0, 0,       ! mpw lowaitlimit
     $  0, 0, 0, 0, 0, 0,       ! mscp credits
     $  0, 0, 0, 0, 0, 0,       ! niscs load pea0
     $  0, 0, 0, 0, 0, 15,      ! nodename
     $  0, 0, 0, 0, 12, 0,      ! node number
     $  0, 8, 0, 0, 6, 0,       ! node votes
     $  0, 0, 0, 0, 0, 0,       ! oldversion
     $  0, 0, 0, 0, 0, 0,       ! pamaxport
     $  0, 0, 0, 0, 0, 0,       ! pastdgbuf
     $  0, 0, 0, 0, 0, 0,       ! pe5
     $  0, 0, 0, 0, 0, 0,       ! physicalpages
     $  0, 0, 0, 0, 0, 0/       ! pql dbiolm
      data (t_syi(i),i=205,368) /
     $  0, 0, 0, 0, 0, 0,       ! pqldjtquota
     $  0, 0, 0, 0, 0, 0,       ! pql dwsquota
     $  0, 0, 0, 0, 0, 0,       ! pql menqlm
     $  0, 0, 0, 0, 0, 0,       ! pql mwsdefault
     $  0, 0, 0, 0, 0, 0,       ! pu options
     $  0, 0, 0, 0, 0, 0,       ! realtime spts
     $  0, 0, 0, 0, 0, 0,       ! rms dfmbc
     $  0, 0, 0, 0, 0, 0,       ! rms dfmbfsur
     $  0, 0, 0, 0, 0, 0,       ! rsrvpagcnt
     $  0, 0, 0, 0, 0, 0,       ! scsbuffcnt
     $  0, 0, 0, 0, 6, 0,       ! scsrespcnt
     $  0, 0, 0, 0, 0, 0,       ! sid
     $  0, 0, 0, 0, 0, 0,       ! smp tick cnt
     $  0, 0, 0, 0, 0, 0,       ! ssinhibit
     $  0, 0, 0, 0, 0, 0,       ! startup p6
     $  0, 0, 0, 0, 0, 0,       ! swpfail
     $  0, 0, 0, 0, 0, 0,       ! syspaging
     $  0, 0, 0, 0, 0, 0,       ! timepromptwait
     $  0, 0, 0, 0, 0, 2,       ! tty classname
     $  0, 0, 0, 0, 0, 0,       ! tty owner
     $  0, 0, 0, 0, 0, 0,       ! tty speed
     $  0, 0, 0, 0, 0, 0,       ! used gblpagmax
     $  0, 0, 0, 0, 0, 0,       ! userd2
     $  0, 0, 0, 8, 0, 0,       ! vms5
     $  0, 0, 0, 0, 0, 0,       ! vmsd3
     $  0, 0, 0, 0, 0, 0,       ! writablesys
     $  0, 0, 0, 0, 0, 0,       ! xcpu
     $  0, 0/
      end
