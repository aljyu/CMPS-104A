/*
 * Most of this code was given to us by the TAs in the TA folder
*/
#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

// Import the relevant STL classes
#include <string>
#include <unordered_map>
#include <vector>

#include "lyutils.h"
#include "astree.h"

// Instead of writing "std::string", we can now just write "string"
using namespace std;
// Create a shorthand notation for a symbol_table
using symbol_table = unordered_map<string*, symbol*>;
using symbol_entry = pair<const string*, symbol*>;

extern size_t next_block;
extern FILE* sym_file;


// The actual definition of the symbol struct
struct symbol {
    attr_bitset attributes;
    symbol_table* fields;
    size_t filenr, linenr, offset;
    size_t block_nr;

    astree* node;
    const string* strType;
    vector<symbol*> parameters;
};

symbol* new_symbol (astree* node);
symbol* insert_symbol(astree* node, symbol_table* table);
symbol* search_symbol(symbol_table* table, astree* node);

#endif
