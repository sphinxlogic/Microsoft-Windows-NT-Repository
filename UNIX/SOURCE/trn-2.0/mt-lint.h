/* $Id: mt-lint.h,v 1.2 92/01/11 16:05:05 usenet Exp $
**
** $Log:	mt-lint.h,v $
 * Revision 1.2  92/01/11  16:05:05  usenet
 * header twiddling, bug fixes
 * 
** Revision 4.4.3.1  1991/11/22  04:12:15  davison
** Trn Release 2.0
** 
** Handle brain-dead lints that only have 6 significant-character names.
*/

#define subject_cnts		sucnts
#define subject_array		suarra
#define subject_strings		sustri
#define subject_str		sustr
#define author_cnts		aucnts
#define author_array		auarra
#define author_root		auroot
#define author_str		austr
#define valid_subject		vsubje
#define valid_author		vautho
#define valid_message_id	vmsgid
#define read_authors		rautho
#define read_articles		rartic
#define read_ids		rids
#define read_item		ritem
#define processed_groups	pgroup
#define process_articles	partic
#define expired_articles	eartic
#define unlink_child		uchild
#define unlink_root		uroot
#define free_article		fartic
#define free_author		fautho
#define enumerate_articles	enarti
#define enumerate_thread	enthrd
#define string_offset		soffst
#define write_authors		wautho
#define write_subjects		wsubje
#define write_roots		wroots
#define write_articles		wartic
#define write_thread		wthred
#define write_ids		wids
#define write_item		witem
#define processed_groups	pgroup
#define timer_off		timoff
#define timer_first		tim1st
#define timer_next		timnxt
#define truncate_len		trulen
#define article_array		artarr

#define safemalloc(x)	(NULL)
#define free(x)		(x = NULL)
#define safefree(x)	(*x = NULL)
#define signal(x,y)
