 #module BannerLink "V1.0"
 
 /*
 **++
 **  FACILITY:
 **
 **      The DECwindows Banner program
 **
 **  ABSTRACT:
 **
 **      This module contains all the code, to run NCP Link monitor.
 **
 **  AUTHORS:
 **
 **      JIM SAUNDERS
 **
 **
 **  CREATION DATE:     20th June 1988
 **
 **  MODIFICATION HISTORY:
 **--
 **/
 
 
 /*
 **
 **  INCLUDE FILES
 **
 **/
 
 
 #include "stdio.h"
 #include "MrmAppl.h"
*9I
 #include "jpidef.h"
*9E
 #include    	iodef
 #include    	ssdef
 #include    	stsdef
 #include    	descrip
 
 
 #include "Banner"
 
 
 /*
 **
 **  MACRO DEFINITIONS
 **
 **/
 
 /*
  * now our runtime data structures
  */
 
 extern	 Bnr$_Blk    Bnr;
 extern	 Clk$_Blk    Clk;
 extern   Link$_Blk    Link;
 extern	 GC	     BannerGC;
 extern	 XGCValues   GcValues;
 
 static   Link$_Blk   New_Link;
 
 
 static int Link_update_count, Link_update_number;
 static int Link_init = 0;
*2I
 static int Link_nml_init = 0;
*2E
 
 /*	Brief description:
 
 	To get the equivalent result of the NCP command  "SHOW KNOW LINK" from
 	NICE, the command to be issued must be build joining the following codes
 	into the NCB to write by a $QIO through mailbox :
 
 	22 04 20 07 -1
 	 \  \  \  \   \
 	  \  \	\  \   KNOW (Information type)
 	   \  \	 \  LINK (Entity type)
 	    \  \  READ (Requested operation)
 	     \  VMS (Operating System)
 	      FUNCTION CODE (system specific function)
 */
 				/* Fields for building the NICE version into the
 				   NCB sent to NML for opening the new link */
 #define		NICE$K_NICEVER	    4
 #define		NICE$K_DECECO	    0
 #define		NICE$K_USERECO	    0
 
 				/* Return status codes found in first byte of
 				   each block transmitted by NICE */
 
 #define		NICE$K_SUCCESS	    ((unsigned char) 1)
 #define		NICE$K_ACCEPTED	    ((unsigned char) 2)
 #define		NICE$K_PARTIAL	    ((unsigned char) 3)
 #define		NICE$K_NOPRIV	    ((unsigned char) -3)
 #define		NICE$K_UNRECCOMP    ((unsigned char) -8)
 #define		NICE$K_FILEOPENERR  ((unsigned char) -13)
 #define		NICE$K_OPFAILED	    ((unsigned char) -25)
 #define		NICE$K_DONE	    ((unsigned char) -128)
 #define		NICE$K_NOSTATUS	    ((unsigned short int) ~0)
 
 				/* Entities definition to identify which
 				   information are to be returned by NICE */
 #define		NICE$K_EN_NODE	    0
 #define		NICE$K_EN_LINE	    1
 #define		NICE$K_EN_LOGGING   2
 #define		NICE$K_EN_CIRCUIT   3
 #define		NICE$K_EN_MODULE    4
 #define		NICE$K_EN_AREA	    5
 #define		NICE$K_EN_LINK	    7
 				/* Node identification stuff
 			if >0 then it's the start of the ASCII nodename string.
 */
 							/* KNOW NCP command */
 #define		NICE$K_NI_KNOWN		((unsigned char) -1)
 						/* NICE operation codes */
 #define		NICE$K_OP_WRITE	   19
 #define		NICE$K_OP_READ	   20
 #define		NICE$K_OP_FUNCTION 22
 				
 #define		NICE$K_OP_VMS      4		/* Operating System type */
 
 							/* $QIO parameters */
 #define		RUNT$S_BUFFER	512
 #define		RUNT$S_NCBDATA	17
 #define		RUNT$S_CHANNEL	2
 
 static	char		CC[2];
 static	int		ii, jj;
 static	unsigned char	*src_node = "0";	/* To use the current node */
 
 static	unsigned char	pNCB_1 [RUNT$S_BUFFER];
 static	unsigned char	*pNCB = pNCB_1;
 static	unsigned char	*sNCB;
 
 static	unsigned short	src_chan;
 static	unsigned short	iosb[4];
 
 static	unsigned long	RetStatus;
 
 typedef struct	_dsc$descriptor1 {
 	    short int	dsc$w_length;
 	    char	type;
 	    char	class;
 	    int		string;
 	    } dsc$descriptor1;
 
*2D
 static  dsc$descriptor1	dncb =
     {
 	    0,
 	    DSC$K_DTYPE_T, DSC$K_CLASS_S,
 	    pNCB
     };
*2E
*2I
 static  dsc$descriptor1	dncb;
*2E
 
 struct	Node_Str {
    char			Node_Number_Length;
    unsigned short	Node_Number;
    char			Dummy;
    unsigned char	Node_Lenght;
    char			Node_Name[7];
 };
 struct	PID_Str {
    unsigned long	PID_Value;
 };
 struct	Rem_Link_Str {
    unsigned short 	Rem_Link;
 };
 struct	Loc_Link_Str {
    short		Dummy1;
    unsigned short	Loc_Link;
 };
 struct	Rem_User_Str {
    unsigned char	User_Lenght;
    char			User_Name[13];
 };
 struct	Proc_Str {
    unsigned char	Proc_Lenght;
    char			Proc_Name[13];
 };
 
 typedef struct _ptype {
        unsigned short 	Field_Id;
        unsigned char        Field_Type;
        union {
 	  struct	Node_Str	F_Node;
 	  struct	PID_Str		F_PID;
 	  struct	Rem_Link_Str	F_Rem_Link;
 	  struct	Loc_Link_Str	F_Loc_Link;
 	  struct	Rem_User_Str	F_User;
 	  struct	Proc_Str	F_Process;
        } VarPart;
     } ptype;
 
 static ptype *pField;
 
 typedef  struct _dtype {
*9D
        unsigned short	Local_Link;
*9E
*9I
        unsigned short		Local_Link;
*9E
        char			Remote_Node[7];
*9D
        long int		Local_PID;
*9E
*9I
        long int			Local_PID;
*9E
        char			Process[13];
*9D
        unsigned short	Remote_Link;
*9E
*9I
        unsigned short		Remote_Link;
*9E
        char	    		User[16];
     } dtype;
 
*9I
 typedef struct _vms_item_list {
 	short	 buffer_length;
 	short	 item_code;
 	void	*buffer;
 	int	*returned_buffer_length;
 	} VMS_ItemList;
 
*9E
 static dtype Data_Line;
*2D
 
*2E
*2I
 static int xsize, spacesize, p1start, p2start, p3start, p4start, p5start, p6start;
*2E
*9I
 static our_pid, our_link;
*9E
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerLinkSetup ()
 **
 **      This is the setup routine for the Banner Link monitor, it initialises all
 **	the data structurtes for the monitor section of Banner.
 **
 **  FORMAL PARAMETERS:
 **
 **      none
 **
 **  IMPLICIT INPUTS:
 **
 **      none
 **
 **  IMPLICIT OUTPUTS:
 **
 **      none
 **
 **  COMPLETION CODES:
 **
 **      none
 **
 **  SIDE EFFECTS:
 **
 **      none
 **
 **--
 **/
 int
 BannerLinkSetup()
 {
*2I
*9I
     VMS_ItemList itemlist[2] = {
 	4,  JPI$_PID, &our_pid, 0,
 	0,  0, 0, 0};
*9E
     int xsize;
*2E
*9I
     int status;
*9E
 
*9I
     if ( (status = SYS$GETJPIW (NULL, NULL, NULL, &itemlist, NULL, NULL, NULL))
 	 != 1)
 	BannerSignalError ("Failed to get our pid for SYS$GETJPI service",
 	    status);
     
*9E
     Link.Lines[0].LastLine = 1;
 
     Link.Lines[0].MaxNameSize = 100;
*2D
     Link.Lines[0].MaxNamePoints = 
 	XTextWidth(Bnr.font_ptr, "XXXXXXXXXXX", 8) * 10;
 
     strcpy(pNCB, src_node);
     strcat(pNCB, "::\"NML=/");
 
     memset (pNCB_1, 0, sixeof (pNCB_1));
 
     dncb.dsc$w_length = strlen(pNCB);
     pNCB[dncb.dsc$w_length++] = 0;
     pNCB[dncb.dsc$w_length++] = 0;
 
     pNCB[dncb.dsc$w_length++] = 3;
     pNCB[dncb.dsc$w_length++] = NICE$K_NICEVER;
     pNCB[dncb.dsc$w_length++] = NICE$K_DECECO;
     pNCB[dncb.dsc$w_length++] = NICE$K_USERECO;
     dncb.dsc$w_length = strlen(pNCB) + RUNT$S_CHANNEL + RUNT$S_NCBDATA;
     pNCB[dncb.dsc$w_length++] = '\"';
 					    /* Assign a channel to NML */
     RetStatus = SYS$ASSIGN( &dncb, &src_chan, 0, 0 );
     if ( !$VMS_STATUS_SUCCESS( RetStatus ))	return( RetStatus );
 				    /* Build a NICE "show link" request */
 
*2E
*2I
     xsize = XTextWidth(Bnr.font_ptr, "X", 1);
     spacesize = XTextWidth(Bnr.font_ptr, " ", 1);
     Link.Lines[0].MaxNamePoints = xsize * 100;
 
*9D
     p2start = xsize*6 + spacesize*1;
*9E
*9I
     p2start = xsize*5 + spacesize*1;
*9E
*5D
     p3start = p2start + xsize*6 + spacesize*1;
*5E
*5I
     p3start = p2start + xsize*7 + spacesize*1;
*5E
     p4start = p3start + xsize*8 + spacesize*1;
     p5start = p4start + xsize*12 + spacesize*1;
*5D
     p6start = p5start + xsize*6 + spacesize*1;
*5E
*5I
     p6start = p5start + xsize*7 + spacesize*1;
*5E
 
     if (!Link_nml_init)
 	{
 
 	memset (pNCB_1, 0, sizeof (pNCB_1));
 
 	strcpy(pNCB, src_node);
 	strcat(pNCB, "::\"NML=/");
 
 
 	
 	dncb.dsc$w_length = strlen(pNCB);
 	dncb.type = DSC$K_DTYPE_T;
 	dncb.class = DSC$K_CLASS_S;
 	dncb.string = pNCB;
 
 	pNCB[dncb.dsc$w_length++] = 0;
 	pNCB[dncb.dsc$w_length++] = 0;
 
 	pNCB[dncb.dsc$w_length++] = 3;
 	pNCB[dncb.dsc$w_length++] = NICE$K_NICEVER;
 	pNCB[dncb.dsc$w_length++] = NICE$K_DECECO;
 	pNCB[dncb.dsc$w_length++] = NICE$K_USERECO;
 	dncb.dsc$w_length = strlen(pNCB) + RUNT$S_CHANNEL + RUNT$S_NCBDATA;
 	pNCB[dncb.dsc$w_length++] = '\"';
 
 	RetStatus = SYS$ASSIGN( &dncb, &src_chan, 0, 0 );
*10D
 	if ( !$VMS_STATUS_SUCCESS( RetStatus ))	return( RetStatus );
*10E
*10I
 	if ( !$VMS_STATUS_SUCCESS( RetStatus ))	
 	    {
 	    printf ("Banner failed to establish NML link to NCP\n");
 	    printf ("Link display turned off\n");
 	    Bnr.Param_Bits.link_window = 0;
 	    return( RetStatus );
 	    }
*10E
 					/* Build a NICE "show link" request */
 	Link_nml_init = 1;				    /* Assign a channel to NML */
 	}
 
     if (Bnr.link_update == 0)
 	Link_update_number = Bnr.cpu_update;
     else
 	Link_update_number = Bnr.link_update;
 
     Link_update_count = Link_update_number;
*2E
 
     Link_init = 1;
 
 
 }
*2I
 
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **      SetText
 **
 **  FORMAL PARAMETERS:
 **
 **      {@subtags@}
 **
 **  RETURN VALUE:
 **
 **      {@description or none@}
 **
 **  SIDE EFFECTS:
 **
 **      {@description or none@}
 **
 **  DESIGN:
 **
 **      {@description or none@}
 **
 **  [@logical properties@]...
 **
 **  [@optional function tags@]...
 **
 **--
 */
 static void SetText (out, p1, p2, p3, p4, p5, p6)
 char *out, *p1,*p2,*p3,*p4,*p5,*p6;
 {
 int size;
 
 strcpy (out, "");
 
 strcat (out, p1);    
 size = XTextWidth(Bnr.font_ptr, out, strlen(out));
 while (size < p2start)
     {
     strcat (out, " ");
     size += spacesize;
     }
 
 strcat (out, p2);    
 size = XTextWidth(Bnr.font_ptr, out, strlen(out));
 while (size < p3start)
     {
     strcat (out, " ");
     size += spacesize;
     }
 
 strcat (out, p3);    
 size = XTextWidth(Bnr.font_ptr, out, strlen(out));
 while (size < p4start)
     {
     strcat (out, " ");
     size += spacesize;
     }
 
 strcat (out, p4);    
 size = XTextWidth(Bnr.font_ptr, out, strlen(out));
 while (size < p5start)
     {
     strcat (out, " ");
     size += spacesize;
     }
 
 strcat (out, p5);    
 size = XTextWidth(Bnr.font_ptr, out, strlen(out));
 while (size < p6start)
     {
     strcat (out, " ");
     size += spacesize;
     }
 
 strcat (out, p6);    
 }
 
*2E
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerLinkRefresh ()
 **
 **      This routine rewrites all the static information display by the 
 **	Monitor section of Banner.
 **
 **  FORMAL PARAMETERS:
 **
 **
 **  IMPLICIT INPUTS:
 **
 **
 **  IMPLICIT OUTPUTS:
 **
 **      none
 **
 **  COMPLETION CODES:
 **
 **      none
 **
 **  SIDE EFFECTS:
 **
 **      none
 **
 **--
 **/
 void	
 BannerLinkRefresh()
 {
 
     if (!Link_init)
 	BannerLinkSetup ();
 
*2D
     sprintf (Link.Lines[0].Text, 
 "  Link     Node         PID         Process        Remote Link   Remote User");
*2E
*2I
 
     SetText (Link.Lines[0].Text, "Link", "Node", "PID", "Process",
 	"Rem Link", "Remote User");
*2E
 
     BannerPaintTextLines (XtDisplay(Bnr.link_widget), XtWindow(Bnr.link_widget), 
 	Bnr.link_width, Bnr.link_height, 
 	&Link.Lines[0], 1);
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **      ReadNextLink
 **
 **  FORMAL PARAMETERS:
 **
 **      Text
 **
 **  RETURN VALUE:
 **
 **      {@description or none@}
 **
 **  SIDE EFFECTS:
 **
 **      {@description or none@}
 **
 **  DESIGN:
 **
 **      {@description or none@}
 **
 **  [@logical properties@]...
 **
 **  [@optional function tags@]...
 **
 **--
 */
 static int ReadNextLink (char *text)
 {
        RetStatus = 0;
*8I
*9D
        memset (Data_Line, 0, sizeof(Data_Line));
*9E
*9I
        memset (&Data_Line, 0, sizeof(Data_Line));
*9E
*8E
        while (RetStatus != 1) {		/* until the end of block */
 	  switch (pField->Field_Id) {
 	   case (unsigned short) 0XFF01:
 	     Data_Line.Local_Link = pField->VarPart.F_Loc_Link.Loc_Link;
 	     pNCB  += 7;
 	     pField = pNCB  ;
 	     break;
 	   case (unsigned short) 0X0065:
 	     Data_Line.Local_PID = pField->VarPart.F_PID.PID_Value;
 	     pNCB  += 7;
 	     pField = pNCB;
 	     break;
 	   case (unsigned short) 0X0078:
 	     Data_Line.Remote_Link = pField->VarPart.F_Rem_Link.Rem_Link;
 	     pNCB  += 5;
 	     pField = pNCB;
 	     break;
 	   case (unsigned short) 0X0066:
 	     if (pField->Field_Type == 0XC2) {
 		jj = 0;
 		for (ii = 0; ii < pField->VarPart.F_Node.Node_Lenght;ii++) {
 		   Data_Line.Remote_Node[jj] = 
 				    pField->VarPart.F_Node.Node_Name[ii];
 		   jj++;
 		}
 		Data_Line.Remote_Node[jj] = '\0';
 		pNCB  += (8 + pField->VarPart.F_Node.Node_Lenght);
 		pField = pNCB; }
 	     else
 		 if (pField->Field_Type == 0XC1) {
 		    int area, node;
 
 		    area = (pField->VarPart.F_Node.Node_Number >> 10) & 63;
 		    node = (pField->VarPart.F_Node.Node_Number & 1023);
 		    sprintf (Data_Line.Remote_Node, "%d.%d", 
 			area, node);
 		    pNCB  += 6;
 		    pField = pNCB;
 		 }
 		 else {
 		    pNCB  += 8;
 		    pField = pNCB;
 		 }
 	     break;
 	   case (unsigned short) 0X0079:
 		jj = 0;
 	     for (ii = 0; ii < pField->VarPart.F_User.User_Lenght;ii++) {
 		Data_Line.User[jj] = pField->VarPart.F_User.User_Name[ii];
 		jj++;
 	     }
 		Data_Line.User[jj] = '\0';
 	     pNCB += (4 + pField->VarPart.F_User.User_Lenght);
 	     pField = pNCB;
 	     break;
 	   case (unsigned short) 0X0083:
 	     jj = 0;
 	     for (ii = 0; ii < pField->VarPart.F_Process.Proc_Lenght;ii++) {
 		Data_Line.Process[jj] = pField->VarPart.F_Process.Proc_Name[ii];
 		jj++;
 	     }                                 
 		Data_Line.Process[jj] = '\0';
 	     RetStatus = 1;
 	     break;
*5I
 	   default:
 	    RetStatus = 1;
 	    break;
*5E
 	  }
 	}
 
*2D
 sprintf(text, "\n%7d   %-7s    %X     %-16s   %7d     %-13s",
 	    Data_Line.Local_Link,
 	    Data_Line.Remote_Node,
 	    Data_Line.Local_PID,
 	    Data_Line.Process,
 	    Data_Line.Remote_Link,
 	    Data_Line.User);
*2E
*2I
     {
     char link[10];
     char pid [10];
     char rem [10];
*2E
*3I
     char name[20];
*3E
 
*2I
     sprintf (link, "%d", Data_Line.Local_Link);
*6D
     sprintf (pid, "%X", Data_Line.Local_PID);
*6E
*6I
     if (Data_Line.Local_PID > 0)
 	sprintf (pid, "%X", Data_Line.Local_PID);
     else
         strcpy (pid, " ");
*6E
     sprintf (rem, "%d", Data_Line.Remote_Link);
*3D
     SetText (text, link, Data_Line.Remote_Node, pid, Data_Line.Process,
*3E
*3I
*4D
     sprintf (name, "%-12s", Data_Line.Process);
 
*4E
*4I
     memset (name, 0, sizeof(name));
*7D
     memcpy (name, Data_Line.Process, 12);
*7E
*7I
     memcpy (name, Data_Line.Process, 11);
*7E
*4E
     SetText (text, link, Data_Line.Remote_Node, pid, name,
*3E
 	rem, Data_Line.User);
*4I
 
*9D
     if (strcmp (Data_Line.User, "NML") == 0 ||
         strncmp (name, "NML_", 4) == 0)
*9E
*9I
 /*
  * Dont show links generated by us.
  */
     if (Data_Line.Local_PID == our_pid)
 	{
 	our_link = Data_Line.Remote_Link;
 	return 0;
 	}
     if (Data_Line.Local_Link == our_link)
 	{
*9E
 	return 0;
*4E
*9I
 	}
*9E
     }
*2E
 return RetStatus;
 }
 
 
 /*
 **++
 **  FUNCTIONAL DESCRIPTION:
 **
 **	BannerLink ()
 **
 **      This routine runs the Monitor display of banner.
 **
 **  FORMAL PARAMETERS:
 **
 **      none
 **
 **  IMPLICIT INPUTS:
 **
 **      none
 **
 **  IMPLICIT OUTPUTS:
 **
 **      none
 **
 **  COMPLETION CODES:
 **
 **      none
 **
 **  SIDE EFFECTS:
 **
 **      none
 **
 **--
 **/
 int
 BannerLink()
 {
     char text[200];
     int j;
     int	 i;
 
     if (!Link_init)
 	BannerLinkRefresh ();
 
     Link_update_count --;
     if (Link_update_count > 0)
 	return;
 	
*2D
     Link_update_count = Bnr.link_update;
*2E
*2I
     Link_update_count = Link_update_number;
*2E
 
 /*
  * Start request for stats
  */
     dncb.dsc$w_length = 0;
     pNCB[dncb.dsc$w_length++] = NICE$K_OP_FUNCTION;
     pNCB[dncb.dsc$w_length++] = NICE$K_OP_VMS;
     pNCB[dncb.dsc$w_length++] = NICE$K_OP_READ;
     pNCB[dncb.dsc$w_length++] = NICE$K_EN_LINK;
     pNCB[dncb.dsc$w_length++] = NICE$K_NI_KNOWN;
 
 
 				    /* Send the NCP command to NML */
     RetStatus = SYS$QIOW( 0, src_chan, IO$_WRITEVBLK, iosb, 0, 0,
 		    pNCB, dncb.dsc$w_length, 0, 0, 0, 0 );
 
 				    /* Read the header block from NML */
     RetStatus = SYS$QIOW( 0, src_chan, IO$_READVBLK, iosb, 0, 0,
 		    pNCB, RUNT$S_BUFFER, 0, 0, 0, 0 );
     if (*pNCB != NICE$K_ACCEPTED)	return( SS$_ABORT );
 
 				    /* Read the first data block from NML */
     RetStatus = SYS$QIOW( 0, src_chan, IO$_READVBLK, iosb, 0, 0,
 		    pNCB, RUNT$S_BUFFER, 0, 0, 0, 0 );
     if (*pNCB != NICE$K_SUCCESS)		return( SS$_ABORT );
     if (*(unsigned short *) (pNCB + 1 ) != NICE$K_NOSTATUS)
 		    return( SS$_ABORT );
 
 /*
  * Now read each line, and update our 
  * structures
  */
*2D
     i = 1;
*2E
*2I
     i = 0;
*2E
     memcpy (&New_Link, &Link, sizeof (Link));
*3D
     while (!New_Link.Lines[i].LastLine)
*3E
*3I
     while (1)
*3E
 	{
 	New_Link.Lines[i].seen = 0;
*2D
 	New_Link.Lines[i].highlight = 0;
*2E
*2I
 	New_Link.Lines[i].text_changed = 0;
*2E
*3I
 	if (New_Link.Lines[i].LastLine)
 	    break;
*3E
 	i++;
 	}
 
*2I
     New_Link.Lines[0].seen = 1;
*5I
     New_Link.Lines[0].LastLine = 1;
*5E
 
*2E
     i = 1;
*4I
     j = 1;
*4E
 
     sNCB = pNCB;
     while (*pNCB != NICE$K_DONE) 
 	{
 	pField = pNCB;
 	if (ReadNextLink (text))
 	    {
*3I
 	    int found;
*3E
*4I
 	    int highlight;
*4E
 
*4D
 	    j = 0;
*4E
*3I
 	    found = 0;
*3E
*4I
 	    highlight = 0;
 	    i = 1;
*4E
 
*3D
 	    while (!New_Link.Lines[j].LastLine)
*3E
*3I
 	    while (1)
*3E
 		{
*4D
 		if (strcmp (New_Link.Lines[j].Text, text) == 0)
*4E
*4I
 		if (strcmp (Link.Lines[i].Text, text) == 0)
*4E
 		    {
*2I
*3I
 		    found = 1;
*3E
*4D
 		    if (New_Link.Lines[j].highlight)
 			{
 			New_Link.Lines[j].highlight = 0;
 			New_Link.Lines[j].text_changed = 1;
 			}
*4E
*2E
*4D
 		    New_Link.Lines[j].seen = 1;
 		    break;
*4E
*4I
 		    highlight = Link.Lines[i].highlight;
*4E
 		    }
*2I
*3I
*4D
 		if (New_Link.Lines[j].LastLine)
*4E
*4I
 		if (Link.Lines[i].LastLine)
*4E
 		    break;
*3E
*4D
 		j++;
*4E
*4I
 		i++;
*4E
*2E
 		}
*4I
 
 	    New_Link.Lines[j-1].LastLine = 0;
 
 	    sprintf (New_Link.Lines[j].Text, text);
 	    if (highlight)
 		New_Link.Lines[j].text_changed = 1;
 	    New_Link.Lines[j].LastLine = 1;
 	    New_Link.Lines[j].seen = 1;
 	    New_Link.Lines[j].highlight = 0;
 
*4E
*3D
 	    if (New_Link.Lines[j].LastLine)
*3E
*3I
 	    if (!found)
*3E
 		{
*4D
 		sprintf (New_Link.Lines[j+1].Text, text);
 		New_Link.Lines[j+1].text_changed = 1;
 		New_Link.Lines[j+1].LastLine = 1;
 		New_Link.Lines[j+1].seen = 1;
 		New_Link.Lines[j+1].highlight = 1;
 		New_Link.Lines[j].LastLine = 0;
*4E
*4I
 		New_Link.Lines[j].highlight = 1;
 		New_Link.Lines[j].text_changed = 1;
*4E
 		}
*4I
 	    j++;
*4E
 	    }
 	pNCB = sNCB;
 	RetStatus = SYS$QIOW( 0, src_chan, IO$_READVBLK, iosb, 0, 0,
 		    pNCB, RUNT$S_BUFFER, 0, 0, 0, 0 );
 	if (*pNCB != NICE$K_SUCCESS)		break;
 	if (*(unsigned short *) (pNCB + 1 ) != NICE$K_NOSTATUS)	break;
     }
 
*2D
 i = 1;
 j = 1;
*2E
*2I
 i = 0;
 j = 0;
*2E
 
*3D
 while (!New_Link.Lines[j].LastLine)
*3E
*3I
 while (1)
*3E
     {
     if (New_Link.Lines[j].seen)
 	{
*2D
 	memcpy (&Link.Lines[i], &New_Link.Lines[j], sizeof (New_Link.Lines[0]));
*2E
*2I
*3D
 	if (strcmp (Link.Lines[i].Text, New_Link.Lines[j].Text) != 0)
*3E
*3I
*4D
 	if (strcmp (Link.Lines[i].Text, New_Link.Lines[i].Text) != 0)
*4E
*4I
 	if (strcmp (Link.Lines[i].Text, New_Link.Lines[j].Text) != 0)
*4E
*3E
 	    New_Link.Lines[j].text_changed = 1;
 	memcpy (&Link.Lines[i], &New_Link.Lines[j], sizeof (TextDisplay$_Line));
*2E
 	i++;
 	}
*3I
     if (New_Link.Lines[j].LastLine)
 	break;
*3E
     j++;
     }
 
 Link.Lines[i].LastLine = 1;
 
 BannerPaintTextLines (XtDisplay(Bnr.link_widget), XtWindow(Bnr.link_widget), 
     Bnr.link_width, Bnr.link_height, 
     &Link.Lines[0], 0);
 }
