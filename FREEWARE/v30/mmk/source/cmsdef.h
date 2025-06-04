/*
**  CMSDEF.H
**
**  Definitions for use with DEC/CMS.
**
**  COPYRIGHT © 1994, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
**
**  MODIFICATION HISTORY:
**
**  	01-JUL-1994 V1.0    Madison 	Initial coding.
**
*/
#define CMS$M_CMD_CREATE    2
#define CMS$M_CMD_REPLACE   256

#define CMS$M_OCC_NOCLASS   1
#define CMS$M_OCC_NOELEMENT 2
#define CMS$M_OCC_NOGROUP   4
#define CMS$M_OCC_NOOTHER   8

    struct CMS_LDB {
    	unsigned int longword_count;
    	unsigned int return_status;
    	struct dsc$descriptor library_descriptor;
    	unsigned int user_arg;
    	unsigned int occlusion_mask;
    	unsigned int reserved[46];
    };

    struct CMS_FDB {
    	unsigned int reserved[5];
    };
