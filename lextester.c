#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

int main()
{
    char token[20];
    startTokens(20);
    List scheme;
    List result;
    
    printf("Enter any number of lines of input.\n");
    printf("This will echo tokens as they are found.\n");
    printf("Terminate by typing \"@@\" (two @\'s in a row).\n");
    printf("The function also calls (exit) quits.\n");
    printf("\nScheme> ");
    strcpy(token, getToken());
    
    while (strcmp(token, "@@"))
    {
        scheme = startParsing();
        S_Expression(token, scheme);
        // printList(scheme);
        result = eval(scheme);
        printList(result);
        // freeList(result);
        freeList(scheme);
        printf("\nScheme> ");
        strcpy(token, getToken());
    }
    return 0;
}
