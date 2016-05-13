Name: Ziting Shen & Rachel Xu
Date: 05/09/2016
Exercise: Final Project

1. Introduction
This project is an imitation of the Unix shell program with our own 
customization. As a command-line interpreter, the Unix shell interprets the 
commands enter by the user and executes them. For this project, we implemented 
about 30 functionalities in C/C++, descriptions and examples of which are listed
 below.

2. Structure and Implementation of the Program
Here's a list of files (including header files) included in this program:

joblist.cpp (Ziting)
rachel.cpp (Rachel)
rachel.h (Rachel)
shell.cpp (Ziting & Rachel)
sleeper.cpp (Ziting)
tester.cpp (Rachel)
ziting.cpp (Ziting)
ziting.h (Ziting)


shell.cpp is the main program


joblist.cpp implements the structs joblist_t and job_t for job handling


rachel.cpp/rachel.h implements the following functionalities:

  1) pwd (pwd)
  Prints out the path of current working directory.

  2) grep/grep_regex (grep)
  Searches for lines of text that match a regular expression, and outputs only 
the matching lines by default.

  3) truncateComments (#)
  Processes the command line input and removes the parts following # (comments).

  4) changeDirectory (cd)
  Changes working directory to a directory specified by a pathname.

  5) redirect (redirection ">" & "<")
  Redirects the input/output of a command from/to a file.

  6) piper (pipeline "|")
  Connects two or more programs together so that the output from one program 
  becomes the input of next program.


sleeper.cpp uses the sleep() function for shell program testing


tester.cpp implements a tester program for command line parser


ziting.cpp/ziting.h implements the following functionlities:

  1) jobs (jobs)
  Prints out all running and suspended jobs.

  2) SIGINT (ctrl-c)
  Sends a signal to shell when the user enters ctrl-c at the prompt.

  3) SIGTSTP (ctrl-z)
  Sends a signal to shell when the user enters ctrl-z at the prompt.

  4) historyPrint/saveHistory (history)
  Saves and prints out command line history.

  5) do_fgbg (fg/bg)
  Sends a suspended job to background or sends a background/suspended job to 
  foreground.

  6) kill (kill)
  Sends signals to running processes to request termination of the process.

  7) setvariable/unsetvariable (set/unset)
  Assigns a value to or removes a local variable.

  8) alias/unalias (alias/unalias)
  Assigns a value to an alias or removes an alias.


3. Compile and Run

Compile with "make" command and run "./shell.cpp". 
Enter any command at the "shell>" prompt.


4. Known Bugs & Deficiencies

  1) The command "vim" causes a signal 22 and does not start vim.
  2) The command "./shell" also causes a signal 22.
  3) Quotes are treated as part of string.
  4) Wildcard expansion is not implemented.
  5) Grep does not support multiple files or multiple regular expressions.
  6) Man page cannot be opened from our shell.

