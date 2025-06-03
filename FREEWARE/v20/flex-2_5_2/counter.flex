%{
int num_lines = 0, num_chars = 0;
%}

%%
\n      ++num_lines; ++num_chars;
.       ++num_chars;
<<EOF>> yyterminate();
%%
main()
   {
   yylex();
   printf( "# of lines = %d, # of chars (incl. '\\n') = %d\n",
           num_lines, num_chars );
   }
