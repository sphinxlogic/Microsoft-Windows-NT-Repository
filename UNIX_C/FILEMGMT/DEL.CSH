#!/bin/csh -f
#File deletion with options
#Rahul Dhesi 1987/02/25
#revised 1987/02/26
if ($#argv < 1) then
   echo Usage:  del file ...
   exit (1)
endif
foreach file ($*)
again:
   set type='Delete ['`/usr/bin/file "$file"|/bin/sed -e 's/	/ /g'`']? [Yes/No/List/Quit] '
   switch ("$type")
      case '*o such file or*'
         echo "Could not find $file"
         goto nextloop
         breaksw
      case '*directory*'
         echo "Skipping $file because it is a directory"
         goto nextloop
         breaksw
      case '*mission denied*'
         echo "Skipping $file because access to it was denied"
         goto nextloop
         breaksw
   endsw

ask:
   echo -n "$type"
   set ans=($<)
   switch ("$ans")
      case 'y'
      case 'Y'
         /bin/rm "$file"
	 if (-e "$file") then
            echo "Sorry, $file could not be deleted" | \
               /bin/sed -e 's/\([^ 	]\)/_\1/g' | \
               /usr/ucb/more
         else
            echo "File $file" has been deleted
         endif
         breaksw
      case 'n'
      case 'N'
         echo "Skipping $file"
         breaksw
      case 'l'
      case 'L'
         switch ("$type")
            case '*text*'
            case '*script*'
               echo '*****' "$file" '*****'
               /bin/cat -uv "$file" | /usr/ucb/more -10d
               breaksw
            case '*empty*'
               echo "***** File $file is empty *****"
               echo '<...empty...>'
               breaksw
            case '*link to*'
               echo "***** Searching for printable strings in $file ... ***"
               /usr/ucb/strings "$file" | /bin/cat -uv | /usr/ucb/more -10dl
               breaksw
            case '*zoo archive*'
               echo -n "***** Archive $file contains: *****"
               /usr/local/zoo l "$file" | /bin/cat -uv | /usr/ucb/more -10dl
                breaksw
            default
               echo "***** Searching for printable strings in $file ... ***"
               /usr/ucb/strings "$file" | /bin/cat -uv | /usr/ucb/more -10dl
               breaksw
         endsw
         echo '*****'
         goto again
         breaksw
      case 'q'
      case 'Q'
         exit
      default
         echo 'Please type the first letter:  Y, N, L, or Q'
         goto ask
         breaksw
   endsw
nextloop:
end
exit (0)

-- 
Rahul Dhesi         UUCP:  {ihnp4,seismo}!{iuvax,pur-ee}!bsu-cs!dhesi


