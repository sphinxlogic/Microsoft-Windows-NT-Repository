{ SDLPAT.PAT
{			C H A N G E    L O G
{
{      Date	! Name	! Description
{_______________!_______!______________________________________________________
{   11-Jan-1988	|  PG	| X3.2-0 Added STRING clause to CONSTANT.
{		!	|        Changed STRING token to STRING_LITERAL
{_______________!_______!______________________________________________________
{   14-Jan-1988	|  jg	| X3.2-1 Added standard change log.
{   		|	| 	 Added syntax for TYPEDEF, Entry point
{   		|	| 	 return types, and SIZEOF.
{_______________!_______!______________________________________________________
{   22-Jan-1988	|  jg	| X3.2-2 Add DECLARE statement.
{_______________!_______!______________________________________________________
{   26-Jan-1988	|  jg	| X3.2-3 Changes to DECLARe syntax.
{		|	|	 Add colldecl action.
{_______________!_______!______________________________________________________
{   27-Jan-1988	|  jg	| X3.2-4 Modify address object to allow sizeof if
{		|	|	  not an aggregate name
{_______________|_______|______________________________________________________
{   15-Feb-1988	|  jg	| X3.2-5 Add conditional and literal statements.
{		|	|	 Includes new keywords IFLANGUAGE,
{		|	|	 END_IFLANGUAGE, LITERAL and END_LITERAL.
{		|	|	 Check for null token on NAME.
{_______________|_______|______________________________________________________
{   26-Feb-1988	|  jg	| X3.2-6 Add READ statement
{_______________|_______|______________________________________________________
{   14-Feb-1990 |  lw   | X3.2-7 Add support for INTEGER data type
{               |       |        Add new keywords IFSYMBOL and END_IFSYMBOL
{_______________|_______|______________________________________________________
{   17-Mar-92	|  jak  | EV1-2  Added ALIGN and NOALIGN.
{		|	|	 Corrected spelling of "chkalign".
{		|	|	 Changed "pop_parent" to "popparent"
{		|	|	 and "pop_cond" to "popcond".
{_______________|_______|______________________________________________________
{    4-Jun-92 	|  jak  | EV1-12  Added SIGNED option to integer types.
{_______________|_______|______________________________________________________
{    8-Dec-92 	|  jak  | EV1-20  Syntax changes to simplify SIZEOF.
{   21-Jan-93	|	|	Added object_option to POINTER types.
{		|	|	Added BASEALIGN keyword and option.
{_______________|_______|______________________________________________________
{    8-Feb-93	|  jak	| EV1-21  Added ELSE, ELSEIF to IFSYMBOL.
{_______________|_______|______________________________________________________
{   18-Mar-93	|  jak	| EV1-21A Added action /endconst needed for bug fix.
{_______________|_______|______________________________________________________


}

TERMINAL
 
GOALSY					=  00 ;

{ tokens }
COMMENT,t_comment			=  01 ;
LOCAL_NAME,t_local_name			=  02 ;
NAME,t_name				=  03 ;
NUMERIC,t_numeric			=  04 ;
STRING_LITERAL,t_string_literal 	=  05 ;
 
{ statement keywords }
AGGREGATE,t_aggregate			=  06 ;
CONSTANT,t_constant			=  07 ;
'END',t_end				=  08 ;
END_MODULE,t_end_module			=  09 ;
ENTRY,t_entry				=  10 ;
ITEM,t_item				=  11 ;
MODULE,t_module				=  12 ;
EQUALS,t_equals				=  13 ;
 
{ statement options }
COMMON,t_common				=  14 ;
DIMENSION,t_dimension			=  15 ;
PREFIX,t_prefix				=  16 ;
TAG,t_tag				=  17 ;
INCREMENT,t_increment			=  18 ;
PARAMETER,t_parameter			=  19 ;
RETURNS,t_returns			=  20 ;
VALUE,t_value				=  21 ;
GLOBAL,t_global				=  22 ;
VARIABLE,t_variable			=  23 ;
 
{ data types }
ADDRESS,t_address	= 24 ;
BOOLEAN,t_boolean	= 25 ;
BYTE,t_byte		= 26 ;
CHARACTER,t_character	= 27 ;
DECIMAL,t_decimal	= 28 ;
D_FLOATING,t_d_floating	= 29 ;
F_FLOATING,t_f_floating	= 30 ;
G_FLOATING,t_g_floating	= 31 ;
H_FLOATING,t_h_floating	= 32 ;
LONGWORD,t_longword	= 33 ;
OCTAWORD,t_octaword	= 34 ;
QUADWORD,t_quadword	= 35 ;
BITFIELD,t_bitfield	= 36 ;
WORD,t_word		= 37 ;
STRUCTURE,t_structure	= 38 ;
UNION,t_union		= 39 ;
ANY,t_any		= 40 ;
 
{ data type options } 
LENGTH,t_length		= 41 ;
MASK,t_mask		= 42 ;
PRECISION,t_precision	= 43 ;
UNSIGNED,t_unsigned	= 44 ;
VARYING,t_varying	= 45 ;

{new keyword}
TYPEDEF,t_typedef	= 46 ;
ORIGIN,t_origin		= 47 ;
DESCRIPTOR,t_descriptor	= 48 ;
COUNTER,t_counter	= 49 ;
IN,t_in			= 50 ;
OUT,t_out		= 51 ;
NAMED,t_named		= 52 ;
IDENT,t_ident		= 53 ;
BASED,t_based		= 54 ;
FILL,t_fill		= 55 ;
ALIAS,t_alias		= 56 ;
DEFAULT,t_default	= 57 ;
REFERENCE,t_reference	= 58 ;
LINKAGE,t_linkage	= 59 ;
 
{ punctuation }
'!',t_exclamation	= 60;
'&',t_ampersand		= 61;
'(',t_l_paren		= 62;
')',t_r_paren		= 63;
'*',t_star		= 64;
'+',t_plus		= 65;
',',t_comma		= 66;
'-',t_minus		= 67;
'.',t_dot		= 68;
'/',t_slash		= 69;
':',t_colon		= 70;
';',t_semicolon		= 71;
'=',t_equal		= 72;
'@',t_at		= 73;
'^',t_circumflex	= 74;
EOL,t_eol		= 75;
EOF,t_eof		= 76;
INCLUDE,t_include	= 77;
SIGNED,t_signed		= 78;
OPTIONAL,t_optional	= 79;
MARKER,t_marker		= 80;
TYPENAME,t_typename	= 81;
LIST,t_list		= 82;
RTL_STR_DESC,t_rtl_str_desc = 83;
COMPLEX,t_complex	= 84;
STRING,t_string		= 85;
VOID,t_void		= 86;
SIZEOF,t_sizeof		= 87;
DECLARE,t_declare	= 88;
IFLANGUAGE,t_iflanguage = 89;
END_IFLANGUAGE,t_end_iflanguage = 90;
LITERAL,t_literal	= 91;
END_LITERAL,t_end_literal = 92;
READ,t_read		= 93;
INTEGER,t_integer	= 94;
IFSYMBOL,t_ifsymbol	= 95;
END_IFSYMBOL,t_end_ifsymbol = 96;
HARDWARE_ADDRESS,t_hardware_address = 97;
HARDWARE_INTEGER,t_hardware_integer = 98;
POINTER_HW,t_pointer_hw = 99;
POINTER_LONG,t_pointer_long = 100;
POINTER,t_pointer	= 101;
POINTER_QUAD,t_pointer_quad = 102;
INTEGER_BYTE,t_integer_byte = 103;
INTEGER_WORD,t_integer_word = 104;
INTEGER_LONG,t_integer_long = 105;
INTEGER_QUAD,t_integer_quad = 106;
INTEGER_HW,t_integer_hw = 107;
ERRORMARK,t_errormark	= 108 ;
ALIGN,t_align		= 109;
NOALIGN,t_noalign	= 110;
BASEALIGN,t_basealign	= 111;
ELSE,t_else		= 112;
ELSE_IFSYMBOL,t_else_ifsymbol		= 113;
ELSE_IFLANGUAGE,t_else_iflanguage	= 114;
END;
 

 
BEGIN
 
ADDLE			= program GOALSY;
 
program			= m1 m2 ;
 
m1			= {epsilon}				/initialize ;
 
m2			= module_list EOF
			| ERRORMARK EOF 			/SYNERROR;

module_list		= module_declaration 
			| module_list module_declaration ;

module_declaration	= module_statement module_body end_module_statement
			| local_assignment 
			| COMMENT				/commentnod;
 
module_statement	= m_name m_options ';'			/makechild;

m_name			= MODULE namestring			/makmodnod
			| ERRORMARK MODULE namestring		/SYNERROR ;

m_options		= IDENT namestring			/setident
			| {epsilon} ;
 
module_body		= module_body statement 
			| {epsilon} ;
 
end_module_statement	= END_MODULE name_option ';'		/endmod
			| ERRORMARK END_MODULE name_option ';'	/SYNERROR ;
 
name_option		= namestring				/checkname
			| {epsilon} ;
 
statement		= local_assignment 
			| constant_statement 
			| entry_statement 
			| aggregate_statement 
			| item_statement 
			| declare_statement
			| ifsymbol_statement 
			| conditional_statement
			| literal_statement
			| read_statement ';'
			| COMMENT				/commentnod 
			| include_clause ';'
			| ERRORMARK ';'				/SYNERROR
			| ';' ;

include_clause		= INCLUDE include_name stat_list EOF 	/resettext 
			| INCLUDE include_name EOF 		/resettext ;

include_name		= STRING_LITERAL			/includetext ;

read_statement		= READ STRING_LITERAL			/readfile ;

stat_list		= stat_list statement
			| statement ;
 
local_assignment	= local_symbol '=' expression ';'	/localasn ;
 
local_symbol		= LOCAL_NAME				/savelocalname ;
 
constant_statement	= constant_key constant_body ';'	/endconst ;

constant_key		= CONSTANT 				/startconst ;
 
constant_body		= constant_phrase 
			| constant_body ',' comment_list constant_phrase ;

constant_phrase		= name_or_list EQUALS value_or_string	/makcstnod ;

value_or_string		= constant_value constant_options
			| STRING string_value string_options ;

comment_list		= comment_list COMMENT			/commentnod
			| {epsilon} ;

name_or_list		= c_name_1 
			| '(' comment_list name_list ')' c_comment_list 
			| '(' comment_list name_list c_comma2 ')' c_comment_list ;
 
name_list		= c_name_1
			| c_comma1 c_name_1
			| name_list c_comma2 c_name_1 ;

c_name_1		= c_name c_comment_list ;

c_name			= namestring				/maknamlis ;
 
c_comma1		= c_comma				/nullname 
			| c_comma1 c_comma			/nullname ;
 
c_comma2		= c_comma
			| c_comma2 c_comma			/nullname ;

c_comma			= ',' c_comment_list ;

c_comment_list		= c_comment_list COMMENT		/setnamcom
			| {epsilon} ;
 
constant_value		= expression				/setconval ;
 
constant_options	= constant_options INCREMENT expression	/setincr
			| constant_options PREFIX namestring	/saveprefix
			| constant_options TAG namestring	/savetag
			| constant_options COUNTER LOCAL_NAME	/savecounter
			| constant_options TYPENAME typestring	/savetypenam
			| {epsilon} ;
                                                                             
string_value		= STRING_LITERAL			/setconstr ;

string_options		= string_options PREFIX namestring	/saveprefix
			| string_options TAG namestring		/savetag
			| {epsilon} ;

namestring		= NAME					/checknull
			| STRING_LITERAL ;
 
entry_statement		= entry_clause entry_option ';'		/setlineno_checklist;
 
entry_clause		= ENTRY namestring			/makentnod ;
 
entry_option		= entry_option parameter_keyword '(' param_list ')'
								/popparent
			| entry_option return_keyword ret_data_type
				return_name_option
			| entry_option return_keyword ret_data_type
				return_name_option COMMENT	/commentnod
			| entry_option VARIABLE			/setvaropt
			| entry_option TYPENAME typestring	/settypname
			| entry_option ALIAS namestring		/setalias
			| entry_option LINKAGE namestring	/setlink
			| {epsilon} ;
 
parameter_keyword	= PARAMETER				/setparm ;
 
return_keyword		= RETURNS				/setreturn ;
 
ret_data_type		= data_or_user_type
			| VOID 					/setvoid ;

return_name_option	= NAMED namestring			/setretname
			| {epsilon} ;

param_list		= comment_list p1 param p_opt p2
			| param_list ',' comment_list p1 param p_opt p2 ;
 
p1			= {epsilon}				/makparnod ;

p2			= {epsilon}				/setdefprmatt ;

param			= data_or_user_type 
			| ANY					/setany ;

p_opt			= p_opt VALUE				/setvalopt
			| p_opt DESCRIPTOR			/setdescrip
			| p_opt RTL_STR_DESC			/setrtlstrdesc
			| p_opt DIMENSION dimen_expr
			| p_opt IN				/setin
			| p_opt OUT				/setout
			| p_opt LIST				/setlist
			| p_opt NAMED namestring		/setpname
			| p_opt DEFAULT expression		/setdefault
			| p_opt OPTIONAL			/setoptional
			| p_opt REFERENCE			/setref
			| p_opt COMMENT				/commentnod
			| p_opt TYPENAME typestring		/settypname
			| {epsilon} ;

typestring		= NAME                                  /checknull
                        | STRING_LITERAL;

dimen_expr		= expression				/setdimen
			| lodimen ':' expression		/sethidim
			| '*'					/setvardim;
 
lodimen			= expression				/setlodim ;
 
data_type		= BYTE sign_option			/setbyte
			| WORD sign_option			/setword
			| LONGWORD sign_option			/setlong
			| QUADWORD sign_option			/setquad
			| OCTAWORD sign_option			/setocta
			| F_FLOATING complex_option		/setfloat
			| D_FLOATING complex_option		/setdouble
			| G_FLOATING complex_option		/setgrand
			| H_FLOATING complex_option		/sethuge
			| DECIMAL PRECISION '(' prec ',' scale ')' /setdec
			| BITFIELD vield_options		/setvield
			| CHARACTER  char_options		/setchar
			| BOOLEAN				/setbool
			| ADDRESS      object_option		/setaddr 
                        | POINTER      object_option            /setps
                        | POINTER_LONG object_option            /setpl
                        | POINTER_QUAD object_option            /setpq
                        | POINTER_HW   object_option            /setph
                        | INTEGER_BYTE sign_option		/setib
                        | INTEGER_WORD sign_option	        /setiw
                        | INTEGER_LONG sign_option	        /setil
                        | INTEGER_QUAD sign_option	        /setiq
                        | INTEGER_HW   sign_option	        /setih
			| INTEGER      sign_option	        /setint
			| HARDWARE_ADDRESS object_option	/sethwadr
			| HARDWARE_INTEGER sign_option		/sethwint;
 
object_option		= obj_paren object_type base_option ')'	/popparent
			| {epsilon} ;
 
obj_paren		= '('					/makobjnod;
 
object_type		= data_or_user_type dimension_option
			| entry_object entry_option 		/popparent;

entry_object		= ENTRY					/setentry;
 
base_option		= basealign_option 
			| {epsilon} ;

sign_option		= SIGNED				/setsigned
			| UNSIGNED				/setunsign
			| {epsilon} ;

complex_option		= COMPLEX				/setcomplex
			| {epsilon} ;
 
prec			= expression				/setprec ;
 
scale			= expression				/setscale ;
 
vield_options		= vield_options MASK			/setmask
			| vield_options LENGTH expression	/setlength
			| vield_options SIGNED			/setsigned
			| vield_options UNSIGNED		/setunsign
			| {epsilon} ;
 
char_options		= char_options VARYING			/setchrvar
			| char_options LENGTH length_expr
			| {epsilon} ;
 
length_expr		= '*'					/setunklength
			| expression				/setlength ;

dimension_option	= DIMENSION dimen_expr
			| {epsilon} ;
 
aggregate_statement	= aggregate_dcl aggregate_body aggregate_end ;
 
aggregate_dcl		= aggregate_clause aggregate_type 
			  impl_union_datatype aggr_options ';'	/makechild ;
 
aggregate_clause	= AGGREGATE namestring			/makitmnod ;
 
aggregate_type		= STRUCTURE				/setstruc
			| UNION					/setunion ;
 
impl_union_datatype     = data_type
			| {epsilon} ;

aggr_options		= aggr_options COMMON			/setcommon
			| aggr_options GLOBAL			/setglobal
    			| aggr_options TYPEDEF                  /settypedef
			| aggr_options ALIGN			/setalign
			| aggr_options NOALIGN			/setnoalign
			| aggr_options basealign_option
			| aggr_options PREFIX namestring	/setprefix
			| aggr_options TAG namestring		/settag
			| aggr_options DIMENSION dimen_expr
			| aggr_options ORIGIN namestring	/setorigin
			| aggr_options BASED namestring		/basedptr
			| aggr_options BASED
			| aggr_options MARKER namestring	/setmark
			| aggr_options FILL			/setfill
			| {epsilon} ;

aggregate_body		= aggregate_body aggregate_member
			| aggregate_body local_assignment
			| aggregate_body constant_statement
			| aggregate_body COMMENT		/commentnod
			| aggregate_body ifsymbol_clause 
			| aggregate_body ';'
			| {epsilon} ;
 
aggregate_member	= member_name rest_of_member 
			| ERRORMARK ';'				/SYNERROR ;
 
member_name		= namestring				/makitmnod ;
 
rest_of_member		= data_or_user_type member_options ';'	/endmember
			| sub_agg_dcl aggregate_body aggregate_end ;
 
member_options		= member_options PREFIX namestring	/setprefix
			| member_options TAG namestring		/settag
			| member_options FILL			/setfill
			| member_options DIMENSION dimen_expr
			| member_options ALIGN			/setalign
			| member_options NOALIGN		/setnoalign
			| member_options basealign_option
			| {epsilon} ;
 
sub_agg_dcl		= aggregate_type impl_union_datatype 
					member_options ';'	/makechild ;

aggregate_end		= 'END' name_option ';'			/aggend
			| ERRORMARK 'END' name_option ';'	/SYNERROR ;

ifsymbol_clause 	= ifsymbol_start ';' 
						aggregate_body 
			  else_ifsymbol_clause 
			  else_symbol_clause 
			  ifsymbol_end ';' ;

else_ifsymbol_clause	= else_ifsymbol_clause 
			  else_ifsymbol_start ';' aggregate_body 
			| {epsilon} ;

else_symbol_clause	= else_symbol ';'	aggregate_body 
			| {epsilon} ;

ifsymbol_statement 	= ifsymbol_start ';' 
						module_body 
			  else_ifsymbol_statement 
			  else_symbol_statement 
			  ifsymbol_end ';' ;

else_ifsymbol_statement	= else_ifsymbol_statement 
			  else_ifsymbol_start ';' module_body 
			| {epsilon} ;

else_symbol_statement	= else_symbol ';'	module_body 
			| {epsilon} ;

ifsymbol_start		= IFSYMBOL namestring			/ifsymbol_start ;

else_ifsymbol_start	= ELSE_IFSYMBOL namestring		/ifsymbol_elseif ;

else_symbol		= ELSE					/ifsymbol_else ;

ifsymbol_end		= END_IFSYMBOL				/ifsymbol_end ;

item_statement		= item_clause 
			  data_or_user_type item_options ';'	/enditem;
 
item_clause		= ITEM namestring			/makitmnod ;
 
item_options		= item_options COMMON			/setcommon
			| item_options GLOBAL			/setglobal
    			| item_options TYPEDEF			/settypedef
			| item_options PREFIX namestring	/setprefix
			| item_options TAG namestring		/settag
			| item_options DIMENSION dimen_expr
			| item_options basealign_option
			| {epsilon} ;
 
data_or_user_type	= data_type
			| user_typename sizeof_clause_opt	/setuser ;

user_typename		= namestring				/saveusername ;

sizeof_clause_opt	= sizeof_clause
			| {epsilon} ;

sizeof_clause		= sizeof_keyword sizeof_option		/endsizeof ;

sizeof_keyword		= SIZEOF				/setsizeof ;

sizeof_option		= data_or_user_type 
			| '(' expression ')'			/setsizexpr ;

basealign_option	= basealign_key data_or_user_type	/setbasetype 
			| basealign_key '(' expression ')'	/setbaseexpr ;

basealign_key		= BASEALIGN				/setbasealign ;

declare_statement	= decl_clause decl_body decl_options	/setdecl ;

decl_clause		= DECLARE user_typename			/makitmnod ;

decl_body		= sizeof_clause				/setuser ;

decl_options		= decl_options PREFIX namestring	/setprefix
			| decl_options TAG namestring		/settag
			| {epsilon} ;

conditional_statement	= cond1 cond2 module_body
			  END_IFLANGUAGE lang_list_opt		/endcond ;

cond1			= IFLANGUAGE				/startcond ;

cond2			= language_list				/popcond ;

language_list		= lang_name 
			| language_list lang_name ;

lang_name		= namestring				/condobj ;

lang_list_opt		= lang_list_opt namestring		/marklang
			| {epsilon} ;

literal_statement	= literal_keyword 
				stringlist END_LITERAL		/endlit ;

literal_keyword		= LITERAL ';'				/startlit ;

stringlist		= stringlist STRING_LITERAL		/maklitnod
			| {epsilon} ;

expression		= expression '&' expression_1		/andterms
			| expression_1 ;
 
expression_1		= expression_1 '!' expression_2		/orterms
			| expression_2 ;
 
expression_2		= expression_2 '@' expression_3		/shifterms
			| expression_3 ;
 
expression_3		= expression_3 '+' expression_4		/addterms
			| expression_3 '-' expression_4		/subterms
			| expression_4 ;
 
expression_4		= expression_4 '*' expression_5		/multerms
			| expression_4 '/' expression_5		/divterms
			| expression_5 ;
 
expression_5		= '+' expression_6			/plusterm
			| '-' expression_6			/minusterm
			| expression_6 ;
 
expression_6		= NUMERIC				/pushterm
			| LOCAL_NAME				/pushlocal
			| namestring				/pushconst
			| '.'					/pushdot
			| '^'					/pushcirc
			| ':'					/pushcolon
			| '(' expression ')' ;
 
END.
