#ifndef __ASTREE_H__
#define __ASTREE_H__

// Import the relevant STL classes
#include <string>
#include <vector>
#include <bitset>
#include <unordered_map>

#include "auxlib.h"


// The enum for all the possible flags
// ATRR_bitset_size is not actually used
// but its value is equal to the number of attributes
enum { ATTR_void, ATTR_int, ATTR_null, ATTR_string,
       ATTR_struct, ATTR_array, ATTR_function, ATTR_variable,
       ATTR_field, ATTR_typeid, ATTR_param, ATTR_lval, ATTR_const,
       ATTR_vreg, ATTR_vaddr, ATTR_bitset_size };

using namespace std;
// Forward declaration of the symbol struct
using attr_bitset = bitset<ATTR_bitset_size>;

struct symbol;


struct location {
   size_t filenr;
   size_t linenr;
   size_t offset;
};

struct astree {
   // Fields.
   int token_code;           // token code
   location lloc;            // source location
   const string* lexinfo;    // pointer to lexical information
   vector<astree*> children; // children of this n-way node
   // For the symbol table
   size_t block_nr;
   attr_bitset attributes;
   struct astree* parent;
   struct symbol* sym;
   const string* strType;

   // Functions.
   astree (int token_code, const location&, const char* lexinfo);
   ~astree();
};

astree* adopt1(astree* node, astree* child);

astree* adopt2(astree* node, astree* left, astree* right);

astree* adopt3(astree* node, astree* left, astree* middle, 
               astree* right);

astree* convert(astree* child, int token_code);

void dump_node(FILE* outfile, astree* node);

void dump_astree(FILE* outfile, astree* root, int depth = 0);

void dump (FILE* outfile, astree* tree);

void print (FILE* outfile, astree* tree, int depth = 0);

void free_ast (astree* node);

void free_ast2 (astree* tree1, astree* tree2);

void destroy (astree* tree1, astree* tree2 = nullptr);

void errllocprintf (const location&, const char* format, const char*);

void asg2_astree_print (FILE* tok_file, int token_code, 
                       const char *yytext);

void asg5_string_const_queue(int token_code, astree* yylval);

#endif

