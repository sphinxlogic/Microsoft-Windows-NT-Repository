%
% Mute (Dead or accent) keys for JED.
%
%   To use this package, put a line like the following in your jed.rc
%
%    mute_set_mute_keys ("`'^");
%
%   Here, the keys `, ', and ^ will be mute keys.  That is, pressing one of 
%   these followed by the character to accent will insert an accented 
%   character in the buffer.   This affects only the local keymap.  Valid
%
%   Valid Mute keys are:  
%      ^, ~, ', `, \d168 (ISO Diaeresis), \d180 (ISO Acute), and ".

define mute_insert_accent (wants_accent, ok_chars, maps_to)
{
   variable pos, ch;
   
   ch = maps_to[0];
   if (wants_accent)
     {
	!if (input_pending (10)) 
	  {
	     message (strcat (char(ch), "-"));
	     update (0);
	  }
	
	ch = getkey ();
	pos = is_substr (ok_chars, char (ch));
	!if (pos)
	  {
	     beep ();
	     return;
	  }
	pos--;
	ch = maps_to[pos];
     }
   insert_char (ch);
}

variable Mute_Acute = 0;
variable Mute_ISOAcute = 0;
variable Mute_Circumflex = 0;
variable Mute_Grave = 0;
variable Mute_Tilde = 0;
variable Mute_Diaeresis = 0;
variable Mute_ISODiaeresis = 0;

define mute_set_mute_keys (str)
{
   variable i;

   Mute_Acute = 0;
   Mute_ISOAcute = 0;
   Mute_Circumflex = 0;
   Mute_Grave = 0;
   Mute_Tilde = 0;
   Mute_Diaeresis = 0;
   Mute_ISODiaeresis = 0;
   
   _for (0, strlen(str) - 1, 1)
     {
	i = ();
	switch (str[i])
	  {
	   case '\'':
	     local_setkey ("mute_keymap_39", "'");
	     Mute_Acute = 1;
	  }
	  {
	     %% Asciitilde would be better to acute accent, if included in keyboard
	   case '\d180':
	     local_setkey ("mute_keymap_180", "\d180");
	     Mute_ISOAcute = 1;
	  }
	  {
	   case '^':
	     local_setkey ("mute_keymap_94", "^");
	     Mute_Circumflex = 1;
	  }
	  {
	   case '`':
	     local_setkey ("mute_keymap_96", "`");
	     Mute_Grave = 1;
	  }
	  {
	   case '~':
	     local_setkey ("mute_keymap_126", "~");
	     Mute_Tilde = 1;
	  }
	  {
	   case '"':
	     local_setkey ("mute_keymap_34", "\"");
	     Mute_Diaeresis = 1;
	  }
	  {
	     %% ISOLatin 1 diaeresis would be better, if included in keyboard
	   case '\d168':
	     local_setkey ("mute_keymap_168", "\d168");
	     Mute_ISODiaeresis = 1;
	  }
     }
}


define mute_keymap_39 () % ' map
{
#ifdef MSDOS OS2
   variable ok_chars = "'Eeaiou";
   variable maps_to  = "'\d144\d130\d160\d161\d162\d163";
#else
   variable ok_chars = "'AEIOUYaeiouy";
   variable maps_to  =
   "'\d193\d201\d205\d211\d218\d221\d225\d233\d237\d243\d250\d253";
#endif   
   mute_insert_accent (Mute_Acute, ok_chars, maps_to);
}

define mute_keymap_180 () % \d180 map
{
#ifdef MSDOS OS2
   variable ok_chars = "'Eeaiou";
   variable maps_to  = "'\d144\d130\d160\d161\d162\d163";
#else
   variable ok_chars = "\d180AEIOUYaeiouy";
   variable maps_to  =
   "\d180\d193\d201\d205\d211\d218\d221\d225\d233\d237\d243\d250\d253";
#endif   
   mute_insert_accent (Mute_ISOAcute, ok_chars, maps_to);
}

define mute_keymap_94 () % ^ map
{
#ifdef OS2 MSDOS
   variable ok_chars = "^aeiou";
   variable maps_to  = "^\d131\d136\d140\d147\d150";
#else
   variable ok_chars = "^aeiou";
   variable maps_to  = "^\d226\d234\d238\d244\d251";
#endif
   mute_insert_accent (Mute_Circumflex, ok_chars, maps_to);
}
   
define mute_keymap_96  % ` map
{
#ifdef OS2 MSDOS
   variable ok_chars = "`aeiou";
   variable maps_to  = "`\d133\d138\d141\d149\d151";
#else
   variable ok_chars = "`AEIOUaeiou";
   variable maps_to  = 
   "`\d192\d200\d204\d210\d217\d224\d232\d236\d242\d249";
#endif
   mute_insert_accent (Mute_Grave, ok_chars, maps_to);
}

define mute_keymap_126  % ~ map
{
#ifdef MSDOS OS2
   variable ok_chars = "~Nn";
   variable maps_to  = "~\d165\d164";
#else
   variable ok_chars = "~NnAOao";
   variable maps_to  = "~\d209\d241\d195\d213\d227\d245";
#endif
   mute_insert_accent (Mute_Tilde, ok_chars, maps_to);
}

define mute_keymap_34  % \" map
{
#ifdef MSDOS OS2
   variable ok_chars = "\"uaAeioyOU";
   variable maps_to  = "\"\d129\d132\d142\d137\d139\d148\d152\d153\d154";
#else
   variable ok_chars = "\"AEIOUaeiouy";
   variable maps_to  = 
   "\"\d196\d203\d207\d214\d220\d228\d235\d239\d246\d252\d255";
#endif
   mute_insert_accent (Mute_Diaeresis, ok_chars, maps_to);
}

define mute_keymap_168  % \d168 map
{
#ifdef MSDOS OS2
   variable ok_chars = "\"uaAeioyOU";
   variable maps_to  = "\"\d129\d132\d142\d137\d139\d148\d152\d153\d154";
#else
   variable ok_chars = "\d168AEIOUaeiouy";
   variable maps_to  = 
   "\d168\d196\d203\d207\d214\d220\d228\d235\d239\d246\d252\d255";
#endif
   mute_insert_accent (Mute_ISODiaeresis, ok_chars, maps_to);
}

