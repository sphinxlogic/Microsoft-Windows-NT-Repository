#include <maildef.h>
#include <descrip.h>

main()
    {
    int target_newmail_count = 654;
    $DESCRIPTOR( target_user, "HOFFMAN" );

    int retstat;
    int contxt = 0;
    struct itemlist3
	{
	unsigned short int il_size;
	unsigned short int il_code;
	unsigned long int *il_buffer;
	unsigned short int il_rla;
	}
	*il3, *il3_out;

    il3 = calloc( sizeof( struct itemlist3 ), 5 );
    il3_out = calloc( sizeof( struct itemlist3 ), 5 );

    retstat = mail$user_begin( &contxt, il3, il3_out );

    (il3 + 0)->il_size = target_user.dsc$w_length;
    (il3 + 0)->il_code = MAIL$_USER_USERNAME;
    (il3 + 0)->il_buffer = target_user.dsc$a_pointer;
    (il3 + 0)->il_rla = 0;
    (il3 + 1)->il_size = 0;
    (il3 + 1)->il_code = 0;
    (il3 + 1)->il_buffer = 0;
    (il3 + 1)->il_rla = 0;

    retstat = mail$user_set_info( &contxt, il3, il3_out );

    (il3 + 0)->il_size = 2;
    (il3 + 0)->il_code = MAIL$_USER_SET_NEW_MESSAGES;
    (il3 + 0)->il_buffer = &target_newmail_count;
    (il3 + 0)->il_rla = 0;
    (il3 + 1)->il_size = 0;
    (il3 + 1)->il_code = 0;
    (il3 + 1)->il_buffer = 0;
    (il3 + 1)->il_rla = 0;

    retstat = mail$user_set_info( &contxt, il3, il3_out );

    return( retstat );
    }
