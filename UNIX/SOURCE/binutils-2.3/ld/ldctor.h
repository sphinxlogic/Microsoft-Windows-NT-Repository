/* Constructors for GNU LD. */

void find_constructors PARAMS ((void));
void ldlang_check_for_constructors PARAMS ((
				     struct lang_input_statement_struct *));

void ldlang_add_constructor PARAMS ((ldsym_type *name));

