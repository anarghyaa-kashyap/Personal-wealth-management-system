#include "wealth.h"
#include <string.h> 
#include <stdlib.h> 
#include <stdio.h>  
#include <ctype.h> 
#include <math.h> 

UserHeap* g_userHeap = NULL;

WealthNode* createWealthNode(const char* name, double value) {
    WealthNode* newNode = (WealthNode*)malloc(sizeof(WealthNode));
    if (newNode == NULL) {
        printf("ERROR: Memory allocation failed for WealthNode.\n");
        exit(1);
    }
    strncpy(newNode->name, name, 49);
    newNode->name[49] = '\0';
    newNode->value = value;
    newNode->interestRate = 0.0;
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
    if (root->interestRate > 0.0) {
        printf("+- %s: (Rs.%.2f) [Rate: %.1f%%]\n", root->name, root->value, root->interestRate);
    } else {
        printf("+- %s: (Rs.%.2f)\n", root->name, root->value);
    }
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
    if (heap == NULL) return NULL;
    heap->userArray = (UserProfile**)malloc(sizeof(UserProfile*) * capacity);
    if (heap->userArray == NULL) { free(heap); return NULL; }
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
        if (newArr == NULL) return;
        for (int i = heap->capacity; i < newCap; i++) newArr[i] = NULL;
        heap->userArray = newArr;
        heap->capacity = newCap;
    }
    heap->userArray[heap->size] = user;
    heapifyUp(heap, heap->size);
    heap->size++;
}

UserProfile* getTopWealthUser(UserHeap* heap) {
    if (heap == NULL || heap->userArray == NULL || heap->size <= 0) return NULL;
    return heap->userArray[0];
}

int findUserIndex(UserHeap* heap, UserProfile* user) {
    if (heap == NULL || user == NULL || heap->userArray == NULL) return -1;
    for (int i = 0; i < heap->size; i++) {
        if (heap->userArray[i] == user) return i;
    }
    return -1;
}

void displayHeap(UserHeap* heap) {
    if (heap == NULL || heap->size == 0) {
        printf("\nNo users in the system to display.\n");
        return;
    }
    printf("\n----- ALL USERS -----\n");
    for (int i = 0; i < heap->size; i++) {
        if (heap->userArray[i] != NULL) {
            printf("%d. Name: %s, Net Worth: Rs.%.2f\n", i + 1, heap->userArray[i]->name, heap->userArray[i]->netWorth);
        }
    }
}

double recursiveUpdateAndGetWorth(WealthNode* root) {
    if (root == NULL) return 0.0;
    if (root->firstChild == NULL) return root->value;
    
    double childrenSum = 0.0;
    WealthNode* child = root->firstChild;
    while (child != NULL) {
        childrenSum += recursiveUpdateAndGetWorth(child);
        child = child->nextSibling;
    }
    root->value = childrenSum;
    if (strcmp(root->name, "Expenses") == 0) return -root->value;
    return root->value;
}

double calculateProjectedNetWorth(WealthNode* root, int years) {
    if (root == NULL) return 0.0;

    if (root->firstChild == NULL) {
        double projectedValue = root->value;
        if (root->interestRate > 0.0) {
            projectedValue = root->value * pow((1.0 + root->interestRate / 100.0), years);
        }
        return projectedValue;
    }

    double childrenSum = 0.0;
    WealthNode* child = root->firstChild;
    while (child != NULL) {
        childrenSum += calculateProjectedNetWorth(child, years);
        child = child->nextSibling;
    }

    if (strcmp(root->name, "Expenses") == 0) {
        return -root->value; 
    }

    return childrenSum;
}

void finalizeUserUpdates(UserProfile* user) {
    if (user == NULL || g_userHeap == NULL) return;
    
    double oldNetWorth = user->netWorth;
    if (user->wealthTreeRoot != NULL) {
        user->netWorth = recursiveUpdateAndGetWorth(user->wealthTreeRoot);
        user->wealthTreeRoot->value = user->netWorth;
    }

    int userIndex = findUserIndex(g_userHeap, user);
    if (userIndex == -1) return;

    if (user->netWorth > oldNetWorth) {
        heapifyUp(g_userHeap, userIndex);
    } else if (user->netWorth < oldNetWorth) {
        heapifyDown(g_userHeap, userIndex);
    }
}

void freeHeap(UserHeap* heap) {
    if (heap == NULL) return;
    if (heap->userArray != NULL) {
        for (int i = 0; i < heap->size; i++) {
            UserProfile* user = heap->userArray[i];
            if (user == NULL) continue;
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
    if (heap == g_userHeap) g_userHeap = NULL;
}

void logExpenseToList(UserProfile* user, const char* category, const char* desc, double amount, InvestmentType invType) {
     if (!user || !category || !desc || amount < 0) {
        printf("Invalid transaction details.\n");
        return;
    }
    ExpenditureNode* newNode = (ExpenditureNode*)malloc(sizeof(ExpenditureNode));
    if (!newNode) return;

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

void manageStock(UserProfile* user, const char* ticker, double amount, double rate, int isAdding) {
    if (!user || !user->wealthTreeRoot) return;

    WealthNode* investments = findWealthNode(user->wealthTreeRoot, "Investments");
    if (!investments) return;
    
    WealthNode* stockCategory = findWealthNode(investments, "stock");
    if (!stockCategory) return; 

    WealthNode* specificStock = findWealthNode(stockCategory, ticker);

    if (!specificStock) {
        if (isAdding) {
            specificStock = createWealthNode(ticker, 0.0);
            addWealthChild(stockCategory, specificStock);
        } else {
            printf("Error: You do not own any stock named '%s'. Cannot update.\n", ticker);
            return;
        }
    }

    if (isAdding) {
        specificStock->value += amount;
    } else {
        specificStock->value = amount; 
    }
    
    if (rate >= 0) {
        specificStock->interestRate = rate;
    }

    printf("Stock '%s' updated. New Value: %.2f, Rate: %.1f%%\n", ticker, specificStock->value, specificStock->interestRate);
    finalizeUserUpdates(user);
}

void manageAsset(UserProfile* user, const char* assetName, double amount, double rate, int isAdding) {
    if (!user || !user->wealthTreeRoot) return;

    WealthNode* investments = findWealthNode(user->wealthTreeRoot, "Investments");
    if (!investments) return;

    WealthNode* assetNode = findWealthNode(investments, assetName);
    
    if (!assetNode) {
        assetNode = createWealthNode(assetName, 0.0);
        addWealthChild(investments, assetNode);
    }

    if (isAdding) {
        assetNode->value += amount;
    } else {
        assetNode->value = amount;
    }

    if (rate >= 0) {
        assetNode->interestRate = rate;
    }
    
    printf("Asset '%s' updated. New Value: %.2f, Rate: %.1f%%\n", assetName, assetNode->value, assetNode->interestRate);
    finalizeUserUpdates(user);
}

void setWealthNodeValue(UserProfile* user, const char* nodeName, double newValue) {
    if (!user || !user->wealthTreeRoot || !nodeName) return;
    WealthNode* node = findWealthNode(user->wealthTreeRoot, nodeName);
    if (node) node->value = newValue;
}

void updateExpenseCategoryTotal(UserProfile* user, const char* category, double amount) {
    if (!user || !user->wealthTreeRoot || !category) return;
    WealthNode* expensesRoot = findWealthNode(user->wealthTreeRoot, "Expenses"); 
    if (!expensesRoot) return;
    WealthNode* node = findWealthNode(expensesRoot, category);
    if (node) node->value += amount;
}

void registerNewUser(const char* name) {
    if (!name || strlen(name) == 0) return; 
    UserProfile* user = (UserProfile*)malloc(sizeof(UserProfile));
    if (!user) return; 

    strncpy(user->name, name, 49);
    user->name[49] = '\0';
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
    addWealthChild(expenses, createWealthNode("education", 0.0));
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
        printf("  [%s] %s - Rs.%.2f (%s)\n", 
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