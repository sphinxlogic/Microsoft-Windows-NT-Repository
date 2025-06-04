/*	Fudge to allow the UNIX unlink() to work	*/

#include <descrip.h>

unlink(filename)
char filename[];
{
    char tempname[255];
    $DESCRIPTOR(file_desc, tempname);

    file_desc.dsc$w_length = strlen(filename);
    strcpy(tempname, filename);

    return (LIB$DELETE_FILE(&file_desc));

}
