From: Dan Kary <ihnp4!mgnetp!dicomed!ndsuvax!nckary@Ucb-Vax.ARPA>
Newsgroups: net.sources
Subject: "Public Domain cpio.c"
Date: 12 Jun 85 13:32:32 GMT

A while back there was a discussion about reading cpio format tapes on
4.2BSD systems.  We lost news for a while and I never found out how or
if the problem was resolved.  Faced with a need to read a cpio tape I
finally decided to write something that would read cpio format.  It
turned out to be a much smaller problem than I thought it would be.
No guarantees about portability are made, but I was able to read my
AT&T System V distribution tape on our VAX 11/780 running 4.2BSD with
this program.  I hope this is useful to someone.
						
						Dan Kary

(one file only)
--------------------- cut here -----------------------------------
/* A public domain program by Daniel D. Kary
 *                            300 Minard Hall
 *                            North Dakota State University
 *                            Fargo, ND   58105
 *                            ihnp4!dicomed!ndsuvax!nckary
 *
 * This program will read cpio format files (tapes)
 * use dd(1) to read the tape into a file then  'cpio file'
 * to read the files out of the tape file.  This program will
 * create directories and files using the correct mode and
 * ignoring time, user and group info.  It is useful for reading
 * distribution tapes (AT&T) on systems lacking cpio (Berkeley).
 * The process's effective user ID must be super-user.
 */

#include    <sys/types.h>
#include    <sys/file.h>
#include    <stdio.h>

struct header{  
    ushort  magic_number,
    device,
    inode,
    mode,
    uid,
    gid,
    h_nlink,
    dev_type,
    modify_time[2],
    name_length,
    file_size[2];
} 
hdr;

char buf[10240], fname[512];
int fd;

main(argc, argv)
int argc;
char *argv[];
{
    if(argc != 2){
        fprintf(stderr,"Usage: cpio filename\n");
        exit();
    }
    fd = open(argv[1], O_RDONLY);
    while(fd > 0){
        read_hdr();
        if(strcmp("TRAILER!!!", fname) == 0) exit();
    }
}

read_hdr()
{
    char c;
    int f, i, size;

    read(fd, &hdr.magic_number,2);
    read(fd, &hdr.device,2);
    read(fd, &hdr.inode,2);
    read(fd, &hdr.mode,2);
    read(fd, &hdr.uid,2);
    read(fd, &hdr.gid,2);
    read(fd, &hdr.h_nlink,2);
    read(fd, &hdr.dev_type ,2);
    read(fd, hdr.modify_time,4);
    read(fd, &hdr.name_length,2);
    read(fd, &hdr.file_size[1],2);
    read(fd, &hdr.file_size[0],2);
    i = hdr.name_length;
    if(((i/2)*2) != i) i++;
    read(fd, fname, i);
    size = hdr.file_size[1] * 65536 + hdr.file_size[0];
    if(((size/2)*2) != size) size++;
    if(size == 0)
        mkdir(fname, hdr.mode);
    else{
        f = open(fname, O_CREAT | O_WRONLY, hdr.mode);
        while(size >= 10240){
            read(fd, buf, 10240);
            write(f, buf, 10240);
            size -= 10240;
        }
        if(size > 0){
            read(fd, buf, size);
            write(f, buf, size);
	    close(f);
        }
    }
}
