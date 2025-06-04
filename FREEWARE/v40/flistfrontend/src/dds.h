/* $Id: dds.h,v 1.7 1995/10/26 23:49:48 tom Exp $
 *
 * public interface of dds.c (FLIST's display functions)
 */

#ifndef DDS_H
#define DDS_H

#include "dirent.h"

#define	DDS_U_S	(-3)		/* Up one screen		*/
#define	DDS_U_C	(-2)		/* To top of current screen	*/
#define	DDS_U_1	(-1)		/* Up one line			*/

#define	DDS_0	(0)		/* To middle of current screen	*/

#define	DDS_D_1	(1)		/* Down one line		*/
#define	DDS_D_C	(2)		/* To bottom of current screen	*/
#define	DDS_D_S	(3)		/* Down one screen		*/

extern	int	dds_add (FILENT *z);
extern	int	dds_fast (int opt);
extern	int	dds_index (int ifile);
extern	int	dds_inx1 (int ifile);
extern	int	dds_inx2 (int inx);
extern	int	dds_last (int *curfile_);
extern	int	dds_move (int curfile, int opt);
extern	int	dds_pack (int *curfile_, int inscreen);
extern	int	dds_scroll (int	ifile);
extern	void	dds_add2 (FILENT *z, int curfile);
extern	void	dds_all (int top_set, int now_set);
extern	void	dds_ast1 (void);
extern	void	dds_done (void);
extern	void	dds_hold (int hold);
extern	void	dds_line(int inx);
extern	void	dds_spawn (char *cli_, int ifile, char *cmd_, char *msg_, int nowait, int refresh);
extern	void	dds_tell (char *msg_, int ifile);
extern	void	dds_while (char *msg_);
extern	void	dds_width (FILENT *z, int curfile);

#endif /* DDS_H */
