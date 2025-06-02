cd PCBTOOL
call all
cd ..
cd OPUS
call all
cd ..
cd RBBSTOOL
call all
call makeffs
cd ..
cd SFTOOL
call all
cd ..
cd WC30
call all
cd ..
cd MAXTOOL
call all
cd \
ncd /r
mv \ncdtree\treeinfo.ncd \
rd \ncdtree
cd \_bbs

cd \
0del /s 
rm /f *.bak logfile newlist.txt autoscan.dat foo.* 00 epsilon.ses out
rm /f files.fix 00_inde0.txt 00_inde1.txt 00_inde2.txt
rm /f smartchk.cps
global rd tmp
global rd temp

