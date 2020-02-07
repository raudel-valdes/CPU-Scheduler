#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


typedef struct Node{
  char *word;
  int count;
  struct Node *next;
  struct Node *prev;
} Node;

typedef struct List {
  Node *head;
  Node *tail;
} List;

void insertNodeAtTail(List *, char **, int);
void implementFCFS();
void implementPP();
void printList(List *, int);
void printListToFile(List *, FILE **);
void destroyList(List *);

int main(int argc, char *argv[]) {

  if (argc != 4) {

    printf("Please include the two input text file names and the output file name at execution time! \n");
    exit(EXIT_FAILURE);

  }

  List firstFileList;
  List secondFileList;

  firstFileList.head = NULL;
  firstFileList.tail = NULL;

  secondFileList.head = NULL;
  secondFileList.tail = NULL;

  FILE *fPtr1;
  FILE *fPtr2;

  char *scannedWord = NULL;
  
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

  while(fscanf(fPtr1,"%ms", &scannedWord) != EOF)
    insertNodeAtTail(&firstFileList, &scannedWord, 1);

  printListToFile(&secondFileList, &fPtr2);

  destroyList(&firstFileList);
  destroyList(&secondFileList);

  fclose(fPtr1);
  fclose(fPtr2);

  return 0;
}

void insertNodeAtTail(List *firstFileList, char **scannedWord, int wordCount) {

  char *wordToInsert = strndup(*scannedWord, strlen(*scannedWord));
  Node *nextTailNode = malloc(sizeof(Node));

  nextTailNode->word = wordToInsert;
  nextTailNode->count = wordCount;

  Node *currentNode = firstFileList->head;
  Node *currentTailNode = firstFileList->tail;

  if (currentNode == NULL) {

    nextTailNode->prev = NULL;
    nextTailNode->next = NULL;

    firstFileList->head = nextTailNode;
    firstFileList->tail = nextTailNode;

  } else {
 
    while(currentNode != NULL) {
      
      if (strcmp(currentNode->word, wordToInsert) == 0) {

        currentNode->count++;

        free(nextTailNode->word);
        free(nextTailNode);

        break;
        
      } else 
        currentNode = currentNode->next;

      if (currentNode == NULL) {

        nextTailNode->prev = currentTailNode;
        nextTailNode->next = NULL;
        currentTailNode->next = nextTailNode;
        firstFileList->tail = nextTailNode;

      }

    }
  
  }

  free(*scannedWord);
}

void implementFCFS() {

}

void implementPP() {

}

void printList(List *list, int reverse) {

  Node *currentNode = NULL;

  if (!reverse) {
    currentNode = list->head;
    
    while (currentNode != NULL) {

      printf("%s,%d\n", currentNode->word, currentNode->count);
      currentNode = currentNode->next;

    }

  } else {
    
    currentNode = list->tail;
    
    while (currentNode != NULL) {

      printf("%s,%d\n", currentNode->word, currentNode->count);
      currentNode = currentNode->prev;

    }

  }

}

void printListToFile(List *listToPrint, FILE **filePtr) {
  
  Node *traverseNode = NULL;
  traverseNode = listToPrint->head;

  while(traverseNode != NULL) {

    fprintf((*filePtr), "%s,%d\n", traverseNode->word, traverseNode->count);
    traverseNode = traverseNode->next;

  }
}

void destroyList(List *listToDestroy) {

  Node *nodeToDestroy = NULL;
  Node *tempNode = NULL;

  nodeToDestroy = listToDestroy->head;
    
  while(nodeToDestroy != NULL) {

    tempNode = nodeToDestroy->next;

    free(nodeToDestroy->word);
    free(nodeToDestroy);

    nodeToDestroy = tempNode;

  }
}