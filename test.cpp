/*
	Citations:
	- http://www.tutorialspoint.com/c_standard_library/c_function_fopen.htm
	- https://www.cs.bu.edu/teaching/c/file-io/intro/
	- https://linux.die.net/man/3/getopt
*/
/*
	Program Logic
	Step 1: Open the file for reading
	Step 2: Pass the file through the CPP
	Step 3: Insert every word found in the file into the stringset
*/

// Needed for fopen() and fgets()
#include <stdio.h>
#include <string>
using namespace std;

int main (int argc, char **argv) {
	FILE *file_name;
	int options;
	int yy_flex_debug = 0;
	int yydebug = 0;
	bool d_flag = false;
	
	// While there are still command-line options left to be parsed
	// Instead of getopt() use fgets()?
	while ((options = getopt(argc, argv, "ly@D:")) != -1) {
		switch (options) {
			case 'l':
				yy_flex_debug = 1;
				break;
			case 'y':
				yydebug = 1;
				break;
			case '@':
				for (int i = 0; i < argc; i++) {
					set_debugflags(argv[i]);
				}
				break;
			case 'D':
				d_flag = true;
				
				break;
			default:
				break;
		}
	}
	
	for (int i = 0; i < argc; i++) {
		
		// Creates an empty file "output_file" for writing. If "output_file" already exists, its contents get erased and "output_file" is treated as a new, empty file.
		file_name = fopen(output_file, "w");
		// If "output_file" cannot be opened and written to, print out an error to STDERR (since all debugging output should be printed to standard error instead of standard output, as specified in the instructions) and exit the program
		if (file_name == NULL) {
			fprintf(stderr, "The file cannot be opened!");
			exit(1);
		}
	}
}
