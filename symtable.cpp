/*
 * This is the outer hash table
 * For building a symbol table
 *
 * http://www.cplusplus.com/reference/utility/make_pair/
*/

#include "symtable.h"


// Helper function for insert_symbol()
// Creates a new symbol
symbol* new_symbol(astree* node) {
    // Create a new symbol that corresponds to the AST node
    symbol* sym = new symbol();
    sym->attributes = node->attributes;
    sym->fields = nullptr;
    sym->filenr = node->lloc.filenr;
    sym->linenr = node->lloc.linenr;
    sym->offset = node->lloc.offset;
    sym->block_nr = node->block_nr;

    sym->node = node;
    node->sym = sym;

    return sym;
}

// Referenced Section 3
// Insert a symbol corresponding to the AST node into the specified 
// symbol table
symbol* insert_symbol(astree* node, symbol_table* table) {
    if ((node != nullptr) && (table != nullptr)) {
        // Create a new symbol and add it into the symbol table
        symbol* sym = new_symbol(node);
        table->insert(make_pair((string*)node->lexinfo, sym));
        return sym;
    }
    return nullptr;
}

// Referenced Section 3
// Search for an identifier in the specified symbol table
symbol* search_symbol(symbol_table* table, astree* node) {
    if ((table != nullptr) && (node != nullptr)) {
        // Check to see that the node exists
        if (table->count((string*)node->lexinfo) != 0) {
            // Find the desired symbol entry inside of the table
            auto wantedEntry = table->find((string*)node->lexinfo);
            // Ensure that there are still elements (symbol_entry) left 
            // inside of the table
            if (wantedEntry != table->end()) {
                // Since every entry in the symbol table is a 
                // pair<const string*, symbol*> the symbol* portion can
                // be accessed using "->second"
                return wantedEntry->second;
            }
        }
    }
    return nullptr;
}

