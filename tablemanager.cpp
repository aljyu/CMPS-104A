#include <stdbool.h>
#include "tablemanager.h"

extern vector<symbol_table*> symbol_stack;
extern vector<size_t> block_stack;
extern size_t next_block;
extern FILE* sym_file;


/*------------------------ HELPER FUNCTIONS ------------------------*/
// Initialize everything
TableManager::TableManager() {
    /*
      * Create the 3 different symbol tables
     */
    // Type name symbol table
    type_name = new symbol_table;
    // Field name symbol table
    field_name = new symbol_table;
    // Function and variable name symbol table
    identifier_name = new symbol_table;
    stack = new symstack;
    stack->symbol_stack.push_back(new symbol_table);
    stack->block_stack.push_back(0);

    /*
     * Create strings to keep track of which struct and func we are in
     */    
    // Which struct we are currently in
    currStruct = nullptr;
    // Which func we are currently in
    currFunc = nullptr;

    // Create a local variable to keep track of the scope
    scope_nr = 0;

    // Set the bits so we know which attributes are references, 
    // primitives, or both
    attr_prim.set(ATTR_int);
    attr_ref.set(ATTR_array);
    attr_ref.set(ATTR_string);
    attr_ref.set(ATTR_struct);
    attr_ref.set(ATTR_array);
    attr_prim_ref.set(ATTR_int);
    attr_prim_ref.set(ATTR_array);
    attr_prim_ref.set(ATTR_string);
    attr_prim_ref.set(ATTR_struct);
    attr_prim_ref.set(ATTR_null);
    // Set the bits so we know which attributes go with arrays
    array.set(ATTR_int);
    array.set(ATTR_string);
    array.set(ATTR_struct);
}

string TableManager::attrList(symbol* sym, int alreadyDone) {
    string list = "";
    for(int index = 0; index < ATTR_bitset_size; index++) {
        if ((index != alreadyDone) && (sym->attributes.test(index))) {
            switch (index) {
                case 0:
                    list += "void ";
                    break;
                case 1:
                    list += "int ";
                    break;
                case 2:
                    list += "null ";
                    break;
                case 3:
                    list += "string ";
                    break;
                case 4:
                    list += "struct ";
                    break;
                case 5:
                    list += "array ";
                    break;
                case 6:
                    list += "function ";
                    break;
                case 7:
                    list += "variable ";
                    break;
                case 8:
                    list += "field ";
                    break;
                case 9:
                    list += "typeid ";
                    break;
                case 10:
                    list += "param ";
                    break;
                case 11:
                    list += "lval ";
                    break;
                case 12:
                    list += "constant ";
                    break;
                case 13:
                    list += "vreg ";
                    break;
                case 14:
                    list += "vaddr ";
                    break;
                case 15:
                    list += "bitset_size ";
                    break;
                default:
                    list += "attrList default ";
                    break;
            }
            if (index == ATTR_struct && sym->strType) {
                list += "\"" + *sym->strType + "\"" + " ";
            }
        }
    }
    return list;
}

symbol* TableManager::get_declar_sym(attr_bitset testAttr, 
                      symbol_table* table, astree* node, 
                      size_t blockNum) {
        if ((node->token_code == TOK_VOID) && 
           (!testAttr.test(ATTR_function))) {
                errprintf
                   ("Error! There cannot be a void declaration!\n");
                return nullptr;
        }

        astree* declar;
        const string* declarType;

        if (testAttr.test(ATTR_array)) {
                switch (node->children[0]->token_code) {
                        case TOK_VOID:
                                errprintf
                           ("Error! There cannot be a void array!\n");
                                return nullptr;
                        case TOK_INT:
                                testAttr.set(ATTR_int);
                                break;
                        case TOK_STRING:
                                testAttr.set(ATTR_string);
                                break;
                        case TOK_TYPEID:
                                testAttr.set(ATTR_struct);
                                break;
                        case TOK_ARRAY:
                                testAttr.set(ATTR_array);
                                break;
                        default:
                                break;
                }
                declar = node->children[1];
                declarType = node->children[0]->lexinfo;
        }
        else {
                declar = node->children[0];
                declarType = node->lexinfo;
        }

        symbol* sym = search_symbol(table, declar);
        if (sym != nullptr) {
                if (testAttr.test(ATTR_function)) {
                }
                else {
                        errprintf
            ("Error! There cannot be a duplicate declaration!\n");
                        return nullptr;
                }
        }

        sym = insert_symbol(declar, table);
        sym->attributes = testAttr;
        sym->block_nr = blockNum;
        node->block_nr = blockNum;
        declar->block_nr = blockNum;

        if (testAttr.test(ATTR_struct)) {
                declar->strType = declarType;
                sym->strType = declarType;
        }
        return sym;
}

void TableManager::do_block(astree* node) {
    astree* rightRChild = node->children[2];
    if (rightRChild == nullptr) {
        return;
    }
    rightRChild->block_nr = stack->block_stack.back();

    for (size_t childIdx = 0; childIdx < rightRChild->children.size(); 
        childIdx++) {
        typecheck(rightRChild->children[childIdx]);
    }

    fprintf(sym_file, "\n");
}

symbol* TableManager::do_parameter(symbol_table* symStackTop, 
                                  astree* node) {
        attr_bitset tempAttrList;
        tempAttrList.set(ATTR_param);
        tempAttrList.set(ATTR_variable);
        tempAttrList.set(ATTR_lval);

        switch(node->token_code) {
                case TOK_VOID:
                        tempAttrList.set(ATTR_void);
                        break;
                case TOK_INT:
                        tempAttrList.set(ATTR_int);
                        break;
                case TOK_STRING:
                        tempAttrList.set(ATTR_string);
                        break;
                case TOK_TYPEID:
                        tempAttrList.set(ATTR_struct);
                        break;
                case TOK_ARRAY:
                        tempAttrList.set(ATTR_array);
                        break;
                default:
                        break;
        }

        symbol* param_sym = get_declar_sym(tempAttrList, symStackTop, 
                                    node, stack->block_stack.back());
        if (param_sym == nullptr) {
                return nullptr;
        }
    fprintf(sym_file, "    %s (%ld.%ld.%ld) {%ld} %s\n", 
            param_sym->node->lexinfo->c_str(), 
            param_sym->node->lloc.filenr, param_sym->node->lloc.linenr, 
            param_sym->node->lloc.offset, param_sym->block_nr, 
            attrList(param_sym, ATTR_bitset_size).c_str());
        return param_sym;
}

void TableManager::parameter_traversal(symbol* sym, astree* node) {
    astree* rightChild = node->children[1];
    rightChild->block_nr = stack->block_stack.back();
    symbol* param_sym;

    for (size_t childIdx = 0; childIdx < rightChild->children.size(); 
         childIdx++) {
        param_sym = do_parameter(stack->symStackTop(), 
                                 rightChild->children[childIdx]);
        if (param_sym != nullptr) {
            sym->parameters.push_back(param_sym);
        }
    }

    fprintf(sym_file, "\n");
}

void TableManager::do_function(astree* parent, astree* leftChild) {
    // Get the left child
    // Check to see if the left child is TOK_ARRAY
//    if (parent->children.size() == 2) {
        astree* typenode;
        // If the left child is TOK_ARRAY, get its right child
        if (leftChild->token_code == TOK_ARRAY) {
            typenode = leftChild->children[1];
        }
        // Otherwise, get its left child
        else {
            typenode = leftChild->children[0];
        }
        // Search for the symbol to make sure that it doesn't exist
        if (search_symbol(stack->symbol_stack[0], typenode) 
            != nullptr) {
            return;
        }
//    }
    attr_bitset tempAttr;
    tempAttr.set(ATTR_function);

    switch(leftChild->token_code) {
        case TOK_VOID:
            tempAttr.set(ATTR_void);
            break;
        case TOK_INT:
            tempAttr.set(ATTR_int);
            break;
        case TOK_STRING:
            tempAttr.set(ATTR_string);
            break;
        case TOK_TYPEID:
            tempAttr.set(ATTR_struct);
            break;
        case TOK_ARRAY:
            tempAttr.set(ATTR_array);
            break;
        default:
            break;
    }

    symbol* sym = get_declar_sym(tempAttr, stack->symbol_stack[0], 
                            leftChild, stack->block_stack.back());
    if (sym == nullptr) {
        return;
    }
    // Print out the symbol
    fprintf(sym_file, "%s (%ld.%ld.%ld) {%ld} %s\n", 
            sym->node->lexinfo->c_str(), sym->node->lloc.filenr, 
            sym->node->lloc.linenr, sym->node->lloc.offset, 
            sym->block_nr, attrList(sym, ATTR_bitset_size).c_str());
        // Enter the block
        stack->enter_block();
        // Store the name of the current struct
        currStruct = sym->node->lexinfo;
        // Traverse the parameters
        parameter_traversal(sym, parent);
        if (parent->token_code == TOK_FUNCTION) {
                do_block(parent);
        }
        stack->leave_block();
        currStruct = nullptr;
}

/*
void TableManager::child_to_parent_attributes(astree* parent, 
                                              astree* child) {
    for (size_t gen = 0; gen < ATTR_bitset_size; gen++) {
        if (child->attributes[gen]) {
            parent->attributes.set(gen);
        }
    }
}

void TableManager::child_to_parent_types(astree* parent, 
                                         astree* child) {
    for (size_t gen = 0; gen < ATTR_function; gen++) {
        if (child->attributes[gen]) {
            parent->attributes.set(gen);
        }
    }
}
*/

symbol* TableManager::getSymbol(attr_bitset testAttr, 
                                symbol_table* fields, astree* node, 
                                size_t blockNum) {
    if ((node->token_code == TOK_VOID) && 
        (!testAttr.test(ATTR_function))) {
        errprintf("Error! There cannot be a void declaration!\n");
        return nullptr;
    }

    astree* declar;
    const string* declarType;

    if (testAttr.test(ATTR_array)) {
        switch (node->children[0]->token_code) {
            case TOK_VOID:
                errprintf("Error! There cannot be a void array!\n");
                return nullptr;
            case TOK_INT:
                testAttr.set(ATTR_int);
                break;
            case TOK_STRING:
                testAttr.set(ATTR_string);
                break;
            case TOK_TYPEID:
                testAttr.set(ATTR_struct);
                break;
            case TOK_ARRAY:
                testAttr.set(ATTR_array);
                break;
            default:
                break;
        }
        declar = node->children[1];
        declarType = node->children[0]->lexinfo;
    }
    else {
        declar = node->children[0];
        declarType = node->lexinfo;
    }

    symbol* sym = search_symbol(fields, declar);
    if (sym != nullptr) {
        if (testAttr.test(ATTR_function)) {
        }
        else {
            errprintf
            ("Error! There cannot be a duplicate declaration!\n");
            return nullptr;
        }
    }

    sym = insert_symbol(declar, fields);
    sym->attributes = testAttr;
    sym->block_nr = blockNum;
    node->block_nr = blockNum;
    declar->block_nr = blockNum;

    if (testAttr.test(ATTR_struct)) {
        declar->strType = declarType;
        sym->strType = declarType;
    }
    return sym;
}

void TableManager::process_fields(symbol_table* fields, 
                                  astree* node) {
    // Set the attributes for the testAttr variable
    attr_bitset testAttr;
    testAttr.set(ATTR_field);
    switch (node->token_code) {
        case TOK_VOID:
            testAttr.set(ATTR_void);
            break;
        case TOK_INT:
            testAttr.set(ATTR_int);
            break;
        case TOK_STRING:
            testAttr.set(ATTR_string);
            break;
        case TOK_TYPEID:
            testAttr.set(ATTR_struct);
            break;
        case TOK_ARRAY:
            testAttr.set(ATTR_array);
            break;
        default:
            break;
    }
    // Get the node's symbol
    symbol* sym = getSymbol(testAttr, fields, node, 0);
    // Check to see that the symbol is valid and then print out the 
    // symbol and its info
    if (sym != nullptr) {
        // sym->node->lexinfo is returning "int" but is returning 
        // "foo" instead
        // attrList(sym, ATTR_field)).c_str() should be returning 
        // "int" but isn't returning anything
        fprintf(sym_file, "    %s (%zu.%zu.%zu) field {%s} %s ", 
                (sym->node->lexinfo)->c_str(), sym->node->lloc.filenr, 
                sym->node->lloc.linenr, sym->node->lloc.offset, 
                currStruct->c_str(), 
                (attrList(sym, ATTR_field)).c_str());
    }
}

void TableManager::traverse_fields(symbol* sym, astree* parent, 
                                   astree* leftChild) {
    // Increment scope_nr
    scope_nr++;
    // Store the current struct name
    currStruct = leftChild->lexinfo;
    // Loop through all the fields and process each child's fields
    for (size_t child = 1; child < parent->children.size(); child++) {
        process_fields(sym->fields, parent->children[child]);
//        process_fields(sym, parent->children[child]);
    }
    // Decrement scope_nr
    scope_nr--;
    currStruct = nullptr;
    fprintf(sym_file, "\n");
}

/*---------------------- TRAVERSAL & TYPECHECK ----------------------*/
void TableManager::typecheck(astree* node) {
    // Create 2 new ASTs that correspond to the current node's left 
    // and right child respectively
    astree* leftChild = nullptr;
    astree* rightChild = nullptr;
    // Compilation complained about multiple declarations of symbol*
    // sym when used inside case statements
    symbol* sym = nullptr;

    // Check to see that the current node's left child exists
    if (node->children.size() > 0) {
        leftChild = node->children[0];
        // Check to see that the current node's right child exists
        if (node->children.size() > 1) {
            rightChild = node->children[1];
        }
    }
    switch (node->token_code) {
        case TOK_VOID:
            break;
        case TOK_INT:
            break;
        case TOK_STRING:
            break;
        case TOK_IF:
            // Done
            break;
        case TOK_ELSE:
            // Done
            break;
        case TOK_WHILE:
            // Done
            break;
        case TOK_RETURN:
            // Done
            break;
        case TOK_STRUCT:
            // Store the left child
            // Search for the left child's symbol
            sym = search_symbol(type_name, leftChild);
            // If it exists, print an error
            if (sym != nullptr) {
                errprintf
                   ("Error! There cannot be a duplicate symbol!\n");
                break;
            }
            // Otherwise, print it out
            fprintf(sym_file, "%s (%zu.%zu.%zu) {0} struct \"%s\"\n", 
                   (leftChild->lexinfo)->c_str(), node->lloc.filenr, 
                   node->lloc.linenr, node->lloc.offset, 
                   (leftChild->lexinfo)->c_str());
            // Insert the symbol into the symbol table
            sym = insert_symbol(leftChild, type_name);
            // DO I NEED THIS?
            sym->attributes.set(ATTR_typeid);
            sym->fields = new symbol_table;
            sym->block_nr = 0;
            leftChild->sym = sym;
            leftChild->block_nr = 0;
            // Traverse the fields
            traverse_fields (sym, node, leftChild);
            break;
        case TOK_NULL:
            // Done
            node->attributes.set(ATTR_null);
            node->attributes.set(ATTR_const);
            break;
        case TOK_NEW:
//            child_to_parent_attributes(node, leftChild);
            break;
        case TOK_ARRAY:
            break;
        case TOK_EQ:
            // Done
            break;
        case TOK_NE:
            // Done
            break;
        case TOK_LT:
            // Done
            break;
        case TOK_LE:
            // Done
            break;
        case TOK_GT:
            // Done
            break;
        case TOK_GE:
            // Done
            break;
        case TOK_IDENT:
            sym = stack->search_ident(node);
            if (sym == nullptr) {
                // Check to see if we are looking for a struct
                sym = search_symbol(type_name, node);
            }
            if (sym == nullptr) {
//errprintf("Error! Reference to undefined variables aren't allowed!
//\n");
                break;
            }
            node->attributes = sym->attributes;
            break;
        case TOK_INTCON:
            // Done
            node->attributes.set(ATTR_int);
            node->attributes.set(ATTR_const);
            break;
        case TOK_CHARCON:
            // Done
            node->attributes.set(ATTR_int);
            node->attributes.set(ATTR_const);
            break;
        case TOK_STRINGCON:
            // Done
            node->attributes.set(ATTR_string);
            node->attributes.set(ATTR_const);
            break;
        case TOK_BLOCK:
            stack->enter_block();
            for (size_t i = 0; i < node->children.size(); i++) {
                typecheck(node->children[i]);
            }
            stack->leave_block();
            break;
        case TOK_CALL:
            if (leftChild->sym != nullptr) {
                node->attributes = leftChild->sym->attributes;
                node->attributes.set(ATTR_vreg);
                node->attributes.flip(ATTR_function);
                node->strType = leftChild->sym->strType;
            }
            break;
        case TOK_IFELSE:
            // Done
            break;
        case TOK_INITDECL:
            break;
        case TOK_POS:
            // Done
            if (rightChild == nullptr) {
                break;
            }
            if (!(rightChild->attributes[ATTR_int])) {
                errprintf
                   ("Error! The right hand side must be an int!\n");
                break;
            }
            node->attributes.set(ATTR_int);
            node->attributes.set(ATTR_vreg);
            break;
        case TOK_NEG:
            // Done
            if (rightChild == nullptr) {
                break;
            }
            if (!(rightChild->attributes[ATTR_int])) {
                errprintf
                   ("Error! The right hand side must be an int!\n");
                break;
            }
            node->attributes.set(ATTR_int);
            node->attributes.set(ATTR_vreg);
            break;
        case TOK_NEWARRAY:
            break;
        case TOK_TYPEID:
//            node->attributes.set(ATTR_typeid);
            break;
        case TOK_FIELD:
/*
            node->attributes.set(ATTR_field);
            if (leftChild != nullptr) {
                leftChild->attributes.set(ATTR_field);
                child_to_parent_types(node, leftChild);
            }
*/
            break;
        case TOK_ORD:
            break;
        case TOK_CHR:
            break;
        case TOK_ROOT:
            // Done
            break;
        case TOK_DECLID:
            break;
        case TOK_INDEX:
            break;
        case TOK_NEWSTRING:
            break;
        case TOK_RETURNVOID:
            // Done
            break;
        case TOK_VARDECL:
            break;
        case TOK_FUNCTION:
            do_function(node, leftChild);
            break;
        case TOK_PARAMLIST:
            break;
        case TOK_PROTOTYPE:
            do_function(node, leftChild);
            break;
        case '=':
            if ((leftChild == nullptr) || (rightChild == nullptr)) {
                break;
            }
            if (!(leftChild->attributes[ATTR_lval])) {
                break;
            }
            node->attributes = (node->attributes & attr_prim_ref);
            node->attributes.set(ATTR_vreg);
            break;
        case '+':
            // Done
            if ((leftChild == nullptr) || (rightChild == nullptr)) {
                break;
            }
            if ((leftChild->attributes[ATTR_int]) && 
               (rightChild->attributes[ATTR_int])) {
                node->attributes.set(ATTR_int);
                node->attributes.set(ATTR_vreg);
            }
            else {
                errprintf("Error! Both sides must be an int!\n");
            }
            break;
        case '-':
            // Done
            if ((leftChild == nullptr) || (rightChild == nullptr)) {
                break;
            }
            if ((leftChild->attributes[ATTR_int]) && 
               (rightChild->attributes[ATTR_int])) {
                node->attributes.set(ATTR_int);
                node->attributes.set(ATTR_vreg);
            }
            else {
                errprintf("Error! Both sides must be an int!\n");
            }
            break;
        case '*':
            // Done
            if ((leftChild == nullptr) || (rightChild == nullptr)) {
                break;
            }
            if ((leftChild->attributes[ATTR_int]) && 
               (rightChild->attributes[ATTR_int])) {
                node->attributes.set(ATTR_int);
                node->attributes.set(ATTR_vreg);
            }
            else {
                errprintf("Error! Both sides must be an int!\n");
            }
            break;
        case '/':
            // Done
            if ((leftChild == nullptr) || (rightChild == nullptr)) {
                break;
            }
            if ((leftChild->attributes[ATTR_int]) && 
               (rightChild->attributes[ATTR_int])) {
                node->attributes.set(ATTR_int);
                node->attributes.set(ATTR_vreg);
            }
            else {
                errprintf("Error! Both sides must be an int!\n");
            }
            break;
        case '%':
            // Done
            if ((leftChild == nullptr) || (rightChild == nullptr)) {
                break;
            }
            if ((leftChild->attributes[ATTR_int]) && 
               (rightChild->attributes[ATTR_int])) {
                node->attributes.set(ATTR_int);
                node->attributes.set(ATTR_vreg);
            }
            else {
                errprintf("Error! Both sides must be an int!\n");
            }
            break;
        case '!':
            // Done
            if (rightChild == nullptr) {
                break;
            }
            if (!(rightChild->attributes[ATTR_int])) {
                errprintf
                   ("Error! The right hand side must be an int!\n");
                break;
            }
            node->attributes.set(ATTR_int);
            node->attributes.set(ATTR_vreg);
            break;
        case '.':
            break;
        case '[':
            break;
        default:
            break;
    }
}


// This function does a post-order depth-first traversal of the AST
// This is an adaption of the pre-order depth-first traversal method 
// from astree.cpp
void TableManager::traverse_AST(astree* root) {
    for (astree* child : root->children) {
        traverse_AST (child);
    }
    typecheck(root);
}

