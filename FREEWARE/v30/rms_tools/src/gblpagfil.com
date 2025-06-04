$CREATE GBLPAGFILE.C      
/*
** GBLPAGFIL.C    Hack to manipulate GBLPAGFIL sysgen parameter.
** 		  Hein van den Heuvel, November 1993.
** RMS Global buggers are not backed by the data file, but by the page file.
** They are charged against the system, not against a process using them.
** To protect a system against accidental (?) pagefile depletion by a non-
** privved user opening files with too many global buffers there is a sysgen
** parameter GBLPAGFIL. Unfortunatly this is NOT a dynamic param and the 
** default is rather low (1024). This poses problem with systems starting to
** experiment with more global buffers, requiring a re-boot.
** 
** See note 11.4 in The RMS_OPENVMS conference for more background.
**
** This program will set GBLPAGFIL to the specified value, if acceptable.
** Requires CMKRNL privelege and definition of the image as foreign command.
** Must be linked against SYS.STB to resolve version dependend global symbols.
** Have fun, Hein.
*/
    
#include <descrip>
#include <stdio>
globalref volatile int sgn$gl_gblpagfil, mmg$gl_gblpagfil;

int set_gblpagfil ( int	count )
{
sgn$gl_gblpagfil += count;
mmg$gl_gblpagfil += count;
return 1;
}

main (int argc, char *argv[])
{
unsigned int cmkrnl_privsmask[2] = {1,0}; 
int stat, gblpagfil, adjustment, cmkrnl_args[2];

printf ("old values: SGN$GL_GBLPAGFIL: %d  MMG$GL_GBLPAGFIL: %d\n ",
	    sgn$gl_gblpagfil, mmg$gl_gblpagfil);

if ( argc < 2 ) return 268435456; 

stat = sys$setprv(1,cmkrnl_privsmask,0,0);
if (!(stat & 1 )) return stat;

/*
** Grab requested value, convert to adjustment and make sure it can be used.
*/
sscanf( argv[1], "%d", &gblpagfil);
adjustment = gblpagfil - sgn$gl_gblpagfil;
if ( (mmg$gl_gblpagfil + adjustment) < 0 ) return 16; /* pad param */

cmkrnl_args[0] = 1;
cmkrnl_args[1] = adjustment;
return sys$cmkrnl(&set_gblpagfil, cmkrnl_args);
}
$CC GBLPAGFIL
$LINK GBLPAGFIL,SYS$SYSTEM:SYS.STB/SELECT
$GBLPAGFIL == "$"+F$PARSE("GBLPAGFIL.EXE")
$GBLPAGFIL
$DELETE GBLPAGFIL.C.,.OBJ.
$EXIT
