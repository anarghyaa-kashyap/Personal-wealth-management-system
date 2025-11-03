#include "wealth.h"
#include <string.h> 
#include <stdlib.h> 
#include <stdio.h>  

WealthNode* createWealthNode(const char* name, double value) {
    //Ahana
    return NULL;
}

void addWealthChild(WealthNode* parent, WealthNode* newChild) {
    //Ahana
}

WealthNode* findWealthNode(WealthNode* root, const char* name) {
    //Ahana
    return NULL;
}

void printWealthTree(WealthNode* root, int indent) {
    //Ahana
}


void freeWealthTree(WealthNode* root) {
    //Ahana
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
    // Error handling: Check if heap is NULL
    if (heap == NULL) {
        fprintf(stderr, "Error [getTopWealthUser]: Heap is NULL\n");
        return NULL;
    }
    
    // Error handling: Check if heap's user array is NULL
    if (heap->userArray == NULL) {
        fprintf(stderr, "Error [getTopWealthUser]: Heap's user array is NULL\n");
        return NULL;
    }
    
    // Error handling: Check if heap is empty
    if (heap->size <= 0) {
        fprintf(stderr, "Error [getTopWealthUser]: Heap is empty (size: %d)\n", heap->size);
        return NULL;
    }
    
    // The root of a max-heap (index 0) contains the maximum element
    UserProfile* topUser = heap->userArray[0];
    
    // Additional safety check
    if (topUser == NULL) {
        fprintf(stderr, "Error [getTopWealthUser]: Top user at index 0 is NULL\n");
        return NULL;
    }
    
    return topUser;
}

int findUserIndex(UserHeap* heap, UserProfile* user) {
    // Error handling: Check if heap is NULL
    if (heap == NULL) {
        fprintf(stderr, "Error [findUserIndex]: Heap is NULL\n");
        return -1;
    }
    
    // Error handling: Check if user is NULL
    if (user == NULL) {
        fprintf(stderr, "Error [findUserIndex]: User is NULL\n");
        return -1;
    }
    
    // Error handling: Check if heap's user array is NULL
    if (heap->userArray == NULL) {
        fprintf(stderr, "Error [findUserIndex]: Heap's user array is NULL\n");
        return -1;
    }
    
    // Error handling: Check if heap size is valid
    if (heap->size < 0) {
        fprintf(stderr, "Error [findUserIndex]: Invalid heap size: %d\n", heap->size);
        return -1;
    }
    
    // Linear search through the heap array
    for (int i = 0; i < heap->size; i++) {
        // Safety check for each array element
        if (heap->userArray[i] == NULL) {
            fprintf(stderr, "Warning [findUserIndex]: NULL user at index %d\n", i);
            continue;
        }
        
        // Compare user pointers (memory addresses)
        if (heap->userArray[i] == user) {
            return i;
        }
    }
    
    // User not found in heap
    fprintf(stderr, "Warning [findUserIndex]: User '%s' not found in heap\n", 
            user->name);
    return -1;
}

double sumAllDescendants(WealthNode* firstChild) {
    double total = 0.0;
    WealthNode* current = firstChild;
    
    // Iterate through the list of siblings (e.g., "stock", "gold", "real estate")
    while (current != NULL) {
        
        // 1. Add the current node's value
        // (e.g., the value of the "stock" node)
        total += current->value;
        
        // 2. Recursively add the value of all its children
        // (e.g., if "stock" had children "AAPL", "MSFT")
        total += sumAllDescendants(current->firstChild);
        
        // 3. Move to the next sibling
        // (e.g., move from "stock" to "gold")
        current = current->nextSibling;
    }
    return total;
}

double calculateNetWorth(WealthNode* root) {
    if (root == NULL) {
        return 0.0;
    }

    double totalAssets = 0.0;
    double totalLiabilities = 0.0;

    // 1. Find the main "Investments" branch
    // We assume "findWealthNode" is implemented to find a node by name
    WealthNode* investmentsNode = findWealthNode(root, "Investments");
    
    if (investmentsNode != NULL) {
        // Sum all children of "Investments" (stock, gold, etc.)
        totalAssets = sumAllDescendants(investmentsNode->firstChild);
    }

    // 2. Find the main "Expenses" branch
    WealthNode* expensesNode = findWealthNode(root, "Expenses");
    
    if (expensesNode != NULL) {
        // Sum all children of "Expenses" (health, travel, etc.)
        totalLiabilities = sumAllDescendants(expensesNode->firstChild);
    }

    // 3. Net Worth is Assets minus Liabilities
    return totalAssets - totalLiabilities;
}

void finalizeUserUpdates(UserProfile* user) {
    // Error handling: Check if user is NULL
    if (user == NULL) {
        fprintf(stderr, "Error [finalizeUserUpdates]: User is NULL\n");
        return;
    }
    
    // Error handling: Check if global heap exists
    if (g_userHeap == NULL) {
        fprintf(stderr, "Error [finalizeUserUpdates]: Global heap is NULL\n");
        return;
    }
    
    // Error handling: Check if user has a wealth tree
    if (user->wealthTreeRoot == NULL) {
        fprintf(stderr, "Warning [finalizeUserUpdates]: User '%s' has NULL wealth tree\n", 
                user->name);
        user->netWorth = 0.0;
        return;
    }
    
    // Step 1: Recalculate net worth from wealth tree
    double oldNetWorth = user->netWorth;
    user->netWorth = calculateNetWorth(user->wealthTreeRoot);
    
    // Step 2: Find user's current position in heap
    int userIndex = findUserIndex(g_userHeap, user);
    
    if (userIndex == -1) {
        fprintf(stderr, "Error [finalizeUserUpdates]: User '%s' not found in heap\n", 
                user->name);
        return;
    }
    
    // Step 3: Fix heap property based on net worth change
    // If net worth increased, bubble up; if decreased, bubble down
    if (user->netWorth > oldNetWorth) {
        // Net worth increased - may need to move up in max-heap
        heapifyUp(g_userHeap, userIndex);
    } else if (user->netWorth < oldNetWorth) {
        // Net worth decreased - may need to move down in max-heap
        heapifyDown(g_userHeap, userIndex);
    }
    // If net worth unchanged, no heap adjustment needed
}

void freeHeap(UserHeap* heap) {
    // Error handling: Check if heap is NULL
    if (heap == NULL) {
        fprintf(stderr, "Warning [freeHeap]: Heap is NULL, nothing to free\n");
        return;
    }
    
    // Free all users and their associated data structures
    if (heap->userArray != NULL) {
        for (int i = 0; i < heap->size; i++) {
            UserProfile* user = heap->userArray[i];
            
            // Safety check for each user
            if (user == NULL) {
                fprintf(stderr, "Warning [freeHeap]: NULL user at index %d\n", i);
                continue;
            }
            
            // Free the user's wealth tree (recursive)
            if (user->wealthTreeRoot != NULL) {
                freeWealthTree(user->wealthTreeRoot);
                user->wealthTreeRoot = NULL;
            }
            
            // Free the user's expense list (iterative)
            if (user->expenseListHead != NULL) {
                freeExpenseList(user->expenseListHead);
                user->expenseListHead = NULL;
            }
            
            // Free the UserProfile struct itself
            free(user);
            heap->userArray[i] = NULL;
        }
        
        // Free the user array
        free(heap->userArray);
        heap->userArray = NULL;
    }
    
    // Reset heap metadata
    heap->size = 0;
    heap->capacity = 0;
    
    // Free the heap struct itself
    free(heap);
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