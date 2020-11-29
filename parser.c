//
//  parser.c
//  CS170Project
//
//  Created by 李宗耀 on 3/26/19.
//  Copyright © 2019 myteam. All rights reserved.
//

#include "lexer.h"
#include <stdio.h>
#include <string.h>
#include "parser.h"



char space[3] = "  ";
int helperCount = 0;                // help to count '(' and ')'. if helpCount come back to 0, the whole expression ends.
struct node {
    struct node* first;
    char kwd[20];
    struct node* rest;
    int categ;                      // 0 for empty, 1 for First, -1 for last,
                                    // 2 for list, 3 for #t, 4 for #f, 5 for operation, 6 for symbol
};

List association_list;
List t_con_cell;                                // predefined true concell
List f_con_cell;                                // predefined false concell


// The main body of parsing an S_Expression
void S_Expression (const char* token, List current_node){
    if (token[0] == ')') {                      // if meet ')', means current '(' meets an end, return and update categ
        helperCount--;
        current_node->categ = -1;
        setRest(current_node, NULL);
        return;
    }
    if (token == NULL) {                        // if nothing passed, return null.
        return;
    }
    if (current_node == NULL) {
        return;
    }
    List new_first = newNode();                 // create a new node as current's first node
    setFirst(current_node, new_first);
    
    if (!strcmp(token, "()")) {                 // if (), record current node as null by set keyword to '()'
        setKWD(new_first, "()");
        if (!helperCount) {
            return;
        }
    }
    
    else if (!strcmp(token, "#t")) {            // mark current node as 3 if #t
        setKWD(current_node, "#t");
        current_node->categ = 3;
        if (!helperCount) {
            return;
        }
    }
    
    else if (!strcmp(token, "#f")) {            // mark current nod as 4 if #f
        setKWD(current_node, "#f");
        current_node->categ = 4;
        if (!helperCount) {
            return;
        }
    }
    
    else if (token[0] == '\'') {                // if a list starts, set current node to 2, and kwd to 'quote'
        current_node->categ = 2;
        setKWD(current_node, "quote");
        if (!helperCount) {
            return;
        }
    }

    else if (token[0] == '('){                  // if meet '(', start a new s_expression
        helperCount++;
        current_node->categ = 1;
        S_Expression(getToken(), new_first);
        if (!helperCount) {
            return;
        }
    }
    
    else if (isalpha(token[0]) || isdigit(token[0]) ){          // if meet a number or a string, set it to the new node
        strcpy(new_first->kwd, token);
        new_first->categ = 6;
        current_node->categ = 6;
        if (!helperCount) {
            return;
        }
    }
    List new_rest = newNode();                              // create the rest node
    setRest(current_node, new_rest);
    char currToken[20];
    strcpy(currToken, token);
    
    S_Expression(getToken(), current_node->rest);           // recursion
    if (!helperCount) {
        return;
    }
}


// creat new node
List newNode(){
    List new_node = (List) malloc(sizeof(struct node));
    new_node->first = NULL;
    new_node->rest = NULL;
    new_node->kwd[0] = '\0';
    new_node->categ = 0;
    return new_node;
}

//set node's first node
void setFirst(List dest, List first){
    if (first == NULL) {
        dest->first = NULL;
        return;
    }
    dest->first = first;
}

// set node's rest node
void setRest(List dest, List rest){
    if (rest == NULL) {
        dest->rest = NULL;
        return;
    }
    dest->rest = rest;
}

//set the string
void setKWD(List dest, char* kwd){
    strcpy(dest->kwd, kwd);
}

// the enter of new s_expression
List startParsing(){
    t_con_cell = newNode();
    f_con_cell = newNode();
    t_con_cell->categ = 3;
    f_con_cell->categ = 4;
    setKWD(t_con_cell, "#t");                      // initializing the #t and #f concell
    setKWD(f_con_cell, "#f");
    List new_node = newNode();
    return new_node;                                // return the entry node
}

// print out a list
void printList(const List alist){
    if (alist == NULL) {
        return;
    }
    if (alist->categ == 1 ) {                       // if this is a start node, print a (
        printf("(");
    }
    if (!strcmp("#f", alist->kwd)) {                // print () to represent #f
        printf("()");
    }
    else
        if (alist->kwd[0] !='\0') {                 // if current node has word, then print
            printf(" %s ", alist->kwd);
        }
    printList(alist->first);                        // recursivly print nodes
    printList(alist->rest);
    if (alist->categ == -1) {
        printf(")");                                // if this is an end node, print a )
    }
}

// helper method, get the category
int getCateg(List alist) {
    return alist->categ;
}

// helper method, get the kwd
char* getSymble(List alist){
    if (alist == NULL) {
        return NULL;
    }
    if (alist->kwd[0] == '\0') {
        return alist->first->kwd;
    }
    return alist->kwd;
}

// helper method, get the rest node
List getRest(List alist){
    if (alist == NULL) {
        return NULL;
    }
    return alist->rest;
}

// helper method, get the first node
List getFirst(List alist) {
    if (alist == NULL) {
        return NULL;
    }
    return alist->first;
}

// check is a list has null element
List isNull(List alist){
    if (getCateg(alist) == 0) {
        return t_con_cell;
    }
    if (getFirst(alist) == NULL && getRest(alist) == NULL && alist->kwd[0] == '\0') {
        return t_con_cell;
    }
    if (getCateg(alist) == 1) {
        if (getCateg(getFirst(alist)) == -1) {
            return t_con_cell;
        }
    }
    if (getCateg(alist) == 4) {
        return t_con_cell;
    }
    return f_con_cell;
}

// take the first node to return
List car(List alist){
    return getFirst(alist);
}

// take the rest node to return
List cdr(List alist){
    List result = newNode();
    if (getRest(alist) == NULL) {
        return result;
    }
    setFirst(result, getFirst(getRest(alist)));
    setRest(result, getRest(getRest(alist)));
    result->categ = 1;
    return result;
}

List cons(List listA, List listB){
    List con_node = newNode(), first = listA, second = listB;       // cons needs a new starter node to construct
    con_node->categ = 1;                                            // set the category to 1, as the first element in a list
    setFirst(con_node, first);
    if (getCateg(listB) == 0) {
        List rest = newNode();
        rest->categ = -1;
        setRest(con_node, rest);
        return con_node;
    }
    listB->categ = 6;                                               // a symbol is here
    setRest(con_node, second);
    while (getRest(second) != NULL) {
        second = getRest(second);
    }
    second->categ = -1;
    return con_node;
}

// the implementation of append. linear access elements
List append(List listA, List listB){
    List result = newNode(), curr_appen = newNode(), curr_A = listA, second = listB;
    setFirst(result, curr_appen);
    result->categ = 1;
    
    while ( curr_A!= NULL && getCateg(curr_A) != -1) {
        if (getRest(curr_A) != NULL) {
            setFirst(curr_appen, car(curr_A));
        }
        else if (getCateg(curr_A) != 0)
            setFirst(curr_appen, curr_A);
        setRest(curr_appen, newNode());
        curr_appen = getRest(curr_appen);
        curr_A = getRest(curr_A);
    }
    if (getRest(second) != NULL) {
        setFirst(curr_appen, car(second));
        setRest(curr_appen, getRest(second));
    }
    else if (getCateg(second) != 0){
        setFirst(curr_appen, second);
        setRest(curr_appen, newNode());
    }
    while (getRest(curr_appen)!= NULL) {
        curr_appen =getRest(curr_appen);
    }
    curr_appen->categ = -1;
    return result;
}


// used a lot of if statements. very complecated and many conditions
List isEqual(List listA, List listB) {
    if (listA == listB) {
        return t_con_cell;
    }
    
    if (getCateg(listA) == getCateg(listB)) {                               // compare basic properties of two elements
        if (getCateg(listB) == 6) {                                         // if anyone is missed, return false
            if (!strcmp(getSymble(listB), getSymble(listA))) {
                if (getRest(listA) == NULL && getRest(listB) == NULL) {
                    return t_con_cell;
                }
                int a_categ = getCateg(getRest(listA)), b_categ = getCateg(getRest(listB));
                if (a_categ == b_categ) {
                    if (a_categ == -1) {
                        return t_con_cell;
                    }
                }
                else
                    return f_con_cell;
                return t_con_cell;              // probably duplicated return.
            }
            else
                return f_con_cell;
        }
        List currA = listA, currB = listB;
        while (getCateg(currA) != -1 && getCateg(listB) != -1) {            // compare each elements in both lists
            if (getCateg(listA) != getCateg(listB)) {
                return f_con_cell;
            }
            if (strcmp(getSymble(getFirst(currA)), getSymble(getFirst(currB)))) {
                return f_con_cell;
            }
            else if (getCateg(currA) == 1 || getCateg(currB) == 1) {
                List result = isEqual(getRest(getFirst(currA)), getRest(getFirst(currB)));
                if (!strcmp(getSymble(result), getSymble(f_con_cell))) {
                    return f_con_cell;
                }
            }
            currA = getRest(currA);
            currB = getRest(currB);
        }
        if (getCateg(currA) == getCateg(currB)) {
            return t_con_cell;
        }
    }
    return f_con_cell;
}

List assoc(List listA, List listB) {
    if (listB == NULL || getCateg(listB) == 0 || getCateg(listB) == -1 ||
                    (getFirst(listB) == NULL && getRest(listB) == NULL)) {
        return f_con_cell;
    }
    if (!strcmp(getSymble(listA), getSymble(car(car(listB))))) {
        List new_node = newNode(), new_first = getFirst(getFirst(listB)), new_rest = newNode(), new_rest_rest = newNode();
        new_rest_rest->categ = -1;
        setFirst(new_rest, getRest(getFirst(listB)));
        setFirst(new_node, new_first);
        setRest(new_node, new_rest);
        setRest(new_rest, new_rest_rest);
        new_node->categ =1;
        return new_node;
    }
    else
        return assoc(listA, getRest(listB));
}

List isSymbol(List alist){
    if (!strcmp(getSymble(alist), "quote")) {               // other words than "quote" are all symbols
        return f_con_cell;
    }
    if (getRest(alist) != NULL) {
        return f_con_cell;
    }
    if (isalpha(getFirst(alist)->kwd[0])) {
        return t_con_cell;
    }
    return f_con_cell;
}

List cond(List alist){
    if (alist == NULL || getCateg(alist) == -1) {
        return f_con_cell;
    }
    
    if (!strcmp(getSymble(eval(getFirst(alist))), getSymble(t_con_cell))) {
        return eval(getRest(getFirst(alist)));
    }
    return cond(getRest(alist));
}

List define(List partA, List alist){
    char* name = getSymble(car(partA));
    List curr_search = association_list;
                                                            // find if is defined, if predefined, return the symbol
    List isContain = assoc(car(partA), association_list);   // and do nothing else
    if (getCateg(isContain) != 4) {
        return car(isContain);
    }
    /*
    while (curr_search != NULL) {
        if (!strcmp(name, getSymble(getFirst(curr_search)))) {
            setFirst(curr_search, alist);
            return getFirst(partA);
        }
        else
            curr_search = getRest(curr_search);
    }
     */
    
    List new_name_node = newNode();;                    // create new subnode for the associate line
    setKWD(new_name_node, name);
    new_name_node->categ = 6;
    
    if (association_list == NULL) {                     // search for a free spot for the new guy
        association_list = newNode();
        association_list->categ = 1;
        curr_search = association_list;
        setFirst(curr_search, cons(cons(new_name_node, cdr(car(partA))), alist));
        getFirst(getFirst(curr_search))->categ = 1;
        return getFirst(getFirst(curr_search));
    }
    curr_search = association_list;                     // if association_list is empty, create new node for it
    while (getRest(curr_search) != NULL) {
        curr_search = getRest(curr_search);
    }
    setRest(curr_search, newNode());                    // else find a new spot for the new guy
    curr_search = getRest(curr_search);
    curr_search->categ = 1;
    setFirst(curr_search, cons(cons(new_name_node, cdr(car(partA))), alist));
    getFirst(getFirst(curr_search))->categ = 1;
    return getFirst(getFirst(curr_search));
}



List eval(List alist){
    if (alist == NULL) {
        return NULL;
    }
    if (getFirst(alist) == NULL && getRest(alist) == NULL) {
        return alist;
    }
    
    char * opt;
    List target = alist;
    List result = NULL;
    int use_symbol_quote = 0;
    
    if (getCateg(alist) == 1) {                                     // if alist is a parent node,
        target = getFirst(alist);
    }
    if (getFirst(alist) != NULL) {
        if (getCateg(getFirst(alist)) == 6) {                       // if alist is double layer of 6.
            target = getFirst(alist);
        }
        if (getCateg(getFirst(alist)) == 2) {
            use_symbol_quote = 1;                                   // if s_expression uses quote symbol, instead of 'quote'
        }
    }
    if (getCateg(alist) == 0)                                       // if alist is a null node
        return alist;
    if (getCateg(alist) == -1){
        return NULL;                                                // if alist is an end node
    }
    if (getCateg(alist) == 3) {                                     // if alist is #t
        return t_con_cell;
    }
    if (getCateg(alist) == 4) {                                     // if alist is #f
        return f_con_cell;
    }
    
    opt = getSymble(target);
    if (!strcmp(opt, "exit")) {
        printf("Have a nice day!\n");
        exit(0);
    }
    if (!strcmp(opt, "car")) {
        List par1 = eval(getRest(target));                             // get the first parameter node
        result = car(par1);
    }
    else if (!strcmp(opt, "cdr")){
        List par1 = eval(getRest(target));                             // get the first parameter node
        result = cdr(par1);
    }
    else if (!strcmp(opt, "cons")){
        List par1 = eval(getRest(target));                             // get the first parameter node
        List par2;
        if (use_symbol_quote)
            par2 = eval(getRest(getRest(getRest(target))));        // get the second parameter node
        else
            par2 = eval(getRest(getRest(target)));
        result = cons(par1, par2);
    }
    else if (!strcmp(opt, "append")){
        List par1 = eval(getRest(target));                             // get the first parameter node
        List par2;
        if (use_symbol_quote)
            par2 = eval(getRest(getRest(getRest(target))));        // get the second parameter node
        else
            par2 = eval(getRest(getRest(target)));
        result = append(par1, par2);
    }
    else if (!strcmp(opt, "equal?")){
        List par1 = eval(getRest(target));                             // get the first parameter node
        List par2;
        if (use_symbol_quote)
            par2 = eval(getRest(getRest(getRest(target))));        // get the second parameter node
        else
            par2 = eval(getRest(getRest(target)));
        result = isEqual(par1, par2);
    }
    else if (!strcmp(opt, "assoc")){
        List par1 = eval(getRest(target));                             // get the first parameter node
        List par2;
        if (use_symbol_quote)
            par2 = eval(getRest(getRest(getRest(target))));        // get the second parameter node
        else
            par2 = eval(getRest(getRest(target)));
        result = assoc(par1, par2);
    }
    else if (!strcmp(opt, "define")){
        result = define(getRest(target), getRest(getRest(target)));
        // printList(association_list);
        setRest(getRest(target), NULL);
        //result = association_list;
        // result = getRest(target);
    }
    else if (!strcmp(opt, "else")) {
        result = eval(getRest(target));
    }
    else if (!strcmp(opt, "symbol?")){
        List par1 = eval(getRest(target));                             // get the first parameter node
        result = isSymbol(par1);
    }
    else if (!strcmp(opt, "null?")){
        List par1 = eval(getRest(target));                             // get the first parameter node
        result = isNull(par1);
    }
    else if (!strcmp(opt, "cond")){
        result = cond(getRest(target));
    }
    else if (!strcmp(opt, "quote")){                                    // not getting target node for '()
        result = getFirst(getRest(target));
        if (getRest(result) != NULL)
            result->categ = 1;
    }
    else {
        // find is current symbol defined
        List isContain = assoc(target, association_list);       // check is the symbol defined, if defined eval it
        if (getCateg(isContain) != 4) {                         // if not, return the origional list
            result = eval(car(cdr(isContain)));
        }
    }
    return result;
}

void freeList(List alist){                  // to free the allocated memories
    if (alist == NULL) {
        return;
    }
    freeList(getFirst(alist));
    freeList(getRest(alist));
    free(alist);
}
