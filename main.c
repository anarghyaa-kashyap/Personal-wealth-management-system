#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/**
 * @brief Converts an InvestmentType enum to its matching WealthNode name.
 */
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

/**
 * @brief Handles the 'Add Expense' workflow.
 * FIXED: No more double-counting. Clear separation of concerns.
 */
void handleAddExpense(UserProfile* user) {
    if (user == NULL) {
        printf("Error: Invalid user profile.\n");
        return;
    }

    char category[50];
    char description[100];
    double amount;
    InvestmentType invType = INV_NONE;

    printf("\n--- Add New Transaction ---\n");
    printf("Categories: health, travel, regular, investment\n");
    getStringInput("Enter category: ", category, 50);

    if (strlen(category) == 0) {
        printf("Error: Category cannot be empty.\n");
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

        getStringInput("Enter description (e.g., '10 shares AAPL'): ", description, 100);
    } else {
        getStringInput("Enter description (e.g., 'Groceries'): ", description, 100);
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

    // STEP 1: Log the transaction to the expense list (LINEAR structure)
    logExpenseToList(user, category, description, amount, invType);

    // STEP 2: Update the wealth tree (NON-LINEAR structure)
    if (strcmp(category, "investment") == 0) {
        // For investments, add to the asset's current value
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
        // For regular expenses, add to category total
        updateExpenseCategoryTotal(user, category, amount);
    }

    // STEP 3: Recalculate net worth and fix heap position
    finalizeUserUpdates(user);

    printf("Transaction logged successfully. New net worth: ₹%.2f\n", user->netWorth);
}

/**
 * @brief Handles the 'Update Investment' workflow.
 * Use this to update market value (not for purchases).
 */
void handleUpdateInvestment(UserProfile* user) {
    if (user == NULL) {
        printf("Error: Invalid user profile.\n");
        return;
    }

    char nodeName[50];
    double value;

    printf("\n--- Update Market Value of Investment ---\n");
    printf("(Use this when your asset value changes due to market, e.g., stocks go up)\n");
    printf("Investment nodes: gold, stock, real estate, others, cash\n");
    getStringInput("Enter investment name to update: ", nodeName, 50);

    if (strlen(nodeName) == 0) {
        printf("Error: Investment name cannot be empty.\n");
        return;
    }

    // Check if node exists
    WealthNode* node = findWealthNode(user->wealthTreeRoot, nodeName);
    if (node == NULL) {
        printf("Error: Investment '%s' not found in your wealth tree.\n", nodeName);
        return;
    }

    printf("Current value: ₹%.2f\n", node->value);
    value = getDoubleInput("Enter new total value: ");

    if (value < 0) {
        printf("Error: Value cannot be negative.\n");
        return;
    }

    updateInvestmentValue(user, nodeName, value);
    finalizeUserUpdates(user);

    printf("Investment market value updated successfully!\n");
    printf("New net worth: ₹%.2f\n", user->netWorth);
}

/**
 * @brief Displays a summary of the user's portfolio based on total cost.
 */
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
    printf("  Property:    ₹%.2f\n", portfolioTotals[INV_PROPERTY]);
    printf("  Stocks:      ₹%.2f\n", portfolioTotals[INV_STOCKS]);
    printf("  Gold:        ₹%.2f\n", portfolioTotals[INV_GOLD]);
    printf("  Others:      ₹%.2f\n", portfolioTotals[INV_OTHERS]);
    printf("--------------------------------------------------\n");

    double totalInvested = portfolioTotals[INV_PROPERTY] +
                           portfolioTotals[INV_STOCKS] +
                           portfolioTotals[INV_GOLD] +
                           portfolioTotals[INV_OTHERS];

    printf("  Total Invested (Cost): ₹%.2f\n", totalInvested);
}

/**
 * @brief Handles the 'Register New User' workflow.
 */
void handleRegister() {
    char name[50];
    char gender[10];

    printf("\n--- Register New User ---\n");
    getStringInput("Enter name: ", name, 50);

    if (strlen(name) == 0) {
        printf("Error: Name cannot be empty.\n");
        return;
    }

    // Check if user already exists
    if (g_userHeap != NULL) {
        for (int i = 0; i < g_userHeap->size; i++) {
            if (strcmp(g_userHeap->userArray[i]->name, name) == 0) {
                printf("Error: User '%s' already exists.\n", name);
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

/**
 * @brief Handles the 'Login' workflow.
 */
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

    for (int i = 0; i < g_userHeap->size; i++) {
        if (g_userHeap->userArray[i] != NULL &&
            strcmp(g_userHeap->userArray[i]->name, name) == 0) {
            printf("Login successful. Welcome, %s!\n", name);
            return g_userHeap->userArray[i];
        }
    }

    printf("Error: User '%s' not found.\n", name);
    return NULL;
}

// ================================================================
// --- (C) THE LOGGED-IN USER MENU ---
// ================================================================

void loggedInMenu(UserProfile* user) {
    if (user == NULL) {
        printf("Error: Invalid user session.\n");
        return;
    }

    int choice = 0;
    while (choice != 7) {
        printf("\n--- Welcome, %s (Net Worth: ₹%.2f) ---\n", user->name, user->netWorth);
        printf("1. Add Transaction (Expense or Investment Purchase)\n");
        printf("2. Update Investment Market Value\n");
        printf("3. View My Transaction Log\n");
        printf("4. View My Wealth Tree (Category Totals)\n");
        printf("5. View My Net Worth\n");
        printf("6. View Investment Portfolio (by Cost)\n");
        printf("7. Logout\n");
        choice = getIntInput("Enter your choice: ");

        switch (choice) {
            case 1:
                handleAddExpense(user);
                break;
            case 2:
                handleUpdateInvestment(user);
                break;
            case 3:
                printf("\n--- %s's Transaction Log ---\n", user->name);
                printExpenseLog(user->expenseListHead);
                break;
            case 4:
                printf("\n--- %s's Wealth Tree ---\n", user->name);
                printWealthTree(user->wealthTreeRoot, 0);
                break;
            case 5:
                printf("\nYour current calculated net worth is: ₹%.2f\n", user->netWorth);
                break;
            case 6:
                handleViewInvestmentPortfolio(user);
                break;
            case 7:
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

    // Initialize the global heap
    g_userHeap = createHeap(100);
    if (g_userHeap == NULL) {
        printf("Error: Failed to initialize the system.\n");
        return 1;
    }

    printf("Welcome to the Personal Wealth Management System!\n");

    while (choice != 5) {
        printf("\n--- Main Menu ---\n");
        printf("1. Register New User\n");
        printf("2. Login\n");
        printf("3. View Top Wealthiest User\n");
        printf("4. Display All Users (unsorted heap)\n");
        printf("5. Exit\n");
        choice = getIntInput("Enter your choice: ");

        switch (choice) {
            case 1:
                handleRegister();
                break;
            case 2: {
                UserProfile* user = handleLogin();
                if (user != NULL) {
                    loggedInMenu(user);
                }
                break;
            }
            case 3: {
                UserProfile* topUser = getTopWealthUser(g_userHeap);
                if (topUser == NULL) {
                    printf("\nNo users in the system yet.\n");
                } else {
                    printf("\n--- Top Wealthiest User ---\n");
                    printf("Name:      %s\n", topUser->name);
                    printf("Net Worth: ₹%.2f\n", topUser->netWorth);
                }
                break;
            }
            case 4:
                displayHeap(g_userHeap);
                break;
            case 5:
                printf("Exiting program. Cleaning up memory...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    // Clean up all allocated memory
    freeHeap(g_userHeap);

    printf("Cleanup complete. Goodbye!\n");
    return 0;
}