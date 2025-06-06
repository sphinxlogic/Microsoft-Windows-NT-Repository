case $CONFIG in
'')
    if test ! -f config.sh; then
	ln ../config.sh . || \
	ln ../../config.sh . || \
	ln ../../../config.sh . || \
	(echo "Can't find config.sh."; exit 1)
    fi
    . ./config.sh
    ;;
esac
case "$0" in
*/*) cd `expr X$0 : 'X\(.*\)/'` ;;
esac

echo "Extracting utils/checkalias (with variable substitutions)"
echo $startsh > checkalias
cat >>checkalias <<'!NO!SUBS!'
# checkalias: part of the Elm mail system
# @(#)$Id: checkalias.SH,v 5.1 1992/10/04 00:46:45 syd Exp $
#		Copyright (c) 1988-1992 USENET Community Trust
# 		Copyright (c) 1986,1987 Dave Taylor
 
if [ "$*" = "" ]; then
  echo Usage: checkalias alias \[alias ...\] 1>&2
  exit 1
fi
 
exec elm -c $*
!NO!SUBS!
