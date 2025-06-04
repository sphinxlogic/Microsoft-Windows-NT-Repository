/*
 * udp - Check if UDP traffic is allowed on this host; we open port 1527 on
 *       a system (default of cs.widener.edu), which is expecting it; the
 *       date is output (e.g. very similar to the daytime service).  This
 *       will conclusively tell us if UDP traffic on ports > 1000 is allowed.
 *
 *	It should print out the date if UDP traffic's not blocked on your
 *	system.  If it just hangs, try these tests too:
 *	  a. run it with -d  (e.g. "udp -d"); that goes to the normal UDP port
 *	     to print the date.  If it works, then you can be sure that any
 *	     UDP traffic > port 1000 is blocked on your system.
 *	  b. if it hangs too, try "telnet 147.31.254.130 13" and see if
 *	     _that_ prints the date; if it doesn't, it's another problem (your
 *	     network can't get to me, e.g.).
 *
 * Compile by: cc -o udp udp.c
 *
 * Brendan Kehoe, brendan@cs.widener.edu, Oct 1991.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifndef hpux
# include <arpa/inet.h>
#endif

#define	SIZE	2048
#define	HOST	"147.31.254.130"	/* cs.widener.edu */
#define PORT	1527

main (argc, argv)
     int argc;
     char **argv;
{
  int s, len;
  struct sockaddr_in server, sa;
  char buf[SIZE];

  if ((s = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
      perror ("socket()");
      exit (1);
    }

  bzero ((char *) &sa, sizeof (sa));
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = htonl (INADDR_ANY);
  sa.sin_port = htons (0);

  if (bind (s, (struct sockaddr *) &sa, sizeof (sa)) < 0)
    {
      perror ("bind()");
      exit (1);
    }

  bzero ((char *) &server, sizeof (server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr (HOST);
  if (argc > 1 && strcmp(*(argv + 1), "-d") == 0)
    server.sin_port = htons ((unsigned short) 13);
  else
    server.sin_port = htons ((unsigned short) PORT);

  /* yoo hoo, we're here .. */
  if (sendto (s, "\n", 1, 0, &server, sizeof (server)) < 0)
    {
      perror ("sendto()");
      exit (1);
    }

  /* slurp */
  len = sizeof (server);
  if (recvfrom (s, buf, sizeof (buf), 0, &server, &len) < 0)
    {
      perror ("recvfrom");
      exit (1);
    }

  printf ("%s", buf);
  close (s);
}
