 5-Nov-85 21:19:29-MST,1526;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Tue 5 Nov 85 21:19:18-MST
Received: from usenet by TGR.BRL.ARPA id a023646; 5 Nov 85 22:50 EST
From: sources-request@panda.uucp
Newsgroups: mod.sources
Subject: rename: a companion to restor
Message-ID: <1067@panda.UUCP>
Date: 5 Nov 85 14:28:02 GMT
Sender: jpn@panda.uucp
Approved: jpn@panda.UUCP
To:       unix-sources@BRL-TGR.ARPA

Mod.sources:  Volume 3, Issue 36
Submitted by: genrad!amd!amdcad!phil (Phil Ngai)

This is a shell script, not a shell archive.

#! /bin/sh
# "rename" shell script
# by Phil Ngai, 11/4/85
# Moves files after V7 or Xenix style restor has left you with a
# set of files with numeric names. It makes intermediate directories.
# To use, first get a list of inodes and final pathname from dumpdir,
# then edit out the names you don't want. In the top level directory,
# feed the remaining lines to this script and feed its output to sh.
# Sample expected input:
#   11	/lib/uucp/dial.c
#   13	/lib/tabset/3101
#   30	/lib/atrun
#  103	/lib/uucp/L.sys
# Sample output:
# mkdir ./lib
# mkdir ./lib/uucp
# mv 103	/lib/uucp/L.sys
# mv  11	/lib/uucp/dial.c
# mv  13	/lib/tabset/3101
# mv  30	/lib/atrun
while read in
do
set `echo $in`
INODE=$1
PATHNAME=$2
FINALNAME=$PATHNAME
OIFS=$IFS
IFS=/
set $PATHNAME
IFS=$OIFS
DIR=.
while expr $# '>=' 2 > /dev/null
do
DIR=$DIR/$1
echo mkdir $DIR
shift
done
echo mv $INODE .$FINALNAME
done \
| sort | uniq
