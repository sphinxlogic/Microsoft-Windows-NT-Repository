/* cutcp.h -- defs for cutcp code */


int 		netlisten(unsigned int port);
struct machinfo *Shostlook(char *name);
int 		Sdomain(char *name);
struct machinfo *Sgethost(char *name);
struct machinfo *Slookip(unsigned long *address);
void		netgetftp(unsigned int array[], unsigned int port);
int 		netopen(unsigned long *address, unsigned int port);
int 		Snetopen(struct machinfo *m, unsigned int port);
int		netqlen(int handle);
int		netroom(int handle);
int		netread(int pnum, unsigned char *buffer, unsigned int n);
int		netwrite(int pnum, unsigned char *buffer, unsigned int n);
int		netest(int pnum);
int		netpush(int pnum);
void		netulisten(unsigned int portnum);
int		netusend(unsigned long *address, unsigned destport, 
			unsigned sourceport, unsigned char *buffer, int count);
int		neturead(unsigned char	*buffer);
struct machinfo *Slooknum(int pnum);
int		Snetinit();
int		Shostfile(char *name);
int		netshut();
int		Sgetevent(int classes, int *clss, int *data);
int		netclose(int pnum);
unsigned int	intswap(unsigned int val);
unsigned long	n_clicks(void *p);
void		Stask();

#define	ntohs(a)	(intswap(a))
#define	htons(a)	(intswap(a))
