#include <string>
using namespace std;

#include <errno.h>
#include <libgen.h>
// Needed for fprintf()
#include <stdio.h>
#include <stdlib.h>
// Needed for strstr()
#include <string.h>
#include <wait.h>
// Needed for getopt()
#include <unistd.h>

// Needed since functions are being used from different files
#include "auxlib.h"
#include "stringset.h"
/*
   Do I really need cppstrtok.cpp?
*/
#include "cppstrtok.h"

const string CPP = "/usr/bin/cpp";
constexpr size_t LINESIZE = 1024;

// Keeps track of the yydebug and yy_flex_debug flag
int yydebug = 0;
int yy_flex_debug = 0;

// Keeps track of whether or not the "-D string" option was used
int d_option = 0;

// If the option "-D string" was used, pass the option and its argument to cpp
char* d_option_args;


// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}

// Print the meaning of a signal.
static void eprint_signal (const char* kind, int signal) {
   fprintf (stderr, ", %s %d", kind, signal);
   const char* sigstr = strsignal (signal);
   if (sigstr != NULL) fprintf (stderr, " %s", sigstr);
}

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

// Run cpp against the lines of the file.
void cpplines (FILE* pipe, char* filename) {
   int linenr = 1;
   char inputname[LINESIZE];
   strcpy (inputname, filename);
   for (;;) {
      char buffer[LINESIZE];
      char* fgets_rc = fgets (buffer, LINESIZE, pipe);
      if (fgets_rc == NULL) break;
      chomp (buffer, '\n');
//      printf ("%s:line %d: [%s]\n", filename, linenr, buffer);
      // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
      int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"", &linenr, filename);
      if (sscanf_rc == 2) {
//         printf ("DIRECTIVE: line %d file \"%s\"\n", linenr, filename);
         continue;
      }
      char* savepos = NULL;
      char* bufptr = buffer;
      // Use strtok on each line for tokens
      for (int tokenct = 1;; ++tokenct) {
         char* token = strtok_r (bufptr, " \t\n", &savepos);
         bufptr = NULL;
         if (token == NULL) break;
//         printf ("token %d.%d: [%s]\n", linenr, tokenct, token);
      }
      ++linenr;
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
            for (int argi = 1; argi < argc; argi++) {
               if (strstr(argv[argi], "@") != NULL) {
                  set_debugflags(argv[argi]);
               }
            }
//            printf("Option @\n");
            break;
         case 'D':
            // Find where in the line the letter "D" appears and set everything from that letter onwards equal to d_option_args
            // Starts checking at index 1 because index 0 will be "oc"
            d_option = 1;
            for (int argi = 1; argi < argc; argi++) {
               if (strstr(argv[argi], "D") != NULL) {
                  d_option_args = argv[argi];
               }
            }
//            printf("Option D\n");
            break;
         case 'l':
            yy_flex_debug = 1;
//            printf("Option l\n");
            break;
         case 'y':
            yydebug = 1;
//            printf("Option y\n");
            break;
         default:
            fprintf(stderr, "No valid options were specified\n");
            exit_status = EXIT_FAILURE;
//            printf("No valid options were specified\n");
         break;
      }
   }
}

int main (int argc, char** argv) {
   const char* execname = basename (argv[0]);
   int exit_status = EXIT_SUCCESS;
   parse_options(argc, argv);
   for (int argi = 1; argi < argc; ++argi) {
      char* filename = argv[argi];
      string command;
      // If the "-D string" option was used, it should be passed to cpp
      if (d_option) {
         command = CPP + " " + d_option_args + " " + filename;
      }
      else {
         command = CPP + " " + filename;
      }
//      printf ("command=\"%s\"\n", command.c_str());
      FILE* pipe = popen (command.c_str(), "r");
      if (pipe == NULL) {
         exit_status = EXIT_FAILURE;
         fprintf (stderr, "%s: %s: %s\n", execname, command.c_str(), strerror (errno));
      }
      else {
         cpplines (pipe, filename);
         int pclose_rc = pclose (pipe);
         eprint_status (command.c_str(), pclose_rc);
         if (pclose_rc != 0) exit_status = EXIT_FAILURE;
      }
      
      
      /* Stuff about the file extension type? */
            
      
   }
   return exit_status;
}