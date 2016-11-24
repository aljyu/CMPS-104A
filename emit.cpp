#include "emit.h"

FILE* oil;
vector<astree*> global_list;


void emit_function(astree* child) {
	leftChild = child->children[0];
	rightChild = child->children[1];

	emit_basetype(leftChild)'
	fprintf(oil, "__%s (\n", leftChild->children.back()->lexinfo->c_str());
//	emit_parameter(rightChild);
	fprintf(oil, "{\n");
//	emit_block(child->children[2]);
	fprintf(oil, "}\n");
}

void emit_immediate_children(astree* tree) {
	for (int childIdx = 0; childIdx < tree->children.size(); childIdx++) {
		if (tree->children[childIdx]->token_code == TOK_FUNCTION) {
			emit_function(tree->children[childIdx]);
		}
	}
}

void emit_global(astree* tree) {
	for (int childIdx = 0; childIdx < tree->children.size(); childIdx++) {
		if (tree->children[childIdx] == TOK_VARDECL) {
			global_list.pushback(childIdx);
		}
	}
	for (int index = 0; index < global_list.size(); index++) {
//		emit_basetype(global_list[index]->children[0]);
		fprintf(oil, "__%s;\n", global_list[index]->children[0]->children.back()->lexinfo->c_str());
	}
}

void emit_string_const() {
	int charIndex;

	for (int index = 1; index <= stringcons.size(); index++) {
		charIndex = index++;
		fprintf(oil, "char* s%d = %s;\n", charIndex++, stringcons[index]->lexinfo->c_str());
	}
}

void primt_emit_struct(astree* child) {
	leftChild = child->children[0];

	fprintf(oil, "struct s_%s {\n", leftChild->lexinfo->c_str());
	for (size_t childIdx = 1; childIdx < child->children.size(); childIdx++) {
		fprintf(oil, "        ");
//		emit_basetype(child->children[childIdx]);
//		emit_field_name(child->children[childIdx], leftChild->lexinfo);
		fprintf(oil, ";\n");
	}
	fprintf(oil, "};\n");
}

void emit_struct(astree* tree) {
	for (int child = 0; child < tree->children.size(); child++) {
		if (child->token_code == TOK_STRUCT) {
			print_emit_struct(child);
		}
	}
}

void emit_prolog() {

}

// Referenced from Section 3.1
void emit_all(FILE* output, astree* tree) {
	oil = output;

	emit_prolog();
	emit_struct(tree);
	emit_string_const();
	emit_global(tree);
	emit_immediate_children(tree);
	emit_main();
}

