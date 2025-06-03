%%
%%  execute man then remove backspaces
%%

. ( [clean buf cmd subj ctrl_h] "*manual-entry*" =buf
.   "Cleaning man page..." =clean
  
.   "man" Null_String Null_String read_mini =subj
.   subj strlen {return} !if
  
#ifdef OS2
.   "man " subj strcat " 2> nul" strcat =cmd
#else
.   "man " subj strcat " 2> /dev/null" strcat =cmd
#endif
.   buf pop2buf
.   0 set_readonly
.   erase_buffer
.   "Getting man page..." flush
.   cmd shell_cmd
.   clean flush
.   8 char =ctrl_h
.   bob "_" ctrl_h strcat Null_String replace     %% remove _^H combinations
.   bob  {ctrl_h fsearch} {del del} while  %% remove remaining overstrikes
  %%
  %%  remove multiple blank lines--- should make this standalone
  %%
.   bob {eolp eobp not and}{del} while
.   {1 down} {
.     eol bolp 
.       { 1 down {break} !if
.         {{eol bolp}{eobp not} andelse} {del} while
.       } if
.   } while
.   bob
  %%
  %%  set buffer no modified
  %%
.   getbuf_info pop 0 setbuf_info
.   clean "done" strcat flush
.   most_mode
.   1 set_readonly
. ) unix_man
