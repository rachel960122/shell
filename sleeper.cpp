/* Name: Ziting Shen
 * Date:  04/14/2016
 * Exercise: final project
 *
 * Purpose:test the shell program can run with or without a command line 
 * argument that specifies how many times the program should sleep for 2 seconds
 * before exiting. With no command line argument it does this 5 times.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

   int num = 5;
   if(argc == 2) {
      num = atoi(argv[1]);
   }
   for(int i = 0; i < num; i++) {
      sleep(2);     
   }
   return 0;
}
