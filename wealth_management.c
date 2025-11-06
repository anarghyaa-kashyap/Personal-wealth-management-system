#include "wealth.h"
#include <string.h> 
#include <stdlib.h> 
#include <stdio.h>  
#include <ctype.h> // For strcicmp (if needed here, though it's in main)

//global definition for user heap - mentioned as extern in header
UserHeap* g_userHeap = NULL;


WealthNode* createWealthNode(const char* name, double value) {
    //function to create a wealthNode;allocate memory and initialize values to NULL
    WealthNode* newNode = (WealthNode*)malloc(sizeof(WealthNode));
    if (newNode == NULL) {
        printf("ERROR: Memory allocation failed for WealthNode.\n");
        exit(1);
    }

    strncpy(newNode->name, name, 49);
    newNode->name[49] = '\0';
    newNode->value = value;
    newNode->firstChild = NULL;
    newNode->nextSibling = NULL;
    return newNode;
}

void addWealthChild(WealthNode* parent, WealthNode* newChild) {
    if (parent == NULL || newChild == NULL){ //if function parameter is not provided/is NULL
        return; 
    }
    if (parent->firstChild == NULL) {//if parent has no children yet
        parent->firstChild = newChild;
    } else {
        WealthNode* temp = parent->firstChild;
        while (temp->nextSibling != NULL){ //going till the last child
            temp = temp->nextSibling;
        }
        temp->nextSibling = newChild;//adding the new node
    }
}

WealthNode* findWealthNode(WealthNode* root, const char* name) {
    if (root == NULL) { //failure base case- stops the recursion after traversal till leaf hasn't found required node
        return NULL;
    }
    if (strcmp(root->name, name) == 0) {
        return root;  //success base case - returns found node and stops recursion
    }
    WealthNode* found = findWealthNode(root->firstChild, name);//recursively searches in all children of current root
    if (found != NULL) {
        return found; //success base case - returns found node and stops recursion
    }
    return findWealthNode(root->nextSibling, name);//then takes the sibling branch if not found among current root or its children
}

void printWealthTree(WealthNode* root, int indent) {//takes indent as param for matching root lvl when printing
    //recursively printing wealth tree
    if (root == NULL) {
        return; 
    }
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("+- %s: (Rs.%.2f)\n", root->name, root->value);
    //traverses through (and prints) all children of a node first
    printWealthTree(root->firstChild, indent + 2);//increases indent/spacing by 2 for each child lvl
    //then recursively moves to all siblings of the current node
    printWealthTree(root->nextSibling, indent);
}


void freeWealthTree(WealthNode* root) {
    //recursively frees entire tree
    if (root == NULL) {
        return; 
    }
    freeWealthTree(root->firstChild);
    freeWealthTree(root->nextSibling);
    free(root);
}

/*compare two users to decide MAX-heap order.
- higher netWorth = higher priority
- if equal, name earlier in A–Z = higher priority
returns 0 when they are null/identical, -1 when b is greater and 1 when a is greater*/
static int userCompare(const UserProfile* a, const UserProfile* b) {
    if (a == NULL && b == NULL) return 0;
    if (a == NULL) return -1;
    if (b == NULL) return 1;

    if (a->netWorth > b->netWorth) return 1;
    if (a->netWorth < b->netWorth) return -1;

    //tie-break by name
    int cmp = strcmp(a->name, b->name);
    if (cmp > 0) return 1;     //a is above b
    if (cmp < 0) return -1;    //a is below b
    return 0;
}

//Creates heap with given capacity - sets to 1 if not given/incorrectly entered
UserHeap* createHeap(int capacity) {
    if (capacity <= 0) capacity = 1;

    //allocating memory for user heap
    UserHeap* heap = (UserHeap*)malloc(sizeof(UserHeap));
    if (heap == NULL) {
        printf("Memory allocation for heap failed.\n");
        return NULL;
    }

    //allocating memory for userArray present within userHeap struct
    heap->userArray = (UserProfile**)malloc(sizeof(UserProfile*) * capacity);
    if (heap->userArray == NULL) {
        printf("Memory allocation for heap array failed.\n");
        free(heap);
        return NULL;
    }

    //initializes array slots to NULL
    for (int i = 0; i < capacity; i++) heap->userArray[i] = NULL;
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

void swapUsers(UserHeap* heap, int i, int j) {
    //swaps two positions in the heap array - used during heapification
    if (heap == NULL || heap->userArray == NULL) return; //checks if userHeap, or userArray are empty
    if (i < 0 || j < 0 || i >= heap->size || j >= heap->size) return; //checks if positions to be swapped are valid

    //swapping logic using temporary var
    UserProfile* temp = heap->userArray[i];
    heap->userArray[i] = heap->userArray[j];
    heap->userArray[j] = temp;
}

void heapifyUp(UserHeap* heap, int index) {
    //moves a node up until heap property holds
    if (heap == NULL || heap->userArray == NULL) return;

    //traverses from leaf nodes to root node of heap
    while (index > 0) {
        int parent = (index - 1) / 2;

        //checks if child has higher priority than parent, swaps accordingly
        if (userCompare(heap->userArray[index], heap->userArray[parent]) > 0) {
            swapUsers(heap, index, parent);
            index = parent;
        } else {
            break;
        }
    }
}

void heapifyDown(UserHeap* heap, int index) {
    //move a node down until heap property holds
    if (heap == NULL || heap->userArray == NULL) return;
    while (1) {
        int left = index * 2 + 1;
        int right = index * 2 + 2;
        int largest = index;
        if (left < heap->size &&
            userCompare(heap->userArray[left], heap->userArray[largest]) > 0) { //checks for left child, then compares child and current largest(index)
            largest = left;
        }
        if (right < heap->size &&
            userCompare(heap->userArray[right], heap->userArray[largest]) > 0) { //now checks for right child, then compares child and current largest(index)
            largest = right;
        }

        if (largest != index) { //if one of the children is larger, bubbles current node down
            swapUsers(heap, index, largest);
            index = largest;
        } else {
            break;
        }
    }
}


void heapInsert(UserHeap* heap, UserProfile* user) {
    //insert a users and fixes heap order
    if (heap == NULL || heap->userArray == NULL || user == NULL) return;

    //increases heap capacity if needed
    if (heap->size >= heap->capacity) {
        int newCap = heap->capacity * 2;
        if (newCap == 0) newCap = 10; //handles edgecase of initial capacity 0
        UserProfile** newArr =
            (UserProfile**)realloc(heap->userArray, sizeof(UserProfile*) * newCap);
        if (newArr == NULL) {
            printf("Heap resize failed.\n");
            return;
        }
        //initalizes new slots to NULL
        for (int i = heap->capacity; i < newCap; i++) newArr[i] = NULL;
        heap->userArray = newArr;
        heap->capacity = newCap;
    }

    //adds a node at the bottom of the heap then heapifies upward
    heap->userArray[heap->size] = user;
    heapifyUp(heap, heap->size);
    //updates heap size
    heap->size++;
}


//return pointer to richest user(top). Null if empty
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

//find the index of a given user pointer 
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

//prints all the heap entries(in array order)
void displayHeap(UserHeap* heap) {
    if (heap == NULL || heap->size == 0) {
        printf("\nNo users in the system to display.\n");
        return;
    }
    printf("\n--- All Users ---\n");
    for (int i = 0; i < heap->size; i++) {
        if (heap->userArray[i] != NULL) {
            printf("%d. Name: %s, Net Worth: Rs.%.2f\n", 
                   i + 1, 
                   heap->userArray[i]->name, 
                   heap->userArray[i]->netWorth);
        }
    }
}

//recursively sum children to compute total networth(treat expenses as negative)
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

//recalculate user's networth and fix their heap position
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
    
    //keep old to decide move direction
    double oldNetWorth = user->netWorth;

    //recompute from tree and set root value
    user->netWorth = recursiveUpdateAndGetWorth(user->wealthTreeRoot);
    user->wealthTreeRoot->value = user->netWorth;

    //find where the user is in the heap array
    int userIndex = findUserIndex(g_userHeap, user);
    if (userIndex == -1) {
        fprintf(stderr, "Error [finalizeUserUpdates]: User '%s' not found in heap\n", 
                user->name);
        return;
    }

    //move up if richer, else move down
    if (user->netWorth > oldNetWorth) {
        heapifyUp(g_userHeap, userIndex);
    } else if (user->netWorth < oldNetWorth) {
        heapifyDown(g_userHeap, userIndex);
    }
}

//free all users inside heap, then free heap memory
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

//push a new expense entry at the head of the user's list
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

    //copy strings and fill fields
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

//update the value of the given investment node
void updateInvestmentValue(UserProfile* user, const char* nodeName, double newValue) {
     if (!user || !user->wealthTreeRoot || !nodeName) { 
        printf("Invalid input to updateInvestmentValue.\n");
        return;
    }
    
    //find the node by name
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

//add amount to a given expense category
void updateExpenseCategoryTotal(UserProfile* user, const char* category, double amount) {
    if (!user || !user->wealthTreeRoot || !category) { 
        printf("Invalid input to updateExpenseCategoryTotal.\n");
        return;
    }
    
    //go to the expenses branch
    WealthNode* expensesRoot = findWealthNode(user->wealthTreeRoot, "Expenses");
    if (!expensesRoot) {
        printf("Error: 'Expenses' category not found in tree.\n");
        return;
    }

    //find the category
    WealthNode* node = findWealthNode(expensesRoot, category);
    
    if (!node) {
        printf("Category '%s' not found under 'Expenses'.\n", category);
        return;
    }
    
    node->value += amount;
}

//create a new user, build a tree and insert into heap
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

    //fill user fields
    strncpy(user->name, name, 49);
    user->name[49] = '\0';
    strncpy(user->gender, gender, 9);
    user->gender[9] = '\0';
    
    user->netWorth = 0.0;
    user->expenseListHead = NULL;

    //build root and main branches
    user->wealthTreeRoot = createWealthNode(name, 0.0); 
    
    WealthNode* income = createWealthNode("Income", 0.0);
    WealthNode* expenses = createWealthNode("Expenses", 0.0);
    WealthNode* investments = createWealthNode("Investments", 0.0);
    
    addWealthChild(user->wealthTreeRoot, income);
    addWealthChild(user->wealthTreeRoot, expenses);
    addWealthChild(user->wealthTreeRoot, investments);

    //add subnodes
    addWealthChild(income, createWealthNode("salary", 0.0));

    addWealthChild(investments, createWealthNode("gold", 0.0));
    addWealthChild(investments, createWealthNode("stock", 0.0));
    addWealthChild(investments, createWealthNode("real estate", 0.0));
    addWealthChild(investments, createWealthNode("others", 0.0));

    addWealthChild(expenses, createWealthNode("health", 0.0));
    addWealthChild(expenses, createWealthNode("travel", 0.0));
    addWealthChild(expenses, createWealthNode("regular", 0.0));
    
    //add user to global heap
    heapInsert(g_userHeap, user);
}

//print the expense list
void printExpenseLog(ExpenditureNode* head) {
    if (!head) {
        printf("No transactions found.\n");
        return;
    }
    printf("\n--- Transaction Log ---\n");
    ExpenditureNode* temp = head;
    while(temp != NULL) {
        //ctime converts time to a readable string
        char* timeStr = ctime(&temp->date); 
        //remove trailing newline
        timeStr[strcspn(timeStr, "\n")] = 0; 

        printf("  [%s] %s - Rs.%.2f (%s)\n", 
               temp->category, temp->description, temp->amount, timeStr);
        temp = temp->next;
    }
}

//free all the nodes in the linked list
void freeExpenseList(ExpenditureNode* head) {
    ExpenditureNode* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}
