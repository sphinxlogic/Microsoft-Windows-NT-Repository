/*	xcxmdm.c -- XMODEM Protocol module for XC
	This file uses 4-character tabstops
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <setjmp.h>
#include "xc.h"

#define CPMEOF	032	/* ^Z */
#define WANTCRC 'C'
#define OK		 0
#define TIMEOUT	-1	 /* -1 is returned by readbyte() upon timeout */
#define ERROR	-2
#define WCEOT	-3
#define RETRYMAX 10
#define SECSIZ  128
#define Resume_Not_Allowed	1

/* crc_xmodem_tab calculated by Mark G. Mendel, Network Systems Corporation */
ushort crc_xmodem_tab[256] = {
	 0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	 0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	 0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	 0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	 0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	 0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	 0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	 0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	 0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	 0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	 0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	 0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	 0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

short crcheck = TRUE;		/* CRC check enabled? */
static FILE *xfp;			/* buffered file pointer */
static short firstsec,		/* first sector of file or not? */
			textmode,		/* Text translations enabled? */
			save_crc;		/* Saved crcheck value */
static char wcbuf[SECSIZ];	/* Ward Christensen sector buffer */
static ushort Updcrc();
static jmp_buf our_env;
static void (*oldvec)();

/*	send 10 CAN's to try to get the other end to shut up */
static void
canit()
{
	int i;

	for(i = 0; i < 20; i++)
		sendbyte(CAN);
}

static void
xmsigint(junk)
int junk;
{
	show_abort();
	signal(SIGINT, SIG_IGN);	/* Ignore subsequent DEL's */
	canit();					/* Abort the transmission */
	longjmp(our_env,1);
}

/*	fill the CP/M sector buffer from the UNIX file
	do text adjustments if necessary
	return 1 if more sectors are to be read, or 0 if this is the last
*/
static
getsec()
{
	int i;
	register c;

	i = 0;
	while(i < SECSIZ && (c = getc(xfp)) != EOF){
		if (textmode && c == '\n'){
			wcbuf[i++] = '\r';
			if (i >= SECSIZ){		 /* handle a newline on the last byte */
				ungetc(c, xfp);		 /* of the sector */
				return(1);
			}
		}
		wcbuf[i++] = c;
	}
	/* make sure that an extra blank sector is not sent */
	if (c != EOF && (c = getc(xfp)) != EOF){
		ungetc(c, xfp);
		return(1);
	}
	/* fill up the last sector with ^Z's if text mode or 0's if binary mode */
	while(i < SECSIZ)
		wcbuf[i++] = textmode ? CPMEOF : '\0';
	return(0);
}

/*	wcgetsec() inputs an XMODEM "sector".
	This routine returns the sector number encountered, or ERROR if a valid
	sector is not received or CAN received; or WCEOT if EOT sector.

	Maxtime is the timeout for the first character, set to 6 seconds for
	retries. No ACK is sent if the sector is received ok. This must be
	done by the caller when it is ready to receive the next sector.
*/
static
wcgetsec(maxtime)
unsigned maxtime;
{
	register ushort oldcrc;
	register checksum, j, c;
	int sectcurr, sectcomp, attempts;

	for(attempts = 0; attempts < RETRYMAX; attempts++){
		do {
			c = readbyte(maxtime);
		} while(c != SOH && c != EOT && c != CAN && c != TIMEOUT);

		switch(c){
		case SOH:
			sectcurr = readbyte(3);
			sectcomp = readbyte(3);
			if ((sectcurr + sectcomp) == 0xff){
				oldcrc = checksum = 0;
				for(j = 0; j < SECSIZ; j++){
					if ((c = readbyte(3)) == TIMEOUT)
						goto timeout;
					wcbuf[j] = c;
					if (crcheck)
						oldcrc = Updcrc(c, oldcrc);
					else
						checksum += c;
				}
				if ((c = readbyte(3)) < 0)
					goto timeout;
				if (crcheck){
					oldcrc = Updcrc(c, oldcrc);
					if ((c = readbyte(3)) == TIMEOUT)
						goto timeout;
					if (Updcrc(c, oldcrc)){
						S2("CRC error");
						break;
					}
				}
				else if (((checksum - c) & 0xff) != 0){
					S2("Checksum error");
					break;
				}
				firstsec = FALSE;
				return(sectcurr);
			}
			else
				sprintf(Msg, "Sector number garbled 0%03o 0%03o",
					sectcurr, sectcomp);
				S2(Msg);
			break;
		case EOT:
			if (readbyte(3) == TIMEOUT)
				return(WCEOT);
			break;
		case CAN:
			S2("Sender CANcelled");
			return(ERROR);
		case TIMEOUT:
			if (firstsec)
			break;
timeout:
		S2("Timeout");
		break;
		}
		S2("Trying again on this sector");
		purge();
		if (firstsec)
			sendbyte(crcheck ? WANTCRC : NAK);
		else
			maxtime = 6,
			sendbyte(NAK);
	}
	S2("Retry count exceeded");
	canit();
	return(ERROR);
}

static
putsec()
{
	int i;
	register c;

	for(i = 0; i < SECSIZ; i++){
		c = wcbuf[i];
		if (textmode){
			if (c == CPMEOF)
				return(1);
			if (c == '\r')
				continue;
		}
		putc(c, xfp);
	}
	return(0);
}

/* Receive a file using XMODEM protocol */
static
wcrx()
{
	register sendchar, sectnum, sectcurr;

	strcpy(Name, word);
	if (!(xfp=QueryCreate(Resume_Not_Allowed)))
		return(ERROR);

	firstsec = TRUE;
	sectnum = 0;
	sendchar = crcheck ? WANTCRC : NAK;
	fputc('\r',tfp),
	fputc('\n', tfp);
	S1("Sync...");

	while(TRUE){
		purge();
		sendbyte(sendchar);
		sectcurr = wcgetsec(6);
		if (sectcurr == ((sectnum + 1) & 0xff)){
			sectnum++;
			putsec();
			fprintf(tfp,"Received sector #%d\r", sectnum);
			sendchar = ACK;
			continue;
		}

		if (sectcurr == (sectnum & 0xff)){
			sprintf(Msg, "Received duplicate sector #%d", sectnum);
			S2(Msg);
			sendchar = ACK;
			continue;
		}

		fclose(xfp);

		if (sectcurr == WCEOT){
			S1("File received OK");
			sendbyte(ACK);
			return(OK);
		}

		if (sectcurr == ERROR)
			return(ERROR);

		sprintf(Msg, "Sync error ... expected %d(%d), got %d",
			(sectnum + 1) & 0xff, sectnum, sectcurr);
		S;
		return(ERROR);
	}
}

/*	wcputsec outputs a Ward Christensen type sector.
	it returns OK or ERROR
*/
static
wcputsec(sectnum)
int sectnum;
{
	register ushort oldcrc;
	register checksum, j, c, attempts;

	oldcrc = checksum = 0;
	for(j = 0; j < SECSIZ; j++)
		c = wcbuf[j],
		oldcrc = Updcrc(c, oldcrc),
		checksum += c;
	oldcrc = Updcrc(0, Updcrc(0, oldcrc));

	for(attempts = 0; attempts < RETRYMAX; attempts++){
		sendbyte(SOH);
		sendbyte(sectnum);
		sendbyte(-sectnum - 1);
		for(j = 0; j < SECSIZ; j++)
			sendbyte(wcbuf[j]);
		purge();
		if (crcheck){
			sendbyte((int) (oldcrc >> 8));
			sendbyte((int) oldcrc);
		}
		else
			sendbyte(checksum);
		switch(c = readbyte(10)){
		case CAN:
		S2("Receiver CANcelled");
			return(ERROR);
		case ACK:
			firstsec = FALSE;
			return(OK);
		case NAK:
		S2("Got a NAK on sector acknowledge");
			break;
		case TIMEOUT:
		S2("Timeout on sector acknowledge");
			break;
		default:
			sprintf(Msg, "Got 0%03o for sector acknowledge", c);
			S2(Msg);
			do {
				if ((c = readbyte(3)) == CAN){
					S2("Receiver CANcelled");
					return(ERROR);
				}
			} while(c != TIMEOUT);
			if (firstsec)
				crcheck = (c == WANTCRC);
			break;
		}
	}
	S2("Retry count exceeded");
	return(ERROR);
}

/* Transmit a file using XMODEM protocol */
static
wctx()
{
	register sectnum, eoflg, c, attempts;

	if (!(xfp = fopen(word, "r"))){
		sprintf(Msg, "Can't open '%s'", word);
		S;
		return(ERROR);
	}
	firstsec = TRUE;
	attempts = 0;
	S1("Sync...");

	while((c = readbyte(30)) != NAK && c != WANTCRC && c != CAN)
		if (c == TIMEOUT && ++attempts > RETRYMAX){
			S1("Receiver not responding");
			fclose(xfp);
			return(ERROR);
		}
	if (c == CAN){
		S1("Receiver CANcelled");
		fclose(xfp);
		return(ERROR);
	}
	crcheck = (c == WANTCRC);
	sprintf(Msg,"%s error checking requested", crcheck ? "CRC-16" : "Checksum");
	S;
	sectnum = 1;

	do {
		eoflg = getsec();
		fprintf(tfp,"Transmitting sector #%d\r", sectnum);

		if (wcputsec(sectnum) == ERROR){
			fclose(xfp);
			return(ERROR);
		}
		sectnum++;
	} while(eoflg);

	fclose(xfp);
	attempts = 0;
	sendbyte(EOT);
	while(readbyte(5) != ACK && attempts++ < RETRYMAX)
		sendbyte(EOT);
	if (attempts >= RETRYMAX){
		S1("Receiver not responding to completion");
		return(ERROR);
	}

	S1("Transmission complete");
	return(OK);
}

/*	update the cyclic redundancy check value */
static ushort
Updcrc(c, crc)
register c;
register unsigned crc;
{
	int i;

	for(i = 0; i < 8; i++){
		if (crc & 0x8000)
			crc <<= 1,
			crc += (((c <<= 1) & 0400) != 0),
			crc ^= 0x1021;
		else
			crc <<= 1,
			crc += (((c <<= 1) & 0400) != 0);
	}
	return(crc);
}

static
setmode(c)
int c;
{
	switch(tolower(c)){
	case 't':
		textmode = TRUE;
		break;
	case 'b':
		textmode = FALSE;
	case ' ':
		break;

	default:
		return FAILURE;
	}

	sprintf(Msg, "XMODEM %s file transfer mode", textmode ? "Text" : "Binary");
	S;

	return SUCCESS;
}

/*	Put received WC sector into a UNIX file
	using text translations if neccesary.
*/

void
xreceive(c)
int c;
{
	save_crc = crcheck;
	oldvec = signal(SIGINT, xmsigint);

	if (setmode(c)){
		if (!setjmp(our_env)){
			/* crcheck = 0xff; */

			sprintf(Msg, "Ready to receive single file %s", word);
			S;
			if (wcrx() == ERROR)
				canit();
			return;
		}
	}

	signal(SIGINT, oldvec);
	crcheck = save_crc;
}

void
xsend(c)
int c;
{
	save_crc = crcheck;
	oldvec = signal(SIGINT, xmsigint);

	if (setmode(c)){
		if (!setjmp(our_env)){
			/* crcheck = 0xff; */
			if (wctx() == ERROR){
				sprintf(Msg, "Error transmitting file %s", word);
				S;
				return;
			}
		}
	}

	signal(SIGINT, oldvec);
	crcheck = save_crc;
}
