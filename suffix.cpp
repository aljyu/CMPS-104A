/*
 * Citations:
 * - http://stackoverflow.com/questions/5309471/getting-file-extension-in-c
 * - http://stackoverflow.com/questions/1563168/example-of-realpath-function-c-programming
*/

// Needed for getopt()
#include <unistd.h>
// Needed for fprintf()
#include <stdio.h>
// Needed for strcmp()
#include <string.h>
// Needed for basename()
#include <libgen.h>
// Needed to concatenate strings
#include <string>
// Needed for realpath()
#include <limits.h>
#include <stdlib.h>

int yy_flex_debug = 0;
int yydebug = 0;
char* symbol_flags;
char* d_option_args;
char* program_basename;

// Looks at the argument's filename suffix. If the suffix is not .oc or if it does not exist, the program should abort with an error message.
void check_suffix (const char* program_name) {
   char buf[PATH_MAX + 1];
   char file_basename[strlen(program_name)];
   const char* file_extension = strrchr(program_name, '.');
   char* pathname = realpath(program_name, buf);

   pathname = buf;
   fprintf(stdout, "\n\nThe program's name is %s\n\n", pathname);

   if (pathname == NULL) {
      fprintf(stdout, "The program name is missing.\n");
   }
   else {
      // If there is no suffix
      if (file_extension == NULL) {
         fprintf(stdout, "The input filename must have a suffix!\n");
      }
      // If the suffix is not .oc
      else if (strcmp(file_extension, ".oc") != 0) {
         fprintf(stdout, "The input filename has a suffix that is not .oc!\n");
      }
      // If the suffix is .oc
      else {
         strncpy(file_basename, program_name, strlen(program_name) - 3);
         program_basename = file_basename;
      }
   }
}


// Takes in a line and parses through all of the options
void parse_options(int argc, char** argv) {
   int option;

   while ((option = getopt(argc, argv, "@:D:ly")) != -1) {
      switch (option) {
         case '@':
            // Find where in the line the symbol "@" appears and call set_debugflags
            // Starts checking at index 1 because index 0 will be "oc"
            // optarg is the following argv element (the flags in this case)
               fprintf(stdout, "Option @\n");
               symbol_flags = optarg;
            break;
         case 'D':
            // Find where in the line the letter "D" appears and set everything from that letter onwards equal to d_option_args
            // Starts checking at index 1 because index 0 will be "oc"
            // optarg is the following argv element (the string in this case)
               fprintf(stdout, "Option D\n");
               d_option_args = optarg;
            break;
         case 'l':
            yy_flex_debug = 1;
            fprintf(stdout, "Option l\n");
            break;
         case 'y':
            yydebug = 1;
            fprintf(stdout, "Option y\n");
            break;
         default:
            fprintf(stdout, "No valid options were specified\n");
         break;
      }
   }
}
int main(int argc, char** argv) {

   fprintf(stdout, "RESULTS:\n");
   parse_options(argc, argv);
   // optind is the index of the next element to be processed in argv (the program name in this case)
   const char* program_name = argv[optind];

   check_suffix(program_name);
   fprintf(stdout, "The basename is %s\n", program_basename);

   for (int argi = 1; argi < argc; ++argi) {
      char* filename = argv[argi];

//      command = "-D " + d_option_args + " " + filename;
//      fprintf(stdout, "%s\n", command);
//      fprintf(stdout, "Program name: %s\n", filename);
   }

   fprintf(stdout, "\nArguments for option @: %s\n", symbol_flags);
   fprintf(stdout, "Arguments for option -D: %s\n", d_option_args);
   return 0;
}
