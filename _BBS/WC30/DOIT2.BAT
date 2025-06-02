cd %1
\_bbs\c %in_type% wildcat %1 < 00_index.txt > wildcat.txt
if not errorlevel 1 goto ok
echo error in %1 >> \_bbs\logfile
:ok
cd \_bbs\wc30

