/*
 * This is the inner hashtable
 * Build a symbol stack
*/
#include "symstack.h"


/*
// For go_block()
    symbol_stack.push_back(nullptr);
    block_stack.push_back(next_block++);
*/

// From Section 3.2(c)
void symstack::enter_block() {
    // Push the global block count onto the block count stack
    block_stack.push_back(next_block);
    // Increment next_block
    next_block++;
    // Push nullptr onto the top of symbol_stack
    symbol_stack.push_back(nullptr);
}

// From Section 3.2(d)
void symstack::leave_block() {
    // Pop the global block number from the block count stack
    block_stack.pop_back();
    // Pop the top of symbol_stack
    symbol_stack.pop_back();
}

// From Section 3.2(e)
void symstack::define_ident(astree* node) {
    // If the symbol table is null, create it
    if (symbol_stack.back() == nullptr) {
        symbol_stack.back() = new symbol_table;
    }
    // Define and insert the new symbol into the symbol table at the 
    // top of the symbol stack
    insert_symbol (node, symbol_stack.back());
}

// From Section 3.2(f)
symbol* symstack::search_ident(astree* node) {
    // Start from the stop of the symbol stack and search downwards
    for (int i = symbol_stack.size() - 1; i >= 0; i--) {
        // If the entry is empty (the symbol table has nothing in it or 
        // does not exist), skip over it
        if ((symbol_stack[i] == nullptr) || 
           (!symbol_stack[i]->empty())) {
           continue;
        }
        // Otherwise, check each symbol table for the identifier
        symbol* sym = search_symbol(symbol_stack[i], node);
        // If the identifier is found, return it
        if (sym != nullptr) {
           return sym;
        }
    }
    return nullptr;
}

// Helper function for tablemanager.cpp
symbol_table* symstack::symStackTop() {
    // If the top of the symbol stack is not a nullptr, return it
    if (symbol_stack.back() != nullptr) {
    }
    // Otherwise, create a new entry (a new symbol table)
    else {
        symbol_stack.back() = new symbol_table;
    }
    return symbol_stack.back();
}
