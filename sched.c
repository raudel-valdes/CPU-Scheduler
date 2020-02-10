/*
IMPORTANT QUESTIONS:

1) Do we need to allocate memory for the integers or only for the nodes?
  -Do we need to worry about very large numbers? Should we use longs instead of ints?

2) If there is a tie for PP, then FCFS’s rule will be used to break the tie.

3) Are prioritie values unique?
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
  int finishedTime;
  int waitTime;
  struct Node *next;
  struct Node *prev;
  struct Node *readyNext;
  struct Node *readyPrev;  
  struct Node *finishedNext;
  struct Node *finishedPrev;
} Node;

typedef struct List {
  Node *head;
  Node *tail;
} List;

void insertNodeAtTail(List *, int, int, int, int);
void insertReadyNodeAtTail(List **, Node **);
void insertFinishedPPNodeAtTail(List **, Node **);
Node * removeReadyNodeFromTail(List **);
void implementFCFS(List *);
void implementPP(List *, List *, List *);
void printList(List *, int);
void printListToFile(List *, FILE **);
void printPPListToFile(List *, FILE **);
void destroyList(List *);

int main(int argc, char *argv[]) {

  List fileList;
  List readyList;
  List finishedPPList;

  fileList.head = NULL;
  fileList.tail = NULL;

  readyList.head = NULL;
  readyList.tail = NULL;

  finishedPPList.head = NULL;
  finishedPPList.tail = NULL;

  FILE *fPtr1;
  FILE *fPtr2;

  int pid = 0;
  int arrvTime = 0;
  int finishedTime = 0;
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

  if (fPtr1 == NULL) {

    printf("The file %s was not found or could not be open. Please try again!", argv[1]);
    exit(EXIT_FAILURE);

  }

  fPtr2 = fopen(argv[2], "w+");

  if (fPtr2 == NULL) {

    printf("The file %s was not created or could not be open. Please try again!", argv[3]);
    exit(EXIT_FAILURE);

  }

  if (argc == 5) {

    char *ptr;
    plimit = strtol(argv[4], &ptr, 10);

    if (*ptr != '\0'){
      printf("Please provide an INTEGER for the limit of processes to execute!");
      exit(EXIT_FAILURE);
    }

    useLimit = 1;

  }

  while (fscanf(fPtr1,"%i %i %i %i", &pid, &arrvTime, &finishedTime, &waitTime) != EOF) {

    if (useLimit && pcounter >= plimit)
      break;

    pcounter++;
    insertNodeAtTail(&fileList, pid, arrvTime, finishedTime, waitTime);

  }

  // if (argv[3] == "FCFS") {
  //  implementFCFS(&fileList);
  //  printListToFile(&fileList, &fPtr2);
  // }
  // else {
    implementPP(&fileList, &readyList, &finishedPPList);
    printPPListToFile(&finishedPPList, &fPtr2);
  //} 


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
  nextTailNode->finishedTime = 0;
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

void insertReadyNodeAtTail(List **list, Node **nextTailNode) {

  Node *currentHeadNode = (*list)->head;
  Node *currentTailNode = (*list)->tail;
  Node *traversingNode = (*list)->head;

  if (currentHeadNode == NULL) {

    (*nextTailNode)->readyPrev = NULL;
    (*nextTailNode)->readyNext = NULL;

    (*list)->head = (*nextTailNode);
    (*list)->tail = (*nextTailNode);

  } else {

      while (traversingNode != NULL && (*nextTailNode)->priority < traversingNode->priority) {

        traversingNode = traversingNode->readyNext;

        //takes care of the case in which there are two different processes in the ready list
        //and we must decide who gets to go first. In this scenario we choose the process who 
        //arrived first.
        if (traversingNode != NULL && traversingNode->priority == (*nextTailNode)->priority) {

          if (traversingNode->arrvTime < (*nextTailNode)->arrvTime) {
            
            break;

          } else {

            traversingNode = traversingNode->readyNext;
            break;

          }

        }

      }
      
      if ((*nextTailNode)->priority >= currentHeadNode->priority) {

        currentHeadNode->readyPrev = (*nextTailNode);
        (*nextTailNode)->readyPrev = NULL;
        (*nextTailNode)->readyNext = currentHeadNode;
        (*list)->head = (*nextTailNode);   

      } else if (traversingNode != NULL) {

        traversingNode->readyPrev->readyNext = (*nextTailNode);
        (*nextTailNode)->readyPrev = traversingNode->readyPrev;
        (*nextTailNode)->readyNext = traversingNode;
        traversingNode->readyPrev = (*nextTailNode);

      } else {

        currentTailNode->readyNext = (*nextTailNode);
        (*nextTailNode)->readyPrev = currentTailNode;
        (*nextTailNode)->readyNext = NULL;
        (*list)->tail = (*nextTailNode);   

      }

  }

}

void insertFinishedPPNodeAtTail(List **list, Node **nextTailNode) {

  Node *currentHeadNode = (*list)->head;
  Node *currentTailNode = (*list)->tail;

  if (currentHeadNode == NULL) {

    (*nextTailNode)->finishedPrev = NULL;
    (*nextTailNode)->finishedNext = NULL;

    (*list)->head = (*nextTailNode);
    (*list)->tail = (*nextTailNode);

  } else {

    currentTailNode->finishedNext = (*nextTailNode);
    (*nextTailNode)->finishedPrev = currentTailNode;
    (*nextTailNode)->finishedNext = NULL;
    (*list)->tail = (*nextTailNode);    

  }
}

Node *removeReadyNodeFromTail(List **list) {
  
  Node * nodeToRemove = (*list)->tail;
  Node * prevNode =  NULL;
  
  if (nodeToRemove != NULL) {
    
    prevNode = nodeToRemove->readyPrev;
    nodeToRemove->readyPrev = NULL;

    if (prevNode != NULL)
      prevNode->readyNext = NULL;
    else (*list)->head = NULL;

    (*list)->tail = prevNode;

  }

  return nodeToRemove; 

}

void implementFCFS(List *list) {

  Node *traverseNode = NULL;
  int remainingBurst = 0;
  int waitingTime = 0;
  int tcounter = 0;

  traverseNode = list->head;

  while (traverseNode != NULL) {

    remainingBurst = traverseNode->remainingBurstTime;

    while (remainingBurst > 0) {
      
      remainingBurst--;
      tcounter++;

    }

    waitingTime = tcounter - traverseNode->arrvTime - traverseNode->burstTime;
    traverseNode->waitTime = waitingTime;
    traverseNode->remainingBurstTime = 0;
    traverseNode->finishedTime = tcounter;

    traverseNode = traverseNode->readyNext;
  }

}

void implementPP(List *fileList, List *readyList, List *finishedPPList) {

  Node *processingNode = fileList->head;
  Node *pArrv = processingNode->next;
  Node *readyTail = readyList->tail;

  int remainingBurst = 0;
  int tcounter = 0;
  int nextpArrvt = 0;
  int waitingTime = 0;

  nextpArrvt = pArrv->arrvTime;
  remainingBurst = processingNode->burstTime;

  while (processingNode != NULL) {
    
    while (remainingBurst > 0) {

      if (nextpArrvt == tcounter) {

        if (pArrv->priority < processingNode->priority) {
          
          processingNode->remainingBurstTime = remainingBurst;

          insertReadyNodeAtTail(&readyList, &processingNode);
          
          processingNode = pArrv;
          remainingBurst = processingNode->burstTime;

        } else insertReadyNodeAtTail(&readyList, &pArrv);

        pArrv = pArrv->next;

        if(pArrv != NULL)
          nextpArrvt = pArrv->arrvTime;

      }

      remainingBurst--;
      tcounter++;

    }

    processingNode->remainingBurstTime = 0;
    processingNode->finishedTime = tcounter;
    waitingTime = tcounter - processingNode->arrvTime - processingNode->burstTime;
    processingNode->waitTime = waitingTime;
      
    insertFinishedPPNodeAtTail(&finishedPPList, &processingNode);
    readyTail = readyList->tail;

    if (tcounter == nextpArrvt) {

      processingNode = pArrv;
      pArrv = pArrv->next;
      nextpArrvt = pArrv->arrvTime;
      
      if (readyTail != NULL && readyTail->priority < processingNode->priority) {

        insertReadyNodeAtTail(&readyList, &processingNode);
        processingNode = removeReadyNodeFromTail(&readyList);

      }      

    } else processingNode = removeReadyNodeFromTail(&readyList);

    if (processingNode != NULL)
      remainingBurst = processingNode->remainingBurstTime;

  }

}

void printList(List *list, int reverse) {

  Node *traverseNode = NULL;

  if (!reverse) {

    traverseNode = list->head;
    
    while (traverseNode != NULL) {

      printf("%i %i %i %i %i %i\n", traverseNode->pid, traverseNode->arrvTime, traverseNode->burstTime, traverseNode->priority, traverseNode->finishedTime, traverseNode->waitTime);
      traverseNode = traverseNode->next;

    }

  } else {
    
    traverseNode = list->tail;
    
    while (traverseNode != NULL) {

      printf("%i %i %i %i %i %i\n", traverseNode->pid, traverseNode->arrvTime, traverseNode->burstTime, traverseNode->priority, traverseNode->finishedTime, traverseNode->waitTime);
      traverseNode = traverseNode->prev;

    }

  }

}

void printListToFile(List *listToPrint, FILE **filePtr) {
  
  Node *traverseNode = NULL;
  traverseNode = listToPrint->head;

  while (traverseNode != NULL) {

    fprintf((*filePtr), "%i %i %i %i\n", traverseNode->pid, traverseNode->arrvTime, traverseNode->finishedTime, traverseNode->waitTime);
    traverseNode = traverseNode->next;

  }
}

void printPPListToFile(List *listToPrint, FILE **filePtr) {
  
  Node *traverseNode = NULL;
  traverseNode = listToPrint->head;

  while (traverseNode != NULL) {

    fprintf((*filePtr), "%i %i %i %i\n", traverseNode->pid, traverseNode->arrvTime, traverseNode->finishedTime, traverseNode->waitTime);
    traverseNode = traverseNode->finishedNext;

  }
}

void destroyList(List *listToDestroy) {

  Node *nodeToDestroy = NULL;
  Node *tempNode = NULL;

  nodeToDestroy = listToDestroy->head;
    
  while (nodeToDestroy != NULL) {

    tempNode = nodeToDestroy->next;
    free(nodeToDestroy);

    nodeToDestroy = tempNode;

  }
}