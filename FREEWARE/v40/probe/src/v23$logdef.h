/* V23$LOGDEF.H
 *
 * Log file record definition
 *
 */

# define prb$entry_size			48
# define prb$log_capacity		16
# define prb$log_node_size		8
# define prb$log_adapter_size		8
# define prb$object_name_size		12
# define rpl$object_name_size		17	/* rpl$ >= prb$object_name_size */

union
   {
   unsigned char element[prb$entry_size];	/* record storage */
   unsigned char type;				/* primary record type */
   struct prb$record_0
      {
      struct
         {
         unsigned : 8;				/* type offset */
	 unsigned is_ieee : 1;			/* ethernet or 802.3 */
	 unsigned is_data : 1;			/* data in block (>=V2.2)*/
	 unsigned is_more : 1;			/* more data follows */
	 unsigned warning : 1;			/* acquisition IO warning */
	 unsigned overhead : 4;			/* IEEE headers overhead size */
         } tag;
      unsigned short size;			/* frame size */
               float time;			/* interval time */
      unsigned char  source[6];			/* source address */
      unsigned short pad1;			/* source padding */
      unsigned char  destination[6];		/* destination address */
      unsigned short pad2;			/* destination padding */
      unsigned char  protocol[2];		/* [0]=DSAP [1]=SSAP */
      unsigned char  ctlpi[6];			/* 802.3 control/protocol */
      unsigned char  data[prb$log_capacity];	/* front-payload data area */
      } prb$frame;
   struct prb$record_1
      {
      unsigned : 8;				/* type offset */
      union
         {
	 struct
	    {
	    struct
	       {
	       unsigned is_extracted : 1;	/* original or reduced */
	       unsigned : 6;			/* reserved */
	       unsigned from_a_vax : 1;		/* VAX data type */
	       } flags;
	    unsigned char  size;		/* record size */
	    unsigned : 8;			/* reserved */
	    } v20;
	 struct
	    {
	    unsigned char  size;		/* record size */
	    union
	       {
	       unsigned payload : 12;		/* maximum payload size */
	       struct
		  {
		  unsigned : 12 ;		/* payload storage */
		  unsigned : 2 ;		/* reserved */
		  unsigned from_a_vax : 1;	/* VAX data type */
		  unsigned is_extracted : 1;	/* original or reduced */
		  } flags;
	       } data;
	    } v22;
	 struct
	    {
	    unsigned char  size;		/* record size */
	    union
	       {
	       unsigned payload : 12;		/* maximum payload size */
	       struct
		  {
		  unsigned : 12 ;		/* payload storage */
		  unsigned map : 1 ;		/* flag arrangements : MBZ */
		  unsigned is_extracted : 1 ;	/* original or reduced */
		  unsigned is_vms_type : 1;	/* VAX data type : MB1 */
		  unsigned platform : 1;	/* VAX or Alpha produced */
		  } flags;
	       } data;
	    } v23;				/* includes 2.2-1 */
	 } specific;
      unsigned short patch_level;		/* maintenance level */
      unsigned char  minor_id;			/* version structure level */
      unsigned char  major_id;			/* version level */
      unsigned long  abstime[2];		/* creation time */
      unsigned long  csr;			/* initial state */
      unsigned long  pid;			/* logger process id */
      unsigned short active;			/* schedule activity period */
      unsigned short standby;			/* schedule standby period */
      unsigned short cycle;			/* cycle repetitions */
      unsigned char  asize;			/* adapter string size */
      unsigned char  nsize;			/* node string size */
	       char  adapter[prb$log_adapter_size];	/* adapter string */
	       char  node[prb$log_node_size];		/* node string */
      } prb$intro;
   struct prb$record_2				/* identical to type=6 */
      {
      unsigned : 8;				/* type offset */
      unsigned char  sequence;			/* sequence number [1..] */
      unsigned char  content[prb$entry_size-2];	/* data */
      } prb$data;
   struct prb$record_3
      {
      unsigned : 8;				/* type offset */
      unsigned char  subtype;			/* subtype id=0,1,2,3 */
      unsigned short id;			/* protocol, node or vendor id */
      unsigned mask : 16;			/* protocol or node mask */
      unsigned short size;			/* name string size */
               char  name[prb$object_name_size];	/* name string */
      union
         {
         struct
            {
	    unsigned char value[2];		/* protocol value */
            } protocol;
         struct
            {
	    unsigned char address[6];		/* node address */
            } node;
         struct
            {
	    unsigned char prefix[3];		/* vendor MAC */
	    unsigned : 8;			/* MBZ */
	    unsigned short count;		/* vendor node predefined */
            } vendor;
         } object;
      } prb$table;
   struct prb$record_4
      {
      unsigned : 32;				/* type offset + reserved */
      unsigned long  sequence;			/* report number */
      unsigned long  abstime[2];		/* report time */
      unsigned long  fxmitted;			/* interval frames transmitted */
      unsigned long  fdeferred;			/* interval frames deferred */
      unsigned long  single;			/* interval single collisions */
      unsigned long  multiple;			/* interval multiple collisions */
      unsigned long  fxmltcst;			/* interval multicast frames sent */
      unsigned long  bxmltcst;			/* interval multicast bytes sent */
      unsigned long  bxmitted;			/* interval transmitted bytes */
      unsigned failmask : 16;			/* send failure mask */
      unsigned short failcount;			/* send failure count */
      } prb$collision;
   struct prb$record_5
      {
      unsigned : 32;				/* type offset + reserved */
      unsigned long  sequence;			/* cycle number */
      unsigned long  abstime[2];		/* start time */
      unsigned long  csr;			/* master state */
      } prb$start;
   struct prb$record_7
      {
      struct
         {
	 unsigned : 8;				/* type offset */
	 unsigned is_soft : 1;			/* interim or final stop */
	 unsigned : 7;				/* reserved */
	 unsigned : 16;				/* reserved */
	 } flags;
      unsigned long  sequence;			/* cycle number */
      unsigned long  abstime[2];		/* stop time */
      unsigned long  csr;			/* master state */
      struct
         {
	 unsigned : 16;				/* reserved */
	 } state;
      unsigned short nodes;			/* node list size */
      unsigned long  frames;			/* processed frames */
      unsigned long  drops;			/* dropped frames + Qslots */
      unsigned long  ios;			/* number of performed IO's */
      } prb$stop;
   } log;
