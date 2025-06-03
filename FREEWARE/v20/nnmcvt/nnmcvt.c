/* Mail conversion program
 *
 *  001	  27 Apr 1995	mlj	Initial version
 *
 *  002	  28 Apr 1995	kdg	- Don't generate \rs, strip them from to/from/subj
 *
 *  003	   3 May 1995	mlj	- tweak date handling
 *				- handle all folders
 *
 *  004	   3 May 1995	kdg	- add trailing "00" to time since it comes out as nn:nn:
 *				- string lengths needed to be 256 to account
 *				  for trailing null
 */
#include <maildef>
#include <descrip>
#include <stdio>

struct item_list_3 {
	unsigned short item_length;
	unsigned short item_code;
	void * item_address;
	unsigned long * item_return_length;
};

struct folder {
	struct folder * next;
	char * folder_name;
};

static unsigned long mailfile_context = { 0 };
static unsigned long message_context = { 0 };
static struct folder * folder_list[2]; 

void nnmcvt_begin_folder();
void nnmcvt_begin_message();
void nnmcvt_continue_message();
void nnmcvt_end_message();
void strip_cr();

static unsigned long folder_action(int user, struct dsc$descriptor * desc)
{
    if (desc->dsc$w_length != 0)
	{
	struct folder * f;
	f = (struct folder *) malloc(sizeof(struct folder));
	if (folder_list[0] == NULL)
	    folder_list[0] = f;
	    else (folder_list[1])->next = f;
	folder_list[1] = f;

	f->next = NULL;
	f->folder_name = (char *) malloc(desc->dsc$w_length + 1);
	strncpy(f->folder_name, desc->dsc$a_pointer, desc->dsc$w_length);
	f->folder_name[desc->dsc$w_length] = '\0';
	}
    return 1;
}

main(int argc, char * argv[])
{
#define MAXSTRLEN 255
struct item_list_3 in_item_list[3];
struct item_list_3 out_item_list[7];
char resultant_string[MAXSTRLEN+1];
unsigned long resultant_string_length;
struct folder * f;
unsigned long message_id;
char message_from[MAXSTRLEN+1];
unsigned long message_from_length;
char message_to[MAXSTRLEN+1];
unsigned long message_to_length;
char message_cc[MAXSTRLEN+1];
unsigned long message_cc_length;
char message_subj[MAXSTRLEN+1];
unsigned long message_subj_length;
char message_date[MAXSTRLEN+1];
unsigned long message_date_length;
char message_text[MAXSTRLEN+1];
unsigned long message_text_length;
unsigned short message_text_type;

    in_item_list[0].item_length = 0;
    in_item_list[0].item_code = 0;
    out_item_list[0].item_length = 0;
    out_item_list[0].item_code = 0;
    mail$mailfile_begin(&mailfile_context, in_item_list, out_item_list);

    in_item_list[0].item_length = 0;
    in_item_list[0].item_code = 0;
    out_item_list[0].item_length = MAXSTRLEN;
    out_item_list[0].item_code = MAIL$_MAILFILE_RESULTSPEC;
    out_item_list[0].item_address = resultant_string;
    out_item_list[0].item_return_length = &resultant_string_length;
    out_item_list[1].item_length = 0;
    out_item_list[1].item_code = 0;
    mail$mailfile_open(&mailfile_context, in_item_list, out_item_list);
    resultant_string[resultant_string_length] = '\0';

    in_item_list[0].item_length = sizeof(void *);
    in_item_list[0].item_code = MAIL$_MAILFILE_FOLDER_ROUTINE;
    in_item_list[0].item_address = folder_action;
    in_item_list[0].item_return_length = NULL;
    in_item_list[1].item_length = 0;
    in_item_list[1].item_code = 0;
    out_item_list[0].item_length = 0;
    out_item_list[0].item_code = 0;
    mail$mailfile_info_file(&mailfile_context, in_item_list, out_item_list);

    for (f = folder_list[0]; f != NULL; f = f->next)
	{
	in_item_list[0].item_length = sizeof(mailfile_context);
	in_item_list[0].item_code = MAIL$_MESSAGE_FILE_CTX;
	in_item_list[0].item_address = &mailfile_context;
	in_item_list[0].item_return_length = NULL;
	in_item_list[1].item_length = 0;
	in_item_list[1].item_code = 0;
	out_item_list[0].item_length = 0;
	out_item_list[0].item_code = 0;
	mail$message_begin(&message_context, in_item_list, out_item_list);

	in_item_list[0].item_length = strlen(f->folder_name);
	in_item_list[0].item_code = MAIL$_MESSAGE_FOLDER;
	in_item_list[0].item_address = f->folder_name;
	in_item_list[0].item_return_length = NULL;
	in_item_list[1].item_length = 0;
	in_item_list[1].item_code = 0;
	out_item_list[0].item_length = 0;
	out_item_list[0].item_code = 0;
	mail$message_select(&message_context, in_item_list, out_item_list);

	nnmcvt_begin_folder(f->folder_name);

	while (TRUE)
	    {
	    unsigned long status;

	    in_item_list[0].item_length = 0;
	    in_item_list[0].item_code = MAIL$_MESSAGE_NEXT;
	    in_item_list[0].item_address = NULL;
	    in_item_list[0].item_return_length = NULL;
	    in_item_list[1].item_length = 0;
	    in_item_list[1].item_code = MAIL$_NOSIGNAL;
	    in_item_list[1].item_address = NULL;
	    in_item_list[1].item_return_length = NULL;
	    in_item_list[2].item_length = 0;
	    in_item_list[2].item_code = 0;
	    out_item_list[0].item_length = sizeof(message_id);
	    out_item_list[0].item_code = MAIL$_MESSAGE_CURRENT_ID;
	    out_item_list[0].item_address = &message_id;
	    out_item_list[0].item_return_length = NULL;
	    out_item_list[1].item_length = MAXSTRLEN;
	    out_item_list[1].item_code = MAIL$_MESSAGE_FROM;
	    out_item_list[1].item_address = message_from;
	    out_item_list[1].item_return_length = &message_from_length;
	    out_item_list[2].item_length = MAXSTRLEN;
	    out_item_list[2].item_code = MAIL$_MESSAGE_TO;
	    out_item_list[2].item_address = message_to;
	    out_item_list[2].item_return_length = &message_to_length;
	    out_item_list[3].item_length = MAXSTRLEN;
	    out_item_list[3].item_code = MAIL$_MESSAGE_CC;
	    out_item_list[3].item_address = message_cc;
	    out_item_list[3].item_return_length = &message_cc_length;
	    out_item_list[4].item_length = MAXSTRLEN;
	    out_item_list[4].item_code = MAIL$_MESSAGE_SUBJECT;
	    out_item_list[4].item_address = message_subj;
	    out_item_list[4].item_return_length = &message_subj_length;
	    out_item_list[5].item_length = MAXSTRLEN;
	    out_item_list[5].item_code = MAIL$_MESSAGE_DATE;
	    out_item_list[5].item_address = message_date;
	    out_item_list[5].item_return_length = &message_date_length;
	    out_item_list[6].item_length = 0;
	    out_item_list[6].item_code = 0;
	    status = mail$message_get(&message_context, in_item_list, out_item_list);
	    if (status == 0x007E8052) break;
	    if ((status & 1) == 0) lib$signal(status);
	    message_from[message_from_length] = '\0';
	    message_to[message_to_length] = '\0';
	    message_cc[message_cc_length] = '\0';
	    message_subj[message_subj_length] = '\0';
	    message_date[message_date_length] = '\0';
	    nnmcvt_begin_message(f->folder_name, message_id,
		message_from, message_to, message_cc, message_subj, message_date);

	    while (TRUE)
		{
		in_item_list[0].item_length = 0;
		in_item_list[0].item_code = MAIL$_MESSAGE_CONTINUE;
		in_item_list[0].item_address = NULL;
		in_item_list[0].item_return_length = NULL;
		in_item_list[1].item_length = 0;
		in_item_list[1].item_code = MAIL$_NOSIGNAL;
		in_item_list[1].item_address = NULL;
		in_item_list[1].item_return_length = NULL;
		in_item_list[2].item_length = 0;
		in_item_list[2].item_code = 0;
		out_item_list[0].item_length = MAXSTRLEN;
		out_item_list[0].item_code = MAIL$_MESSAGE_RECORD;
		out_item_list[0].item_address = message_text;
		out_item_list[0].item_return_length = &message_text_length;
		out_item_list[1].item_length = sizeof(message_text_type);
		out_item_list[1].item_code = MAIL$_MESSAGE_RECORD_TYPE;
		out_item_list[1].item_address = &message_text_type;
		out_item_list[1].item_return_length = NULL;
		out_item_list[2].item_length = 0;
		out_item_list[2].item_code = 0;
		status = mail$message_get(&message_context, in_item_list, out_item_list);
		if (status == 0x007EDFA8) break;
		if ((status & 1) == 0) lib$signal(status);
		message_text[message_text_length] = '\0';
		if (message_text_type == MAIL$_MESSAGE_TEXT)
		    nnmcvt_continue_message(message_text);
		}

	    nnmcvt_end_message();
	    }

	in_item_list[0].item_length = 0;
	in_item_list[0].item_code = 0;
	out_item_list[0].item_length = 0;
	out_item_list[0].item_code = 0;
	mail$message_end(&message_context, in_item_list, out_item_list);
	}

    in_item_list[0].item_length = 0;
    in_item_list[0].item_code = 0;
    out_item_list[0].item_length = 0;
    out_item_list[0].item_code = 0;
    mail$mailfile_close(&mailfile_context, in_item_list, out_item_list);

    in_item_list[0].item_length = 0;
    in_item_list[0].item_code = 0;
    out_item_list[0].item_length = 0;
    out_item_list[0].item_code = 0;
    mail$mailfile_end(&mailfile_context, in_item_list, out_item_list);

    return 1;
}

static char nnmcvt_output_directory[255];
static FILE * nnmcvt_fp;

void nnmcvt_begin_folder(
	char * folder)
{
extern unsigned long lib$create_dir();
struct dsc$descriptor_s desc;
unsigned long status;

    desc.dsc$w_length = sprintf(nnmcvt_output_directory, "[.%s]", folder);
    desc.dsc$b_dtype = DSC$K_DTYPE_T;
    desc.dsc$b_class = DSC$K_CLASS_S;
    desc.dsc$a_pointer = nnmcvt_output_directory;
    status = lib$create_dir(&desc);
    if ((status & 1) == 0) lib$signal(status);
    return;
}

void nnmcvt_begin_message(
	char * folder,
	unsigned long id,
	char * from,
	char * to,
	char * cc,
	char * subj,
	char * date)
{
char		output_file[255];
char		output_date[19];
int		from_length;
int		i;

    sprintf(output_file, "%sMSG%04d.MAI", nnmcvt_output_directory, id);
    nnmcvt_fp = fopen(output_file, "w", "rfm=stm");
    printf("%s\n", output_file);

    output_date[0] = ((date[0] == ' ') ? '0' : date[0]);
    output_date[1] = date[1];
    output_date[2] = ' ';
    output_date[3] = date[3];
    output_date[4] = date[4] | 0x20;
    output_date[5] = date[5] | 0x20;
    output_date[6] = ' ';
    output_date[7] = date[9];
    output_date[8] = date[10];
    output_date[9] = ' ';
    output_date[10] = date[12];
    output_date[11] = date[13];
    output_date[12] = ':';
    output_date[13] = date[15];
    output_date[14] = date[16];
    output_date[15] = ':';
    output_date[16] = date[18];
    output_date[17] = date[19];
    output_date[18] = '\0';

    fprintf(nnmcvt_fp, "Received: via Nnmcvt X0.1; %s\n", output_date);

    strip_cr(from);
    from_length = strlen(from);
    for (i = 0; i < from_length; i += 1)
	if (from[i] == ' ') break;
    from[i] = '\0';    
    fprintf(nnmcvt_fp, "From: %s", from);
    for ( ; i < from_length; i += 1)
	if (from[i] == '\"')
	    {
	    from[from_length - 1] = '\0';
	    fprintf(nnmcvt_fp, " (%s)", &from[i + 1]);
	    break;
	    }
    fprintf(nnmcvt_fp, "\n");
    strip_cr(to);
    fprintf(nnmcvt_fp, "To: %s\n", to);
    strip_cr(cc);
    if (cc[0] != '\0') fprintf(nnmcvt_fp, "Cc: %s\n", cc);
    strip_cr(subj);
    fprintf(nnmcvt_fp, "Subject: %s\n\n", subj);
    return;
}

void nnmcvt_continue_message(
	char * text)
{
    fprintf(nnmcvt_fp, "%s\n", text);
    return;
}

void nnmcvt_end_message(void)
{
    fclose(nnmcvt_fp);
    return;
}

void strip_cr(
	char * s)
{
    char *d = s;
    while ((*d = *s++) != '\0')
	if (*d != '\r') d += 1;
    return;
}
