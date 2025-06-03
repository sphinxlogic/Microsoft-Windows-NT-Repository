#include <stdio.h>
#include <string.h>
#include "slang.h"


volatile int Lang_Error = 0;
FILE *FPout = stdout;

void lang_doerror(char *error)
{
   char err[80]; char *str;

   if (!Lang_Error) Lang_Error = UNKNOWN_ERROR;
   *err = 0;
   switch(Lang_Error)
     {
	case (UNDEFINED_NAME): str = "Undefined_Name"; break;
	case (SYNTAX_ERROR): str = "Syntax_Error"; break;
	case (STACK_OVERFLOW): str = "Stack_Overflow"; break;
	case (STACK_UNDERFLOW): str = "Stack_Underflow"; break;
	case (DUPLICATE_DEFINITION): str = "Duplicate_Definition"; break;
	case (TYPE_MISMATCH): str = "Type_Mismatch"; break;
	case(READONLY_ERROR): str = "Variable is read only."; break;
	case (MALLOC_ERROR) : str = "S-Lang: Malloc Error."; break;
      case USER_BREAK: strcpy(err, "User Break!"); break;
	case (INTRINSIC_ERROR): str = "Intrinsic Error"; break;
      case DIVIDE_ERROR: str = "Divide by zero."; break;
	/* application code should handle this */
	default: if (error != NULL) str = error; else str = "Unknown Error.";
     }

   if (*err == 0) sprintf(err, "S-Lang Error: %s\n", str);
   
   fputs(err, stderr);
}

static int Slang_Line_Len = 257;

static int defining_variables = 0;

extern char *byte_compile_name(char *);
void lang_compile(char *s)
{
   int n = Slang_Line_Len;

   int dn;
   
   if (Lang_Error) return;
   
   if (!defining_variables) s = byte_compile_name(s);
   if (*s == '[') defining_variables = 1;
   else if (*s == ']') defining_variables = 0;
   
   dn = strlen(s) + 1;
   n += dn;
   if (n > 250)
     {
	fputs("\n.", FPout);
	n = dn;
     }
   fputs(s, FPout);
   putc(' ', FPout);
   fflush(FPout);
   Slang_Line_Len = n;
}

int doit(char *f)
{
   lang_load_file(f);
   if (Lang_Error && (f != NULL))
     {
	fprintf(stderr, "Parse Error: file: %s\n", f);
	return(-1);
     }
   putc('\n', FPout);
   return(0);
}


int main(int argc, char **argv)
{
   char file[256], *f;

   init_lang();
   if (argc <= 1) return doit(NULL);
   argc--; argv++;
   while (argc--)
     {
	f = *argv++;
	strcpy(file, f);
	strcat(file, "c");
	if ((FPout = fopen(file, "wt")) == NULL)
	  {
	     fprintf(stderr, "Unable to open %s\n");
	     exit(1);
	  }
	fprintf(stderr, "processing %s\n", f);
	Slang_Line_Len = 257;
	if (doit(f)) exit(1);
	fclose (FPout);
     }
   return (0);
}


   

