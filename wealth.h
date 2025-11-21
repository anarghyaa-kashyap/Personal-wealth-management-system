#ifndef WEALTH_H
#define WEALTH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h> 

typedef enum InvestmentType {
    INV_NONE,
    INV_PROPERTY,
    INV_STOCKS,
    INV_GOLD,
    INV_OTHERS
} InvestmentType;

typedef struct ExpenditureNode {
    char category[50];
    char description[100];
    double amount;
    time_t date;
    InvestmentType investmentType;
    struct ExpenditureNode* next;
} ExpenditureNode;

typedef struct WealthNode {
    char name[50];
    double value;
    double interestRate;
    struct WealthNode* firstChild;
    struct WealthNode* nextSibling;
} WealthNode;

struct UserProfile;

typedef struct UserHeap {
    struct UserProfile** userArray;
    int size;
    int capacity;
} UserHeap;

typedef struct UserProfile {
    char name[50];
    double netWorth;
    WealthNode* wealthTreeRoot;
    ExpenditureNode* expenseListHead;
} UserProfile;

extern UserHeap* g_userHeap;

WealthNode* createWealthNode(const char* name, double value);
void addWealthChild(WealthNode* parent, WealthNode* newChild);
WealthNode* findWealthNode(WealthNode* root, const char* name);

UserHeap* createHeap(int capacity);
void swapUsers(UserHeap* heap, int i, int j);
void heapifyUp(UserHeap* heap, int index);
void heapifyDown(UserHeap* heap, int index);
void heapInsert(UserHeap* heap, UserProfile* user);
UserProfile* getTopWealthUser(UserHeap* heap);
int findUserIndex(UserHeap* heap, UserProfile* user);
void displayHeap(UserHeap* heap); 

double recursiveUpdateAndGetWorth(WealthNode* root); 
double calculateProjectedNetWorth(WealthNode* root, int years);
void logExpenseToList(UserProfile* user, const char* category, const char* desc, double amount, InvestmentType invType);
void manageStock(UserProfile* user, const char* ticker, double amount, double rate, int isAdding);
void manageAsset(UserProfile* user, const char* assetName, double amount, double rate, int isAdding);
void setWealthNodeValue(UserProfile* user, const char* nodeName, double newValue);
void updateExpenseCategoryTotal(UserProfile* user, const char* category, double amount);
void finalizeUserUpdates(UserProfile* user);
void registerNewUser(const char* name);

void printExpenseLog(ExpenditureNode* head);
void printWealthTree(WealthNode* root, int indent);
void freeExpenseList(ExpenditureNode* head);
void freeWealthTree(WealthNode* root);
void freeHeap(UserHeap* heap);


#endif 