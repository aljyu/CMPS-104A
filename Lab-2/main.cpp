/*
 * http://epaperpress.com/lexandyacc/pry1.html
*/

#include <string>
using namespace std;
// Needed for namespace functino of find_last_of
#include <iostream>

#include <errno.h>
#include <assert.h>
// Needed for basename()
#include <libgen.h>
// Needed for fprintf()
#include <stdio.h>
// Needed for malloc()
#include <stdlib.h>
// Needed for strcmp()
#include <string.h>
#include <wait.h>
// Needed for getopt()
#include <unistd.h>
// Needed for realpath()
#include <limits.h>
#include <stdlib.h>

// Needed since functions are being used from different files
#include "auxlib.h"
#include "stringset.h"
#include "astree.h"
#include "lyutils.h"
#include "main.h"

FILE* tok_file;
const string CPP = "/usr/bin/cpp";
constexpr size_t LINESIZE = 1024;


// Keep track of the flags following "@"
char* symbol_flags;
// Keeps track of whether or not the option "-D" was used
int d_option = 0;
// If the option "-D string" was used, pass the option and its
// argument to cpp
char* d_option_args;
// The basename of the input filename
const char* program_basename = nullptr;
char* name = new char[1024];
// Keeps track of what the exit status is
int exit_status = EXIT_SUCCESS;


// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
    size_t len = strlen (string);
    if (len == 0) return;
    char* nlpos = string + len - 1;
    if (*nlpos == delim) *nlpos = '\0';
}

// Run cpp against the lines of the external variable FILE* yyin
// Grabs the tokens using yylex() and interns it into the stringset
void cpplines () {
    char* str_name = new char[1024];
    char* tok_name = new char[1024];

    // Declare an instance of the stringset struct
    stringset str_string_set_ADT;

    for (;;) {
        // Get the value of yylex()
        int token = yylex();

        // Continuously read in from the external variable FILE* yyin
        // until yylex() returns YYEOF, which stands for the EOF of
        // yyin
        if (token == YYEOF) {
            break;
        }
        else {
           str_string_set_ADT.intern(yytext);
           //astree::dump_tree(tok_file, 2);
        }
    }
    strcpy(str_name, program_basename);
    strcpy(tok_name, program_basename);

    const char* str_trace_file_name = strcat(str_name, ".str");
    FILE* str_trace_file = fopen(str_trace_file_name, "w");
    str_string_set_ADT.dump(str_trace_file);
    fclose(str_trace_file);
    delete[] str_name;

// What does yyparse() do? What is yytext?
    const char* tok_trace_file_name = strcat(tok_name, ".tok");
    FILE* tok_file = fopen(tok_trace_file_name, "w");
    fclose(tok_file);
    delete[] tok_name;
}

// Open a pipe from the C preprocessor.
// Exit failure if can't.
// Assigns opened pipe to FILE* yyin
void cpp_popen (const char* filename) {
    string command;
    // If the "-D string" option was used, it should be passed to
    // cpp
    if (d_option) {
        command = CPP + " -D " + d_option_args + " " + filename;
       // printf ("command=\"%s\"\n", command.c_str());
    }
    else {
        command = CPP + " " + filename;
        // printf ("command=\"%s\"\n", command.c_str());
    }
        
    yyin = popen (command.c_str(), "r");
    if (yyin == NULL) {
        syserrprintf (command.c_str());
        exit_status = EXIT_FAILURE;
    }
    else {
        yy_flex_debug = 0;
        if (yy_flex_debug) {
            fprintf (stderr, "-- popen (%s), fileno(yyin) = %d\n",
                     command.c_str(), fileno(yyin));
        }
//        lexer::newfilename (command);
    }
}

void cpp_pclose() {
    int pclose_rc = pclose (yyin);
    if (pclose_rc != 0) {
        exit_status = EXIT_FAILURE;
    }
}

// Takes in a line and parses through all of the options
void parse_options(int argc, char** argv) {
    int option;

    while ((option = getopt(argc, argv, "@:D:ly")) != -1) {
        switch (option) {
            case '@':
                // Find where in the line the symbol "@" appears and 
                // call set_debugflags
                // Starts checking at index 1 because index 0 will be 
                // "oc"
                // optarg is the following argv element (the flags in
                // this case)
                symbol_flags = optarg;
                set_debugflags(symbol_flags);
                break;
            case 'D':
                d_option = 1;
                // Find where in the line the letter "D" appears and set
                // everything from that letter onwards equal to
                // d_option_args
                // Starts checking at index 1 because index 0 will be
                // "oc" 
                // optarg is the following argv element (the string in 
                // this case)
                d_option_args = optarg;
                break;
            case 'l':
                yy_flex_debug = 1;
                break;
            case 'y':
                yydebug = 1;
                break;
            default:
                fprintf(stderr, "No valid options were specified\n");
                break;
        }
    }
}

// Looks at the argument's filename suffix. If the suffix is not .oc 
// or if it does not exist, the program should abort with an error 
// message.
void check_suffix (const char* program_name, char* filename) {
    const char* file_extension = strrchr(program_name, '.');
    char* pathname = basename(filename);
    std::string filepath = pathname;
    std::string filepath_basename = filepath.substr(0, 
                                 strlen(pathname) - 3);

    if (pathname == NULL) {
        fprintf(stderr, "The program name is missing.\n");
    }
    else {
        // If there is no suffix
        if (file_extension == NULL) {
            fprintf(stderr, "The input filename must have a suffix!\n");
        }
        // If the suffix is not .oc
        else if (strcmp(file_extension, ".oc") != 0) {
            fprintf(stderr, 
            "The input filename has a suffix that is not .oc!\n");
        }
        // If the suffix is .oc
        else {
            program_basename = filepath_basename.c_str();
            strcpy(name, program_basename);
        }
    }
}

int main (int argc, char** argv) {
    set_execname(argv[0]);
    program_basename = basename(argv[0]);
    int exit_status = EXIT_SUCCESS;

    parse_options(argc, argv);
    const char* program_name = argv[optind];
    // If there are no file names, throw an error and end the program
    if (program_name == NULL) {
        exit_status = EXIT_FAILURE;
        fprintf(stderr, "No program or file name was given.\n");
    }
    else {
        char* file_name = argv[optind];
        check_suffix(program_name, file_name);
        cpp_popen(program_name);
        program_basename = name;
        cpplines();
        cpp_pclose();
    }
    delete[] name;
    return exit_status;
}