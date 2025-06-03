/*
**++
**  FACILITY:  WWW CGI Scripts
**
**  MODULE DESCRIPTION:
**
**      This mails the contents of a FORM POSTed to the URL invoking ths
**	program.  The url embeds the mail address as follows:
**
**	http://www.acornsw.com/htbin/cgi-mailto.exe/mailaddress
**
**	For for mail to local user's, the user's web directory must
**	contain the file .www_mailable before the script will deliver
**	mail to that account.
**
**  AUTHORS:
**
**      Dick Munroe
**	Acorn Software, Inc.
**	179 Great Rd.
**	Acton, Ma. 01720
**
**	(508) 266-9800
**	fax:  266-9707
**	
**	munroe@acornsw.com
**
**      David Jones
**	Ohio State University
**
**  CREATION DATE:  04-Dec-94
**
**  DESIGN ISSUES:
**
**      {@tbs@}
**
**  [@optional module tags@]...
**
**  MODIFICATION HISTORY:
**
**      0.000 Dick Munroe 04-Dec-94
**	    Initial Version Created.
**
**	0.001 David Jones 10-Dec-1994
**	    Added check for .www_mailable to web directory.
**	    
**	0.002 David Jones 6-Feb-1995
**	    Convert extra path info to subject line for mail message.
**	    Include REMOTE_ADDRESS info in mail message.
**
**	0.003 David Jones 28-Feb-1995
**	    Remove FROM_LINE from send_attribute call.
**
**	0.004 David Jones 16-May-1995
**	    Use new cgi_translate_path routine to verify .www_mailable.
**
**      0.005 D. Jones 21-Jun-1995
**	   Delete extra '\r's in cgi_printf() calls ('\n's are automatically
**	   converted to "\r\n" already.
**	[@tbs@]...
**--
*/

#if defined(__ALPHA)
#pragma nomember_alignment
#endif

/*
**
**  INCLUDE FILES
**
*/

#include <stdio.h>
#include <stat.h>

#include "cgi-mailto.h"
#include "cgilib.h"
/* #include "utl-itemlist.h" */
struct ItemList { short int length, code; char *buffer; long returnLength; };

#include <descrip.h>
#include <jpidef.h>
#include <lib$routines.h>
#include <maildef.h>
#include <ssdef.h>
#include <stdlib.h>
#include <str$routines.h>
#include <string.h>
#include <stsdef.h>

static char*				getField(char*) ;
static void				getFieldNameAndValue(char*,char**,char**) ;
static char*				getRepeatedFieldValue(char*) ;
static void				htmlStrcpy(char*, char*) ;
extern unsigned long			mail$send_begin() ;
extern unsigned long			mail$send_add_attribute() ;
extern unsigned long			mail$send_add_address() ;
extern unsigned long			mail$send_add_bodypart() ;
extern unsigned long			mail$send_message() ;
extern unsigned long			mail$send_end() ;

int main ( int argc, char **argv )
{
    char				buffer[1024] ;
    char				*content_data;
    int					content_length;
    char				*cp ;
    int					i ;
    struct ItemList			itemList[10] ;
    struct ItemList			nullItemList = {0, 0, 0, 0} ;
    unsigned long			sendContext = 0 ;
    int					status;
    char				*to ;
    char				*subject, *raddr;
    struct dsc$descriptor_d		xxxDesc = {0, DSC$K_DTYPE_T, DSC$K_CLASS_D, 0};

    status = cgi_init_env ( argc, argv );

    to = cgi_info( ("PATH_INFO") ) ;			/* PATH_INFO is of the form /mailaddress				*/
    to++ ;						/* Advance past the /							*/
    if (strchr (to, '@') != NULL)
    {
    	sprintf (buffer, TO_ADDRESS, to) ;
	subject = "";
	to = buffer ;					/* The address is one on a distant machine, so set up the mail		*/
							/* transport.								*/
    } else {
	/* Verify that user has enabled recieving mail */
	int xlate_len;
	stat_t s_buf;
	char control_doc[400], translation[800];
	subject = strchr ( to, '/' );
	if ( subject ) {
	    int k;
	    *subject++ = '\0';
	    for ( k = 0; subject[k]; k++ ) 
		if (subject[k] == '/') subject[k] = ' ';
	} else subject = "";

	sprintf ( control_doc, "(GET)/~%s/.www_mailable", to );
	status = cgi_translate_path ( control_doc, 1, translation,
		sizeof(translation)-1, &xlate_len );
	translation[xlate_len] = '\0';
	if ( (status&1) && (xlate_len > 0) ) {
	    /* If we can't stat file, treat as translation failure */
	    if ( stat ( translation, &s_buf ) < 0 ) xlate_len = 0;
	}
	if ( xlate_len == 0 ) {
	    cgi_begin_output ( 1 );
	    cgi_printf("content-type: text/plain\n\n");	/* CGI header */
	    cgi_printf ( 
		"Cannot send MAIL to %s, no .www_mailable file present\n",
			to );
	    cgi_printf ( "in user's web directory\n" );
	    cgi_printf ( "(%s)",  translation[0] ? translation : control_doc );
	    return 1;
	}
    }
    cgi_begin_output ( 1 );
    cgi_printf("content-type: text/plain\n\n");	/* CGI header */
    cgi_printf("Sending form data as MAIL to: %s\n",to);
    
    content_length = atoi(cgi_info ("CONTENT_LENGTH"));
    if (content_length == 0)
    {
	cgi_printf("Send failed: No data in CONTENT_LENGTH\n") ;
	return 1;
    }

    content_data = (char *) malloc(sizeof(char)*(content_length+1));
    status = cgi_read(content_data, content_length);
    if (status == 0)
    {
	cgi_printf("Send failed: No data from cgi_read\n") ;
	return 1;
    }

    status =
	mail$send_begin (
	    &sendContext,
	    &nullItemList,
	    &nullItemList) ;				/* Set up to construct the mail context.				 */
    if (!$VMS_STATUS_SUCCESS(status))
    {
	cgi_printf("Send failed: MAIL$SEND_BEGIN returned %%X%0X\n",status) ;
	return 1 ;
    } ;

    /*
    ** Now build up the attributes of the message.  
    */

    status = JPI$_USERNAME ;
    lib$getjpi (&status, 0, 0, 0, &xxxDesc,0) ;
    str$trim (&xxxDesc, &xxxDesc) ;

    i = -1 ;
    itemList[++i].length =		strlen(to) ;
    itemList[i].code =			MAIL$_SEND_TO_LINE ;
    itemList[i].buffer =		to ;
    itemList[i].returnLength =	0 ;
#ifdef DONT_NEED_SYSPRV
    itemList[++i].length =		xxxDesc.dsc$w_length ;
    itemList[i].code =			MAIL$_SEND_FROM_LINE ;
    itemList[i].buffer =	        xxxDesc.dsc$a_pointer ;
    itemList[i].returnLength =	0 ;
#endif
    itemList[++i].length =		strlen(subject);
    itemList[i].code =			MAIL$_SEND_SUBJECT ;
    itemList[i].buffer =		subject;
    itemList[i].returnLength =	0 ;
    itemList[++i].length =		0 ;
    itemList[i].code =			0 ;

    status =
	mail$send_add_attribute (
	    &sendContext,
	    &itemList,
	    &nullItemList) ;				/* Add the the message header.						 */
    if (!$VMS_STATUS_SUCCESS(status))
    {
	cgi_printf("Send failed: MAIL$SEND_ADD_ATTRIBUTE returned %%X%0X\n",status) ;
	return 1 ;
    } ;

    i = -1 ;
    itemList[++i].length =		strlen(to) ;
    itemList[i].code =			MAIL$_SEND_USERNAME ;
    itemList[i].buffer =		to ;
    itemList[i].returnLength =	0 ;
    itemList[++i] = nullItemList ;			/* The itemlist describes an address of a receipient.			 */

    status =
	mail$send_add_address (
	    &sendContext,
	    &itemList,
	    &nullItemList) ;				/* Add the sender of the message to the list of receipients.		 */
    if (!$VMS_STATUS_SUCCESS(status))
    {
	cgi_printf("Send failed: MAIL$SEND_ADD_ADDRESS returned %%X%0X\n",status) ;
	return 1 ;
    } ;

    /*
    ** Include address of sender.
    */
    raddr = cgi_info ( "REMOTE_ADDR" );
    if ( raddr ) {
	sprintf ( buffer, "REMOTE_ADDRESS: %s", raddr );
        i = -1 ;
        itemList[++i].length =	strlen(buffer) ;
        itemList[i].code =		MAIL$_SEND_RECORD ;
        itemList[i].buffer =	buffer ;
        itemList[i].returnLength =	0 ;
        itemList[++i] = nullItemList ;		/* The itemlist describes the body of the message.			 */
	    
        status = mail$send_add_bodypart (&sendContext, &itemList, &nullItemList) ;
							/* Add the body to the message.						 */
        if (!$VMS_STATUS_SUCCESS(status))
        {
	    cgi_printf("Send failed: MAIL$SEND_ADD_BODYPART returned %%X%0X\n",status) ;
	    return 1 ;
        } ;
    }
    /*
    ** The content data is of the form:
    **
    **	fieldName=fielddata[&fieldName=fielddata...]
    */
    
    for (cp = getField(content_data); cp; cp = getField(NULL))
    {
	char				*field ;
	int				j = 0 ;
	int				multiline = 0 ;
	char				*name ;
	char				*value ;

#ifdef DEBUG
	printf ("%s\n", cp) ;
	printf ("%d\n", strlen(cp)) ;
#endif
	getFieldNameAndValue(cp, &name, &value) ;

#ifdef DEBUG
	printf ("%p\n", name) ;
	printf ("%s\n", name) ;
	printf ("%d\n", strlen(name)) ;
	printf ("%p\n", value) ;
	printf ("%s\n", value) ;
	printf ("%d\n", strlen(value)) ;
#endif
	htmlStrcpy(name, name) ;
#ifdef DEBUG
	printf ("%s\n", name) ;
	printf ("%d\n", strlen(name)) ;
#endif
	htmlStrcpy(value, value) ;
#ifdef DEBUG
	printf ("%s\n", value) ;
	printf ("%d\n", strlen(value)) ;
#endif
	multiline = ((strchr(value,'\n')) && (strlen(name) > 8)) ;
#ifdef DEBUG
	printf ("multiline = %d\n", multiline) ;
#endif
	if (!multiline)
	{
	    multiline = ((strlen(name) > 8) && ((strlen(name) + strlen(value) + 2) > 80)) ;
#ifdef DEBUG
	    printf ("multiline = %d\n", multiline) ;
#endif
	}
	
	for (field = getRepeatedFieldValue(value);  field;  field = getRepeatedFieldValue(NULL))
	{
	    if (j == 0)
	    {
		if (multiline)
		{
		    j = 8 ;
		    sprintf (buffer, "%s:", name) ;

#ifdef DEBUG
		    printf("%s\n", buffer) ;
#endif
		    i = -1 ;
		    itemList[++i].length =	strlen(buffer) ;
		    itemList[i].code =		MAIL$_SEND_RECORD ;
		    itemList[i].buffer =	buffer ;
		    itemList[i].returnLength =	0 ;
		    itemList[++i] = nullItemList ;
		    
		    status = mail$send_add_bodypart (&sendContext, &itemList, &nullItemList) ;
		
		    if (!$VMS_STATUS_SUCCESS(status))
		    {
			cgi_printf("Send failed: MAIL$SEND_ADD_BODYPART returned %%X%0X\n",status) ;
			return 1 ;
		    } ;
		    
		    sprintf (buffer, "%*c%s", j, ' ', field) ;
		}
		else
		{
		    j = strlen(name) + 2 ;
		    sprintf (buffer, "%s: %s", name, field) ;
		}
	    }
	    else
	    {
		sprintf (buffer, "%*c%s", j, ' ', field) ;
	    }
	    
#ifdef DEBUG
	    printf("%s\n", buffer) ;
#endif
	    i = -1 ;
	    itemList[++i].length =	strlen(buffer) ;
	    itemList[i].code =		MAIL$_SEND_RECORD ;
	    itemList[i].buffer =	buffer ;
	    itemList[i].returnLength =	0 ;
	    itemList[++i] = nullItemList ;		/* The itemlist describes the body of the message.			 */
	    
	    status = mail$send_add_bodypart (&sendContext, &itemList, &nullItemList) ;
							/* Add the body to the message.						 */
	    if (!$VMS_STATUS_SUCCESS(status))
	    {
		cgi_printf("Send failed: MAIL$SEND_ADD_BODYPART returned %%X%0X\n",status) ;
		return 1 ;
	    } ;
	}
    }

    status =
	mail$send_message (
	    &sendContext,
	    &nullItemList,
	    &nullItemList) ;				/* Send the message.							 */
    if (!$VMS_STATUS_SUCCESS(status))
    {
	cgi_printf("Send failed: MAIL$SEND_MESSAGE returned %%X%0X\n",status) ;
	return 1 ;
    } ;
	 
    status =
	mail$send_end (
	    &sendContext,
	    &nullItemList,
	    &nullItemList) ;	    			/* Set up to construct the mail context.				 */
    if (!$VMS_STATUS_SUCCESS(status))
    {
	cgi_printf("Send failed: MAIL$SEND_END returned %%X%0X\n",status) ;
	return 1 ;
    } ;

    str$free1_dx (&xxxDesc) ;

    cgi_printf("Send Succeeded: submitted form data mailed\n") ;
    return 1;
}

char* getField (
    char* field)
{
    static char				*cp = NULL ;
    char				*next ;
    char				*xxx ;
    
    if (field)
    {
    	cp = field ;
    }

    if (!cp)
    {
	return cp ;
    }

    next = strchr(cp, '&') ;
    if (next)
    {
	*next++ = 0 ;
    } ;
    
    xxx = cp ;
    cp = next ;
    return xxx ;
}

void getFieldNameAndValue (
    char* field,
    char** name,
    char** value)
{
    char				*cp ;
    
    if (!field)
    {
	*name = NULL ;
	*value = NULL ;
	return ;
    }

    *name = field ;
    cp = strchr(field, '=') ;
    if (!cp)
    {
	*name = NULL ;
	*value = NULL ;
	return ;
    }
    *cp++ = 0 ;
    *value = cp ;
    return ;
}

char* getRepeatedFieldValue (
    char* field)
{
    static char				*cp = NULL ;
    char				*next ;
    char				*xxx ;
    
    if (field)
    {
    	cp = field ;
    }

    if (!cp)
    {
	return cp ;
    }

    next = strchr(cp, '\n') ;
    if (next)
    {
	*next++ = 0 ;
    } ;
    
    xxx = cp ;
    cp = next ;
    return xxx ;
}

void htmlStrcpy (
    char *out,
    char *in)
{
    int					i ;
    for (i = strlen(in);  i >= 0;  i--)
    {
    	if (*in == '%')
	{
	    int				xxx ;

	    sscanf (&in[1], "%02x", &xxx) ;
	    *out++ = (char) xxx ;
	    i -= 2 ;
	    in += 3 ;
	}
	else if ( *in == '+' ) {
	    *out++ = ' ';
	    in++;
	}
	else
	{
	    *out++ = *in++ ;
	}
    }
}
