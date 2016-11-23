/*
 * This is is the inner hash table
 * For building a symbol stack
 *
 * http://www.cplusplus.com/doc/tutorial/classes/
*/
#ifndef __SYMSTACK__H
#define __SYMSTACK__H
#include <vector>
#include "symtable.h"

class symstack {
public:
    // Vector of symbol tables where each entry represents an open block
    vector<symbol_table*> symbol_stack;
    // Block count stack. 
    // Whenever a new block is encountered, the global block count will 
    // be incremented and pushed onto this stack.
    vector<size_t> block_stack;
    // Enters a block
    void enter_block();
    // Leaves a block
    void leave_block();
    // Defines a new identifier
    void define_ident (astree* node);
    // Searches for an identifier
    symbol* search_ident (astree* node);
    // Returns whatever is at the top of the symbol stack
    symbol_table* symStackTop();
};

#endif
