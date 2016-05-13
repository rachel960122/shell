/* Name: Ziting Shen & Rachel Xu
 * Exercise: Final Project
 * Date: 04/14/2016

 * Purpose: Tester program for CS246 final project: parsecmd
 */
#include "rachel.h"
#include "ziting.h"
#include <iomanip>

// static: means these functions cannot be called from outside this file
static void print_cmd_args(char* cmdline, vector<string> argv);
static void print_bg(bool bg) ;

/******************************************************************/
int main( ){ 

  bool bg;
  char* cmdline;
  string argv;
  vector<string> args;
  map<string, string> aliases;
  map<string, string> variables;
  // int i;

  rl_bind_key('\t', rl_complete);

  printf("Enter quit to stop\n");
  while(1) {
    // (1) print the shell prompt
    cout << "enter a cmd line> ";

    // (2) read in the next command
    cmdline = readline("shell> ");
   
    bg = false;
    
    args = parse(cmdline, bg, variables, aliases);
    if(!args.empty()) {
      print_cmd_args(cmdline, args);
      print_bg(bg);

      if (args[0] == "quit") {
	     return 0;
      }
    }
  }
  return 0;
}
/*******************************************************************/
// This function prints out a message based on the value of bg
// indicating if the command is run in the background or not
void print_bg(bool bg) {
      if(bg) { 
        cout << "run in the background is true\n";
      } else { 
        cout << "run in the background is false\n";
      }
}
/*******************************************************************/
// This function prints out the cmdline and argv list
//   cmdline: the command line string
//   argv: the argv list of command line arguments string
//
 void print_cmd_args(char* cmdline, vector<string> argv ){

  unsigned int i = 0;

  cout << "\nCommand line: " << cmdline << "\n";

  while (i < argv.size()) {
    // note: I'm printing out each argv string between # chars
    //       so that I can see if I have left any space or other 
    //       invisible characters in an argv[i] string (I shouldn't)
    cout << setw(3) << i << " #" << argv[i] << "#\n";
    i++;
  }
}
