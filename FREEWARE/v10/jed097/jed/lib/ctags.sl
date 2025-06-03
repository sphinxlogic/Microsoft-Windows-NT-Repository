%
% read a tags file produced by unix ctags program
%
%   put the line:
%  
%     "find_tag"  "ctags.sl"  autoload  "find_tag" "^[." setkey
%
%   in your .jedrc file  (or perhaps in site.sl)
%   Then this file is loaded  when ^[. (escape-period) is executed
%
%   By default, the file "TAGS" is used.  However, setting the variable
%   'Tag_File' as in
%
%      "mytag.file" =Tag_File
%   
%   will over ride this.
%

. "Tag_File" is_defined 
.   { 
.     ". [Tag_File] \"tags\"  =Tag_File" eval
.   } !if
    
  
. ( [tag str cbuf tbuf file] =tag whatbuf =cbuf " *tags*" =tbuf
.   [dir cbuf] whatbuf =cbuf
.   tbuf bufferp tbuf setbuf
.     {
.       Tag_File insert_file 0 < { "File tags not found!" error } if
.     } !if
    
  % -- Tag_File extract_directory_name =dir
  
.   bob tag bol_fsearch 
.   {"Tag not found!" error} !if
  
.   CASE_SEARCH 1 =CASE_SEARCH
.   "!-z" skip_chars skip_white
.   push_mark
.   "!-z" skip_chars
.   =CASE_SEARCH
.   bufsubstr =file  
  % -- dir file dircat =file
  
  
.   skip_white
  % we are looking at "/^" --- we''d better be!
.   "/^"  looking_at { "bad tag file." error } !if
  
.   2 right pop push_mark
.   "$/" ffind { 
.      eol trim 1 left pop
.      "/" looking_at {pop_mark "bad tag file." error } !if
.   } !if

  
.   bufsubstr =str
  
.   file read_file { "File not found." error} !if 
.   bob str fsearch { tag fsearch pop "Tag file needs updated?" message } !if
.   whatbuf pop2buf cbuf pop2buf
. ) tag_find1

. (
.   [wrd] "_a-z0-9$" =wrd
.   push_spot
.   skip_white
.   wrd bskip_chars
.   push_mark
.   wrd skip_chars
.   bufsubstr
.   pop_spot
. ) ctags_get_word
 
  

. (
.    [tag]
.    "Find tag:" ctags_get_word "" read_mini =tag
   
.    tag strlen { return } !if   % later I will treat this better %
.    tag tag_find1
. ) find_tag
