/*
 * NOTES:
 * "vector" is a template class that is a perfect replacement
 * for C-style arrays.
 * The vector does not need to be deleted afterwards.
 *
 * push_back() appends 1 element at a time to the array.
 * reserve() grows the allocated storage of the vector.
 * capacity() tells you the maximum number of elements that can
 * fit inside of the vector.
 * size() tells you how many element are currently inside of the
 * vector.
 * resize() will enlarge the number of contained elements inside
 * of the vector.
 * pop_back() removes the last element in the vector. It doesn't
 * shrink the capacity of the vector, only the size.
 * begin() points to the first element in the vector.
 * end() points to 1 past the the last element in the vector.
 * clear() removes all elements from the vector.
 *
 *
 * CITATIONS:
 * http://www.codeguru.com/cpp/cpp/cpp_mfc/stl/article.php/c4027/
 * C-Tutorial-A-Beginners-Guide-to-stdvector-Part-1.htm
*/
#include <vector>
#include <string>
using namespace std;

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auxlib.h"
#include "lyutils.h"
#include "stringset.h"

// Declare the name of the program.tok file
extern FILE* tok_file;
astree* yyparse_astree = NULL;

bool lexer::interactive = true;
location lexer::lloc = {0, 1, 0};
size_t lexer::last_yyleng = 0;
vector<string> lexer::filenames;

astree* parser::root = nullptr;

const string* lexer::filename (int filenr) {
   return &lexer::filenames.at(filenr);
}

void lexer::newfilename (const string& filename) {
   lexer::lloc.filenr = lexer::filenames.size();
   lexer::filenames.push_back (filename);
}

void lexer::advance() {
/*
   if (not interactive) {
      if (lexer::lloc.offset == 0) {
         printf (";%2zd.%3zd: ",
                 lexer::lloc.filenr, lexer::lloc.linenr);
      }
      printf ("%s", yytext);
   }
*/
   lexer::lloc.offset += last_yyleng;
   last_yyleng = yyleng;
}

void lexer::newline() {
   ++lexer::lloc.linenr;
   lexer::lloc.offset = 0;
}

void lexer::badchar (unsigned char bad) {
   char buffer[16];
   snprintf (buffer, sizeof buffer,
             isgraph (bad) ? "%c" : "\\%03o", bad);
   errllocprintf (lexer::lloc, "invalid source character (%s)\n",
                  buffer);
}

void lexer::badtoken (char* lexeme) {
   errllocprintf (lexer::lloc, "invalid token (%s)\n", lexeme);
}

void lexer::include() {
   size_t linenr;
   static char filename[0x1000];
   assert (sizeof filename > strlen (yytext));
   int scan_rc = sscanf (yytext, "# %zd \"%[^\"]\"", &linenr, filename);
   if (scan_rc != 2) {
      errprintf ("%s: invalid directive, ignored\n", yytext);
   }else {
      if (yy_flex_debug) {
         fprintf (stderr, "--included # %zd \"%s\"\n",
                  linenr, filename);
      }
      lexer::lloc.linenr = linenr - 1;
      lexer::newfilename (filename);
   }
   fprintf(tok_file, "%s\n", yytext);
}

void yyerror (const char* message) {
   assert (not lexer::filenames.empty());
   errllocprintf (lexer::lloc, "%s\n", message);
}

astree* new_parseroot(void) {
   yyparse_astree = new astree(TOK_ROOT, {0, 0, 0}, "");
   return yyparse_astree;
}

