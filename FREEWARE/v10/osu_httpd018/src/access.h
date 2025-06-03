/*
 * Define structures used for access control.
 */
struct acc_info_struct {
    long uic;				/* Uic of object to access. */
    int cache_allowed;			/* If true, can put object in cache */
    char *prot_file;			/* Filename of protection setup for obj.*/
    char rem_user[40];			/* Remote login name */
    char user[40];			/* authenitcation name */
};

typedef struct acc_info_struct *access_info;
