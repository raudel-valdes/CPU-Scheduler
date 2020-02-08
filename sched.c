/*
IMPORTANT QUESTIONS:

1) Do we need to allocate memory for the integers or only for the nodes?
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


typedef struct Node{
  int pid;
  int arrvTime;
  int burstTime;
  int remainingBurstTime;
  int priority;
  int finTime;
  int waitTime;
  struct Node *next;
  struct Node *prev;
} Node;

typedef struct List {
  Node *head;
  Node *tail;
} List;

void insertNodeAtTail(List *, int, int, int, int);
void implementFCFS(List *);
void implementPP(List *);
void printList(List *, int);
void printListToFile(List *, FILE **);
void destroyList(List *);

int main(int argc, char *argv[]) {

  if (argc != 4) {

    printf("Please include the following files at execution time: \n");
    printf("\t 1) Input text file name/location. \n");
    printf("\t 2) Output file to save computed results. \n");
    printf("\t 3) Desire algorithm. \n");
    printf("\t 4) Number of processes to compute. (optional) \n");
    exit(EXIT_FAILURE);

  }

  List fileList;

  fileList.head = NULL;
  fileList.tail = NULL;

  FILE *fPtr1;
  FILE *fPtr2;

  int pid = 0;
  int arrvTime = 0;
  int finTime = 0;
  int waitTime = 0;
  
  fPtr1 = fopen(argv[1], "r");

  if(fPtr1 == NULL) {

    printf("The file %s was not found or could not be open. Please try again!", argv[1]);
    exit(EXIT_FAILURE);

  }

  fPtr2 = fopen(argv[2], "w+");

  if(fPtr2 == NULL) {

    printf("The file %s was not created or could not be open. Please try again!", argv[3]);
    exit(EXIT_FAILURE);

  }

  while(fscanf(fPtr1,"%i %i %i %i", &pid, &arrvTime, &finTime, &waitTime) != EOF)
    insertNodeAtTail(&fileList, pid, arrvTime, finTime, waitTime);

  // if (argv[3] == "FCFS") 
  //   implementFCFS(&fileList);
  // else implementPP(&fileList);

  implementFCFS(&fileList);
  printListToFile(&fileList, &fPtr2);

  destroyList(&fileList);

  fclose(fPtr1);
  fclose(fPtr2);

  return 0;
}

void insertNodeAtTail(List *fileList, int pid, int arrvTime, int burstTime, int priority) {

  Node *nextTailNode = malloc(sizeof(Node));

  nextTailNode->pid = pid;
  nextTailNode->arrvTime = arrvTime;
  nextTailNode->burstTime = burstTime;
  nextTailNode->remainingBurstTime = burstTime;
  nextTailNode->priority = priority;
  nextTailNode->finTime = 0;
  nextTailNode->waitTime = 0;

  Node *currentHeadNode = fileList->head;
  Node *currentTailNode = fileList->tail;

  if (currentHeadNode == NULL) {

    nextTailNode->prev = NULL;
    nextTailNode->next = NULL;

    fileList->head = nextTailNode;
    fileList->tail = nextTailNode;

  } else {
    currentTailNode->next = nextTailNode;
    nextTailNode->prev = currentTailNode;
    nextTailNode->next = NULL;
    fileList->tail = nextTailNode;    
  }
}

void implementFCFS(List *list) {

  Node *traverseNode = NULL;
  int remainingBurst = 0;
  int waitingTime = 0;
  int counter = 0;

  traverseNode = list->head;

  while(traverseNode !=NULL) {
    remainingBurst = traverseNode->remainingBurstTime;

    while(remainingBurst > 0) {
      
      remainingBurst--;
      counter++;

    }

    waitingTime = counter - traverseNode->arrvTime - traverseNode->burstTime;
    traverseNode->waitTime = waitingTime;
    traverseNode->remainingBurstTime = 0;
    traverseNode->finTime = counter;

    traverseNode = traverseNode->next;
  }

}

void implementPP(List *list) {
  
}

void printList(List *list, int reverse) {

  Node *traverseNode = NULL;

  if (!reverse) {
    traverseNode = list->head;
    
    while (traverseNode != NULL) {

      printf("%i %i %i %i %i %i\n", traverseNode->pid, traverseNode->arrvTime, traverseNode->burstTime, traverseNode->priority, traverseNode->finTime, traverseNode->waitTime);
      traverseNode = traverseNode->next;

    }

  } else {
    
    traverseNode = list->tail;
    
    while (traverseNode != NULL) {

      printf("%i %i %i %i %i %i\n", traverseNode->pid, traverseNode->arrvTime, traverseNode->burstTime, traverseNode->priority, traverseNode->finTime, traverseNode->waitTime);
      traverseNode = traverseNode->prev;

    }

  }

}

void printListToFile(List *listToPrint, FILE **filePtr) {
  
  Node *traverseNode = NULL;
  traverseNode = listToPrint->head;

  while(traverseNode != NULL) {

    fprintf((*filePtr), "%i %i %i %i\n", traverseNode->pid, traverseNode->arrvTime, traverseNode->finTime, traverseNode->waitTime);
    traverseNode = traverseNode->next;

  }
}

void destroyList(List *listToDestroy) {

  Node *nodeToDestroy = NULL;
  Node *tempNode = NULL;

  nodeToDestroy = listToDestroy->head;
    
  while(nodeToDestroy != NULL) {

    tempNode = nodeToDestroy->next;
    free(nodeToDestroy);

    nodeToDestroy = tempNode;

  }
}