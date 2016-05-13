/* Name: Rachel Xu
 * Date: 05/08/2016
 * Exercise: Final Project
 * Purpose: Implementations of functions.
 */

#include "rachel.h"
#include "ziting.h"

/* Global variable */
extern struct joblist_t shelljobs; /* The job list */

using namespace std;

/* pwd - Prints out the pathname of current directory. */
void pwd()
{
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    string result = string(cwd);

    cout << result << endl;
  } else {
    cerr << "Something is wrong" << endl;
  }
}

/* grep - Searches the named input file for lines containing a match to the
 * given pattern and prints the matching lines by default.
 */
void grep(vector<string> argv)
{
  vector<string> argList;
  vector<char> chars;
  set<char> flags;
  string pattern;
  string filename;

  for (vector<string>::iterator it = argv.begin()+1; it != argv.end(); ++it) {
    if (it->at(0) == '-' && it->size() > 0) {
      chars = splitString(*it, 1, it->size()-1);
      for (vector<char>::iterator iter = chars.begin(); iter != chars.end();
   ++iter) {
	/* Puts a character in the set of flags if the character is preceded
         * by '-'.
	 */
	flags.insert(*iter);
      }
    } else {
      ifstream f(*it);
      if (f.good()) {
	filename = *it;
      } else {
	pattern = *it;
      }
    }    
  }
  /* Handles the cases where input is not complete */
  if (pattern == "") {
    cout << "Usage: grep [OPTION]... PATTERN [FILE]...\n";
    cout << "Options: [cinov] [-c --count] [-i --ignore-case]"
	 <<" [-n --line-number]\n";
    cout << "                 [-o --only-matching] [-v --invert-match]" << endl;
  } else if (filename == "") {
    cout << "grep error: File not found" << endl;
  } else {
    grep_regex(flags, filename, pattern);
  }
}

/* grep_regex - Creates a regex object, checks for matching lines and decides
 * what to output according to different combinations of flags.
 */
void grep_regex(set<char> flags, string filename, string pattern)
{
  vector<string> lines;
  string line;
  regex e;
  try {
    e.assign(pattern);
  } catch (regex_error& ex) {
    cout << "Regex error: " << ex.what() << endl;
  }
  ifstream f(filename);
  int count = 0;
  if (f.good()) {
    while (getline(f, line)) {
      lines.push_back(line);
    }

    if (flags.empty()) {
      matchLines(lines, false, e);
      return;
    } else {
      /* If "-i" is found, the regex matching is case insensitive */
      if (flags.find('i') != flags.end()) {
        e.assign(pattern, regex::ECMAScript | regex::icase);
      }
      /* If "-v" is found, inverts the sense of matching to select
       * non-matching lines.
       */
      if (flags.find('v') != flags.end()) {
	/* If "-c" is found with the "-v" option, suppresses normal output
	 * and prints out a count for non-matching lines.
	 */
        if (flags.find('c') != flags.end()) {
          count = 0;
          for (vector<string>::iterator it = lines.begin(); it != lines.end();
	       ++it) {
            if (!regex_search(it->begin(), it->end(), e)) {
              count++;
            }
          }
          cout << count << endl;
          return;
        } else if (flags.find('n') != flags.end()) {
          for (unsigned int i = 0; i < lines.size(); ++i) {
            if (!regex_search(lines[i].begin(), lines[i].end(), e)) {
              cout << i + 1 << ": " << lines[i] << endl;;
            }
          }
        } else {
          matchLines(lines, true, e);
        }
      } else {
	/* Prints out a count for matching lines. */
        if (flags.find('c') != flags.end()) {
          count = 0;
          for (vector<string>::iterator it = lines.begin(); it != lines.end();
	       ++it) {
            if (regex_search(it->begin(), it->end(), e)) {
              count++;
            }
          }
          cout << count << endl;
        } else if (flags.find('o') != flags.end()) {
	  /* If "-o" is found, prints only the matched parts of a matching line,
	   * with each such part on a separate output line.
	   */
          smatch m;
          count = 0;
          for (vector<string>::iterator it = lines.begin(); it != lines.end();
	       ++it) {
            count++;
            regex_search(*it, m, e);
            for (unsigned int i = 0; i < m.size(); ++i) {
              if (flags.find('n') != flags.end()) {
                cout << count << ": ";
              }
              cout << m[i] << endl;
            }
          } 
        } else if (flags.find('n') != flags.end()) {
	  /* If "-n" is found, prefixes each lines of output with the 1-based
	   * line number within its input file.
	   */
          for (unsigned int i = 0; i < lines.size(); ++i) {
            if (regex_search(lines[i].begin(), lines[i].end(), e)) {
              cout << i + 1 << ": " << lines[i] << endl;;
            }
          }
        } else {
            matchLines(lines, false, e);
        }
      }
    }
  }
  
}

/* splitString - Splits a string to a vector of characters from start position
 * to end position (both inclusive).
 */
vector<char> splitString(string s, int startPos, int endPos)
{
  vector<char> result;
  for (int i = startPos; i <= endPos; ++i) {
    result.push_back(s.at(i));
  }
  return result;
}

/* matchLines - Prints out the lines that matching/non-matching lines for a
 * given regular expression.
 */
void matchLines(vector<string> lines, bool isInverse, regex e)
{
  int count = 0;
  if (isInverse) { /* If isInverse is true, inverts the matching. */
    for (vector<string>::iterator it = lines.begin(); it != lines.end(); ++it) {
      if (!regex_search(it->begin(), it->end(), e)) {
        count++;
        cout << *it << endl;
      }
    }
  } else {
    for (vector<string>::iterator it = lines.begin(); it != lines.end(); ++it) {
      if (regex_search(it->begin(), it->end(), e)) {
        count++;
        cout << *it << endl;
      }
    }
  }
}

/* truncateComments - Given a vector of string arguments, returns a vector
 * of strings that do not start with '#'.
 */
vector<string> truncateComments(vector<string> argv)
{
  vector<string> newArgv;
  for (unsigned int i = 0; i < argv.size(); ++i) {
    if (argv[i][0] != '#') {
      newArgv.push_back(argv[i]);
    } else {
      return newArgv;
    }
  }
  return newArgv;
}

/* changeDirectory - Changes the working directory of the current shell
 * execution environment.
 */
void changeDirectory(vector<string> argv, string &prev) 
{
  const char *dname;
  char temp[1024];
  string homeDir, newPath;
  
  getcwd(temp, sizeof(temp));

  /* If no directory given, changes working directory to HOME directory. */
  if (argv.size() == 1) { 
    dname = getenv("HOME");
  } else if (argv[1] == "-") { /* If input is "cd -", changes working directory
				  to the previous directory. */
    dname = prev.c_str();
  } else {
    homeDir = getenv("HOME");
    /* Substitutes '~' with the path of HOME directory. */
    newPath = strSub(argv[1], "~", homeDir);
    dname = newPath.c_str();
  }
  
  int result = chdir(dname);
  if (result == -1) {
    cout << "cd error: No such directory" << endl;
  } else {
    /* Sets previous directory to be the current working directory. */
    prev = temp;
  }
}

/* strSub - helper function for replacing all occurrences of a string with
 * another string. */
string strSub(string str, string from, string to) 
{
  while (str.find(from) != string::npos) {
    str.replace(str.find(from), from.length(), to);
  }
  return str;
}

/* checkMode - Given a vector of argument strings, looks for characters
 * that indicates redirections and pipeline in the command.
 */
set<Mode> checkMode(vector<string> argv)
{
  set<Mode> mode_set;
  if (find(argv.begin(), argv.end(), ">") != argv.end()) {
    mode_set.insert(Mode::redirect_write);
  }
  if (find(argv.begin(), argv.end(), "<") != argv.end()) {
    mode_set.insert(Mode::redirect_read);
  }
  if (find(argv.begin(), argv.end(), "|") != argv.end()) {
    mode_set.insert(Mode::pipeline);
  }
  return mode_set;
}

/* redirect - Redirects standard output to a file or redirects standard input
 * from a file.
 */
int redirect(vector<string> argv, set<Mode> mode_set, bool &bg, char *cmdline)
{
  // int status;
  int in, out = -1;
  int count = 0;
  pid_t pid;
  const char *infile, *outfile;
  char** argvc = new char*[argv.size()+1];
  vector<string>::iterator it, it2;

  /* Finds the names of the output and input files. */
  if (mode_set.find(Mode::redirect_write) != mode_set.end()) {
    it = find(argv.begin(), argv.end(), ">");
    outfile = (it + 1)->c_str();
  }
  if (mode_set.find(Mode::redirect_read) != mode_set.end()) {
    it2 = find(argv.begin(), argv.end(), "<");
    infile = (it2 + 1)->c_str();
  }

  for (unsigned int i = 0; i < argv.size(); ++i) {
    if (argv[i] == ">" || argv[i] == "<") {
      try {
        if (argv[i] == ">") {
          outfile = argv.at(i+1).c_str();
        } else {
          infile = argv.at(i+1).c_str();
        }
      }
      catch (const out_of_range& oor) {
        cout << "redirection error: filename unspecfied" << endl;
        return 0;
      }
      argvc[count] = NULL;
      break;
    }
    char* temp = new char[argv[i].size()+1];
    strcpy(temp, argv[i].c_str());
    argvc[i] = temp;
    count++;
  }
  if (mode_set.find(Mode::redirect_read) != mode_set.end()) {
    in = open(infile, O_RDONLY);
  }
  if (mode_set.find(Mode::redirect_write) != mode_set.end()) {
    out = open(outfile, O_WRONLY | O_TRUNC | O_CREAT,
	       S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
  }  
   /* Initialize a set of signals that only contains SIGCHLD */
  sigset_t signalSet;  
  sigemptyset(&signalSet);
  sigaddset(&signalSet, SIGCHLD);

  /* Block SIGCHLD signal */  
  sigprocmask(SIG_BLOCK, &signalSet, NULL);

  /* Creates a child process for the execution of the command. */
  pid = fork();
  
  if (pid < 0) {
    cout << "Fork failed" << endl;
  } else if (pid == 0) {
    /* Create new process group with child */
    setpgid(0, 0);
      
    /* Unblock SIGCHLD signal */
    sigprocmask(SIG_UNBLOCK, &signalSet, NULL);
    
    if (mode_set.find(Mode::redirect_read) != mode_set.end()) dup2(in, 0);
    if (mode_set.find(Mode::redirect_write) != mode_set.end()) dup2(out, 1);
    if (argv[0] == "grep") {
      vector<string> newArgv(argv.begin(), argv.begin() + count);
      grep(newArgv);
      return 1;
    } else if (execvp(argvc[0], argvc) < 0){
      cout << argv[0] << ":Command not found." << endl;
    }
  } else {
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

  for(int i = 0; i < count; i++) {
    delete[] argvc[i];
  }
  delete[] argvc;

  return 0;
}

/* piper - Connects two commands together so that the output from one
 * program becomes the input of the next program. Two or more commands can
 * be connected this way to form a pipe.
 */
int piper(vector<string> argv, bool &bg, char *cmdline)
{
  vector<vector<string>> elements = splitPipes(argv);
  int numOfPipes = elements.size() - 1;
  int count = 0;
  pid_t pid;
  int pipes[2 * numOfPipes];  
  bool redirectOut= false, redirectIn = false;
  const char *outfile, *infile;
  int out, in;

  /* Checks for the presence of output redirection ">" in the last command. */
  vector<string> lastCommand = elements.at(elements.size()-1);
  vector<string>::iterator it_last = find(lastCommand.begin(),
					  lastCommand.end(), ">");
  if (it_last != lastCommand.end()) {
    redirectOut = true;
    outfile = (it_last + 1)->c_str();
    out = open(outfile, O_WRONLY | O_TRUNC | O_CREAT,
	       S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    vector<string> newLastCommand(lastCommand.begin(), it_last);
    elements.at(elements.size()-1) = newLastCommand;
  }

  /* Checks for the presence of input redirection "<" in the first command. */
  vector<string> firstCommand = elements.at(0);
  vector<string>::iterator it_first = find(firstCommand.begin(),
					   firstCommand.end(), "<");
  if (it_first != firstCommand.end()) {
    redirectIn = true;
    infile = (it_first + 1)->c_str();
    in = open(infile, O_RDONLY);
    vector<string> newFirstCommand(firstCommand.begin(), it_first);
    elements.at(0) = newFirstCommand;
  }

  /* Creates a pipe for each group of neighboring commands. */
  for (int i = 0; i < numOfPipes; ++i) {
    if (pipe(pipes + i * 2) < 0) {
      cout << "Pipe failed" << endl;
      return EXIT_FAILURE;
    }
  }
  
  /* Initialize a set of signals that only contains SIGCHLD */
  sigset_t signalSet;  
  sigemptyset(&signalSet);
  sigaddset(&signalSet, SIGCHLD);

  /* Creates a child process for each command. */
  while (count <= numOfPipes) {
    /* Block SIGCHLD signal */  
    sigprocmask(SIG_BLOCK, &signalSet, NULL);
    
    pid = fork();
      
    if (pid < 0) {
      cout << "Pipe error" << endl;
      return EXIT_FAILURE;
    } else if (pid == 0) {
      setpgid(0, 0);
      /* Unblock SIGCHLD signal */
      sigprocmask(SIG_UNBLOCK, &signalSet, NULL);

      /* If not first command, points the file descriptor for standard input
       * to the write end of the pipe that connects the previous command and
       * the current command.
       */
      if (count != 0) {
	if (dup2(pipes[(count-1) * 2], 0) < 0) {
	  cout << "Pipe error" << endl;
	  return EXIT_FAILURE;
	}
      }
      /* If not last command, points the file descriptor for standard output
       * to the read end of the pipe that connects the current command to the
       * next command.
       */
      if (count != numOfPipes) {
	if (dup2(pipes[count * 2 + 1], 1) < 0) {
	  cout << "Pipe error" << endl;
	  return EXIT_FAILURE;
	}
      }
      /* If input redirection is found in the first command, points the file
       * descriptor for standard input to the file that we want to redirect
       * input from.
       */
      if (redirectIn && count == 0) {
	if (dup2(in, 0) < 0) {
	  cout << "Pipe error" << endl;
	  return EXIT_FAILURE;
	}
      }
      /* If output redirection is found in the last command, points the file
       * descriptor for standard output to the file that we want to redirect
       * output to.
       */
      if (redirectOut && count == numOfPipes) {
	if (dup2(out, 1) < 0) {
	  cout << "Pipe error" << endl;
	  return EXIT_FAILURE;
	}
      }
      /* Closes the copies of pipes for each child process. */
      for (int i = 0; i < 2 * numOfPipes; ++i) {
	close(pipes[i]);
      }
      char **command = new char*[elements[count].size() + 1];
      for (unsigned int i = 0; i < elements[count].size(); ++i) {
	char *temp = new char[elements[count][i].size() + 1];
	strcpy(temp, elements[count][i].c_str());
	command[i] = temp;
      }
      command[elements[count].size()] = NULL;
      /* Executes current command. */
      if (execvp(command[0], command) < 0) {
	cout << "Execution error" << endl;
	return EXIT_FAILURE;
      }
    } else {    
      if (count == 0 && bg) {
        shelljobs.add(pid, BG, cmdline);  /* Add job */
	cout << '[' << shelljobs.findpid(pid)->jid << "]\t" << pid << '\t' 
	     << cmdline << endl;
      } else {
        shelljobs.add(pid, FG, cmdline);  /* Add job */
      }
      sigprocmask(SIG_UNBLOCK, &signalSet, NULL); /* Unblock SIGCHLD signal */
    }
    count++;
  }
  /* Closes all pipes in the parent process. */
  for (int i = 0; i < 2 * numOfPipes; ++i) {
    close(pipes[i]);
  }

  if (!bg) {
    waitfg(pid);
  }
  
  return EXIT_SUCCESS;
}

/* splitPipes - Splits a vector of string arguments into groups of string
 * arguments using "|" as the delimiter.
 */
vector<vector<string>> splitPipes(vector<string> v)
{
  vector<vector<string>> vv;
  vector<string>::iterator it = v.begin();
  vector<string>::iterator start = v.begin();

  while (it != v.end()) {
    if (*it == "|") {
      vector<string> newV(start, it);
      vv.push_back(newV);
      start = it + 1;
    } else if (it == v.end() - 1) {
      vector<string> newV(start, it + 1);
      vv.push_back(newV);
    }
    it++;
  }
  return vv;
}

