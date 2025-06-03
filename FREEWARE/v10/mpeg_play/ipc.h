#ifndef _SYS_IPC_H_
#define _SYS_IPC_H_

/* Common IPC Structures */
struct ipc_perm {
	unsigned int	uid;		/* owner's user id	*/
	unsigned int	gid;		/* owner's group id	*/
	unsigned int	cuid;		/* creator's user id	*/
	unsigned int	cgid;		/* creator's group id	*/
	unsigned int	mode;		/* access modes		*/
	unsigned int	seq;		/* slot usage sequence number */
	int		key;		/* key			*/
};

/* common IPC operation flag definitions */
#define	IPC_CREAT	0001000		/* create entry if key doesn't exist */
#define	IPC_EXCL	0002000		/* fail if key exists */
#define	IPC_NOWAIT	0004000		/* error if request must wait */

/* Keys. */
#define	IPC_PRIVATE	(int)0		/* private key */

/* Control Commands. */
#define	IPC_RMID	0	/* remove identifier */
#define	IPC_SET		1	/* set options */
#define	IPC_STAT	2	/* get options */

/* Common ipc_perm mode Definitions. */
#define	IPC_ALLOC	0100000		/* entry currently allocated        */
#define	IPC_R		0000400		/* read or receive permission       */
#define	IPC_W		0000200		/* write or send permission	    */

#endif /* _SYS_IPC_H_ */
