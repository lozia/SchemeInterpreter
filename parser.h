#ifndef PARSER
#define PARSER
#include <stdlib.h>
#include <ctype.h>

// typedef struct node *List;
typedef struct node *List;
void S_Expression (const char* token, List current_node);
List newNode(void);
List newList(List node);
void setFirst(List dest, List first);
void setRest(List dest, List rest);
void setKWD(List dest, char* kwd);
void printList(const List alist);
List startParsing(void);
List eval(List alist);
void freeList(List alist);
#endif
