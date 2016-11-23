#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "astree.h"
#include "stringset.h"
#include "lyutils.h"
#include "symtable.h"


astree::astree (int token_code_, const location& lloc_, 
                const char* info) {
   token_code = token_code_;
   lloc = lloc_;
   lexinfo = stringset::intern (info);
   // vector defaults to empty -- no children
   block_nr = 0;
   attributes =  0;
   ///////////////////
   parent = nullptr;
   sym = nullptr;
}

astree::~astree() {
   while (not children.empty()) {
      astree* child = children.back();
      children.pop_back();
      delete child;
   }
   if (yydebug) {
      fprintf (stderr, "Deleting astree (");
      dump (stderr, this);
      fprintf (stderr, ")\n");
   }
}

// Append 1 child to the pre-existing children
// ("node" in this case)
astree* adopt1(astree* node, astree* child) {
   node->children.push_back(child);
   return node;
}

// Append 2 children to the pre-exiting children
// ("node" in this case)
astree* adopt2(astree* node, astree* left, astree* right) {
   adopt1(node, left);
   adopt1(node, right);
   return node;
}

// Append 3 children to the pre-existing children
// ("node" in this case)
astree* adopt3(astree* node, astree* left, astree* middle, 
                        astree* right) {
   adopt1(node, left);
   adopt1(node, middle);
   adopt1(node, right);
   return node;
}

// Convert a token_code into a TOK block
astree* convert(astree* node, int token_code) {
   node->token_code = token_code;
   return node;
}

string listStr(attr_bitset attributes) {
   vector<string> attrList = 
         { "void ", "int ", "null ", "string ", "struct ", "array ", 
           "function " , "variable ", "field ", "typeid ", "param ", 
           "lval ", "const ", "vreg ", "vaddr " };
   string str = "";

   for (int i = 0; i < ATTR_bitset_size; i++) {
      if (attributes.test(i)) {
         str += attrList[i];
      }
   }

   return str;
}

void dump_node(FILE* outfile, astree* node) {
   const char *tname = parser::get_tname (node->token_code);
   if (strstr(tname, "TOK_") == tname) tname += 4;

   attr_bitset testAttr;
   switch(node->token_code) {
      case TOK_TYPEID:
   case TOK_DECLID:
   case TOK_IDENT:
   case TOK_FIELD:
      if (node->sym != nullptr) {
         testAttr = node->sym->attributes;
      }
      break;
   default:
      testAttr = node->attributes;
   }

   fprintf (outfile, "%s \"%s\" (%zu.%zu.%zu) {%zu} %s\n",
            tname, node->lexinfo->c_str(), node->lloc.filenr,
            node->lloc.linenr, node->lloc.offset, node->block_nr, 
            listStr(testAttr).c_str());
}

void dump_astree(FILE* outfile, astree* root, int depth) {
   if (root == NULL) {
      return;
   }
   for (int i = 0; i < depth; i++) {
      fprintf(outfile, "|   ");
   }
   dump_node(outfile, root);
   // While there are still more children belonging to a parent, print
   // run the dump_astree on the children on that hierarchical level.
   // If there are no more children on that level, move down to the next
   // one.
   for (size_t child = 0; child < root->children.size(); ++child) {
      dump_astree (outfile, root->children[child], depth + 1);
   }
}

void dump (FILE* outfile, astree* tree) {
   dump_astree (outfile, tree, 0);
}

void print(FILE* outfile, astree* tree, int depth) {
   fprintf (outfile, "; %*s", depth * 3, "");
   fprintf (outfile, "%s \"%s\" (%zd.%zd.%zd)\n",
            parser::get_tname (tree->token_code), 
            tree->lexinfo->c_str(),
            tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset);
   for (astree* child: tree->children) {
      print (outfile, child, depth + 1);
   }
}

// This function frees a specific token so that it is not put into the 
// AST
void free_ast (astree* node) {
  while (not node->children.empty()) {
    astree* child = node->children.back();
    node->children.pop_back();
    free_ast (child);
  }
  if (node != nullptr) {
    delete node;
  }
}

void free_ast2(astree* tree1, astree* tree2) {
  free_ast (tree1);
  free_ast (tree2);
}

void destroy (astree* tree1, astree* tree2) {
   if (tree1 != nullptr) delete tree1;
   if (tree2 != nullptr) delete tree2;
}

void errllocprintf (const location& lloc, const char* format,
                    const char* arg) {
   static char buffer[0x1000];
   assert (sizeof buffer > strlen (format) + strlen (arg));
   snprintf (buffer, sizeof buffer, format, arg);
   errprintf ("%s:%zd.%zd: %s", 
              (*lexer::filename (lloc.filenr)).c_str(), lloc.linenr, 
               lloc.offset, buffer);
}

void asg2_astree_print (FILE* tok_file, int token_code, 
                        const char *yytext) {
   // Print out the address of the pointer
//   printf("%p\n", tok_file);
   fprintf(tok_file, "%3lu %2lu.%.3lu %3d %-10s (%s)\n",
         lexer::lloc.filenr, lexer::lloc.linenr, lexer::lloc.offset,
         token_code, parser::get_tname(token_code), yytext);
}

