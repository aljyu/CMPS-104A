%{

#include "lyutils.h"
#include "astree.h"
#include "stringset.h"

extern FILE* tok_file;
extern stringset str_string_set_ADT;

#define YY_USER_ACTION  { lexer::advance(); }

#define RETURN(SYMBOL) { \
   yylval = new astree (SYMBOL, lexer::lloc, yytext); \
   str_string_set_ADT.intern(yytext); \
   asg2_astree_print(tok_file, SYMBOL, yytext); \
   return SYMBOL; \
}

%}

%option 8bit
%option debug
%option nodefault
%option noinput
%option nounput
%option noyywrap
%option warn
%option verbose

LETTER          [A-Za-z_]
DIGIT           [0-9]
IDENT           ({LETTER}({LETTER}|{DIGIT})*)
INTCON          ({DIGIT}+)
CHARCON         ('([^\\'\n]|\\[\\'"0nt])')
STRINGCON       (\"([^\\"\n]|\\[\\'"0nt])*\")

%%

"#".*           { lexer::include(); }
[ \t]+          { }
\n              { lexer::newline(); }
"//".*          { }

"="             { RETURN ('='); }
"+"             { RETURN ('+'); }
"-"             { RETURN ('-'); }
"*"             { RETURN ('*'); }
"/"             { RETURN ('/'); }
"^"             { RETURN ('^'); }
"("             { RETURN ('('); }
")"             { RETURN (')'); }
";"             { RETURN (';'); }

"["             { RETURN ('['); }
"]"             { RETURN (']'); }
"{"             { RETURN ('{'); }
"}"             { RETURN ('}'); }
","             { RETURN (','); }
"."             { RETURN ('.'); }
"%"             { RETURN ('%'); }
"!"             { RETURN ('!'); }

"void"          { RETURN (TOK_VOID) ; }
"char"          { RETURN (TOK_CHAR); }
"int"           { RETURN (TOK_INT) ; }
"string"        { RETURN (TOK_STRING) ; }
"if"            { RETURN (TOK_IF) ; }
"else"          { RETURN (TOK_ELSE) ; }
"while"         { RETURN (TOK_WHILE) ; }
"return"        { RETURN (TOK_RETURN) ; }
"struct"        { RETURN (TOK_STRUCT) ; }
"null"          { RETURN (TOK_NULL) ; }
"new"           { RETURN (TOK_NEW) ; }
"[]"            { RETURN (TOK_ARRAY); }
"=="            { RETURN (TOK_EQ); }
"!="            { RETURN (TOK_NE); }
"<"             { RETURN (TOK_LT); }
"<="            { RETURN (TOK_LE); }
">"             { RETURN (TOK_GT); }
">="            { RETURN (TOK_GE); }

{IDENT}         { RETURN (TOK_IDENT); }
{INTCON}        { RETURN (TOK_INTCON); }
{CHARCON}       { RETURN (TOK_CHARCON); }
{STRINGCON}     { RETURN (TOK_STRINGCON); }

.               { lexer::badchar (*yytext); }

%%

