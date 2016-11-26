#include "auxlib.h"
#include "lyutils.h"
#include "stringset.h"
#include "astree.h"
#include "symtable.h"


extern vector<struct astree*> string_const_queue;


void emit_all(astree* tree);

void emit_prolog();

void emit_struct(astree* tree);

void emit_field_name(astree* child, const string* childName);

void emit_string_const();


