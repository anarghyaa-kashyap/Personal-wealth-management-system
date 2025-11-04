#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include "wealth.h"

// ================================================================
// --- (A) HELPER FUNCTIONS FOR USER INPUT ---
// ================================================================

void getStringInput(const char* prompt, char* buffer, int size) {
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = 0;
}

int getIntInput(const char* prompt) {
    char buffer[100];
    int value;
    while (1) {
        getStringInput(prompt, buffer, sizeof(buffer));
        if (sscanf(buffer, "%d", &value) == 1) {
            return value;
        } else {
            printf("Invalid input. Please enter a number.\n");
        }
    }
}

double getDoubleInput(const char* prompt) {
    char buffer[100];
    double value;
    while (1) {
        getStringInput(prompt, buffer, sizeof(buffer));
        if (sscanf(buffer, "%lf", &value) == 1 && value >= 0) {
            return value;
        } else {
            printf("Invalid input. Please enter a non-negative number.\n");
        }
    }
}

int strcicmp(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        if (toupper((unsigned char)*s1) != toupper((unsigned char)*s2)) {
            return *s1 - *s2;
        }
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

const char* getInvestmentNodeName(InvestmentType type) {
    switch (type) {
        case INV_PROPERTY: return "real estate";
        case INV_STOCKS:   return "stock";
        case INV_GOLD:     return "gold";
        case INV_OTHERS:   return "others";
        default:           return NULL;
    }
}


// ================================================================
// --- (B) SUB-MENU HANDLER FUNCTIONS ---
// ================================================================

void handleAddTransaction(UserProfile* user) {
    if (user == NULL) {
        printf("Error: Invalid user profile.\n");
        return;
    }
    char category[50];
    char description[100];
    double amount;
    InvestmentType invType = INV_NONE;
    printf("\n--- Add New Transaction ---\n");
    printf("Select a category:\n");
    printf("  1. Health\n");
    printf("  2. Travel\n");
    printf("  3. Regular\n");
    printf("  4. Investment\n");
    int catChoice = getIntInput("Enter choice (1-4): ");
    switch (catChoice) {
        case 1: strcpy(category, "health"); break;
        case 2: strcpy(category, "travel"); break;
        case 3: strcpy(category, "regular"); break;
        case 4: strcpy(category, "investment"); break;
        default:
            printf("Invalid category choice.\n");
            return;
    }
    if (strcmp(category, "investment") == 0) {
        printf("\nSelect investment type:\n");
        printf("  1. Property\n");
        printf("  2. Stocks\n");
        printf("  3. Gold\n");
        printf("  4. Others\n");
        int choice = getIntInput("Enter type (1-4): ");
        switch (choice) {
            case 1: invType = INV_PROPERTY; break;
            case 2: invType = INV_STOCKS;   break;
            case 3: invType = INV_GOLD;     break;
            case 4: invType = INV_OTHERS;   break;
            default: 
                printf("Invalid choice. Defaulting to 'Others'.\n");
                invType = INV_OTHERS; 
                break;
        }
        getStringInput("Enter description: ", description, 100);
    } else {
        getStringInput("Enter description: ", description, 100);
        invType = INV_NONE;
    }
    if (strlen(description) == 0) {
        printf("Error: Description cannot be empty.\n");
        return;
    }
    amount = getDoubleInput("Enter amount: ");
    if (amount <= 0) {
        printf("Error: Amount must be positive.\n");
        return;
    }
    logExpenseToList(user, category, description, amount, invType);
    if (strcmp(category, "investment") == 0) {
        const char* assetName = getInvestmentNodeName(invType);
        if (assetName != NULL) {
            WealthNode* node = findWealthNode(user->wealthTreeRoot, assetName);
            if (node != NULL) {
                double newValue = node->value + amount;
                updateInvestmentValue(user, assetName, newValue);
                printf("Investment purchase recorded. Asset '%s' updated.\n", assetName);
            } else {
                printf("Warning: Asset node '%s' not found in wealth tree.\n", assetName);
            }
        }
    } else {
        updateExpenseCategoryTotal(user, category, amount);
    }
    finalizeUserUpdates(user);
    printf("Transaction logged successfully. New net worth: $%.2f\n", user->netWorth);
}

void handleAddIncome(UserProfile* user) {
    if (user == NULL) {
        printf("Error: Invalid user profile.\n");
        return;
    }

    printf("\n--- Add Income (Salary, etc.) ---\n");
    double amount = getDoubleInput("Enter amount to add: ");
    if (amount <= 0) {
        printf("Error: Amount must be positive.\n");
        return;
    }

    WealthNode* salaryNode = findWealthNode(user->wealthTreeRoot, "salary");
    if (salaryNode == NULL) {
        printf("Error: 'salary' asset node not found. Cannot add income.\n");
        return;
    }
    
    double newValue = salaryNode->value + amount;
    
    updateInvestmentValue(user, "salary", newValue);
    finalizeUserUpdates(user);
    
    printf("Income added successfully. New net worth: $%.2f\n", user->netWorth);
}

void handleUpdateInvestment(UserProfile* user) {
    if (user == NULL) {
        printf("Error: Invalid user profile.\n");
        return;
    }

    char nodeName[50];
    double value;

    printf("\n--- Update Market Value of Investment ---\n");
    printf("(Use this when your asset value changes, e.g., stocks go up)\n");
    printf("Investment nodes: gold, stock, real estate, others\n");
    getStringInput("Enter investment name to update: ", nodeName, 50);

    if (strlen(nodeName) == 0) {
        printf("Error: Investment name cannot be empty.\n");
        return;
    }
    
    WealthNode* invRoot = findWealthNode(user->wealthTreeRoot, "Investments");
    WealthNode* node = NULL;
    
    if (invRoot) {
        WealthNode* child = invRoot->firstChild;
        while(child) {
            if (strcicmp(child->name, nodeName) == 0) {
                node = child; 
                break;
            }
            child = child->nextSibling;
        }
    }
    
    if (node == NULL) {
        printf("Error: Investment '%s' not found in your wealth tree.\n", nodeName);
        return;
    }

    printf("Current value: $%.2f\n", node->value);
    value = getDoubleInput("Enter new total value: ");

    if (value < 0) {
        printf("Error: Value cannot be negative.\n");
        return;
    }

    updateInvestmentValue(user, node->name, value);
    finalizeUserUpdates(user);

    printf("Investment market value updated successfully!\n");
    printf("New net worth: $%.2f\n", user->netWorth);
}

void handleViewInvestmentPortfolio(UserProfile* user) {
    if (user == NULL) {
        printf("Error: Invalid user profile.\n");
        return;
    }

    double portfolioTotals[5] = {0.0, 0.0, 0.0, 0.0, 0.0};

    ExpenditureNode* current = user->expenseListHead;
    while (current != NULL) {
        if (current->investmentType != INV_NONE && current->investmentType < 5) {
            portfolioTotals[current->investmentType] += current->amount;
        }
        current = current->next;
    }

    printf("\n--- %s's Investment Portfolio (by Total Cost) ---\n", user->name);
    printf("  Property:    $%.2f\n", portfolioTotals[INV_PROPERTY]);
    printf("  Stocks:      $%.2f\n", portfolioTotals[INV_STOCKS]);
    printf("  Gold:        $%.2f\n", portfolioTotals[INV_GOLD]);
    printf("  Others:      $%.2f\n", portfolioTotals[INV_OTHERS]);
    printf("--------------------------------------------------\n");

    double totalInvested = portfolioTotals[INV_PROPERTY] +
                           portfolioTotals[INV_STOCKS] +
                           portfolioTotals[INV_GOLD] +
                           portfolioTotals[INV_OTHERS];

    printf("  Total Invested (Cost): $%.2f\n", totalInvested);
}

void handleRegister() {
    char name[50];
    char gender[10];

    printf("\n--- Register New User ---\n");
    getStringInput("Enter name: ", name, 50);

    if (strlen(name) == 0) {
        printf("Error: Name cannot be empty.\n");
        return;
    }

    if (strcicmp(name, "admin") == 0) {
         printf("Error: 'admin' is a reserved name.\n");
         return;
    }

    if (g_userHeap != NULL) {
        for (int i = 0; i < g_userHeap->size; i++) {
            if (strcicmp(g_userHeap->userArray[i]->name, name) == 0) {
                printf("Error: A user with that name (or similar case) already exists.\n");
                return;
            }
        }
    }

    getStringInput("Enter gender: ", gender, 10);

    if (strlen(gender) == 0) {
        printf("Error: Gender cannot be empty.\n");
        return;
    }

    registerNewUser(name, gender);
    printf("User '%s' registered successfully!\n", name);
}

UserProfile* handleLogin() {
    if (g_userHeap == NULL || g_userHeap->size == 0) {
        printf("\nError: No users registered in the system.\n");
        return NULL;
    }

    char name[50];
    printf("\n--- User Login ---\n");
    getStringInput("Enter name: ", name, 50);

    if (strlen(name) == 0) {
        printf("Error: Name cannot be empty.\n");
        return NULL;
    }
    
    if (strcicmp(name, "admin") == 0) {
        printf("Admin login successful. Welcome.\n");
        static UserProfile adminUser; 
        strcpy(adminUser.name, "admin");
        adminUser.netWorth = 0; 
        return &adminUser;
    }

    for (int i = 0; i < g_userHeap->size; i++) {
        if (g_userHeap->userArray[i] != NULL &&
            strcicmp(g_userHeap->userArray[i]->name, name) == 0) {
            printf("Login successful. Welcome, %s!\n", g_userHeap->userArray[i]->name);
            return g_userHeap->userArray[i];
        }
    }

    printf("Error: User '%s' not found.\n", name);
    return NULL;
}

// ================================================================
// --- (C) THE LOGGED-IN USER MENUS ---
// ================================================================

void adminMenu() {
    int choice = 0;
    while (choice != 3) {
        printf("\n--- Admin Menu ---\n");
        printf("1. View Top Wealthiest User\n");
        printf("2. Display All Users\n");
        printf("3. Logout\n");
        choice = getIntInput("Enter your choice: ");

        switch (choice) {
            case 1: {
                UserProfile* topUser = getTopWealthUser(g_userHeap);
                if (topUser == NULL) {
                    printf("\nNo users in the system yet.\n");
                } else {
                    printf("\n--- Top Wealthiest User ---\n");
                    printf("Name:      %s\n", topUser->name);
                    printf("Net Worth: $%.2f\n", topUser->netWorth);
                }
                break;
            }
            case 2:
                displayHeap(g_userHeap);
                break;
            case 3:
                printf("Logging out admin...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}


void loggedInMenu(UserProfile* user) {
    if (user == NULL) {
        printf("Error: Invalid user session.\n");
        return;
    }

    int choice = 0;
    while (choice != 7) { // <-- Logout is now 7
        printf("\n--- Welcome, %s (Net Worth: $%.2f) ---\n", user->name, user->netWorth);
        printf("1. Add Transaction (Expense or Investment Purchase)\n");
        printf("2. Add Income (Salary, etc.)\n"); 
        printf("3. Update Investment Market Value\n");
        printf("4. View My Transaction Log\n");
        printf("5. View My Wealth Tree (Category Totals)\n");
        printf("6. View Investment Portfolio (by Cost)\n");
        printf("7. Logout\n"); // <-- Renumbered
        choice = getIntInput("Enter your choice: ");

        switch (choice) {
            case 1:
                handleAddTransaction(user);
                break;
            case 2:
                handleAddIncome(user);
                break;
            case 3:
                handleUpdateInvestment(user);
                break;
            case 4:
                printf("\n--- %s's Transaction Log ---\n", user->name);
                printExpenseLog(user->expenseListHead);
                break;
            case 5:
                printf("\n--- %s's Wealth Tree ---\n", user->name);
                printWealthTree(user->wealthTreeRoot, 0);
                break;
            case 6:
                handleViewInvestmentPortfolio(user);
                break;
            case 7: // <-- Renumbered
                printf("Logging out...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}


// ================================================================
// --- (D) THE MAIN APPLICATION LOOP ---
// ================================================================

int main() {
    int choice = 0;

    g_userHeap = createHeap(100);
    if (g_userHeap == NULL) {
        printf("Error: Failed to initialize the system.\n");
        return 1;
    }

    printf("Welcome to the Personal Wealth Management System!\n");

    while (choice != 3) {
        printf("\n--- Main Menu ---\n");
        printf("1. Register New User\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        choice = getIntInput("Enter your choice: ");

        switch (choice) {
            case 1:
                handleRegister();
                break;
            case 2: {
                UserProfile* user = handleLogin();
                if (user != NULL) {
                    if (strcicmp(user->name, "admin") == 0) {
                        adminMenu();
                    } else {
                        loggedInMenu(user);
                    }
                }
                break;
            }
            case 3:
                printf("Exiting program. Cleaning up memory...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    freeHeap(g_userHeap);

    printf("Cleanup complete. Goodbye!\n");
    return 0;
}