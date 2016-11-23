#include "emit.h"

FILE* oil;


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

