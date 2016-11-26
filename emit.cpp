#include "emit.h"

extern FILE* oil_file;
vector<astree*> global_list;
// Section 2.1
string eight_spaces = "        ";
// Section 3.1(b)
extern vector<struct astree*> string_const_queue;


// Section 2.1
void emit_global_init(astree* tree) {
	
}

// Section 3.1(d)
void emit_main(astree* tree) {
	fprintf(oil_file, "void __ocmain (void)\n");
	emit_global_init(tree);
//	emit_all(tree);
}

/*
void emit_function(astree* child) {
    leftChild = child->children[0];
    rightChild = child->children[1];

    emit_basetype(leftChild)'
    fprintf(oil_file, "__%s (\n", leftChild->children.back()->lexinfo->c_str());
//    emit_parameter(rightChild);
    fprintf(oil_file, "{\n");
//    emit_block(child->children[2]);
    fprintf(oil_file, "}\n");
}

void emit_immediate_children(astree* tree) {
    for (int childIdx = 0; childIdx < tree->children.size(); childIdx++) {
        if (tree->children[childIdx]->token_code == TOK_FUNCTION) {
            emit_function(tree->children[childIdx]);
        }
    }
}

// Section 2.1(c)
void emit_global_vars(astree* tree) {
    for (size_t childIdx = 0; childIdx < tree->children.size(); childIdx++) {
        if (tree->children[childIdx]->token_code == TOK_VARDECL) {
//            global_list.pushback(childIdx);
        }
    }
    for (size_t index = 0; index < global_list.size(); index++) {
//        emit_basetype(global_list[index]->children[0]);
        fprintf(oil_file, "__%s;\n", global_list[index]->children[0]->children.back()->lexinfo->c_str());
    }
}
*/

// Section 2.1(b)
void emit_string_const() {
    int charIndex;

    for (size_t index = 0; index < string_const_queue.size(); index++) {
        charIndex = index + 1;
        fprintf(oil_file, "char* s%d = %s;\n", charIndex++, string_const_queue[index]->lexinfo->c_str());
    }
}

// Helper function for emit_struct
void emit_field_name(astree* child, const string* childName) {
	fprintf(oil_file, "f_%s_%s", childName->c_str(), child->children.back()->lexinfo->c_str());
}

// Section 3.1(a)
void emit_struct(astree* tree) {
    astree* currChild = nullptr;
    for (size_t child = 0; child < tree->children.size(); child++) {
        currChild = tree->children[child];
        // Section 2.1(a)
        if (currChild->token_code == TOK_STRUCT) {
            fprintf(oil_file, "struct s_%s {\n", currChild->children[0]->lexinfo->c_str());
            for (size_t childIdx = 1; childIdx < currChild->children.size(); childIdx++) {
                fprintf(oil_file, "%s", eight_spaces.c_str());
                emit_field_name(currChild->children[childIdx], currChild->children[0]->lexinfo);
                fprintf(oil_file, ";\n");
            }
            fprintf(oil_file, "};\n");
        }
    }
}

// Section 3.1
void emit_prolog() {
    fprintf(oil_file, "This is the prolog\n");
}

// Section 3.1
void emit_all(astree* tree) {
    emit_prolog();
    emit_struct(tree);
    emit_string_const();
/*
    emit_global_vars(tree);
    emit_functions(tree);
*/
    emit_main(tree);
}

