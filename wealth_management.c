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
     if (!user || !category || !desc || amount < 0) {
        printf("Invalid transaction details.\n");
        return;
    }
    ExpenditureNode* newNode = malloc(sizeof(ExpenditureNode));
    if (!newNode) {
        printf("Memory allocation failed.\n");
        return;
    }

    strcpy(newNode->category, category);
    strcpy(newNode->description, desc);
    newNode->amount = amount;
    newNode->invType = invType;
    newNode->next = user->expenseHead;
    user->expenseHead = newNode;

    printf("Transaction added: [%s] %s - %.2f\n", category, desc, amount);
}

void updateInvestmentValue(UserProfile* user, const char* nodeName, double newValue) {
    //Annanya
      if (!user || !user->wealthRoot || !nodeName) {
        printf("Invalid input to updateInvestmentValue.\n");
        return;
    }
    WealthNode* node = findWealthNode(user->wealthRoot, nodeName);
    if (!node) {
        printf("Category '%s' not found.\n", nodeName);
        return;
    }
    if (newValue < 0) {
        printf("Negative value not allowed.\n");
        return;
    }
    node->value = newValue;
    printf("Updated '%s' to %.2f\n", nodeName, newValue);
    
}

void updateExpenseCategoryTotal(UserProfile* user, const char* category, double amount) {
    //Annanya
     if (!user || !user->wealthRoot || !category) {
        printf("Invalid input to updateExpenseCategoryTotal.\n");
        return;
    }
    WealthNode* node = findWealthNode(user->wealthRoot, category);
    if (!node) {
        printf("Category '%s' not found.\n", category);
        return;
    }
    node->value += amount;
    printf("Added %.2f to '%s' (New Total: %.2f)\n", amount, category, node->value);
}

void registerNewUser(const char* name, const char* gender) {
    //Annanya
     if (!name || !gender) {
        printf("Invalid user details.\n");
        return NULL;
    }
    UserProfile* user = malloc(sizeof(UserProfile));
    if (!user) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    strcpy(user->name, name);
    strcpy(user->gender, gender);
    user->netWorth = 0.0;
    user->expenseHead = NULL;

    user->wealthRoot = malloc(sizeof(WealthNode));
    if (!user->wealthRoot) {
        free(user);
        printf("Memory allocation failed.\n");
        return NULL;
    }

    strcpy(user->wealthRoot->name, name);
    user->wealthRoot->value = 0.0;
    user->wealthRoot->firstChild = NULL;
    user->wealthRoot->nextSibling = NULL;

    printf("User '%s' registered successfully.\n", name);
    return user;
}

void printExpenseLog(ExpenditureNode* head) {
    //Annanya
     if (!head) {
        printf("No transactions found.\n");
        return;
    }
    printf("\n--- Transaction Log ---\n");
    for (ExpenditureNode* temp = head; temp; temp = temp->next)
        printf("[%s] %s - %.2f\n", temp->category, temp->description, temp->amount);
}

void freeExpenseList(ExpenditureNode* head) {
    //Annanya
    ExpenditureNode* temp;
    while (head) {
        temp = head;
        head = head->next;
        free(temp);
    }
    printf("Expense list cleared successfully.\n");
}

}
