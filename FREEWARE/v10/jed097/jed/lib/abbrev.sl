
$1 = "Global";
!if (abbrev_table_p ($1)) create_abbrev_table ($1, Null_String);

define toggle_abbrev ()
{
   variable flags = getbuf_info() xor 0x800;
   variable msg = "Abbrev mode OFF";
   setbuf_info(flags);
   if (flags & 0x800) msg = "Abbrev mode ON";
   message (msg);
}

define define_abbrev_for_table (table)
{
   variable n = whatline ();
   variable use_bskip = 1;
   variable exg = "exchange";
   variable abbrev, expans;
   
   if (markp ())
     {
	call (exg);
	if (n == whatline (), call (exg)) use_bskip = 0;
     }
   
   push_spot ();
   if (use_bskip)
     {
	push_mark ();
	bskip_word ();
     }
   expans = bufsubstr ();
   pop_spot ();
   
   abbrev = read_mini (strcat ("Enter abbrev for ", expans), Null_String, Null_String);
   !if (strlen (abbrev)) return;
   
   define_abbrev (table,  abbrev, expans);
}

define define_global_abbrev ()
{
   define_abbrev_for_table ("Global");
}

define define_local_abbrev ()
{
   %define_abbrev_for_table ("Global");
   error ("User interface not implemented yet.");
}

