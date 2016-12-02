#include "auxlib.h"
#include "lyutils.h"
#include "stringset.h"
#include "astree.h"
#include "symtable.h"


extern vector<struct astree*> string_const_queue;


void emit_everything(astree* tree);

//void emit_prolog();
void emit_header();

void emit_struct(astree* tree);

void emit_field_name(astree* child, const string* childName);

void emit_stringdef();

void emit_return_wo_oper();

void emit_return_w_oper(astree* tree);

void emit_if(astree* tree);

void emit_ifelse(astree* tree);

void emit_while(astree* tree);

void emit_func_body(astree* root);

void emit_params(astree* root);

void emit_func_name(astree* root);

void emit_function(astree* tree);

