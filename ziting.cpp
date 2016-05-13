/* Name: Ziting Shen
 * Date: 05/07/2016
 * Exercise: final project
 *
 * Purpose: Implementations of functions.
 */

#include "ziting.h"
#include "rachel.h"

/* Global variables */
extern struct joblist_t shelljobs; /* The job list */

using namespace std;

/* sigchld_handler - The kernel sends a SIGCHLD to the shell when a child job
 * terminates (becomes a zombie), or stops due to a SIGSTOP or SIGTSTP signal.
 * The handler reaps all available zombie children.
 */
void sigchld_handler(int sig) {
  int status;
  int pid = 1;
  while (pid > 0) {
    /* Returns pid of a child whose state changed */
    pid = waitpid(-1, &status, WNOHANG|WUNTRACED); 

    if (pid > 0) { /* Check if a child's state changed */
      if (WIFEXITED(status)) { /* Exited normally */
        if (shelljobs.findpid(pid)->state == BG) {
          shelljobs.findpid(pid)->state = DN;
        } else {
          shelljobs.findpid(pid)->state = DNF;
        }
      } else if (WIFSIGNALED(status)) { /* Exited due to signal not caught */
        shelljobs.findpid(pid)->state = TN;
      } else if (WIFSTOPPED(status)) { /* Stopped by receipt of a signal */
        shelljobs.findpid(pid)->state = ST;
        cout << "[" << shelljobs.pid2jid(pid) << "] (" << pid
          << ")\tStopped\t\tSignal " << WSTOPSIG(status) << endl;
      }
    }
  } 
}

/* sigint_handler - The kernel sends a SIGINT to the shell when users type 
 * ctrl-c at the keyboard. Catch it and send to the foreground job.
 */
void sigint_handler(int sig) {
  for (std::list<job_t>::iterator it = shelljobs.jobs->begin(); 
    it != shelljobs.jobs->end(); ++it) {
    if (shelljobs.fgpid(it)) {
      kill(-(it->pid), SIGTSTP);
    }
  }
}

/* sigtstp_handler - The kernel sends a SIGTSTP to the shell when users type 
 * ctrl-z at the keyboard. Catch it and suspend the foreground job by sending
 * it a SIGTSTP. 
 */
void sigtstp_handler(int sig) {
  for (std::list<job_t>::iterator it = shelljobs.jobs->begin(); 
    it != shelljobs.jobs->end(); ++it) {
    if (shelljobs.fgpid(it)) {
      kill(-(it->pid), SIGTSTP);
    }
  }
}


/* waitfg - Block until process pid is running on the foreground */
void waitfg(pid_t pid) {
  while(shelljobs.findpid(pid)->state == FG) {
    sleep(0);
  }
}

/* historyPrint - Print out all the command lines stored in history. If given 
 * an int n, print the last n command lines.
 */
void historyPrint(vector<string> argv) {
  int n = history_length;
  if (argv.size() > 2) {
    cout << "history: too many arguments" << endl;
    return;
  }
  if (argv.size() == 2) {
    try {
      n = stoi(argv[1]);
    } catch(exception &e) {
      cout << "Usage: history [n]" << endl;
      return;
    }
    if (n < 0) {
      cout << "Usage: history [n]" << endl;
      return;
    }
    if (n > history_length) n = history_length;
  }
  for (int i = history_base + history_length - n; 
    i < history_base + history_length; i++) {
    cout <<  i << " " << history_get(i)->line << endl;
  }
}

/* parse - Parse cmdline and return the parsed strings as a vector<string>.
 * Expand variables and aliases if detected. If cmdline end with a seperated 
 * '&', set bg to be true.
 */
vector<string> parse(char* cmdline, bool &bg, 
  map<string, string> variables, map<string, string> aliases){
  vector<string> argv;
  string c = string(cmdline);
  string piece;

  map<string, string>::iterator it;

  it = aliases.find(c);
  if (it != aliases.end()) {
    c = it->second;
  }
  stringstream ss(c);

  while (ss >> piece) {
    if (piece[0] == '$') {
      string sub = piece.substr(1, string::npos);
      it = variables.find(sub);
      if (it != variables.end()) {
        piece = it->second;
      }
    }
    argv.push_back(piece);
  }

  if (!argv.empty() && argv.back() == "&") {
    argv.pop_back();
    bg = true;
  } else {
    bg = false;
  }
  
  return argv;
}

/* saveHistory - Save the last MAXHIST hitory command lines into a local file.
 */
void saveHistory() {
  if (write_history(HISTFILE) == ENOENT) {
    mkdir("data", 0755);
    write_history(HISTFILE);
  }
  history_truncate_file(HISTFILE, MAXHIST);
}

/* do_fgbg - Given pid or job id, move a suspended job to run in the background
 * with bg command, or move a background or suspended job to run in the 
 * foreground with fg command.
 */
void do_fgbg(vector<string> argv) {
  if (argv.size() != 2) {
    cout << "Command requires pid of %jobid argument" << endl;
    if (argv[0] == "fg") {
      cout << "Usage: fg [pid]/[%jobid]" << endl;
    } else {
      cout << "Usage: bg [pid]/[%jobid]" << endl;
    }
  } else {
    pid_t pid;
    struct job_t *j;

    try {
      if (argv[1][0] == '%') {
        pid = shelljobs.jid2pid(stoi(argv[1].substr(1, string::npos)));
      } else {
        pid = stoi(argv[1]);
      }
    } catch (exception &e){
      if (argv[0] == "fg") {
        cout << "Usage: fg [pid]/[%jobid]" << endl;
      } else {
        cout << "Usage: bg [pid]/[%jobid]" << endl;
      }
      return;
    }
    
    if((j = shelljobs.findpid(pid)) == NULL) {  
      if (argv[1][0] == '%') {
        cout << argv[1].substr(1, string::npos) << ": No such job" << endl;  
      } else {
        cout << argv[1] << ": No such process" << endl;  
      }
      return;  
    }

    if (argv[0] == "fg"){
      j->state = FG;
      kill(j->pid, SIGCONT);
      waitfg(j->pid);
    } else {
      j->state = BG;
      kill(j->pid, SIGCONT);
      cout << '[' << j->jid << "]\t" << j->pid << '\t' << j->cmdline
      << endl;
    }
  }
}

void kill(vector<string> argv) {
  if (argv.size() != 2 && argv.size() != 3) {
    cout << "Usage: kill [signal] pid/%jobid" << endl;
    return;
  }

  int sig = SIGINT;

  try {
    if(argv.size() == 3 && argv[1][0] == '-') {
      sig = stoi(argv[1].substr(1, string::npos));
    }
  } catch (exception &e) {
    cout << "Usage: kill [signal] pid/%jobid" << endl;
    return;
  }

  pid_t pid;
  struct job_t *j;
  int i = argv.size() - 1;

  try {
    if (argv[i][0] == '%') {
      pid  = shelljobs.jid2pid(stoi(argv[i].substr(1, string::npos)));
    } else {
      pid = stoi(argv[i]);
    }
  } catch (exception &e) {
    cout << "Usage: kill [signal] pid/%jobid" << endl;
    return;
  }

  if((j = shelljobs.findpid(pid)) == NULL) {  
    if (argv[i][0] == '%') {
      cout << argv[i].substr(1, string::npos) << ": No such job" << endl;  
    } else {
      cout << argv[i] << ": No such process" << endl;  
    }
    return;  
  }

  kill(pid, sig);
}

/* setvariable - Assign a value to a local variable, or print all variables. */
void setvariable(vector<string> argv, map<string, string> &variables) {
  switch(argv.size()) {
    case 4: {
      variables.emplace(argv[1], argv[3]); 
      break;
    }
    case 1: {
      for (std::map<string, string>::iterator it = variables.begin(); 
        it != variables.end(); ++it) {
        cout << it->first << '=' << it->second << endl;
      }
      break;
    }
    default: cout << "Usage: set [var] = [value]" << endl;
  }
}

/* unsetvariable - Remove a local variable. */
void unsetvariable(vector<string> argv, map<string, string> &variables) {
  if (argv.size() != 2) {
    cout << "Usage: unset [var]" << endl;
  } else {
    variables.erase(argv[1]);
  }
}

/* alias - Assign a value to an alias, or print all aliases. */
void alias(vector<string> argv, map<string, string> &aliases) {
  switch(argv.size()) {
    case 4: {
      aliases.emplace(argv[1], argv[3]);
      break;
    }
    case 1: {
      for (std::map<string, string>::iterator it = aliases.begin(); 
        it != aliases.end(); ++it) {
        cout << "alias " << it->first << '=' << it->second << endl;
      }
      break;
    }
    default: cout << "Usage: alias [var] = [value]" << endl;
  }
}

/* unsetvariable - Remove an alias. */
void unalias(vector<string> argv, map<string, string> &aliases) {
  if (argv.size() != 2) {
    cout << "Usage: unalias [var]" << endl;
  } else {
    aliases.erase(argv[1]);
  }
}

/* builtin_cmd - Attempt to run 2 builtin commands which can be run at bg. */
int builtin_cmd(vector<string> argv) {
  if(argv[0] == "history") {            /* history - Print history commands */
    historyPrint(argv);
    return 0;
  }
  if(argv[0] == "grep") {               /* grep - Search for texts in file */
    grep(truncateComments(argv));
    return 0;
  }
  return -1;
}

/* execute - Execute a command line on the foreground or background. 
 * Return pid to decide whether to terminate.
 */
int execute(vector<string> argv, bool bg, string cmdline) {

  /* Store arguments in c strings */
  char** argvc = new char*[argv.size()+1]; 
  for(unsigned int i = 0; i < argv.size(); i++) {
    char* temp = new char[argv[i].size()+1];
    strcpy(temp, argv[i].c_str());
    argvc[i] = temp;
  }
  argvc[argv.size()] = NULL;

  /* Initialize a set of signals that only contains SIGCHLD */
  sigset_t signalSet;  
  sigemptyset(&signalSet);
  sigaddset(&signalSet, SIGCHLD);

  /* Block SIGCHLD signal */  
  sigprocmask(SIG_BLOCK, &signalSet, NULL);
  
  pid_t pid = fork();

  if (pid == 0) { /* Child execute the command */
    /* Create new process group with child */
    setpgid(0, 0);
      
    /* Unblock SIGCHLD signal */
    sigprocmask(SIG_UNBLOCK, &signalSet, NULL);
    
    /* Execute path as command */
    if (builtin_cmd(argv) < 0) {
      if (execvp(argvc[0], argvc) < 0){
        cout << argv[0] << ":Command not found." << endl;
      }
    }
  } else { /* Parent */
    if (!bg) { 
      shelljobs.add(pid, FG, cmdline);  /* Add job */
      sigprocmask(SIG_UNBLOCK, &signalSet, NULL); /* Unblock SIGCHLD signal */
      waitfg(pid);                  /* Parent waits for fg job to terminate */
    } else {
      shelljobs.add(pid, BG, cmdline);  /* Add job */
      cout << '[' << shelljobs.findpid(pid)->jid << "]\t" << pid << '\t' 
      << cmdline << endl;
      sigprocmask(SIG_UNBLOCK, &signalSet, NULL); /* Unblock SIGCHLD signal */
    }
  }

  for(unsigned int i = 0; i < argv.size(); i++) {
    delete[] argvc[i];
  }
  delete[] argvc;
  return pid;
}


/* eval - Evaluate the cmdline and execute the corresponding command. */
void eval(char* cmdline, vector<string> argv, map<string, string> &variables, 
	  map<string, string> &aliases, string &prevDir, bool bg, bool &cont) {

  if (argv[0] == "exit" || argv[0] == "quit") { /* Exit the shell */
    cout << "bye bye" << endl;
    saveHistory();
    cont = 0;                       /* Exit the execution loop */
  } else if (argv[0] == "cd") {     /* cd - Change working directory */
    changeDirectory(truncateComments(argv), prevDir);
  } else if (argv[0] == "fg" || argv[0] == "bg") {
    do_fgbg (argv);                 /* bg/fg - Move jobs to bg/fg */
  } else if (argv[0] == "set") {    /* set - Set local variables */
    setvariable(argv, variables);
  } else if (argv[0] == "unset") {  /* unset - Remove local variables */
    unsetvariable(argv, variables);
  } else if (argv[0] == "alias") {  /* alias - Set aliases */
    alias(argv, aliases);
  } else if(argv[0] == "unalias") { /* Unalias - Remove aliases */
    unalias(argv, aliases);
  } else if (argv[0] == "kill") {   /* kill - List running/stopped jobs */
    kill(argv);
  } else if (argv[0] == "jobs") {   /* jobs - List running/stopped jobs */
    shelljobs.listjobs();
  } else if(execute(argv, bg, cmdline) == 0) {
    cont = 0;                       /* execute - Execute a job at fg/bg */
  }
  free(cmdline);
}
