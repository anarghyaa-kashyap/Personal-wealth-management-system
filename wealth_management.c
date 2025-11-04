#include "wealth.h"
#include <string.h> 
#include <stdlib.h> 
#include <stdio.h>  
#include <ctype.h> // For strcicmp (if needed here, though it's in main)

// ================================================================
// --- GLOBAL VARIABLE DEFINITION ---
// ================================================================

UserHeap* g_userHeap = NULL;

// ================================================================
// --- (Ahana's Functions) ---
// ================================================================

WealthNode* createWealthNode(const char* name, double value) {
    WealthNode* newNode = (WealthNode*)malloc(sizeof(WealthNode));
    if (newNode == NULL) {
        printf("ERROR: Memory allocation failed for WealthNode.\n");
        exit(1);
    }
    strncpy(newNode->name, name, 49);
    newNode->name[49] = '\0';
    newNode->value = value;
    // previousValue initialization removed
    newNode->firstChild = NULL;
    newNode->nextSibling = NULL;
    return newNode;
}

void addWealthChild(WealthNode* parent, WealthNode* newChild) {
    if (parent == NULL || newChild == NULL){ 
        return; 
    }
    if (parent->firstChild == NULL) {
        parent->firstChild = newChild;
    } else {
        WealthNode* temp = parent->firstChild;
        while (temp->nextSibling != NULL){ 
            temp = temp->nextSibling;
        }
        temp->nextSibling = newChild;
    }
}

WealthNode* findWealthNode(WealthNode* root, const char* name) {
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
    if (root == NULL) {
        return; 
    }
    for (int i = 0; i < indent; i++) {
        printf("  "); 
    }
    printf("+- %s: ($%.2f)\n", root->name, root->value);
    printWealthTree(root->firstChild, indent + 2); 
    printWealthTree(root->nextSibling, indent);
}


void freeWealthTree(WealthNode* root) {
    if (root == NULL) {
        return; 
    }
    freeWealthTree(root->firstChild);
    freeWealthTree(root->nextSibling);
    free(root);
}

// ================================================================
// --- (Anarghyaa's Functions) ---
// ================================================================

static int userCompare(const UserProfile* a, const UserProfile* b) {
    if (a == NULL && b == NULL) return 0;
    if (a == NULL) return -1;
    if (b == NULL) return 1;

    if (a->netWorth > b->netWorth) return 1;
    if (a->netWorth < b->netWorth) return -1;

    int cmp = strcmp(a->name, b->name);
    if (cmp < 0) return 1;
    if (cmp > 0) return -1;
    return 0;
}

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
    for (int i = 0; i < capacity; i++) heap->userArray[i] = NULL;
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

void swapUsers(UserHeap* heap, int i, int j) {
    if (heap == NULL || heap->userArray == NULL) return;
    if (i < 0 || j < 0 || i >= heap->size || j >= heap->size) return;
    UserProfile* temp = heap->userArray[i];
    heap->userArray[i] = heap->userArray[j];
    heap->userArray[j] = temp;
}

void heapifyUp(UserHeap* heap, int index) {
    if (heap == NULL || heap->userArray == NULL) return;
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (userCompare(heap->userArray[index], heap->userArray[parent]) > 0) {
            swapUsers(heap, index, parent);
            index = parent;
        } else {
            break;
        }
    }
}

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

void heapInsert(UserHeap* heap, UserProfile* user) {
    if (heap == NULL || heap->userArray == NULL || user == NULL) return;
    if (heap->size >= heap->capacity) { 
        int newCap = heap->capacity * 2;
        if (newCap == 0) newCap = 10;
        UserProfile** newArr =
            (UserProfile**)realloc(heap->userArray, sizeof(UserProfile*) * newCap);
        if (newArr == NULL) {
            printf("Heap resize failed.\n");
            return;
        }
        for (int i = heap->capacity; i < newCap; i++) newArr[i] = NULL;
        heap->userArray = newArr;
        heap->capacity = newCap;
    }
    heap->userArray[heap->size] = user;
    heapifyUp(heap, heap->size);
    heap->size++;
}

// ================================================================
// --- (Akshata's Functions) ---
// ================================================================

UserProfile* getTopWealthUser(UserHeap* heap) {
    if (heap == NULL) {
        fprintf(stderr, "Error [getTopWealthUser]: Heap is NULL\n");
        return NULL;
    }
    if (heap->userArray == NULL) {
        fprintf(stderr, "Error [getTopWealthUser]: Heap's user array is NULL\n");
        return NULL;
    }
    if (heap->size <= 0) {
        return NULL;
    }
    UserProfile* topUser = heap->userArray[0];
    if (topUser == NULL) {
        fprintf(stderr, "Error [getTopWealthUser]: Top user at index 0 is NULL\n");
        return NULL;
    }
    return topUser;
}

int findUserIndex(UserHeap* heap, UserProfile* user) {
    if (heap == NULL) {
        fprintf(stderr, "Error [findUserIndex]: Heap is NULL\n");
        return -1;
    }
    if (user == NULL) {
        fprintf(stderr, "Error [findUserIndex]: User is NULL\n");
        return -1;
    }
    if (heap->userArray == NULL) {
        fprintf(stderr, "Error [findUserIndex]: Heap's user array is NULL\n");
        return -1;
    }
    if (heap->size < 0) {
        fprintf(stderr, "Error [findUserIndex]: Invalid heap size: %d\n", heap->size);
        return -1;
    }
    for (int i = 0; i < heap->size; i++) {
        if (heap->userArray[i] == NULL) {
            fprintf(stderr, "Warning [findUserIndex]: NULL user at index %d\n", i);
            continue;
        }
        if (heap->userArray[i] == user) {
            return i;
        }
    }
    return -1;
}

void displayHeap(UserHeap* heap) {
    if (heap == NULL || heap->size == 0) {
        printf("\nNo users in the system to display.\n");
        return;
    }
    printf("\n--- All Users ---\n");
    for (int i = 0; i < heap->size; i++) {
        if (heap->userArray[i] != NULL) {
            printf("%d. Name: %s, Net Worth: $%.2f\n", 
                   i + 1, 
                   heap->userArray[i]->name, 
                   heap->userArray[i]->netWorth);
        }
    }
}


double recursiveUpdateAndGetWorth(WealthNode* root) {
    if (root == NULL) {
        return 0.0;
    }

    if (root->firstChild == NULL) {
        return root->value;
    }

    double childrenSum = 0.0;
    WealthNode* child = root->firstChild;
    while (child != NULL) {
        childrenSum += recursiveUpdateAndGetWorth(child);
        child = child->nextSibling;
    }

    root->value = childrenSum;

    if (strcmp(root->name, "Expenses") == 0) {
        return -root->value;
    }

    return root->value;
}

void finalizeUserUpdates(UserProfile* user) {
    if (user == NULL) {
        fprintf(stderr, "Error [finalizeUserUpdates]: User is NULL\n");
        return;
    }
    if (g_userHeap == NULL) {
        fprintf(stderr, "Error [finalizeUserUpdates]: Global heap is NULL\n");
        return;
    }
    if (user->wealthTreeRoot == NULL) {
        fprintf(stderr, "Warning [finalizeUserUpdates]: User '%s' has NULL wealth tree\n", 
                user->name);
        user->netWorth = 0.0;
    }
    
    double oldNetWorth = user->netWorth;

    user->netWorth = recursiveUpdateAndGetWorth(user->wealthTreeRoot);
    
    user->wealthTreeRoot->value = user->netWorth;

    int userIndex = findUserIndex(g_userHeap, user);
    if (userIndex == -1) {
        fprintf(stderr, "Error [finalizeUserUpdates]: User '%s' not found in heap\n", 
                user->name);
        return;
    }
    
    if (user->netWorth > oldNetWorth) {
        heapifyUp(g_userHeap, userIndex);
    } else if (user->netWorth < oldNetWorth) {
        heapifyDown(g_userHeap, userIndex);
    }
}

void freeHeap(UserHeap* heap) {
    if (heap == NULL) {
        fprintf(stderr, "Warning [freeHeap]: Heap is NULL, nothing to free\n");
        return;
    }
    if (heap->userArray != NULL) {
        for (int i = 0; i < heap->size; i++) {
            UserProfile* user = heap->userArray[i];
            if (user == NULL) {
                fprintf(stderr, "Warning [freeHeap]: NULL user at index %d\n", i);
                continue;
            }
            if (user->wealthTreeRoot != NULL) {
                freeWealthTree(user->wealthTreeRoot);
                user->wealthTreeRoot = NULL;
            }
            if (user->expenseListHead != NULL) {
                freeExpenseList(user->expenseListHead);
                user->expenseListHead = NULL;
            }
            free(user);
            heap->userArray[i] = NULL;
        }
        free(heap->userArray);
        heap->userArray = NULL;
    }
    free(heap);
    if (heap == g_userHeap) {
        g_userHeap = NULL;
    }
}

// ================================================================
// --- (Annanya's Functions) ---
// ================================================================

void logExpenseToList(UserProfile* user, const char* category, const char* desc, double amount, InvestmentType invType) {
     if (!user || !category || !desc || amount < 0) {
        printf("Invalid transaction details.\n");
        return;
    }
    ExpenditureNode* newNode = (ExpenditureNode*)malloc(sizeof(ExpenditureNode));
    if (!newNode) {
        printf("Memory allocation failed.\n");
        return;
    }

    strncpy(newNode->category, category, 49);
    newNode->category[49] = '\0';
    strncpy(newNode->description, desc, 99);
    newNode->description[99] = '\0';
    
    newNode->amount = amount;
    newNode->investmentType = invType; 
    newNode->date = time(NULL); 
    
    newNode->next = user->expenseListHead;
    user->expenseListHead = newNode;
}

void updateInvestmentValue(UserProfile* user, const char* nodeName, double newValue) {
     if (!user || !user->wealthTreeRoot || !nodeName) { 
        printf("Invalid input to updateInvestmentValue.\n");
        return;
    }
    
    WealthNode* node = findWealthNode(user->wealthTreeRoot, nodeName);
    
    if (!node) {
        printf("Category '%s' not found.\n", nodeName);
        return;
    }
    if (newValue < 0) {
        printf("Negative value not allowed.\n");
        return;
    }
    
    // previousValue assignment removed
    node->value = newValue;
    printf("Updated '%s' to %.2f\n", nodeName, newValue);
}

void updateExpenseCategoryTotal(UserProfile* user, const char* category, double amount) {
    if (!user || !user->wealthTreeRoot || !category) { 
        printf("Invalid input to updateExpenseCategoryTotal.\n");
        return;
    }
    
    WealthNode* expensesRoot = findWealthNode(user->wealthTreeRoot, "Expenses");
    if (!expensesRoot) {
        printf("Error: 'Expenses' category not found in tree.\n");
        return;
    }

    WealthNode* node = findWealthNode(expensesRoot, category);
    
    if (!node) {
        printf("Category '%s' not found under 'Expenses'.\n", category);
        return;
    }
    
    node->value += amount;
}

void registerNewUser(const char* name, const char* gender) {
    if (!name || !gender || strlen(name) == 0) {
        printf("Invalid user details.\n");
        return; 
    }
    UserProfile* user = (UserProfile*)malloc(sizeof(UserProfile));
    if (!user) {
        printf("Memory allocation failed.\n");
        return; 
    }

    strncpy(user->name, name, 49);
    user->name[49] = '\0';
    strncpy(user->gender, gender, 9);
    user->gender[9] = '\0';
    
    user->netWorth = 0.0;
    user->expenseListHead = NULL;

    user->wealthTreeRoot = createWealthNode(name, 0.0); 
    
    WealthNode* income = createWealthNode("Income", 0.0);
    WealthNode* expenses = createWealthNode("Expenses", 0.0);
    WealthNode* investments = createWealthNode("Investments", 0.0);
    
    addWealthChild(user->wealthTreeRoot, income);
    addWealthChild(user->wealthTreeRoot, expenses);
    addWealthChild(user->wealthTreeRoot, investments);

    addWealthChild(income, createWealthNode("salary", 0.0));

    addWealthChild(investments, createWealthNode("gold", 0.0));
    addWealthChild(investments, createWealthNode("stock", 0.0));
    addWealthChild(investments, createWealthNode("real estate", 0.0));
    addWealthChild(investments, createWealthNode("others", 0.0));

    addWealthChild(expenses, createWealthNode("health", 0.0));
    addWealthChild(expenses, createWealthNode("travel", 0.0));
    addWealthChild(expenses, createWealthNode("regular", 0.0));
    
    heapInsert(g_userHeap, user);
}

void printExpenseLog(ExpenditureNode* head) {
    if (!head) {
        printf("No transactions found.\n");
        return;
    }
    printf("\n--- Transaction Log ---\n");
    ExpenditureNode* temp = head;
    while(temp != NULL) {
        char* timeStr = ctime(&temp->date); 
        timeStr[strcspn(timeStr, "\n")] = 0; 

        printf("  [%s] %s - $%.2f (%s)\n", 
               temp->category, temp->description, temp->amount, timeStr);
        temp = temp->next;
    }
}

void freeExpenseList(ExpenditureNode* head) {
    ExpenditureNode* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// The predictStock function has been entirely removed.