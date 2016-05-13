/* Name: Ziting Shen
 * Date: 05/09/2016
 * Exercise: final project
 *
 * Purpose: Implementations of data structure struct joblist_t and job_t.
 */

#include "ziting.h"

using namespace std;

/* constructor - Initiate the pointer to the joblist. */
joblist_t::joblist_t() {
  jobs = new std::list<job_t>;
}

/* add - Add a job to the joblist. */
int joblist_t::add(pid_t pid, status state, string cmdline) {
  if (pid < 1) return 0;

  jobs->emplace(jobs->end(), next, pid, state, cmdline);
  next++;
  return 1;
}

/* remove - Delete a job from the joblist given the pid. */
int joblist_t::remove(pid_t pid) {
  if (pid < 1) return 0;

  for (std::list<job_t>::iterator it = jobs->begin(); it != jobs->end();
   ++it) {
    if (it->pid == pid) {
      remove(it);
      return 1;
    }
  }
  return 0;
}

/* remove - Delete a job from the joblist given the pointer to the job. */
std::list<job_t>::iterator joblist_t::remove(std::list<job_t>::iterator it) {
  if(jobs->size() == 1) {
    next = 1;
  }
  return jobs->erase(it);
}

/*  - Delete a job from the joblist given the pointer to the job. */
bool joblist_t::fgpid(std::list<job_t>::iterator it) {
  if(it->state == FG) {
    return true;
  }
  return false;
}

/* findpid - Find a job on the joblist given the pid. */
struct job_t *joblist_t::findpid(pid_t pid) {
  if (pid < 1) return NULL;

  for (std::list<job_t>::iterator it = jobs->begin(); it != jobs->end();
   ++it) {
    if (it->pid == pid) {
      return &(*it);
    }
  }
  return NULL;
}

/* jid2pid - Map job id to process id. */
pid_t joblist_t::jid2pid(int jid) {
  if (jid < 1) return 0;

  for (std::list<job_t>::iterator it = jobs->begin(); it != jobs->end();
   ++it) {
    if (it->jid == jid) {
      return it->pid;
    }
  }
  return 0;
}

/* jid2pid - Map process id to job id. */
int joblist_t::pid2jid(pid_t pid) {
  if (pid < 1) return 0;

  for (std::list<job_t>::iterator it = jobs->begin(); it != jobs->end();
   ++it) {
    if (it->pid == pid) {
      return it->jid;
    }
  }
  return 0;
}

/* listjobs - Print the running and suspended jobs on the job list. */
void joblist_t::listjobs() {
  for (std::list<job_t>::iterator it = jobs->begin(); it != jobs->end();
   ++it) {
    cout << '[' << it->jid << "]";
    cout << '\t';
    switch(it->state) {
      case BG:
      case FG: cout << "Running\t"; break;
      case ST: cout << "Stopped\t\t";
      default:;
    }
    cout << it->cmdline << endl << flush;
  }
}

/* listjobs - Print the finished jobs on the job list and then delete them. */
void joblist_t::listdone() {
  for (std::list<job_t>::iterator it = jobs->begin(); it != jobs->end();
   ++it) {
    if (it->state == DN) {
      cout << "[" << it->jid << "] (" << it->pid << ")\tDone\t" 
      << it->cmdline << endl;
      it = remove(it);
    }
    if (it->state == DNF) {
      it = remove(it);
    }
    if (it->state == TN) {
      cout << "[" << it->jid << "] (" << it->pid << ")\tTerminated\t" 
      << it->cmdline << endl;
      it = remove(it);
    }
  }
}

/* constructor - Assign values to the data fields of job_t. */
job_t::job_t(int jid, pid_t pid, status state, string cmdline) {
  this->jid = jid;
  this->pid = pid;
  this->state = state;
  this->cmdline = cmdline;
}
