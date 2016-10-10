/*
 * Partner: Alice Yu (aljyu)
 * Partner: Paula Espiritu (mespirit)
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

const string CPP = "/usr/bin/cpp";
constexpr size_t LINESIZE = 1024;


// Keeps track of the yydebug and yy_flex_debug flag
int yydebug = 0;
int yy_flex_debug = 0;
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


// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
    size_t len = strlen (string);
    if (len == 0) return;
    char* nlpos = string + len - 1;
    if (*nlpos == delim) *nlpos = '\0';
}

/*
// Print the meaning of a signal.
static void eprint_signal (const char* kind, int signal) {
    fprintf (stderr, ", %s %d", kind, signal);
    const char* sigstr = strsignal (signal);
    if (sigstr != NULL) fprintf (stderr, " %s", sigstr);
}
*/

/*
// Print the status returned from a subprocess.
void eprint_status (const char* command, int status) {
    if (status == 0) return; 
    fprintf (stderr, "%s: status 0x%04X", command, status);
    if (WIFEXITED (status)) {
        fprintf (stderr, ", exit %d", WEXITSTATUS (status));
    }
    if (WIFSIGNALED (status)) {
        eprint_signal ("Terminated", WTERMSIG (status));
        #ifdef WCOREDUMP
        if (WCOREDUMP (status)) fprintf (stderr, ", core dumped");
        #endif
    }
    if (WIFSTOPPED (status)) {
        eprint_signal ("Stopped", WSTOPSIG (status));
    }
    if (WIFCONTINUED (status)) {
        fprintf (stderr, ", Continued");
    }
    fprintf (stderr, "\n");
}
*/

// Run cpp against the lines of the file.
void cpplines (FILE* pipe, char* filename) {
    // Declare an instance of the stringset struct
    stringset string_set_ADT;
    int linenr = 1;
    char inputname[LINESIZE];
   
    strcpy (inputname, filename);
    for (;;) {
        char buffer[LINESIZE];
        char* fgets_rc = fgets (buffer, LINESIZE, pipe);
        if (fgets_rc == NULL) break;
        chomp (buffer, '\n');
        // printf ("%s:line %d: [%s]\n", filename, linenr, buffer);
        // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
        int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                          &linenr, inputname);
        if (sscanf_rc == 2) {
            // printf ("DIRECTIVE: line %d file \"%s\"\n", linenr, 
            // filename);
            continue;
        }
        char* savepos = NULL;
        char* bufptr = buffer;
        // Use strtok on each line for tokens
        for (int tokenct = 1;; ++tokenct) {
            char* token = strtok_r (bufptr, " \t\n", &savepos);
            bufptr = NULL;
            if (token == NULL) break;
            // printf ("token %d.%d: [%s]\n", linenr, tokenct, token);
            string_set_ADT.intern(token);
        }
        ++linenr;
    }
    program_basename = name;

    const char* trace_file_name = strcat(name, ".str");
    FILE* trace_file = fopen(trace_file_name, "w");
    string_set_ADT.dump(trace_file);
    fclose(trace_file);
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
    int pass = 0;

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

        // optind is the last element in argv (the program name)
        char* filename = argv[optind];
        string command;
        // If the "-D string" option was used, it should be passed to
        // cpp
        if (d_option) {
            if (pass == 0) {
                command = CPP + " -D " + d_option_args + " " + filename;
                pass = 1;
                // printf ("command=\"%s\"\n", command.c_str());
            }
        }
        else {
            if (pass == 0) {
                command = CPP + " " + filename;
                pass = 1;
                // printf ("command=\"%s\"\n", command.c_str());
            }
        }
        
        FILE* pipe = popen (command.c_str(), "r");
        if (pipe == NULL) {
            //exit_status = EXIT_FAILURE;
            errprintf ("%s: %s\n",
            command.c_str(), strerror (errno));
        }
        else {
            cpplines (pipe, filename);
            int pclose_rc = pclose (pipe);
            eprint_status (command.c_str(), pclose_rc);
            if (pclose_rc != 0) exit_status = EXIT_FAILURE;
        }
    }
    delete[] name;
    return exit_status;
}
