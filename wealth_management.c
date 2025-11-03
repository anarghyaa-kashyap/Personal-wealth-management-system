#include "wealth.h"
#include <string.h> 
#include <stdlib.h> 
#include <stdio.h>  

WealthNode* createWealthNode(const char* name, double value) {
    //Ahana
    WealthNode* newNode = (WealthNode*)malloc(sizeof(WealthNode));
    if (newNode == NULL) {
        printf("ERROR: Memory allocation failed for WealthNode.\n");
        exit(1);
    }
    strcpy(newNode->name, name);
    newNode->value = value;
    newNode->firstChild = NULL;
    newNode->nextSibling = NULL;
    return newNode;
}

void addWealthChild(WealthNode* parent, WealthNode* newChild) {
    //Ahana
    if (parent == NULL || newChild == NULL){ //if the parent has no child
        return; 
    }
    if (parent->firstChild == NULL) {
        parent->firstChild = newChild;
    } else {
        WealthNode* temp = parent->firstChild;
        while (temp->nextSibling != NULL){ //going to the last child
            temp = temp->nextSibling;
        }
        temp->nextSibling = newChild;
    }
}

WealthNode* findWealthNode(WealthNode* root, const char* name) {
    //Ahana
    if (root == NULL) {
        return NULL;
    }
    if (strcmp(root->name, name) == 0) {
        return root; 
    }
    WealthNode* found = findWealthNode(root->firstChild, name);
    if (found != NULL) {
        return found;
    }
    return findWealthNode(root->nextSibling, name);
}

void printWealthTree(WealthNode* root, int indent) {
    //Ahana
    if (root == NULL) {
        return; 
    }
    for (int i = 0; i < indent; i++) {
        printf(" ");
    }
    printf("+- %s: ($%.2f)\n", root->name, root->value);
    printWealthTree(root->firstChild, indent + 4);
    printWealthTree(root->nextSibling, indent);
}


void freeWealthTree(WealthNode* root) {
    //Ahana
    if (root == NULL) {
        return; 
    }
    freeWealthTree(root->firstChild);
    freeWealthTree(root->nextSibling);
    free(root);
}

UserHeap* createHeap(int capacity) {
    //Anarghyaa
    return NULL;
}

void swapUsers(UserHeap* heap, int i, int j) {
    //Anarghyaa
}

void heapifyUp(UserHeap* heap, int index) {
    //Anarghyaa
}

void heapifyDown(UserHeap* heap, int index) {
    //Anarghyaa
}

void heapInsert(UserHeap* heap, UserProfile* user) {
    //Anarghyaa
}

UserProfile* getTopWealthUser(UserHeap* heap) {
    //Akshata
    return NULL;
}

int findUserIndex(UserHeap* heap, UserProfile* user) {
    //Akshata
    return -1;
}

double calculateNetWorth(WealthNode* root) {
    //Akshata
    return 0.0;
}

void finalizeUserUpdates(UserProfile* user) {
    //Akshata
}

void freeHeap(UserHeap* heap) {
    //Akshata
}

void logExpenseToList(UserProfile* user, const char* category, const char* desc,double amount, InvestmentType invType) {
    //Annanya
}

void updateInvestmentValue(UserProfile* user, const char* nodeName, double newValue) {
    //Annanya
}

void updateExpenseCategoryTotal(UserProfile* user, const char* category, double amount) {
    //Annanya
}

void registerNewUser(const char* name, const char* gender) {
    //Annanya
}

void printExpenseLog(ExpenditureNode* head) {
    //Annanya
}

void freeExpenseList(ExpenditureNode* head) {
    //Annanya
}