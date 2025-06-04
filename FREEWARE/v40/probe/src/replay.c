/*
 * This software is COPYRIGHT © 1991-1997, Stephane Germain.
 * ALL RIGHTS RESERVED.
 * Permission is granted for not-for-profit redistribution, provided all source
 * and object code remain unchanged from the original distribution, and that all
 * copyright notices remain intact.
 *
 * This software is provided "AS IS". The author makes no representations or
 * warranties with respect to the software and specifically disclaim any implied
 * warranties of merchantability or fitness for any particular purpose.
 *
 ******************************************************************************
 * ETHERNET <PROBE> TRAFFIC ANALYZER
 *
 * This is SYS_PROBE:REPLAY.C
 * Creator S.Germain p.eng
 * Using   VMS 5.4+ (VAXC)
 * History v1.0  28 Jul 91 - Incorporation of basic formatter (FORMAT.C)
 *                           and fundamental structure.
 *	   v1.1     Aug 91 - Unoptimized functional release subset.
 *	   v2.2     Dec 94 - Removed uncompleted internal loading & display code
 *                           portion. Added data formatting capabitity.
 *                           Added manufacturer/logical prefix search (static).
 *         v2.3     May 97 - Added Alpha platform sample (V2.2B+) recognition.
 *                           Added dynamic vendor tables. Static defs removed.
 *			     Added sample filename expansion.
 *			     New default file names are NETWORK (.PRB*).
 *			     Improved IEEE 802.2 header info extraction.
 *
 * CONCEPT
 *
 * This image is responsible for all aspects of PROBE post-recording
 * processing. A network traffic file is provided to the program for
 * interpretation and listing.
 *
 * NOTES
 *
 * 1. Conditions are signaled according to VMS standards.
 * Compilation and linking is done as follows:
 *
 *    $ CC/NODEBUG REPLAY
 *    $ MESSAGE/OBJECT=MSG2 REPLAY
 *    $ SET COMMAND/OBJECT=TBL TABLE
 *    $ LINK/NODEBUG/NOTRACEBACK REPLAY,MSG2,TBL,CRTL/OPT
 *
 * 2. Invocation and run-time information parsing is performed by CLI
 * routines. The command definition in file PROBE.CLD is included in
 * the process command table as follows:
 *
 *    $ SET COMMAND PROBE
 *
 * 3. General user help information is contained in the PROBE.HLP library.
 *
 * 4. Where convenient, some VMS-specific calls and library routines
 * (particularly CLI$) are used. As this affects eventual code portability,
 * these extensions are located in low-level portions of the program call-
 * structure as much as possible and treated as black boxes which could be
 * replaced by functionally equivalent others.
 *
 * CONSTANTS */

# module  REPLAY		"V2.3"
# define  VERSION		0x0203	/* highest known PROBE data format */

# define  eth$envelop		18	/* dst(6)+src(6)+ptcl:length(2)+fcs(4); LLC is extra */
# define  eth$min_data		46	/* IEEE spec */
# define  eth$max_data		1500	/* IEEE spec */
# define  eth$min_frame		eth$min_data + eth$envelop
# define  eth$max_frame		eth$max_data + eth$envelop

# include <ctype.h>
# include <stdio.h>
# include <stdlib.h>
# include <climsgdef.h>			/* CLI$xxx - DCL parsing codes */
# include <descrip.h>			/* DSC$xxx - VMS descriptors */
# include <ssdef.h>			/*  SS$xxx - VMS status codes */
# include <sys_probe:v23$logdef.h>	/* Probe 2.3-x log file layout */
# include <sys_probe:v23$macros.h>	/* Probe 2.3-x macros etc... */

/******************************************************************************
 * GLOBALS                                                                    *
 ******************************************************************************/

 FILE	  *analysis;			/* OUT: analysis file control */
 FILE	  *sample;			/* IN: sample file control */
 FILE	  *table;			/* IN: table file control */
 char	  a_file[81];			/* analysis file name */
 char	  s_file[133];			/* sample file name */
 char	  t_file[81];			/* table file name */

 char	  adapter[prb$log_adapter_size+1];
 char	  log_node[prb$log_node_size+1];

 C_BLOCK  *master_cycle;
 N_BLOCK  *node_header;
 P_BLOCK  *protocol_header;
 V_BLOCK  *vendor_header;

 unsigned long  frame_count = 0;	/* number of sample frames */
 unsigned long	log_record_size;	/* sample entry size */
 unsigned long  log_payload_size;	/* maximum recorded payload size */

 unsigned short patch;			/* revision patch level */
 union
    {
    unsigned short level;		/* revision combined id */
    struct
       {
       unsigned char minor;		/* revision secondary id */
       unsigned char major;		/* revision primary id */
       } id;
    } revision;

 struct
    {
    unsigned analysis_specified : 1;
    unsigned analysis_redirected : 1;
    unsigned is_format_collision : 1;
    unsigned is_format_controls : 1;
    unsigned is_format_cycle : 1;
    unsigned is_format_data : 1;
    unsigned is_format_frame : 1;
    unsigned is_format_header : 1;
    unsigned is_format_table : 1;
    unsigned process_external_table : 1;
    unsigned sample_was_from_vms : 1;
    unsigned sample_was_extracted : 1;
    unsigned platform : 1;
    unsigned : 0;
    } command;				/* overall specified & attained state */

/******************************************************************************
 * MAIN PROGRAM                                                               *
 *                                                                            *
 * Each routine at this level is responsible for continuing or aborting       *
 * processing directly. No transit error codes are used... return is implied  *
 * to mean success and pursuit.                                               *
 ******************************************************************************/

main()
{
 globalref RPL_NODISP;

parse_command_line();
open_files();
read_sample_header();
read_table_definitions();
if (command.analysis_specified)
   {
   format_environment();
   format_sample_data();
   }
else /* interactive */
   {
   lib$stop(&RPL_NODISP);
   }
}

/******************************************************************************
 * PARSE_COMMAND_LINE                                                         *
 *                                                                            *
 * This routine determines the overall user requested action and values based *
 * on the initial DCL-syntax command and possible logical name specifications.*
 ******************************************************************************/

parse_command_line()
{
 globalref RPL_INVCID, RPL_ALLCYC;

 unsigned short id_size;
 unsigned char  id_string[6];
 	  long  new_value;

 C_BLOCK *new_cycle;

 D_CONSTANT(analyze,"PRB_ANAZ");
 D_CONSTANT(sample,"PRB_PLAY");
 D_CONSTANT(table,"PRB_TABL");
 D_CONSTANT(a_out,"ANAZ_OUT");
 D_CONSTANT(a_all,"CLSS_ALL");
 D_CONSTANT(a_col,"CLSS_COL");
 D_CONSTANT(a_ctl,"CLSS_CTL");
 D_CONSTANT(a_cyc,"CLSS_CYC");
 D_CONSTANT(a_dat,"CLSS_DAT");
 D_CONSTANT(a_frm,"CLSS_FRM");
 D_CONSTANT(a_hdr,"CLSS_HDR");
 D_CONSTANT(a_tbl,"CLSS_TBL");
 D_CONSTANT(e_cyc,"EXTR_CYC");
 D_VARIABLE(spec_a,a_file);
 D_VARIABLE(spec_s,s_file);
 D_VARIABLE(spec_t,t_file);
 D_VARIABLE(identifier,id_string);

/* load sample file name */

if (cli$get_value(&sample,&spec_s,&spec_s.dsc$w_length) == SS$_NORMAL)
   s_file[spec_s.dsc$w_length] = '\0';
else
   strcpy(s_file,"NETWORK.PRB");

/* load table file name if specified */

if (cli$present(&table) == CLI$_PRESENT)
   if (cli$get_value(&table,&spec_t,&spec_t.dsc$w_length) == SS$_NORMAL)
      {
      t_file[spec_t.dsc$w_length] = '\0';
      command.process_external_table = 1;
      }

/* load analysis classes (using defaults) & file name if specified */

if (cli$present(&analyze) == CLI$_PRESENT)
   {
   command.analysis_specified = 1;
   if (cli$get_value(&a_out,&spec_a,&spec_a.dsc$w_length) == SS$_NORMAL)
      {
      a_file[spec_a.dsc$w_length] = '\0';
      command.analysis_redirected = 1;
      }
   command.is_format_collision = (cli$present(&a_col) != CLI$_NEGATED);
   command.is_format_cycle     = (cli$present(&a_cyc) != CLI$_NEGATED);
   command.is_format_frame     = (cli$present(&a_frm) != CLI$_NEGATED);
   command.is_format_header    = (cli$present(&a_hdr) != CLI$_NEGATED);
   command.is_format_table     = (cli$present(&a_tbl) != CLI$_NEGATED);
   command.is_format_controls  = (cli$present(&a_ctl) == CLI$_PRESENT) ||
				((cli$present(&a_all) == CLI$_PRESENT) && (cli$present(&a_ctl) != CLI$_NEGATED));
   command.is_format_data      = (cli$present(&a_dat) == CLI$_PRESENT) ||
				((cli$present(&a_all) == CLI$_PRESENT) && (cli$present(&a_dat) != CLI$_NEGATED));
   }

/* prepare master cycle (sample) block & load cycle list if specified */

master_cycle = calloc(1,sizeof(C_BLOCK));		/* allocate the cycle/sample linked list head */
master_cycle->next		  = master_cycle;	/* initially unique member of forward circular list */
master_cycle->previous		  = master_cycle;	/* initially unique member of backward circular list */
master_cycle->reference_count	  = 0;			/* default - empty list means all cycles */
master_cycle->block.sample.max_id = 0;			/* default - permits id=1 list insertion */

if (cli$present(&e_cyc) == CLI$_PRESENT)
   {
   while ((cli$get_value(&e_cyc,&identifier,&id_size)) != CLI$_ABSENT)
      {
      id_string[id_size] = '\0';
      new_value = atoi(id_string);
      if ((new_value <= 0) || (new_value > 32767))	/* reject out of bounds cycle entry */
	 lib$signal(&RPL_INVCID,1,new_value);
      else						/* accept valid cycle entry */
         {
	 new_cycle = calloc(1,sizeof(C_BLOCK));		/* prepare new entry block and list-search parameters */
	 new_cycle->block.object.id = new_value;
         if (valid(c_connect(new_cycle)))		/* attempt insertion of new cycle into cycle list */
	    master_cycle->reference_count += 1;
	 else
	    cfree(new_cycle);
         }
      }
   if (master_cycle->reference_count == 0)		/* no entry exists on list... dynamic processing */
      lib$signal(&RPL_ALLCYC);
   }
}

/******************************************************************************
 * OPEN_FILES                                                                 *
 *                                                                            *
 * This routine opens all input files & (as required) the analysis output     *
 * file. It applies default file types.                                       *
 ******************************************************************************/

open_files()
{
 globalref RPL_NOSAMPLE, RPL_NOACHAN, RPL_EXTABLE;

/* sample file must be accessible ; expand full VMS specification */

if ((sample = fopen(s_file,"r","dna=NETWORK.PRB")) == NULL)
   lib$stop(&RPL_NOSAMPLE,1,s_file);

fgetname(sample,s_file);

/* analysis file must be created if specified */

if (command.analysis_specified)
   if ((analysis = ((command.analysis_redirected) ? fopen(a_file,"w","dna=NETWORK.PRB_ANALYSIS")
                                                  : fopen("SYS$OUTPUT","w"))) == NULL)
      lib$stop(&RPL_NOACHAN);

/* attempt to access external table file */

if (command.process_external_table)
   if ((table = fopen(t_file,"r","dna=SYS_PROBE:NETWORK.TBL")) == NULL)
      lib$signal(&RPL_EXTABLE), command.process_external_table = 0;
}

/******************************************************************************
 * READ_SAMPLE_HEADER                                                         *
 *                                                                            *
 * Get sample master characteristics and additional imbedded node & protocol  *
 * entry definitions. Give priority to any externally loaded definitions.     *
 ******************************************************************************/

read_sample_header()
{
 globalref RPL_NEWLVL, RPL_BADREC, RPL_DATALESS, RPL_EMPTY, RPL_BADTYPE,
	   RPL_BIGCYC, RPL_CYCOOB, RPL_INCREC;

 int i;

 C_BLOCK *cycle;
 P_BLOCK *new_protocol;
 N_BLOCK *new_node;
 V_BLOCK *new_vendor;

/* extract and validate type and version information */

if ((get_data(&log.element[0],8)) && (log.type == 1))
   {
   patch = log.prb$intro.patch_level;
   if ((revision.level = ((log.prb$intro.major_id << 8) | log.prb$intro.minor_id)) > VERSION)
      lib$stop(&RPL_NEWLVL,3,revision.id.major,revision.id.minor,patch);
   }
else
   lib$stop(&RPL_BADREC);

/* versions prior to V2.2 have different primary layout than later versions.
   V2.3 flag interpretation is a compatible (but not identical) superset of V2.2 */

if (revision.level < 0x0202)
   {
   command.sample_was_from_vms  = log.prb$intro.specific.v20.flags.from_a_vax;
   command.sample_was_extracted = log.prb$intro.specific.v20.flags.is_extracted;
   command.platform = 0;
   log_record_size  = log.prb$intro.specific.v20.size;
   if (command.is_format_data)
      lib$signal(&RPL_DATALESS), command.is_format_data = 0;
   }
else
   if (log.prb$intro.specific.v23.data.flags.map == 0)
      {
      command.sample_was_from_vms  = log.prb$intro.specific.v23.data.flags.is_vms_type;
      command.sample_was_extracted = log.prb$intro.specific.v23.data.flags.is_extracted;
      command.platform = log.prb$intro.specific.v23.data.flags.platform;
      log_record_size  = log.prb$intro.specific.v23.size;
      if ((command.is_format_data) && ((log_payload_size = log.prb$intro.specific.v23.data.payload) == 0))
	 lib$signal(&RPL_EMPTY), command.is_format_data = 0;
      }
   else
      command.sample_was_from_vms = 0;
if (!command.sample_was_from_vms)
   lib$stop(&RPL_BADTYPE);

/* get remainder of introduction block */

if (get_data(&log.element[8],log_record_size-8))
   {
   master_cycle->block.sample.max_id	       = ((log.prb$intro.cycle > 0) ? log.prb$intro.cycle : 1);
   master_cycle->block.sample.creation_time[0] = log.prb$intro.abstime[0];
   master_cycle->block.sample.creation_time[1] = log.prb$intro.abstime[1];
   master_cycle->block.sample.activity	       = log.prb$intro.active;
   master_cycle->block.sample.standby	       = log.prb$intro.standby;

   /* build or adjust cycle block list to contain only applicable entries */

   if (master_cycle->reference_count == 0)			/* all cycles to be built */
      for (i = master_cycle->block.sample.max_id; i > 0; --i)
	 {
	 cycle = calloc(1,sizeof(C_BLOCK));			/* create new entry */
	 cycle->block.object.id		= i;			/* identify cycle */
	 cycle->next			= master_cycle->next;	/* place in front of forward list */
	 master_cycle->next		= cycle;		/* adjust front of forward list */
	 cycle->previous		= master_cycle;		/* place behind of backward list */
	 cycle->next->previous		= cycle;		/* adjust rear of backward list */
	 master_cycle->reference_count += 1;			/* update cycle count */
	 }
   else
      {
      for (cycle = master_cycle->next; cycle != master_cycle; cycle = cycle->next)
	 if (cycle->block.object.id > master_cycle->block.sample.max_id)
	    {
	    lib$signal(&RPL_BIGCYC,1,cycle->block.object.id);
	    cycle->previous->next	   = cycle->next;	/* adjust forward list to skip bad entry */
	    cycle			   = cycle->previous;	/* step back to last good entry */
	    cfree(cycle->next->previous);			/* destroy useless entry */
	    cycle->next->previous	   = cycle;		/* adjust backward list to skip removed entry */
	    master_cycle->reference_count -= 1;			/* update cycle count */
	    }
      if (master_cycle->reference_count == 0)
	 lib$stop(&RPL_CYCOOB);
      }

   /* load node and adapter */

   for (i = 0; i < log.prb$intro.nsize; ++i)
      log_node[i] = log.prb$intro.node[i];
   log_node[log.prb$intro.nsize] = '\0';
   for (i = 0; i < log.prb$intro.asize; ++i)
      adapter[i] = log.prb$intro.adapter[i];
   adapter[log.prb$intro.asize] = '\0';
   }
else
   lib$stop(&RPL_INCREC);

/* prepare master node, protocol, vendor lists */

node_header = calloc(1,sizeof(N_BLOCK));			/* allocate the node linked list head */
node_header->next		 = node_header;			/* initially unique circular list member */
node_header->reference_count	 = 0;				/* default - empty list */

protocol_header = calloc(1,sizeof(P_BLOCK));			/* allocate the protocol linked list head */
protocol_header->next		 = protocol_header;		/* initially unique circular list member */
protocol_header->reference_count = 0;				/* default - empty list */

vendor_header = calloc(1,sizeof(V_BLOCK));			/* allocate the vendor linked list head */
vendor_header->next		 = vendor_header;		/* initially unique circular list member */
vendor_header->reference_count	 = 0;				/* default - empty list */

/* skip sample filler & load non-conflicting imbedded protocol, node and vendor definitions */

while ((get_data(&log.element,log_record_size)) && (log.type == 3))
   switch (log.prb$table.subtype)
      {
      case 0: 
	 break;
      case 1:
	 new_protocol = calloc(1,sizeof(P_BLOCK));
	 new_protocol->object.id       = log.prb$table.id;
	 new_protocol->object.value[0] = log.prb$table.object.protocol.value[0];
	 new_protocol->object.value[1] = log.prb$table.object.protocol.value[1];
	 for (i = 0; i < log.prb$table.size; ++i)
	    new_protocol->object.name[i] = log.prb$table.name[i];
	 new_protocol->object.name[log.prb$table.size] = '\0';
	 if (valid(p_insert(new_protocol)))
	    protocol_header->reference_count += 1;
	 else
	    cfree(new_protocol);
	 break;
      case 2:
	 new_node = calloc(1,sizeof(N_BLOCK));
	 for (i = 0; i < 6; ++i)
	    new_node->object.address[i] = log.prb$table.object.node.address[i];
	 for (i = 0; i < log.prb$table.size; ++i)
	    new_node->object.name[i] = log.prb$table.name[i];
	 new_node->object.name[log.prb$table.size] = '\0';
	 if (valid(n_insert(new_node)))
	    node_header->reference_count += 1;
	 else
	    cfree(new_node);
	 break;
      case 3:
	 new_vendor = calloc(1,sizeof(V_BLOCK));
	 for (i = 0; i < 3; ++i)
	    new_vendor->object.prefix[i] = log.prb$table.object.vendor.prefix[i];
	 for (i = 0; i < log.prb$table.size; ++i)
	    new_vendor->object.name[i] = log.prb$table.name[i];
	 new_vendor->object.name[log.prb$table.size] = '\0';
	 if (valid(v_insert(new_vendor)))
	    vendor_header->reference_count += 1;
	 else
	    cfree(new_vendor);
	 break;
      default:
	 break;
      }
}

/******************************************************************************
 * READ_TABLE_DEFINITIONS                                                     *
 *                                                                            *
 * As required, this routine reads the table entry definitions and dispatches *
 * execution until the end-of-file is reached.                                *
 ******************************************************************************/

read_table_definitions()
{
 globalref PROBE_TABLE;		/* external Command Table module address */
 globalref RPL_TBLFLT;
 extern catch();

 long status;
 unsigned long count = 0;
 unsigned char line[134];
 D_VARIABLE(definition,line);

/* as required, process the file to its end */

if (command.process_external_table)
   {
   while (fgets(line,133,table) != NULL)
      {
      count += 1;
      definition.dsc$w_length = strlen(line) - 1;
      lib$establish(&catch);				/* prevent CLI signaling */
      if (valid(status = cli$dcl_parse(&definition,&PROBE_TABLE,0,0,0)))
	 {
	 lib$revert();					/* resume normal signal behavior */
         cli$dispatch();				/* execute command */
	 }
      else
	 {
	 lib$revert();					/* resume normal signal behavior */
	 if (status != CLI$_NOCOMD)
	    lib$signal(&RPL_TBLFLT,1,count);
	 }
      }
   command.process_external_table = 0;			/* indicate end of external processing */
   }
}

/******************************************************************************
 * FORMAT_ENVIRONMENT                                                         *
 *                                                                            *
 * Output the sample characteristics and node & protocol definitions          *
 * according to defaults and user specified request.                          *
 ******************************************************************************/

format_environment()
{
 globalref RPL_ANAPROG;
 extern char * display_time();

 int i;
 unsigned long entity;
 N_BLOCK *node;
 P_BLOCK *protocol;
 V_BLOCK *vendor;

if (command.analysis_redirected)
   lib$signal(&RPL_ANAPROG);
if (command.is_format_header)
   {
   fprintf(analysis,"PROBE/PLAYBACK/ANALYZE 2.3\n\n");
   fprintf(analysis,"Input file: %s\nRecording:  %s   (%d.%d-%d)",s_file,
 	   ((command.sample_was_extracted) ? "extracted" : "original "),revision.id.major,revision.id.minor,patch);
   if ((command.is_format_data) && (log_payload_size < eth$max_data))
      fprintf(analysis," with payload data truncated @ %d bytes",log_payload_size);
   fprintf(analysis,"\n\nMade:       %s\nOn Node:    %-8s\t(%s)\nAdapter:    %-8s\n\n",
	   display_time(master_cycle->block.sample.creation_time),log_node,
	   ((command.platform) ? "an Alpha" : "a VAX"),adapter);
   if (master_cycle->block.sample.activity != 0)
      fprintf(analysis,"Cycles:     %-5d\t(%d sec Standby) + (%d sec Activity)\n\n",
	      master_cycle->block.sample.max_id,master_cycle->block.sample.standby,master_cycle->block.sample.activity);
   else
      fprintf(analysis,"Cycles:     %-5d\t(manually terminated)\n\n",master_cycle->block.sample.max_id);
   }
if (command.is_format_table)
   {
   for (entity = 0, protocol = protocol_header->next; protocol != protocol_header; ++entity, protocol = protocol->next)
      fprintf(analysis,"Protocol %-12s\t(%02X-%02X) ID: %-6d%s",protocol->object.name,
	      protocol->object.value[0],protocol->object.value[1],protocol->object.id,(odd(entity) ? "\n" : "\t\t\t"));
   for (vendor = vendor_header->next; vendor != vendor_header; ++entity, vendor = vendor->next)
      {
      fprintf(analysis,"Vendor   %-12s\t(",vendor->object.name);
      for (i = 0; i < 3; ++i)
	 fprintf(analysis,"%02X%c",vendor->object.prefix[i],((i < 2) ? '-' : ')'));
      fprintf(analysis,"%s",(odd(entity) ? "\n" : "\t\t\t\t"));
      }
   for (node = node_header->next; node != node_header; ++entity, node = node->next)
      {
      fprintf(analysis,"Node     %-12s\t(",node->object.name);
      for (i = 0; i < 6; ++i)
	 fprintf(analysis,"%02X%c",node->object.address[i],((i < 5) ? '-' : ')'));
      fprintf(analysis,"%s",(odd(entity) ? "\n" : "\t\t\t"));
      }
   if (entity != 0)
      fprintf(analysis,"%s",(odd(entity) ? "\n\n" : "\n"));
   }
}

/******************************************************************************
 * FORMAT_SAMPLE_DATA                                                         *
 *                                                                            *
 * Output the specified cycle sample data. No load occurs.                    *
 ******************************************************************************/

format_sample_data()
{
 globalref RPL_OUTOFSEQ, RPL_BADDATA;
 extern char * display_time();

 int i, line;
 unsigned long max_index, index, sequence;
 unsigned char payload[eth$max_data];
 C_BLOCK *cycle;
 N_BLOCK *source, *target;
 P_BLOCK *protocol;
 struct
    {
    unsigned do_this_cycle : 1;
    unsigned p_is_matched : 1;
    unsigned s_is_matched : 1;
    unsigned t_is_matched : 1;
    unsigned : 0;
    } state;

state.do_this_cycle = 0;
cycle = master_cycle->next;
do {
   switch (log.type)
      {
      case 0:
	 frame_count += 1;
	 if (state.do_this_cycle)
	    {
	    cycle->reference_count += 1;
	    if (command.is_format_frame)
	       {
	       state.s_is_matched = 0;
	       for (source = node_header; (source->next != node_header) && !state.s_is_matched; source = source->next)
		  for (i = 5; i >= 0; --i)
		     if (!(state.s_is_matched = (source->next->object.address[i] == log.prb$frame.source[i])))
			break;
	       if (!state.s_is_matched)
		  {
		  source = calloc(1,sizeof(N_BLOCK));
		  build_node(source,log.prb$frame.source);
		  }
	       state.t_is_matched = 0;
	       for (target = node_header; (target->next != node_header) && !state.t_is_matched; target = target->next)
		  for (i = 5; i >= 0; --i)
		     if (!(state.t_is_matched = (target->next->object.address[i] == log.prb$frame.destination[i])))
			break;
	       if (!state.t_is_matched)
		  {
		  target = calloc(1,sizeof(N_BLOCK));
		  build_node(target,log.prb$frame.destination);
		  }
	       index = 0;				/* initialize payload array index */
	       if (log.prb$frame.tag.is_ieee)
		  {
		  fprintf(analysis,"%sFrame %010d (IEEE)     @ %08.2f  From: %-17s  To: %-17s  Size: ",
			  (log.prb$frame.tag.warning ? "?" : " "),frame_count,log.prb$frame.time,
			  source->object.name,target->object.name);
		  if (log.prb$frame.protocol[0] == log.prb$frame.protocol[1])
		     switch (log.prb$frame.protocol[0])
			{
			case 0xAA:			/* Ethernet SNAP (IPX = AAAA 03 000000 8137 FFFF ...) */
			   state.p_is_matched = 0;
			   for (protocol = protocol_header; (protocol->next != protocol_header) && !state.p_is_matched;
				protocol = protocol->next)
			      for (i = 1; i >= 0; --i)
				 if (!(state.p_is_matched = (protocol->next->object.value[i] == log.prb$frame.ctlpi[i+4])))
				    break;
			   if (state.p_is_matched)
			      fprintf(analysis,"%4d/%-4d  Protocol: %s (SNAP)\n",
				      log.prb$frame.size,(log.prb$frame.size+log.prb$frame.tag.overhead < eth$min_data ?
				      eth$min_frame : log.prb$frame.size+log.prb$frame.tag.overhead+eth$envelop),
				      protocol->object.name);
			   else
			      fprintf(analysis,"%4d/%-4d  Protocol: %02X-%02X (SNAP)\n",
				      log.prb$frame.size,(log.prb$frame.size+log.prb$frame.tag.overhead < eth$min_data ?
				      eth$min_frame : log.prb$frame.size+log.prb$frame.tag.overhead+eth$envelop),
				      log.prb$frame.ctlpi[4],log.prb$frame.ctlpi[5]);
			   break;
			case 0xE0:			/* 802.2 LLC (IPX = E0E0 03 FFFF ...) */
			   fprintf(analysis,"%4d/%-4d  D-S SAPs: %02X-%02X (IPX802.2)\n",
				   log.prb$frame.size,(log.prb$frame.size+log.prb$frame.tag.overhead < eth$min_data ?
				   eth$min_frame : log.prb$frame.size+log.prb$frame.tag.overhead+eth$envelop),
				   log.prb$frame.protocol[0],log.prb$frame.protocol[1]);
			   break;
			case 0xFF:			/* 802.3 raw (IPX = FFFF ...) */
			   log.prb$frame.size = log.prb$frame.size + log.prb$frame.tag.overhead;
			   fprintf(analysis,"%4d/%-4d  D-S SAPs: none  (IPX802.3)\n",
				   log.prb$frame.size,(log.prb$frame.size < eth$min_data ?
				   eth$min_frame : log.prb$frame.size+eth$envelop));
			   for ( ; index < log.prb$frame.tag.overhead; ++index)
			      if (index < 2)
				 payload[index] = log.prb$frame.protocol[index];
			      else
				 payload[index] = log.prb$frame.ctlpi[index-2];
			   break;
			default:			/* 802.2 LLC */
			   fprintf(analysis,"%4d/%-4d  D-S SAPs: %02X-%02X\n",
				   log.prb$frame.size,(log.prb$frame.size+log.prb$frame.tag.overhead < eth$min_data ?
				   eth$min_frame : log.prb$frame.size+log.prb$frame.tag.overhead+eth$envelop),
				   log.prb$frame.protocol[0],log.prb$frame.protocol[1]);
			   break;
			}
		  else 					/* 802.2 LLC */
		     fprintf(analysis,"%4d/%-4d  D-S SAPs: %02X-%02X\n",
			     log.prb$frame.size,(log.prb$frame.size+log.prb$frame.tag.overhead < eth$min_data ?
			     eth$min_frame : log.prb$frame.size+log.prb$frame.tag.overhead+eth$envelop),
			     log.prb$frame.protocol[0],log.prb$frame.protocol[1]);
		  }
	       else 					/* ethernet frame type */
		  {
		  fprintf(analysis,"%sFrame %010d (Ethernet) @ %08.2f  From: %-17s  To: %-17s  Size: %4d/%-4d  Protocol: ",
			  (log.prb$frame.tag.warning ? "?" : " "),frame_count,log.prb$frame.time,
			  source->object.name,target->object.name,log.prb$frame.size,
			  (log.prb$frame.size+log.prb$frame.tag.overhead < eth$min_data ?
			   eth$min_frame : log.prb$frame.size+log.prb$frame.tag.overhead+eth$envelop));
		  state.p_is_matched = 0;
		  for (protocol = protocol_header; (protocol->next != protocol_header) && !state.p_is_matched;
		       protocol = protocol->next)
		     for (i = 1; i >= 0; --i)
			if (!(state.p_is_matched = (protocol->next->object.value[i] == log.prb$frame.protocol[i])))
			   break;
		  if (state.p_is_matched)
		     fprintf(analysis,"%s\n",protocol->object.name);
		  else
		     fprintf(analysis,"%02X-%02X\n",log.prb$frame.protocol[0],log.prb$frame.protocol[1]);
		  }
	       if ((command.is_format_data) && (log.prb$frame.tag.is_data))
		  if (log.prb$frame.tag.is_more)	/* store contents until end-of-data block 2 */
		     {
		     sequence = 0;
		     if (log.prb$frame.size < log_payload_size)
			max_index = log.prb$frame.size;
		     else
			max_index = log_payload_size;
		     for (i = 0; ((i < prb$log_capacity) && (index < max_index)); ++i, ++index)
			payload[index] = log.prb$frame.data[i];
		     }
		  else					/* no additional data blocks... format output */
		     {
		     max_index = prb$log_capacity;
		     if (log.prb$frame.size < max_index)
			max_index = log.prb$frame.size;
		     if (log_payload_size < max_index)
			max_index = log_payload_size;
		     for (i = 0; ((i < prb$log_capacity) && (index < max_index)); ++i, ++index)
			payload[index] = log.prb$frame.data[i];
	             fprintf(analysis,"  Data 0000> ");
		     for (i = 0; i < 25; ++i)
			if (i < max_index)
			   fprintf(analysis,"%02X%s",payload[i],(i < max_index-1 ? "," : " "));
			else
			   fprintf(analysis,"   ");
		     fprintf(analysis," ASCII> ");
		     for (i = 0; i < max_index; ++i)
			if ((payload[i] < 0x20) || (payload[i] >= 0x7F))
			   fprintf(analysis,".");
			else
			   fputc(payload[i],analysis);
		     fprintf(analysis,"\n");
		     }
	       }
	    }
	 break;
      case 2:	/* last data block... format output */
	 if ((command.is_format_data) && (state.do_this_cycle))
	    if (sequence == log.prb$data.sequence-1)
	       {
	       for (i = 0; ((i < prb$entry_size-2) && (index < max_index)); ++i, ++index)
		  payload[index] = log.prb$data.content[i];
	       for (line = 0; line*25 < max_index ; ++line)
		  {
		  fprintf(analysis,"  Data %04d> ",line*25);
		  for (i = line*25; i < (line*25)+25; ++i)
		     if (i < max_index)
			fprintf(analysis,"%02X%s",payload[i],(i < max_index-1 ? "," : " "));
		     else
			fprintf(analysis,"   ");
		  fprintf(analysis," ASCII> ");
		  for (i = line*25; ((i < (line*25)+25) && (i < max_index)); ++i)
		     if ((payload[i] < 0x20) || (payload[i] >= 0x7F))
			fprintf(analysis,".");
		     else
			fputc(payload[i],analysis);
		  fprintf(analysis,"\n");
		  }
	       }
	    else /* sequence of data block lost */
	       lib$signal(&RPL_OUTOFSEQ);
	 break;
      case 4:
	 if ((command.is_format_collision) && (state.do_this_cycle))
	    fprintf(analysis," CD report %d from %s - Frames Xmit: %-10d  Deferred: %-10d   C1: %-10d   C2+: %-10d\n",
		    log.prb$collision.sequence,display_time(log.prb$collision.abstime),log.prb$collision.fxmitted,
		    log.prb$collision.fdeferred,log.prb$collision.single,log.prb$collision.multiple);
	 break;
      case 5:
	 if (cycle->block.object.id == log.prb$start.sequence)
	    {
	    if (command.is_format_cycle)
	       fprintf(analysis,"Cycle %d started %s\n",log.prb$start.sequence,display_time(log.prb$start.abstime));
	    state.do_this_cycle = 1;
	    }
	 else
	    if (command.is_format_cycle)
	       fprintf(analysis,"Cycle %d ignored\r",log.prb$start.sequence);
	 break;
      case 6:	/* spanning data block... store contents until end-of-data block 2 */
	 if ((command.is_format_data) && (state.do_this_cycle))
	    if (sequence == log.prb$data.sequence-1)
	       {
	       sequence += 1;
	       for (i = 0; ((i < prb$entry_size-2) && (index < eth$max_data)); ++i, ++index)
		  payload[index] = log.prb$data.content[i];
	       }
	    else	/* sequence of data block lost */
	       lib$signal(&RPL_OUTOFSEQ);
	 break;
      case 7:
	 if (state.do_this_cycle)
	    {
	    if (command.is_format_cycle)
	       fprintf(analysis,"Cycle %d stopped %s (frames: %d)\n",
		       log.prb$stop.sequence,display_time(log.prb$stop.abstime),cycle->reference_count);
	    state.do_this_cycle = 0;
	    cycle = cycle->next;
	    }
	 break;
      default:
	 lib$signal(&RPL_BADDATA);
      }
   } while ((cycle != master_cycle) && (get_data(&log.element,log_record_size)));
}

/******************************************************************************/
/*************************** SECOND LEVEL ROUTINES ****************************/
/******************************************************************************/

/******************************************************************************
 * CATCH                                                                      *
 *                                                                            *
 * Intercept CLI routine signal and replace by SS$_CONTINUE so that messaging *
 * is explicitely controlled by this program. Since DCL_PARSE also returns an *
 * error status, use that value to determine command line validity.           *
 ******************************************************************************/

catch()
{
return(SS$_CONTINUE);
}

/******************************************************************************
 * DISPLAY_TIME                                                               *
 *                                                                            *
 * Converts an input VAX binary time into its equivalent ASCII representation.*
 ******************************************************************************/

char * display_time(value)
 unsigned long *value;
{
 static char result[24];
 static D_VARIABLE(time,result);
 static short length;

if (valid(sys$asctim(&length,&time,value,0)))
   result[length] = '\0';
else
   result[0] = '\0';
return(result);
}

/******************************************************************************
 * UPDATE_NTABLE                                                              *
 *                                                                            *
 * Parses a node definition into a new node block and attempts insertion into *
 * the sorted master node linked list.                                        *
 ******************************************************************************/

update_ntable()
{
 globalref RPL_BADNADR, RPL_NULNNAM;
 extern x_load();

 unsigned short a_size;
 unsigned short n_size;
 unsigned char  string[prb$object_name_size+1];

 N_BLOCK *new_node;

 D_CONSTANT(n_nam,"NODE_NAM");
 D_CONSTANT(n_adr,"NODE_ADR");
 D_VARIABLE(input,string);

new_node = calloc(1,sizeof(N_BLOCK));
if (cli$present(&n_adr) == CLI$_PRESENT)			/* process specified address */
   if (!((cli$get_value(&n_adr,&input,&a_size) == SS$_NORMAL) &&
	 (valid(x_load(string, a_size, new_node->object.address, 6)))))
      {
      cfree(new_node);
      lib$signal(&RPL_BADNADR);
      return(GOOD);
      }
if (cli$present(&n_nam) == CLI$_PRESENT)
   if (cli$get_value(&n_nam,&input,&n_size) == SS$_NORMAL)	/* process specified name */
      {
      string[n_size] = '\0';
      strcpy(new_node->object.name, string);
      }
   else
      lib$signal(&RPL_NULNNAM);
if (valid(n_insert(new_node)))					/* connect */
   node_header->reference_count += 1;
else								/* release */
   cfree(new_node);
return(GOOD);
}

/******************************************************************************
 * UPDATE_PTABLE                                                              *
 *                                                                            *
 * Parses a protocol definition into a new protocol block and attempts        *
 * insertion into the sorted master protocol linked list.                     *
 ******************************************************************************/

update_ptable()
{
 globalref RPL_OVRPID, RPL_BADPVAL, RPL_NULPNAM;
 extern x_load();

 unsigned short i_size;
 unsigned short n_size;
 unsigned short v_size;
 unsigned char  string[prb$object_name_size+1];

 P_BLOCK *new_protocol;

 D_CONSTANT(p_nam,"PTCL_NAM");
 D_CONSTANT(p_idn,"PTCL_IDN");
 D_CONSTANT(p_val,"PTCL_VAL");
 D_VARIABLE(input,string);

new_protocol = calloc(1,sizeof(P_BLOCK));
new_protocol->object.id = 0;					/* unspecified protocol IDs default to zero */
if (cli$present(&p_idn) == CLI$_PRESENT)
   if (cli$get_value(&p_idn,&input,&i_size) == SS$_NORMAL)	/* process specified ID */
      {
      string[i_size] = '\0';
      new_protocol->object.id = atoi(string);
      }
   else      
      lib$signal(&RPL_OVRPID);
if (cli$present(&p_val) == CLI$_PRESENT)			/* process specified value */
   if (!((cli$get_value(&p_val,&input,&v_size) == SS$_NORMAL) &&
	 (valid(x_load(string, v_size, new_protocol->object.value, 2)))))
      {
      cfree(new_protocol);
      lib$signal(&RPL_BADPVAL);
      return(GOOD);
      }
if (cli$present(&p_nam) == CLI$_PRESENT)
   if (cli$get_value(&p_nam,&input,&n_size) == SS$_NORMAL)	/* process specified name */
      {
      string[n_size] = '\0';
      strcpy(new_protocol->object.name, string);
      }
   else
      lib$signal(&RPL_NULPNAM);
if (valid(p_insert(new_protocol)))				/* connect */
   protocol_header->reference_count += 1;
else								/* release */
   cfree(new_protocol);
return(GOOD);
}

/******************************************************************************
 * UPDATE_VTABLE                                                              *
 *                                                                            *
 * Parses a vendor definition into a new vendor block and attempts insertion  *
 * into the sorted master vendor linked list.                                 *
 ******************************************************************************/

update_vtable()
{
 globalref RPL_BADVPRF, RPL_NULVNAM;
 extern x_load();

 unsigned short p_size;
 unsigned short n_size;
 unsigned char  string[prb$object_name_size+1];

 V_BLOCK *new_vendor;

 D_CONSTANT(v_nam,"VEND_NAM");
 D_CONSTANT(v_prf,"VEND_OUI");
 D_VARIABLE(input,string);

new_vendor = calloc(1,sizeof(V_BLOCK));
if (cli$present(&v_prf) == CLI$_PRESENT)			/* process specified prefix */
   if (!((cli$get_value(&v_prf,&input,&p_size) == SS$_NORMAL) &&
	 (valid(x_load(string, p_size, new_vendor->object.prefix, 3)))))
      {
      cfree(new_vendor);
      lib$signal(&RPL_BADVPRF);
      return(GOOD);
      }
if (cli$present(&v_nam) == CLI$_PRESENT)
   if (cli$get_value(&v_nam,&input,&n_size) == SS$_NORMAL)	/* process specified name */
      {
      string[n_size] = '\0';
      strcpy(new_vendor->object.name, string);
      }
   else
      lib$signal(&RPL_NULVNAM);
if (valid(v_insert(new_vendor)))				/* connect */
   vendor_header->reference_count += 1;
else								/* release */
   cfree(new_vendor);
return(GOOD);
}

/******************************************************************************
 * C_CONNECT                                                                  *
 *                                                                            *
 * Insert a new cycle block into the cycle linked list.                       *
 ******************************************************************************/

c_connect(new)
 C_BLOCK *new;
{
 globalref RPL_DUPCID; 
 C_BLOCK *cycle;

/* first, scan the cycle list until the next item ID is larger or equal to the new entry */

for (cycle = master_cycle;
     ((cycle->next != master_cycle) && (cycle->next->block.object.id < new->block.object.id));
     cycle = cycle->next) {}
if (cycle->next->block.object.id == new->block.object.id)	/* reject duplicated ID */
   {
   lib$signal(&RPL_DUPCID,1,new->block.object.id);
   return(BAD);
   }
else								/* insert into list */
   {
   new->next = cycle->next;					/* connect forward */
   new->previous = cycle;					/* connect backward */
   new->next->previous = new;					/* link backward chain to entry */
   new->previous->next = new;					/* link forward chain to entry */
   return(GOOD);
   }
}

/******************************************************************************
 * N_CONNECT                                                                  *
 *                                                                            *
 * Add node entry in sorted order by name (1) and address (2).                *
 ******************************************************************************/

n_connect(new)
 N_BLOCK *new;
{
 N_BLOCK *node;

/* primary sort */

for (node = node_header;
     ((node->next != node_header) && (strcmp(node->next->object.name, new->object.name) < 0));
     node = node->next) {}

/* subsort as required */

for (;
     ((node->next != node_header) && (strcmp(node->next->object.name, new->object.name) == 0) &&
      (compare(&node->next->object.address, &new->object.address, 6) < 0)); node = node->next) {}

/* reconnect chain */

new->next = node->next;
node->next = new;
return(GOOD);
}

/******************************************************************************
 * N_INSERT                                                                   *
 *                                                                            *
 * Inspect the node linked list for an address match.                         *
 * Update found entry & resort list, or insert new block in proper order.     *
 ******************************************************************************/

n_insert(new)
 N_BLOCK *new;
{
 globalref RPL_UPDNDEF, RPL_LOOKATN; 
 N_BLOCK *node;

for (node = node_header; node->next != node_header; node = node->next)
   if (compare(&node->next->object.address, &new->object.address, 6) == 0)
      {
      if (strlen(new->object.name) != 0)			/* update if name specified & different */
	 {
	 if (strcmp(node->next->object.name, new->object.name) != 0)
	    {
	    lib$signal(&RPL_UPDNDEF,8,
		       new->object.address[0],new->object.address[1],new->object.address[2],new->object.address[3],
		       new->object.address[4],new->object.address[5],node->next->object.name,new->object.name);
	    strcpy(node->next->object.name, new->object.name);
	    new->next = node->next;				/* use new as a temporary holder of the updated block pointer */
	    node->next = node->next->next;			/* break & reconnect shortened chain */
	    n_connect(new->next);				/* reinsert updated block */           
	    }                                                   
	 }
      else							/* node status only */
	 lib$signal(&RPL_LOOKATN,8,
		    node->next->object.address[0],node->next->object.address[1],node->next->object.address[2],
		    node->next->object.address[3],node->next->object.address[4],node->next->object.address[5],
		    node->next->object.name,node->next->reference_count);
      return(BAD);						/* caller will deallocate unused new block */
      }
n_connect(new);
return(GOOD);
}

/******************************************************************************
 * P_CONNECT                                                                  *
 *                                                                            *
 * Add protocol entry in sorted order by name (1) and value (2).              *
 ******************************************************************************/

p_connect(new)
 P_BLOCK *new;
{
 P_BLOCK *protocol;

/* primary sort */

for (protocol = protocol_header;
     ((protocol->next != protocol_header) && (strcmp(protocol->next->object.name, new->object.name) < 0));
     protocol = protocol->next) {}

/* subsort as required */

for (;
     ((protocol->next != protocol_header) && (strcmp(protocol->next->object.name, new->object.name) == 0) &&
      (compare(&protocol->next->object.value, &new->object.value, 2) < 0)); protocol = protocol->next) {}

/* reconnect chain */

new->next = protocol->next;
protocol->next = new;
return(GOOD);
}

/******************************************************************************
 * P_INSERT                                                                   *
 *                                                                            *
 * Inspect the protocol linked list for a protocol match.                     *
 * Update found entry & resort list, or insert new block in proper order.     *
 ******************************************************************************/

p_insert(new)
 P_BLOCK *new;
{
 globalref RPL_UPDPDEF, RPL_LOOKATP;
 P_BLOCK *protocol;

for (protocol = protocol_header; protocol->next != protocol_header; protocol = protocol->next)
   if (compare(&protocol->next->object.value, &new->object.value, 2) == 0)
      {
      if ((new->object.id != 0) || (strlen(new->object.name) != 0))
	 {
	 if (((new->object.id != 0) && (protocol->next->object.id != new->object.id)) ||
	     ((strlen(new->object.name) != 0) && (strcmp(protocol->next->object.name, new->object.name) != 0)))
	    lib$signal(&RPL_UPDPDEF,6,
		       protocol->next->object.value[0],protocol->next->object.value[1],
		       protocol->next->object.name,protocol->next->object.id,new->object.name,new->object.id);
	 if (new->object.id != 0)
	    protocol->next->object.id = new->object.id;
	 if ((strlen(new->object.name) != 0) && (strcmp(protocol->next->object.name, new->object.name) != 0))
	    {							/* name change... resort */
	    strcpy(protocol->next->object.name, new->object.name);
	    new->next = protocol->next;				/* use new as a temporary holder of the updated block pointer */
	    protocol->next = protocol->next->next;		/* break & reconnect shortened chain */
	    p_connect(new->next);				/* reinsert updated block */           
	    }
	 }
      else							/* protocol status only */
	 lib$signal(&RPL_LOOKATP,5,
		    protocol->next->object.value[0],protocol->next->object.value[1],
		    protocol->next->object.name,protocol->next->object.id,protocol->next->reference_count);
      return(BAD);						/* caller will deallocate new unused block */
      }
p_connect(new);
return(GOOD);
}

/******************************************************************************
 * V_CONNECT                                                                  *
 *                                                                            *
 * Add vendor entry in sorted order by name (1) and prefix (2).               *
 ******************************************************************************/

v_connect(new)
 V_BLOCK *new;
{
 V_BLOCK *vendor;

/* primary sort */

for (vendor = vendor_header;
     ((vendor->next != vendor_header) && (strcmp(vendor->next->object.name, new->object.name) < 0));
     vendor = vendor->next) {}

/* subsort as required */

for (;
     ((vendor->next != vendor_header) && (strcmp(vendor->next->object.name, new->object.name) == 0) &&
      (compare(&vendor->next->object.prefix, &new->object.prefix, 3) < 0)); vendor = vendor->next) {}

/* reconnect chain */

new->next = vendor->next;
vendor->next = new;
return(GOOD);
}

/******************************************************************************
 * V_INSERT                                                                   *
 *                                                                            *
 * Inspect the vendor linked list for a prefix match.                         *
 * Update found entry & resort list, or insert new block in proper order.     *
 ******************************************************************************/

v_insert(new)
 V_BLOCK *new;
{
 globalref RPL_UPDVDEF, RPL_LOOKATV; 
 V_BLOCK *vendor;

for (vendor = vendor_header; vendor->next != vendor_header; vendor = vendor->next)
   if (compare(&vendor->next->object.prefix, &new->object.prefix, 3) == 0)
      {
      if (strlen(new->object.name) != 0)			/* update if name specified & different */
	 {
	 if (strcmp(vendor->next->object.name, new->object.name) != 0)
	    {
	    lib$signal(&RPL_UPDVDEF,5,
		       new->object.prefix[0],new->object.prefix[1],new->object.prefix[2],
		       vendor->next->object.name,new->object.name);
	    strcpy(vendor->next->object.name, new->object.name);
	    new->next = vendor->next;				/* use new as a temporary holder of the updated block pointer */
	    vendor->next = vendor->next->next;			/* break & reconnect shortened chain */
	    v_connect(new->next);				/* reinsert updated block */           
	    }                                                   
	 }
      else							/* vendor status only */
	 lib$signal(&RPL_LOOKATV,5,
		    vendor->next->object.prefix[0],vendor->next->object.prefix[1],vendor->next->object.prefix[2],
		    vendor->next->object.name,vendor->next->reference_count);
      return(BAD);						/* caller will deallocate unused new block */
      }
v_connect(new);
return(GOOD);
}

/******************************************************************************
 * COMPARE                                                                    *
 *                                                                            *
 * Compare two arbitrary arrays of a given size until divergence.             *
 * Similar to STRNCMP but is not affected by null end-of-string values.       *
 ******************************************************************************/

compare(first,second,size)
 unsigned char *first;
 unsigned char *second;
 unsigned long size;
{
 unsigned long i;
 long result = 0;

for (i = 0; ((i < size) && ((result = first[i] - second[i]) == 0)); ++i) {}
return(result);
}

/******************************************************************************
 * X_LOAD                                                                     *
 *                                                                            *
 * Converts an input character string into its equivalent network hexadecimal *
 * form. This is used during protocol value, node address and vendor prefix   *
 * processing.                                                                *
 ******************************************************************************/

x_load(input,size,output,max)
 unsigned char  *input;
 unsigned short size;
 unsigned char  *output;
 unsigned short max;
{
 long i, offset, nibble;
 unsigned char x;

if (max == 0)							/* sanity check... */
   return(GOOD);
for (i = 0; i < max; ++i)					/* initialise output */
   output[i] = 0;
for (offset = 0; ((offset < size) && (input[offset] == '0')); ++offset) {}
if (offset == size)						/* string has null value... no more work */
   return(GOOD);
if ((nibble = max * 2) < (size-offset))				/* check for sufficient output space */
   return(BAD);
for (i = (size-1), --nibble; i >= offset; --i, --nibble)	/* find all characters corresponding hexadecimal value */
   {
   if ((input[i] >= '0') && (input[i] <= '9'))
      x = input[i] - '0';
   else
      if ((input[i] >= 'A') && (input[i] <= 'F'))
	 x = input[i] - 'A' + 10;
      else
	 if ((input[i] >= 'a') && (input[i] <= 'f'))
	    x = input[i] - 'a' + 10;
	 else							/* not a valid hexadecimal digit */
	    return(BAD);
   output[nibble/2] = output[nibble/2] | (odd(nibble) ? x : (x<<4));	/* insert in reverse byte order into output */
   }
return(GOOD);
}

/******************************************************************************
 * BUILD_NODE                                                                 *
 *                                                                            *
 * Builds a node name from the provided address if its prefix is from a known *
 * manufacturer or the logical DECnet Phase IV. Otherwise, construct the hex  *
 * representation of the address. Insert new node into the node list.         *
 ******************************************************************************/

build_node(new,address)
 N_BLOCK	*new;
 unsigned char	*address;
{
 union
    {
    unsigned short address;
    struct
       {
       unsigned number : 10;
       unsigned area : 6;
       } logical;
    } decnet;

 V_BLOCK  *vendor;
 unsigned int i;
 unsigned found;

for (i = 0; i < 6; ++i)
   new->object.address[i] = address[i];
if (odd(address[0]))	/* multicast */
   sprintf(new->object.name,"%02X-%02X-%02X-%02X-%02X-%02X",address[0],address[1],address[2],address[3],address[4],address[5]);
else
   if ((address[0] == 0xAA) && (address[1] == 0x00) && (address[2] == 0x04) && (address[3] == 0x00))
      {
      decnet.address = ((address[5] << 8) | address[4]);
      sprintf(new->object.name,"DECnet(%d.%d)",decnet.logical.area,decnet.logical.number);
      }
   else
      {
      found = 0;
      for (vendor = vendor_header; (vendor->next != vendor_header) && !found; vendor = vendor->next)
	 for (i = 0; i < 3; ++i)
	    if (!(found = (address[2-i] == vendor->next->object.prefix[2-i])))	/* test most variant byte first */
	       break;
      if (found)
	 sprintf(new->object.name,"%-.8s:%02X-%02X-%02X",vendor->object.name,address[3],address[4],address[5]);
      else
	 sprintf(new->object.name,"%02X-%02X-%02X-%02X-%02X-%02X",
	         address[0],address[1],address[2],address[3],address[4],address[5]);
      }
n_connect(new);
return(GOOD);
}
