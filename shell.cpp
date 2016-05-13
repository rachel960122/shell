/* Name: Ziting Shen & Rachel Xu
 * Date: 05/04/2016
 * Exercise: final project
 *
 * Purpose: This program runs a shell.
 */

#include "rachel.h"
#include "ziting.h"

using namespace std;

/* Global variables */
struct joblist_t shelljobs; /* The job list */

/* Function prototypes */

/* main - Shell's main routine */
int main(){
  char* cmdline;
  map<string, string> aliases;    /* aliases and corresponding values */
  map<string, string> variables;  /* aliases and corresponding values */
  vector<string> argv;            /* arguments parsed from cmdline */
  bool bg;              /* if the job run in background or foreground */
  bool cont = 1;        /* whether continue to run the execution loop */

  /* Declare signal handlers */
  signal(SIGCHLD, sigchld_handler); /* Terminated or stopped child */
  signal(SIGTSTP, sigtstp_handler); /* ctrl-z */
  signal(SIGINT, sigint_handler);    /* ctrl-c */

  /* Set previous directory to current directory */
  string prevDir = getenv("PWD");

  /* Configure readline to auto-complete paths when the tab key is hit */
  rl_bind_key('\t', rl_complete);

  /* Use the readline/history library and read in history command lines from
   * a local file called HISTFILE
   */
  using_history();
  read_history(HISTFILE);

  /* Execute the shell's execution loop */
  while(cont) {

    shelljobs.listdone(); /* List and delete all the jobs with DN state */
    cmdline = readline("shell> "); /* Read command line */

    if (cmdline == NULL) { /* End of file (ctrl-d) */
      cout << endl;
      saveHistory();
      cont = 0;
    }

    /* Check for history expansion */
    char *output;
    int expansion_result = history_expand(cmdline, &output);

    /* If history expansion exists, overwrite cmdline. */
    if (expansion_result > 0) {
      strcpy(cmdline, output);
    } 
    free(output);

    /* If history expansion doesn't exist, print error message. */
    if (expansion_result < 0) {
      cerr << cmdline << ": event not found" << endl;
    } else if (strcmp(cmdline, "") != 0) {
      add_history(cmdline); /* Add cmdline to history command lines*/

      /* Return decides whether process runs in the background */
      argv = parse(cmdline, bg, variables, aliases);
      set<Mode> mode_set = checkMode(truncateComments(argv));

      if (bg && argv.empty()) {    /* Report error on stand alone '&' symbol */
	cerr << "syntax error near unexpected token '&'" << endl;
      } else if (!mode_set.empty()
		 && mode_set.find(Mode::pipeline) == mode_set.end()) {
	int id = redirect(truncateComments(argv), mode_set, bg, cmdline);
	if (id == 1) cont = false;
      } else if (!mode_set.empty()) {   /* piper - Pipeline */
	piper(argv, bg, cmdline);
      } else {
	eval(cmdline, argv, variables, aliases, prevDir, bg, cont); 
      }
    }
  }
  return 0;
}
