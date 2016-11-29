#include "emit.h"

extern FILE* oil_file;
// Section 2.1
string eight_spaces = "        ";
// Section 3.1(b)
extern vector<struct astree*> string_const_queue;

void emit_basetype (astree* root) {
   if (root->sym->attributes[ATTR_int])
      fprintf (outf, "int");

   if (root->sym->attributes[ATTR_void])
      fprintf (outf, "void");

   if (root->sym->attributes[ATTR_string])
      fprintf (outf, "char*");

   if (root->sym->attributes[ATTR_struct])
      fprintf (outf, "struct %s*", root->sym->struct_name->c_str());

   if (root->sym->attributes[ATTR_array])
      fprintf (outf, "*");

   fprintf (outf, " ");
}


// Section 2.1
void emit_global_init(astree* tree) {
	
}

// Section 3.1(d)
void emit_main(astree* tree) {
	fprintf(oil_file, "void __ocmain (void)\n");
//	emit_global_init(tree);
	//emit(tree);
}

void emit_function(astree* child) {
    leftChild = child->children[0];
    rightChild = child->children[1];

    emit_basetype(leftChild);
   // fprintf(oil_file, "__%s (\n", leftChild->children.back()->lexinfo->c_str());
//    emit_parameter(rightChild);
    //fprintf(oil_file, "{\n");
//    emit_block(child->children[2]);
    //fprintf(oil_file, "}\n");
}





/*
// Helper function for emit_while that emits the statement
void emit_statement(astree* node) {
	
}

// Helper function for Section 3.2 that returns the name of the operand
string emit_oper_name(astree* node) {
	return ;
}

// Helper function for emit_while that emits a child or expression unless it's an operand
void emit_node(astree* node) {
	if () {
		
		return;
	}
	
}

// Section 3.2(h)
// The "return" statement does not have an operand. Therefore, just return.
void emit_return_wo_oper() {
	fprintf(oil_file, "%s return;\n", eight_spaces.c_str());
}

// Section 3.2(h)
// The "return" statement has an operand. Therefore, return the operand.
void emit_return_w_oper(astree* tree) {
	fprintf(oil_file, "%sreturn %s;\n", eight_spaces.c_str(), );
}

// Section 3.2(g)
void emit_if(astree* tree) {
	emit_node(tree->children[0]);
	fprintf(oil_file, "%sif (!%s) goto fi_%d_%d_%d;\n", eight_spaces.c_str(), emit_oper_name(tree), tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset);
	emit_node(tree->children[1]);
	fprintf(oil_file, "fi_%d_%d_%d:;\n", tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset);
}

// Section 3.2(f)
void emit_ifelse(astree* tree) {
	emit_node(tree->children[0]);
	fprintf(oil_file, "%sif (!%s) goto else_%d_%d_%d;\n", eight_spaces.c_str(), emit_oper_name(tree), tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset);
	emit_node(tree->children[1]);
	fprintf(oil_file, "%sgoto fi_%d_%d_%d;\n", eight_spaces.c_str(), tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset);
	fprintf(oil_file, "else_%d_%d_%d:;\n", tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset);
	emit_node(tree->children[2]);
	fprintf(oil_file, "fi_%d_%d_%d:;\n", tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset);
}

// Section 3.2(e)
void emit_while(astree* tree) {
	fprintf(oil_file, "while_%d_%d_%d:\n;", tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset);
	emit_node(tree->children[0]);
	fprintf(oil_file, "%sif (!%s) goto break_%d_%d_%d;\n", eight_spaces.c_str(), emit_oper_name(tree), tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset);
	emit_statement(tree->children[1]);
	fprintf(oil_file, "goto while_%d_%d_%d;\n break_%d_%d_%d:;\n", tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset, tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset);
}

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
    // Section 2.4(a)
    for (size_t index = 0; index < global_list.size(); index++) {
//        emit_basetype(global_list[index]->children[0]);
        fprintf(oil_file, "__%s;\n", global_list[index]->children[0]->children.back()->lexinfo->c_str());
    }
}
*/

// Section 2.1(b)
void emit_stringdef() {
// MSI: Maybe use block_nr of yyprse_astree instead?
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

// Section 2.1(a) and 3.1(a)
void emit_struct(astree* tree) {
    astree* currChild = nullptr;
    for (size_t child = 0; child < tree->children.size(); child++) {
        currChild = tree->children[child];
        // Section 2.1(a) and Section 2.4(d)
        if (currChild->token_code == TOK_STRUCT) {
            fprintf(oil_file, "s_%s {\n", currChild->children[0]->lexinfo->c_str());
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
//void emit_prolog() {
void emit_header() {
    fprintf(oil_file, "#define __OCLIB_C__\n#include \"oclib.oh\"\n\n");
}

// Functions that go inside of emit_program() according to MSI
void emit_program(astree* tree) {
//  emit_struct(tree);
    emit_stringdef();
    //emit_vardef();
//    emit_global_vars(tree);
    //emit_function(tree);
}

// Section 3.1
void emit_everything(astree* tree) {
    //emit_prolog();
    emit_header();
    emit_program(tree);
    emit_main(tree);
}

