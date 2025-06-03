%% help.sl

!if (is_defined ("Jed_Doc_File"))
{
   variable Jed_Doc_File = expand_jedlib_file ("jed_funs.hlp");
}


%% apropos function
define apropos()
{
   variable n, cbuf, s;
   if (MINIBUFFER_ACTIVE) return;

   s = read_mini("apropos:", Null_String, Null_String);
   n = slapropos(s);
   cbuf = whatbuf();
   pop2buf("*apropos*");
   erase_buffer();
   loop (n) 
     {
	insert(());
	newline();
     }
   buffer_format_in_columns();   
   bob();
   set_buffer_modified_flag(0);
   pop2buf(cbuf);
}

%!% Prototype: String expand_keystring (String key)
%!% This function takes a key string that is suitable for use in a 'setkey'
%!% definition and expands it to a human readable form.  
%!% For example, it expands ^X to the form "Ctrl-X", ^[ to "ESC", 
%!% ^[[A to "UP", etc...
%!% See also: setkey
define expand_keystring (key)
{
   variable n = strlen (key);
   variable str, i = 1, ch, key_string = Null_String, the_key;
   
   while (i <= n)
     {
	ch = key[i - 1];
	++i;
	switch (ch)
	  {
	     (case '^' and (i <= n)) or (ch < 32) :   
	     % control char
	     %% common cases are '^[, ^[[?, ^[O?'
	  
	     if (ch < 32)
	       {
		  str = strcat (char (ch + '@'), substr (key, i, 2));
		  i--;
	       }
	     else str = substr (key, i, 3);
	     
	     if (int(str) == '[')
	       {
		  i += 3;
		  switch (str)
		    { case "[[A" : "UP"; }
		    { case "[[B" : "DOWN"; }
		    { case "[[C" : "RIGHT"; }
		    { case "[[D" : "LEFT"; }
		    { case "[OP" : "GOLD"; }
		    { case "[OA" : "UP"; }
		    { case "[OB" : "DOWN"; }
		    { case "[OC" : "RIGHT"; }
		    { case "[OD" : "LEFT"; }
		    { pop(); "ESC"; i -= 2;
		    }
	       }
	     else
	       {
		  i++;
		  strcat ("Ctrl-", char(int(str)));
	       }
	  }
	  {
	     char (());
	  }
	the_key = ();
	
	if (strlen(key_string)) key_string = strcat(key_string, " ");
	key_string = strcat (key_string, the_key);
     }
   
   key_string;
}


%% show key
define showkey()
{
   variable f, type, the_type = "internal";
   flush("Show Key: ");
   f = get_key_function();        %% also, type is on stack (if defined)
   !if (strlen(f)) {message("Key is undefined."); return; }
   =type;
   
   !if (type) the_type = "S-Lang";
   
   message (Sprintf ("Key \"%s\" runs %s function %s.",
		     expand_keystring(LASTKEY),
		     the_type, f, 3));
}

. (
.    [n cmd]
.    MINIBUFFER_ACTIVE {return} if
.    "Where is command:" Null_String Null_String 'F' read_with_completion =cmd
.    cmd strlen { return } !if
.    cmd which_key =n
.    n {"Not on any keys." message return} !if
.    expand_keystring message
.    --n
.    n { pop } loop
. ) where_is

%%
%%  This next functions requires a doc file to be produced.  This is made
%%  by running the executable mkdoc which comes with the distribution.
%%
%%  to produce it. It should be placed in JED_LIBRARY.
%%

define help_cleanup ()
{
   bob ();
   while (down (1))
     {
	skip_white ();
	if (looking_at ("%!%")) deln (3);
     }
   bob ();
   set_buffer_modified_flag (0);
   set_readonly (1);
}

%% gets help for a function
. (
.   [f] =f
.   [cbuf] whatbuf =cbuf
.   [flen tmp help full_f cbuf]  " *jed-help*" =tmp 
 
.   f strlen =flen
.   "*function-help*" =help  
.   [p n]
  
.   Jed_Doc_File strlen  { "Unable to read Jed_Doc_File!" error} !if

.   f is_defined 
.     { f 1 }
.     { f slapropos }
.    else
.    =n
.   n 1 > {"Building list..." flush} if
.   help pop2buf 0 set_readonly erase_buffer
  
.   n {
.       =full_f
.       f full_f flen strncmp %%flen and 
.       {continue} if
  
.       full_f is_defined 1 < {continue} if
.       "F" full_f strcat Jed_Doc_File get_doc_string 
.         {  
. 	   push_spot
. 	   insert  %% help string
. 	   pop_spot
. 	   ":" ffind  %% function:cfun:T:n  where T is type and n is number of args
. 	     {
. 	       1 right pop "\tC function: " insert
. 	       POINT =p ":" ffind {"Corrupt doc file?" error} !if
. 	       push_spot 1 right pop 
. 	       what_char
. 	         { 'V' == : "void "}   % V
. 		 { 'S' == : "string "} % S
. 		 { pop "int "}            % I default
. 	       switch p =POINT insert pop_spot
       
.                del del del  % :T: deleted
. 	       " (" insert eol trim " args)" insert
. 	     } if
. 	} { full_f insert} 
.       else
.       eob newline 50 {'-' insert_char} loop newline
.     } loop
      
.   n 1 > {"Building list...done." flush} if
.   help_cleanup
.   cbuf pop2buf  
. ) help_for_function

. (  
.   MINIBUFFER_ACTIVE {return} if
.   "Describe Function:" Null_String Null_String read_mini help_for_function
. ) describe_function


. (
.   [tmp help f full_f cbuf]  " *jed-help*" =tmp 
.   "*function-help*" =help  
.   whatbuf =cbuf
.   [p flen n]

.   MINIBUFFER_ACTIVE {return} if
.   "Describe Variable:" Null_String Null_String read_mini =f
.   f strlen =flen %%flen {return} !if
  
.   Jed_Doc_File strlen  { "Unable to read Jed_Doc_File!" error} !if
  
.   f slapropos =n
.   n 1 > {"Building list..." flush} if
.   help pop2buf 0 set_readonly erase_buffer
  
.   n {
.       =full_f
.       f full_f flen strncmp %%flen and 
.       {continue} if
  
.       full_f is_defined -1 > {continue} if
.       "V" full_f strcat " " strcat Jed_Doc_File get_doc_string
.         {  
. 	   push_spot
. 	   insert  %% help string
. 	   pop_spot
. 	} { full_f insert} 
.       else
       
.       eol trim "\tvalue: " insert full_f eval string insert
      
.       eob newline 50 {'-' insert_char} loop newline
.     } loop
    
    
.   newline
.   help_cleanup
.   cbuf pop2buf
. ) describe_variable


. (
.   [flags modstr]
.   whatmode =flags =modstr
.   modstr {strlen not : "no_mode" =modstr}
.          {is_defined not : 
. 	        modstr strlow =modstr
. 		modstr is_defined
. 		 {
. 		   modstr "_mode" strcat =modstr
. 		 } !if
. 	  }
.        switch 
.   modstr is_defined 
.    {
.      flags  { 1 == :  "text_mode" }
.             { 2 & :  "c_mode" }
.             { pop "no_mode" =modstr} 
. 	 switch
.    } !if
   
.   modstr help_for_function
. ) describe_mode


define describe_bindings ()
{
   flush("Building bindings..");
   variable map = what_keymap ();
   variable buf = whatbuf ();
   variable cse = CASE_SEARCH;  CASE_SEARCH = 1;
   pop2buf("*KeyBindings*");
   erase_buffer ();
   dump_bindings (map);
   bob(); replace ("ESC [ A", "UP");
   bob(); replace ("ESC [ B", "DOWN");
   bob(); replace ("ESC [ C", "RIGHT");
   bob(); replace ("ESC [ D", "LEFT");
   bob(); replace ("ESC O P", "GOLD");
   bob();
   CASE_SEARCH = cse;
   set_buffer_modified_flag(0);
   pop2buf (buf);
   message ("done");
}

	    
   
