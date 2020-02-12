#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

int main (int argc, char *argv[]) {
  FILE *fptr1;
  FILE *fptr2;
  FILE *fptr3;

  int pid = 0;
  int arrvTime = 0;
  int finishedTime = 0;
  int waitTime = 0;

  int pid2 = 0;
  int arrvTime2 = 0;
  int finishedTime2 = 0;
  int waitTime2 = 0;

  int tcounter = 1;


  fptr1 = fopen("sout_pp.txt", "r");
  fptr2 = fopen("correct_pp.txt", "r");
  fptr3 = fopen("foundErrors.txt", "w+");

  while (fscanf(fptr1,"%i %i %i %i", &pid, &arrvTime, &finishedTime, &waitTime) != EOF && fscanf(fptr2,"%i %i %i %i", &pid2, &arrvTime2, &finishedTime2, &waitTime2) != EOF) {

    if (pid != pid2 || arrvTime != arrvTime2 || finishedTime != finishedTime2 || waitTime != waitTime2) {
      fprintf(fptr3,"ERROR ON LINE: %d\n", tcounter);
    } 

    tcounter++;
  }


  return 0;
}