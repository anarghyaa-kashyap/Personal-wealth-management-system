#ifndef WEALTH_H
#define WEALTH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ================================================================
// --- (A) STRUCT DEFINITIONS ---
// ================================================================

/**
 * @enum InvestmentType
 * @brief Enumeration for specific investment categories.
 */
typedef enum InvestmentType {
    INV_NONE,
    INV_PROPERTY,
    INV_STOCKS,
    INV_GOLD,
    INV_OTHERS
} InvestmentType;

/**
 * @struct ExpenditureNode
 * @brief Represents a single expense transaction (Linear Data Structure).
 */
typedef struct ExpenditureNode {
    char category[50];
    char description[100];
    double amount;
    time_t date;
    InvestmentType investmentType;
    struct ExpenditureNode* next;
} ExpenditureNode;

/**
 * @struct WealthNode
 * @brief Represents a single node in the wealth tree (Non-Linear Data Structure).
 */
typedef struct WealthNode {
    char name[50];
    double value;
    struct WealthNode* firstChild;
    struct WealthNode* nextSibling;
} WealthNode;

// Forward declaration of UserProfile for the heap
struct UserProfile;

/**
 * @struct UserHeap
 * @brief Represents the Max-Heap (Priority Queue) - Non-Linear Data Structure.
 */
typedef struct UserHeap {
    struct UserProfile** userArray;
    int size;
    int capacity;
} UserHeap;

/**
 * @struct UserProfile
 * @brief The main struct that ties all data together.
 */
typedef struct UserProfile {
    char name[50];
    char gender[10];
    double netWorth;

    WealthNode* wealthTreeRoot;
    ExpenditureNode* expenseListHead;
} UserProfile;


// ================================================================
// --- (B) GLOBAL VARIABLE DECLARATION ---
// ================================================================

extern UserHeap* g_userHeap;


// ================================================================
// --- (C) FUNCTION PROTOTYPES (DECLARATIONS) ---
// ================================================================

// --- 1. Wealth Tree Functions ---
WealthNode* createWealthNode(const char* name, double value);
void addWealthChild(WealthNode* parent, WealthNode* newChild);
WealthNode* findWealthNode(WealthNode* root, const char* name);

// --- 2. Max-Heap (Priority Queue) Functions ---
UserHeap* createHeap(int capacity);
void swapUsers(UserHeap* heap, int i, int j);
void heapifyUp(UserHeap* heap, int index);
void heapifyDown(UserHeap* heap, int index);
void heapInsert(UserHeap* heap, UserProfile* user);
UserProfile* getTopWealthUser(UserHeap* heap);
int findUserIndex(UserHeap* heap, UserProfile* user);

// --- 3. Core Integration Functions ---
double calculateNetWorth(WealthNode* root);

/**
 * @brief SIMPLIFIED: Only logs the expense to the linear list.
 * Does NOT update tree or heap. Use for clean separation of concerns.
 */
void logExpenseToList(UserProfile* user, const char* category, const char* desc, 
                      double amount, InvestmentType invType);

/**
 * @brief Updates a specific investment node's value in the tree.
 * Also recalculates net worth and fixes heap position.
 */
void updateInvestmentValue(UserProfile* user, const char* nodeName, double newValue);

/**
 * @brief Updates a non-investment category total (e.g., health, travel).
 * Adds the amount to the existing category value.
 */
void updateExpenseCategoryTotal(UserProfile* user, const char* category, double amount);

/**
 * @brief Recalculates user's net worth and fixes their heap position.
 * Call this after any tree modifications.
 */
void finalizeUserUpdates(UserProfile* user);

void registerNewUser(const char* name, const char* gender);

// --- 4. Print & Cleanup Functions ---
void printExpenseLog(ExpenditureNode* head);
void printWealthTree(WealthNode* root, int indent);
void freeExpenseList(ExpenditureNode* head);
void freeWealthTree(WealthNode* root);
void freeHeap(UserHeap* heap);

#endif // WEALTH_H