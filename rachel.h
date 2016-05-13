/* Name: Rachel Xu
 * Date: 05/08/2016
 * Exercise: Final Project
 *
 * Purpose: Header file of functions and data structures implemented by Rachel.
 */

#include <string>
#include <vector>
#include <set>
#include <regex>
#include <readline/readline.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

using namespace std;

enum Mode {redirect_write, redirect_read, pipeline};

/* Function prototypes. */
void pwd();
void grep(vector<string> argv);
void grep_regex(set<char>, string, string);
vector<char> splitString(string, int, int);
void matchLines(vector<string>, bool, regex);
vector<string> truncateComments(vector<string> argv);
void changeDirectory(vector<string>, string &prev);
string strSub(string, string, string);
set<Mode> checkMode(vector<string>);
int redirect(vector<string>, set<Mode>, bool &bg, char *cmdline);
int piper(vector<string>, bool &bg, char *cmdline);
vector<vector<string>> splitPipes(vector<string>);
