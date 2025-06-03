/* File info structure definitions (c_finfo).
 */
#define	SZ_OWNERSTR		16
#define	FI_REGULAR		1	/* file types			*/
#define	FI_DIRECTORY		2
#define	FI_EXECUTABLE		3
#define	FI_SPECIAL		4

struct _finfo {
	int	fi_type;		/* file type			*/
	int	fi_size;		/* file size, machine bytes	*/
	int	fi_atime;		/* time of last access		*/
	int	fi_mtime;		/* time of last modify		*/
	int	fi_ctime;		/* time of file creation	*/
	int	fi_perm;		/* file permission bits		*/
	char	fi_owner[SZ_OWNERSTR*sizeof(int)];
};

#define D_finfo
