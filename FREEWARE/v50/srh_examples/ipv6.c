/*

  Example of testing for the presence of IPv6 on OpenVMS

  Author: Colin Blake

  Related DCL:

    $ cc /decc/prefix=all test
    $ link test
    $ run test
    Got an IPv4 socket on fd 3
    SO_TYPE is 1
    Got an IPv6 socket on fd 4
    SO_TYPE is 1
    $

*/

#include <errno.h>
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <socket.h>
#include <fcntl.h>
#include <in.h>
#include <netdb.h>
#include <inet.h>
#include <unistd.h>
#include <ucx$inetdef.h>

typedef unsigned int socklen_t;

int     sock_1;                         /* socket */
int     sock_2;                         /* socket */

int main (int argc, char *argv[])
{

        if ((sock_1 = socket (AF_INET, SOCK_STREAM, 0)) == -1)
            perror( "socket");
        else {
            int rv, tmp;
            socklen_t optlen = sizeof(tmp);
            rv = getsockopt(sock_1, SOL_SOCKET, SO_TYPE, &tmp, &optlen);
            if (rv == 0) {
                printf("Got an IPv4 socket on fd %d\n",sock_1);
                printf("SO_TYPE is %d\n",tmp);
            }
            else
                perror( "getsockopt");
        }

        if ((sock_2 = socket (AF_INET6, SOCK_STREAM, 0)) == -1)
            perror( "socket6");
        else {
            int rv, tmp;
            socklen_t optlen = sizeof(tmp);
            rv = getsockopt(sock_2, SOL_SOCKET, SO_TYPE, &tmp, &optlen);
            if (rv == 0) {
                printf("Got an IPv6 socket on fd %d\n",sock_2);
                printf("SO_TYPE is %d\n",tmp);
            }
            else
                perror( "getsockopt6");
        }

} /* end main */

