#ifndef __TABLEMANAGER__H
#define __TABLEMANAGER__H

// Import the relevant STL classes
#include <iostream>
#include <string>

#include "astree.h"
#include "symtable.h"
#include "symstack.h"

using namespace std;


class TableManager {
public: 
   symbol_table* type_name;
   symbol_table* field_name;
   symbol_table* identifier_name;
   symstack* stack;
   const string* currStruct;
   const string* currFunc;
   int scope_nr;

   attr_bitset attr_prim;
   attr_bitset attr_ref;
   attr_bitset attr_prim_ref;
   attr_bitset array;
   
   TableManager();
   string attrList(symbol* sym, int alreadyDone);
   symbol* get_declar_sym(attr_bitset testAttr, symbol_table* fields, 
                          astree* node, size_t blockNum);
   void do_block(astree* node);
   symbol* getSymbol(attr_bitset testAttr, symbol_table* fields, 
                     astree* node, size_t blockNum);
   symbol* do_parameter(symbol_table* symStackTop, astree* node);
   void parameter_traversal(symbol* sym, astree* node);
   void process_fields(symbol_table* fields, astree* node);
   void traverse_fields(symbol* sym, astree* parent, astree* leftChild);
   void do_function(astree* parent, astree* leftChild);
   void do_struct(astree* parent, astree* leftChild);
   void do_declar(astree* node);
   void do_new(astree* node);

   // Set the typenames for the tokens
   void typecheck (astree* node);
   // Traverse the AST
   void traverse_AST (astree* root);

};

#endif

