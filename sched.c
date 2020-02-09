/*
IMPORTANT QUESTIONS:

1) Do we need to allocate memory for the integers or only for the nodes?
  -Do we need to worry about very large numbers? Should we use longs instead of ints?

2) If there is a tie for PP, then FCFS’s rule will be used to break the tie.
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
  struct Node *readyNext;
  struct Node *readyPrev;  
} Node;

typedef struct List {
  Node *head;
  Node *tail;
} List;

void insertNodeAtTail(List *, int, int, int, int);
void insertReadyNodeAtTail(List *, Node **);
Node * removeReadyNode(List*);
void sortList(List*, int);
void implementFCFS(List *);
void implementPP(List *);
void printList(List *, int);
void printListToFile(List *, FILE **);
void destroyList(List *);


int main(int argc, char *argv[]) {

  List fileList;

  fileList.head = NULL;
  fileList.tail = NULL;

  FILE *fPtr1;
  FILE *fPtr2;

  int pid = 0;
  int arrvTime = 0;
  int finTime = 0;
  int waitTime = 0;
  long plimit = 0;
  int useLimit = 0;
  long pcounter = 0;

  if (argc < 4 || argc > 5) {

    printf("Please include the following files at execution time: \n");
    printf("\t 1) Input text file name/location. \n");
    printf("\t 2) Output file to save computed results. \n");
    printf("\t 3) Desire algorithm. \n");
    printf("\t 4) Number of processes to compute. (optional) \n");
    exit(EXIT_FAILURE);

  }

  
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

  if(argc == 5) {

    char *ptr;
    plimit = strtol(argv[4], &ptr, 10);

    if(*ptr != '\0'){
      printf("Please provide an INTEGER for the limit of processes to execute!");
      exit(EXIT_FAILURE);
    }

    useLimit = 1;

  }

  while(fscanf(fPtr1,"%i %i %i %i", &pid, &arrvTime, &finTime, &waitTime) != EOF) {

    if(useLimit && pcounter >= plimit)
      break;

    pcounter++;
    insertNodeAtTail(&fileList, pid, arrvTime, finTime, waitTime);

  }

  // if (argv[3] == "FCFS") 
  //  implementFCFS(&fileList);
  // else{
    implementPP(&fileList);
    sortList(&fileList, 0);
  //} 

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

void insertReadyNodeAtTail(List * list, Node **nextTailNode) {

  Node *currentHeadNode = list->head;
  Node *currentTailNode = list->tail;

  if (currentHeadNode == NULL) {

    (*nextTailNode)->readyPrev = NULL;
    (*nextTailNode)->readyNext = NULL;

    list->head = nextTailNode;
    list->tail = nextTailNode;

  } else {

    currentTailNode->readyNext = nextTailNode;
    (*nextTailNode)->readyPrev = currentTailNode;
    (*nextTailNode)->readyNext = NULL;
    list->tail = nextTailNode;    

  }

}

Node * removeReadyNode(List *list) {
  
  Node * nodeToRemove = list->tail;
  Node * prevNode =  nodeToRemove->readyPrev;
  
  nodeToRemove->readyPrev = NULL;
  prevNode->readyNext = NULL;
  list->tail = prevNode;

  return &nodeToRemove; 

}

void sortList(List *unsortedList, int sortByPriority) {

  Node *marker = NULL;
  Node *markerPrev = NULL;
  Node *compareNode = NULL;
  Node *originalSwap = NULL;

  markerPrev = unsortedList->head;
  marker = unsortedList->head->next;  

  while(marker != NULL && markerPrev != NULL) {

    if (strcmp(markerPrev->word, marker->word) < 0) {

      marker = marker->next;
      markerPrev = markerPrev->next;

    } else { 

      swapAdjNodes(&unsortedList, &markerPrev, &marker);

      originalSwap = marker;
      marker = markerPrev->next;
      compareNode = originalSwap->prev;
      
      while(compareNode != NULL && originalSwap != NULL && (strcmp(compareNode->word, originalSwap->word) > 0)) {
       
        swapAdjNodes(&unsortedList, &compareNode, &originalSwap);
        compareNode = originalSwap->prev;
      }

      if (marker != NULL)
        markerPrev = marker->prev;

    }
  }

}

void implementFCFS(List *list) {

  Node *traverseNode = NULL;
  int remainingBurst = 0;
  int waitingTime = 0;
  int tcounter = 0;

  traverseNode = list->head;

  while(traverseNode != NULL) {

    remainingBurst = traverseNode->remainingBurstTime;

    while(remainingBurst > 0) {
      
      remainingBurst--;
      tcounter++;

    }

    waitingTime = tcounter - traverseNode->arrvTime - traverseNode->burstTime;
    traverseNode->waitTime = waitingTime;
    traverseNode->remainingBurstTime = 0;
    traverseNode->finTime = tcounter;

    traverseNode = traverseNode->next;
  }

}

void implementPP(List *list) {

  Node *processingNode = NULL;
  Node *pArrv = NULL;

  List *readyList = malloc(sizeof(List));
  readyList->head = NULL;
  readyList->tail = NULL;

  int remainingBurst = 0;
  int tcounter = 0;
  int nextpArrvt = 0;

  int pid = 0;
  int arrvTime = 0;
  int burstTime = 0;
  int priority = 0;

  processingNode = list->head;
  pArrv = processingNode->next;
  nextpArrvt = pArrv->arrvTime;

  while(processingNode != NULL) {
    
    while(remainingBurst > 0) {

      if(nextpArrvt == tcounter) {

        if(processingNode->priority < pArrv->priority) {
          
          processingNode->remainingBurstTime = remainingBurst;

          insertReadyNodeAtTail(&readyList, &processingNode);
          sortList(&readyList, 1);
          
          processingNode = pArrv;
          remainingBurst = processingNode->burstTime;

        } else {

          insertReadyNodeAtTail(&readyList, &pArrv);
          sortList(&readyList, 1);

        }

        pArrv = pArrv->next;
        nextpArrvt = pArrv->arrvTime;
      }

      remainingBurst--;
      tcounter++;

    }

    processingNode = processingNode->next;

    if(processingNode->priority < readyList->tail->priority) {

      pid = processingNode->pid;
      arrvTime = processingNode->arrvTime;
      burstTime = processingNode->burstTime;
      priority = processingNode->priority;
      
      processingNode = removeReadyNode(&readyList);

    }

    if(processingNode == NULL && readyList != NULL)
      processingNode = removeReadyNode(&readyList);

  }

  destroyList(&readyList);
  
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