% untabify region function

% routine to expand a single tab
define untab_untab1 ()
{
   TAB;                       % on stack
   skip_white ();
   what_column ();   % on stack
   bskip_chars ("\t ");
   () - what_column ();   % on stack
   trim ();
   TAB = 0;
   whitespace (());
   TAB = ();
}

define untab ()
{
   check_region (0);
   narrow ();
   bob ();
   while (fsearch ("\t")) untab_untab1 ();
   widen ();
}

