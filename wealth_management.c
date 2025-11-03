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

// ===== Person 2: Max-Heap Operations =====
#include "wealth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Compare two users for MAX-heap order.
   Higher netWorth = higher priority.
   If equal, name earlier in A–Z = higher priority. */
static int userCompare(const UserProfile* a, const UserProfile* b) {
    if (a == NULL && b == NULL) return 0;
    if (a == NULL) return -1;
    if (b == NULL) return 1;

    if (a->netWorth > b->netWorth) return 1;
    if (a->netWorth < b->netWorth) return -1;

    /* tie-break by name */
    int cmp = strcmp(a->name, b->name);
    if (cmp < 0) return 1;     /* a should be above b */
    if (cmp > 0) return -1;    /* a should be below b */
    return 0;
}

/* Create heap with given capacity (at least 1). */
UserHeap* createHeap(int capacity) {
    if (capacity <= 0) capacity = 1;

    UserHeap* heap = (UserHeap*)malloc(sizeof(UserHeap));
    if (heap == NULL) {
        printf("Heap alloc failed.\n");
        return NULL;
    }

    heap->userArray = (UserProfile**)malloc(sizeof(UserProfile*) * capacity);
    if (heap->userArray == NULL) {
        printf("Heap array alloc failed.\n");
        free(heap);
        return NULL;
    }

    /* initialize slots to NULL */
    for (int i = 0; i < capacity; i++) heap->userArray[i] = NULL;

    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

/* Swap two positions in the heap array. */
void swapUsers(UserHeap* heap, int i, int j) {
    if (heap == NULL || heap->userArray == NULL) return;
    if (i < 0 || j < 0 || i >= heap->size || j >= heap->size) return;

    UserProfile* temp = heap->userArray[i];
    heap->userArray[i] = heap->userArray[j];
    heap->userArray[j] = temp;
}

/* Move a node up until heap property holds. */
void heapifyUp(UserHeap* heap, int index) {
    if (heap == NULL || heap->userArray == NULL) return;

    while (index > 0) {
        int parent = (index - 1) / 2;

        /* if child has higher priority than parent, swap */
        if (userCompare(heap->userArray[index], heap->userArray[parent]) > 0) {
            swapUsers(heap, index, parent);
            index = parent;
        } else {
            break;
        }
    }
}

/* Move a node down until heap property holds. */
void heapifyDown(UserHeap* heap, int index) {
    if (heap == NULL || heap->userArray == NULL) return;

    while (1) {
        int left = index * 2 + 1;
        int right = index * 2 + 2;
        int largest = index;

        if (left < heap->size &&
            userCompare(heap->userArray[left], heap->userArray[largest]) > 0) {
            largest = left;
        }
        if (right < heap->size &&
            userCompare(heap->userArray[right], heap->userArray[largest]) > 0) {
            largest = right;
        }

        if (largest != index) {
            swapUsers(heap, index, largest);
            index = largest;
        } else {
            break;
        }
    }
}

/* Insert a user and fix heap order. Grows array when full. */
void heapInsert(UserHeap* heap, UserProfile* user) {
    if (heap == NULL || heap->userArray == NULL || user == NULL) return;

    /* grow capacity if needed */
    if (heap->size >= heap->capacity) {
        int newCap = heap->capacity * 2;
        UserProfile** newArr =
            (UserProfile**)realloc(heap->userArray, sizeof(UserProfile*) * newCap);
        if (newArr == NULL) {
            printf("Heap resize failed.\n");
            return;
        }
        /* init new slots */
        for (int i = heap->capacity; i < newCap; i++) newArr[i] = NULL;

        heap->userArray = newArr;
        heap->capacity = newCap;
    }

    /* place at end, then bubble up */
    heap->userArray[heap->size] = user;
    heapifyUp(heap, heap->size);
    heap->size++;
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