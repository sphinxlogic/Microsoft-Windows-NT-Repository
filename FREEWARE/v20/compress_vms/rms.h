/*  
 *  Special version for the IRAF/VMS kernel routines.  To get rid of linker
 *  warning messages of "multiply-defined symbols...", redefine the RMS
 *  initialization structures before bringing in the real RMS stuff.  
 *  (see rmsdata.mar)
#define  cc$rms_fab	cc_rms_fab
#define  cc$rms_nam	cc_rms_nam
#define  cc$rms_rab	cc_rms_rab
#define  cc$rms_xaball	cc_rms_xaball
#define  cc$rms_xabdat	cc_rms_xabdat
#define  cc$rms_xabfhc	cc_rms_xabfhc
#define  cc$rms_xabkey	cc_rms_xabkey
#define  cc$rms_xabpro	cc_rms_xabpro
#define  cc$rms_xabrdt	cc_rms_xabrdt
#define  cc$rms_xabsum	cc_rms_xabsum
#define  cc$rms_xabtrm	cc_rms_xabtrm
 */

#include <rms.h>
