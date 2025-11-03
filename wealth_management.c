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

void logExpenseToList(UserProfile* user, const char* category, const char* desc, double amount, InvestmentType invType) {
    //Annanya
     if (!user || !category || !desc || amount < 0) {
        printf("Invalid transaction details.\n");
        return;
    }
    ExpenditureNode* newNode = (ExpenditureNode*)malloc(sizeof(ExpenditureNode));
    if (!newNode) {
        printf("Memory allocation failed.\n");
        return;
    }

    // Use strncpy for safety
    strncpy(newNode->category, category, 49);
    newNode->category[49] = '\0';
    strncpy(newNode->description, desc, 99);
    newNode->description[99] = '\0';
    
    newNode->amount = amount;
    newNode->investmentType = invType; // <-- CORRECTED: invType was wrong
    newNode->date = time(NULL); // Add timestamp
    
    // *** BUG FIX: Use correct struct member name ***
    newNode->next = user->expenseListHead;
    user->expenseListHead = newNode;

    printf("Transaction added: [%s] %s - %.2f\n", category, desc, amount);
}

void updateInvestmentValue(UserProfile* user, const char* nodeName, double newValue) {
    //Annanya
     if (!user || !user->wealthTreeRoot || !nodeName) { // *** BUG FIX: wealthTreeRoot ***
        printf("Invalid input to updateInvestmentValue.\n");
        return;
    }
    
    // *** BUG FIX: wealthTreeRoot ***
    WealthNode* node = findWealthNode(user->wealthTreeRoot, nodeName);
    
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
    
    // *** BUG FIX: CRITICAL INTERCONNECTIVITY STEP ***
    finalizeUserUpdates(user);
}

void updateExpenseCategoryTotal(UserProfile* user, const char* category, double amount) {
    //Annanya
    if (!user || !user->wealthTreeRoot || !category) { // *** BUG FIX: wealthTreeRoot ***
        printf("Invalid input to updateExpenseCategoryTotal.\n");
        return;
    }
    
    // *** BUG FIX: wealthTreeRoot ***
    WealthNode* node = findWealthNode(user->wealthTreeRoot, category);
    
    if (!node) {
        printf("Category '%s' not found.\n", category);
        return;
    }
    
    node->value += amount;
    printf("Added %.2f to '%s' (New Total: %.2f)\n", amount, category, node->value);
    
    // *** BUG FIX: CRITICAL INTERCONNECTIVITY STEP ***
    finalizeUserUpdates(user);
}

void registerNewUser(const char* name, const char* gender) {
    //Annanya
    if (!name || !gender) {
        printf("Invalid user details.\n");
        return; // Changed to void return
    }
    UserProfile* user = (UserProfile*)malloc(sizeof(UserProfile));
    if (!user) {
        printf("Memory allocation failed.\n");
        return; // Changed to void return
    }

    // 1. Set basic info
    strncpy(user->name, name, 49);
    user->name[49] = '\0';
    strncpy(user->gender, gender, 9);
    user->gender[9] = '\0';
    
    user->netWorth = 0.0;
    
    // *** BUG FIX: Use correct struct member name ***
    user->expenseListHead = NULL;

    // 2. Build the default Wealth Tree
    // *** BUG FIX: Use correct struct member name ***
    user->wealthTreeRoot = createWealthNode(name, 0.0); 
    
    // Build main branches
    WealthNode* investments = createWealthNode("Investments", 0.0);
    WealthNode* expenses = createWealthNode("Expenses", 0.0);
    
    addWealthChild(user->wealthTreeRoot, investments);
    addWealthChild(user->wealthTreeRoot, expenses);

    // Add investment sub-nodes (based on enum)
    addWealthChild(investments, createWealthNode("gold", 0.0));
    addWealthChild(investments, createWealthNode("stock", 0.0));
    addWealthChild(investments, createWealthNode("property", 0.0));
    addWealthChild(investments, createWealthNode("others", 0.0));

    // Add expense sub-nodes (based on main.c)
    addWealthChild(expenses, createWealthNode("health", 0.0));
    addWealthChild(expenses, createWealthNode("travel", 0.0));
    addWealthChild(expenses, createWealthNode("regular", 0.0));
    addWealthChild(expenses, createWealthNode("investment", 0.0)); // For tracking *cost*

    // 3. Add the new user to the global heap
    // *** BUG FIX: CRITICAL INTERCONNECTIVITY STEP ***
    heapInsert(g_userHeap, user);

    printf("User '%s' registered successfully.\n", name);
    // Function is void, no return
}

void printExpenseLog(ExpenditureNode* head) {
    //Annanya
    if (!head) {
        printf("No transactions found.\n");
        return;
    }
    printf("\n--- Transaction Log ---\n");
    ExpenditureNode* temp = head;
    while(temp != NULL) {
        printf("[%s] %s - %.2f\n", temp->category, temp->description, temp->amount);
        temp = temp->next;
    }
}

void freeExpenseList(ExpenditureNode* head) {
    //Annanya
    ExpenditureNode* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
    printf("Expense list cleared successfully.\n");
}

// *** BUG FIX: Removed stray '}' that was here ***