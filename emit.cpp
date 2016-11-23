#include "emit.h"

FILE* oil;


void emit_string_const() {
	for (int index = 1; index <= stringcons.size(); index++) {
		
	}
}

void emit_struct() {
	for (int child = 0; child < root->children.size(); child++) {
		if (child->symbol == TOK_STRUCT){
			emit_struct (itor);	
		}
	}
}

void emit_prolog() {

}

// Referenced from Section 3.1
void emit_all(FILE* output, astree* tree) {
	oil = output;

	emit_prolog();
	emit_struct();
	emit_string_const();
	emit_global();
	emit_immediate_children();
	emit_main();
}

