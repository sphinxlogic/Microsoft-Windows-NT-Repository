
# bytes is a program to add the number of bytes shown in an ls -la for the
# directory passed to it as a parameter, or the current directory as default.

total=0
NULL=""
ADD=${1-`pwd`}
if (test $ADD != `pwd`)
   then
cd $ADD
   fi
ls -l | while LINE=`line`
  do
num=`echo "$LINE" | sed 's/  */ /g' | cut -f5 -d" "`
if [ {$num} = {$NULL} ]
   then
continue
   else
total=`expr $total + $num`
echo "Total for ${ADD} directory is:   $total" >tot
   fi
  done

cat tot; rm tot

