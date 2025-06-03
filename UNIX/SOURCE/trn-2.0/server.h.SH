: create server.h file
case $CONFIG in
    '') . ./config.sh ;;
esac
if $test $isrrn = define 
then
$echo "Extracting server.h (with variable substitutions)"
$cat >server.h <<!GROK!THIS!
/*
 * $Id: server.h.SH,v 1.2 92/01/11 16:08:41 usenet Exp $
 * 
 * $Log:	server.h.SH,v $
 * Revision 1.2  92/01/11  16:08:41  usenet
 * header twiddling, bug fixes
 * 
 * 
 * 
 * This software is Copyright 1991 by Stan Barber. 
 * 
 * Permission is hereby granted to copy, reproduce, redistribute or otherwise
 * use this software as long as: there is no monetary profit gained
 * specifically from the use or reproduction of this software, it is not
 * sold, rented, traded or otherwise marketed, and this copyright notice is
 * included prominently in any copy made. 
 *
 * The author make no claims as to the fitness or correctness of this software
 * for any use whatsoever, and it is provided as is. Any use of this software
 *is at the user's own risk. 
 */
#ifdef SERVER

extern	char	*get_server_name ANSI((int));
extern	char	*getserverbyfile ANSI((char *));
extern	int	server_init ANSI((char *));
extern	void	put_server ANSI((char *));
extern	int	get_server ANSI((char *,int));
extern	long	rawcheck_server ANSI((char *,int));
extern	long	rawget_server ANSI((char *,long));
extern	void	close_server ANSI((void));

#include "$NNTPSRC/common/nntp.h"
EXT 	char ser_line[NNTP_STRLEN];

# if defined(XTHREAD) && !defined(OK_BIN)
     ??????  /* This version of NNTP doesn't have XTHREAD support */
# endif

#endif
!GROK!THIS!
else
	rm -f server.h
fi
