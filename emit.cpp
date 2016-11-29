#include "emit.h"

extern FILE* oil_file;
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
//	emit_global_init(tree);
	//emit(tree);
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
*/

/*
void emit_params (astree* root) {
   
   for (size_t i = 0; i < root->children.size(); ++i)
   {
      fprintf (oil_file, "%s", eight_spaces.c_str());
      emit_func_name (root->children[i]);
      fprintf (oil_file, "_%ld_%s", root->children[i]->sym->block_nr,
            root->children[i]->children.back()->lexinfo->c_str());
      if (i < root->children.size() - 1)
         fprintf (oil_file, ",\n");
   }
   fprintf (oil_file, ")\n");

}
*/

void emit_func_body(astree* root){
   
   for (auto i : root->children){
      
      switch (i->symbol){
        
         case TOK_BLOCK: emit_block (root); break;
        
         case TOK_WHILE: emit_while (root); break;
        
         case TOK_IF: emit_if (root); break;
        
         case TOK_IFELSE: emit_ifelse (root); break;
        
         case TOK_RETURNVOID: emit_returnvoid (root); break;
        
         case TOK_RETURN: emit_return (root); break;

         default: break;    
 
       //default: expr_visit (root);
      }
   }

}

// Helper function for emit_function
void emit_func_name (astree* root) {

   if (root->sym->attributes[ATTR_int]) {
      fprintf (oil_file, "int ");
   }
   if (root->sym->attributes[ATTR_void]) {
      fprintf (oil_file, "void ");
   }
   if (root->sym->attributes[ATTR_string]) {
      fprintf (oil_file, "char* ");
   }
}

void emit_params (astree* root) {

   for (size_t i = 0; i < root->children.size(); ++i)
   {
      fprintf (oil_file, "%s", eight_spaces.c_str());
      emit_func_name (root->children[i]);
      fprintf (oil_file, "_%ld_%s", root->children[i]->sym->block_nr,
            root->children[i]->children.back()->lexinfo->c_str());
      if (i < root->children.size() - 1)
         fprintf (oil_file, ",\n");
   }
   fprintf (oil_file, ")\n");

}


// Section 2.1(d) and MSI
void emit_function(astree* tree) {
    for (auto child : tree->children) {
        if (child->token_code == TOK_FUNCTION) {
            emit_func_name(child->children[0]);
            fprintf(oil_file, "__%s (\n", child->children[0]->children.back()->lexinfo->c_str());
            emit_params(child->children[1]);
            fprintf(oil_file, "{\n");
            //emit_func_body(child->children[2]);
            fprintf(oil_file, "}\n");
        }
    }
}

/*
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
    emit_function(tree);
}

// Section 3.1
void emit_everything(astree* tree) {
    //emit_prolog();
    emit_header();
    emit_program(tree);
    emit_main(tree);
    emit_function(tree);
}

