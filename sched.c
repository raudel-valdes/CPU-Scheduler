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
  int taroundTime;
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
  int count;
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
void calcAvg(List *, char *);
void destroyList(List *);

int main(int argc, char *argv[]) {

  List fileList;
  List readyList;
  List finishedPPList;

  fileList.head = NULL;
  fileList.tail = NULL;
  fileList.count = 0;

  readyList.head = NULL;
  readyList.tail = NULL;
  readyList.count = 0;

  finishedPPList.head = NULL;
  finishedPPList.tail = NULL;
  finishedPPList.count = 0;

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

  if (strcmp(argv[3], "FCFS") == 0) {

   implementFCFS(&fileList);
   printListToFile(&fileList, &fPtr2);
   
  } else {

    implementPP(&fileList, &readyList, &finishedPPList);
    printPPListToFile(&finishedPPList, &fPtr2);

  } 

  calcAvg(&fileList, argv[3]);

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
  nextTailNode->taroundTime = 0;

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

  fileList->count++;

}

void insertReadyNodeAtTail(List **readyList, Node **nextTailNode) {

  Node *currentHeadNode = (*readyList)->head;
  Node *currentTailNode = (*readyList)->tail;
  Node *traversingNode = (*readyList)->head;

  if (currentHeadNode == NULL && currentTailNode == NULL) {

    (*nextTailNode)->readyPrev = NULL;
    (*nextTailNode)->readyNext = NULL;

    (*readyList)->head = (*nextTailNode);
    (*readyList)->tail = (*nextTailNode);

  } else {

    while (traversingNode != NULL && (*nextTailNode)->priority < traversingNode->priority)
      traversingNode = traversingNode->readyNext;
    
    while (traversingNode != NULL && (*nextTailNode)->priority == traversingNode->priority && (*nextTailNode)->arrvTime < currentTailNode->arrvTime)
      traversingNode = traversingNode->readyNext;

    if (traversingNode != NULL) {

      (*nextTailNode)->readyNext = traversingNode;

      if (traversingNode->readyPrev != NULL) {
        traversingNode->readyPrev->readyNext = (*nextTailNode);
      } else {
        (*readyList)->head = (*nextTailNode);
      }

      (*nextTailNode)->readyPrev = traversingNode->readyPrev;
      traversingNode->readyPrev = (*nextTailNode);

    } else {

      (*nextTailNode)->readyNext = NULL;
      currentTailNode->readyNext = (*nextTailNode);
      (*nextTailNode)->readyPrev = currentTailNode;
      (*readyList)->tail = (*nextTailNode);   

    }

  }

  (*readyList)->count++;

}

void insertFinishedPPNodeAtTail(List **finishedPPList, Node **nextTailNode) {

  Node *currentHeadNode = (*finishedPPList)->head;
  Node *currentTailNode = (*finishedPPList)->tail;

  if (currentHeadNode == NULL) {

    (*nextTailNode)->finishedPrev = NULL;
    (*nextTailNode)->finishedNext = NULL;

    (*finishedPPList)->head = (*nextTailNode);
    (*finishedPPList)->tail = (*nextTailNode);

  } else {

    currentTailNode->finishedNext = (*nextTailNode);
    (*nextTailNode)->finishedPrev = currentTailNode;
    (*nextTailNode)->finishedNext = NULL;
    (*finishedPPList)->tail = (*nextTailNode);    

  }

  (*finishedPPList)->count++;

}

Node *removeReadyNodeFromTail(List **readyList) {
  
  Node * nodeToRemove = (*readyList)->tail;
  Node * prevNode =  NULL;
  
  if (nodeToRemove != NULL) {

    prevNode = nodeToRemove->readyPrev;
    nodeToRemove->readyPrev = NULL;

    if (prevNode != NULL)
      prevNode->readyNext = NULL;
    else (*readyList)->head = NULL;

    (*readyList)->tail = prevNode;

  }
  
  (*readyList)->count--;

  return nodeToRemove; 

}

void implementFCFS(List *fileList) {

  Node *processingNode = NULL;
  int remainingBurst = 0;
  int tcounter = 0;

  processingNode = fileList->head;

  while (processingNode != NULL) {

    remainingBurst = processingNode->remainingBurstTime;

    while (remainingBurst > 0) {
      
      remainingBurst--;
      tcounter++;

    }

    processingNode->waitTime = tcounter - processingNode->arrvTime - processingNode->burstTime;
    processingNode->remainingBurstTime = 0;
    processingNode->finishedTime = tcounter;
    processingNode->taroundTime = processingNode->burstTime + processingNode->waitTime;


    processingNode = processingNode->next;
    
  }

}

void implementPP(List *fileList, List *readyList, List *finishedPPList) {

  Node *processingNode = fileList->head;
  Node *pArrv = processingNode->next;
  Node *tempNode = NULL;

  int remainingBurst = 0;
  int tcounter = 0;
  int nextpArrvt = 0;

  nextpArrvt = pArrv->arrvTime;
  remainingBurst = processingNode->burstTime;

  while (processingNode != NULL) {
    
    while (remainingBurst > 0) {

      if (nextpArrvt == tcounter && pArrv != NULL) {

        if (pArrv->priority < processingNode->priority || (pArrv->priority == processingNode->priority && pArrv->arrvTime < processingNode->arrvTime)) {
          
          processingNode->remainingBurstTime = remainingBurst;
          insertReadyNodeAtTail(&readyList, &processingNode);
          
          processingNode = pArrv;
          remainingBurst = processingNode->burstTime;

        } else insertReadyNodeAtTail(&readyList, &pArrv);

        pArrv = pArrv->next;

        if (pArrv != NULL)
          nextpArrvt = pArrv->arrvTime;

      }

      remainingBurst--;
      tcounter++;

    }

    tempNode = processingNode;
    processingNode->remainingBurstTime = 0;
    processingNode->finishedTime = tcounter;
    processingNode->waitTime = tcounter - processingNode->arrvTime - processingNode->burstTime;
    processingNode->taroundTime = processingNode->burstTime + processingNode->waitTime;
      
    insertFinishedPPNodeAtTail(&finishedPPList, &processingNode);
      
    if (readyList->tail != NULL)
      processingNode = removeReadyNodeFromTail(&readyList);

    if (processingNode != NULL && processingNode->pid == tempNode->pid && pArrv != NULL) {
      tcounter = nextpArrvt;
      processingNode = pArrv;
      pArrv = pArrv->next;
    }

    if (processingNode != NULL)
      remainingBurst = processingNode->remainingBurstTime;

    if (readyList->tail == NULL && pArrv == NULL && processingNode->remainingBurstTime == 0)
      processingNode = NULL;

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

void calcAvg(List *fileList, char *algo) {
  Node *traversingNode = fileList->head;
  float avgWaitingTime = 0.0;
  float avgTAroundTime = 0.0;
  int numberProcesses = fileList->count;

  while (traversingNode != NULL) {

    avgWaitingTime = avgWaitingTime + traversingNode->waitTime;
    avgTAroundTime = avgTAroundTime + traversingNode->taroundTime;

    traversingNode = traversingNode->next;

  }

  avgWaitingTime = avgWaitingTime/numberProcesses;
  avgTAroundTime = avgTAroundTime/numberProcesses;

  printf("\n\nAlgorithm: %s \n", algo);
  printf("Number of Processes Executed: %d \n", fileList->count);
  printf("Average Waiting Time: %f \nAverage Turn Around Time: %f\n\n", avgWaitingTime, avgTAroundTime);
  
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