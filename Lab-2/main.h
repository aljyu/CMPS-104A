#include <string>
#include <errno.h>
#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

extern FILE* tok_file;

void chomp (char* string, char delim);

void cpplines ();

void cpp_open (const char* filename);

void cpp_pclose();

void parse_options (int argc, char** argv);

void check_suffix (const char* program_name, char* filename);
