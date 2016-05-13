/* Name: Ziting Shen
 * Date: 05/07/2016
 * Exercise: final project
 *
 * Purpose: Header file of functions and data structures implemented by Ziting.
 */

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <readline/history.h>

#define HISTFILE "./data/.history"  /* history storage path*/
#define MAXHIST 500                 /* max history storage cmds */

using namespace std;

/* Jobs states: FG (foreground), BG (background), ST (stopped), DN (done),
 *              TN (terminated), DNF (done w/o done message)
 * Job state transitions and enabling actions:
 *    FG        -> ST       : ctrl-z
 *    ST        -> FG       : fg command
 *    ST        -> BG       : bg command
 *    BG        -> FG       : fg command
 *    BG        -> DN       : background process exit
 *    BG/FG/ST  -> TN       : ctrl-c/kill
 */
enum status {BG, FG, ST, DN, DNF, TN}; 

/* Data structures prototypes */
struct job_t {                 /* The job struct */
  int jid;                     /* job id [1, 2, ...] */
  pid_t pid;                   /* job pid */
  status state;                /* BG, FG, ST, DN, or TN */
  string cmdline;              /* command line */

public:
  job_t(int jid, pid_t pid, status state, string cmdline); /* constructor */
};

struct joblist_t {             /* The joblist struct */
  int next = 1;                /* next job id to allocate */
  list<job_t> *jobs;           /* The job list */

  /* Function prototypes */
  joblist_t();
  int add(pid_t pid, status state, string cmdline);
  int remove(pid_t pid);
  list<job_t>::iterator remove(list<job_t>::iterator it);
  bool fgpid(list<job_t>::iterator it);
  struct job_t *findpid(pid_t pid);
  int jid2pid(int jid);
  int pid2jid(pid_t pid);
  void listjobs();
  void listdone();
};

/* Function prototypes */
vector<string> parse(char* cmdline, bool &bg, 
  map<string, string> variables, map<string, string> aliases);
void historyPrint(vector<string> argv);
void sigchld_handler(int sig);
void sigint_handler(int sig);
void sigtstp_handler(int sig);
void sigquit_handler(int sig);
void waitfg(pid_t pid);
void saveHistory();
void do_fgbg(vector<string> argv);
void kill(vector<string> argv);
void setvariable(vector<string> argv, map<string, string> &variables);
void unsetvariable(vector<string> argv, map<string, string> &variables);
void alias(vector<string> argv, map<string, string> &aliases);
void unalias(vector<string> argv, map<string, string> &aliases);
int builtin_cmd(vector<string> argv);
int execute(vector<string> argv, bool bg, string cmdline);
void eval(char* cmdline, vector<string> argv, map<string, string> &variables, 
	  map<string, string> &aliases, string &prevDir, bool bg, bool &cont);
