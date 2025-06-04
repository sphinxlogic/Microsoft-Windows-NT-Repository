def/add fsb = first_string_block
def/add tsb = first_string_block
def/com sb$prev = -
  "if (csb->prev != 0) -
   then (def csb=csb->prev) -
   else (eval ""*** csb->prev is NULL ***"")"
def/com sb$next = -
  "if (csb->next != 0) -
   then (def csb=csb->next)
   else (eval ""*** csb->next is NULL ***"")"
def/com fsb$next = "def csb=fsb; sb$next; def fsb=csb"
def/com fsb$prev = "def csb=fsb; sb$prev; def fsb=csb"
def/com tsb$next = "def csb=tsb; sb$next; def tsb=csb"
def/com tsb$prev = "def csb=tsb; sb$prev; def tsb=csb"
def/com sb$show = -
  "eval/hex csb;
   exam/hex/long csb->next,csb->prev;
   exam/dec/long csb->pos;
   exam/a:50 csb->chars"
def/com fsb$show = "def csb=fsb; eval ""fsb -->""; sb$show"
def/com tsb$show = "def csb=tsb; eval ""tsb -->""; sb$show"
!
def/com str$init = -
  "def/add str$len=str$dat;-
   def/add str$str=str$dat+8;-
   def/val str$offset=(str$len+0d);-
   def/val str$offset=(str$offset)&(~3)"
def/com str$next = -
  "def/val str$dat=str$dat+str$offset;-
   str$init"
def/com str$show = -
  "eval ""Length(hex,dec), contents(hex,asciz) --> "";-
   exam/hex str$len;  exam/dec str$len;-
   exam/quad str$str; exam/az str$str"
def/com str$walk = -
  "while ((str$dat < str$blkend) && (str$len < 0400)) -
   do (str$show; str$next); -
   eval ""(str$dat > str$blkend) || (str$len > 03ff)"";-
   eval ""str$dat, str$blkend, str$len, str$offset, str$str -->"";-
   eval str$dat, str$blkend, str$len, str$offset, str$str"
!
eval "*** Setting radix to hex, commands depend on this!               ***"
set radix hex
eval "*** Setting str$dat = 0f860c == first_string_block+0c            ***"
eval "*** Set based on _value_ of first_string_block                   ***"
eval "*** Please confirm that first_string_block (next line) is 0f8600 ***"
eval first_string_block
def/add str$blk = first_string_block
def/val str$blkbeg = &(first_string_block->chars[0])
def/val str$blkbeg = str$blkbeg-0c
def/val str$blkend = str$blkbeg+first_string_block->pos
def/val str$dat = 0f860c
str$init; str$show
def/com str$usrhelp = -
  "eval ""*** Please do `def/val str$dat' to the _value_ of str$blkbeg+0c ***"" -
   eval ""*** then do `str$init': str$blkbeg -->"",str$blkbeg "
def/com str$moveptr = -
  "def/val str$blkbeg=&(str$blk->chars[0]);-
   def/val str$blkbeg=str$blkbeg-0c;-
   def/val str$blkend=str$blkbeg+fsb->pos"
def/com str$nextblk = -
  "if (str$blk->next != 0) -
   then (def str$blk=str$blk->next; def csb=str$blk; sb$show; str$moveptr; str$usrhelp) -
   else (eval ""*** str$blk->next is NULL"")"
def/com str$prevblk = -
  "if (str$blk->prev != 0) -
   then (def str$blk=str$blk->prev; def csb=str$blk; sb$show; str$moveptr; str$usrhelp) -
   else (eval ""*** str$blk->prev is NULL"")"
