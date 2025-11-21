#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include "wealth.h"

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

void handleAddTransaction(UserProfile* user) {
    if (user == NULL) return;

    char category[50];
    char description[100];
    double amount;
    double interestRate = 0.0;
    InvestmentType invType = INV_NONE;

    printf("\n--- Add New Transaction ---\n");
    printf("Select a category:\n");
    printf("  1. Health\n");
    printf("  2. Travel\n");
    printf("  3. Education\n");
    printf("  4. Regular\n");
    printf("  5. Investment\n");
    int catChoice = getIntInput("Enter choice (1-5): ");

    switch (catChoice) {
        case 1: strcpy(category, "health"); break;
        case 2: strcpy(category, "travel"); break;
        case 3: strcpy(category, "education"); break;
        case 4: strcpy(category, "regular"); break;
        case 5: strcpy(category, "investment"); break;
        default: printf("Invalid category choice.\n"); return;
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
            default: invType = INV_OTHERS; break;
        }
        if (invType == INV_STOCKS) {
            getStringInput("Enter Stock Name/Ticker (e.g., AAPL): ", description, 100);
        } else {
            getStringInput("Enter description: ", description, 100);
        }
        printf("Enter expected annual interest rate (%%) [0 for none]: ");
        interestRate = getDoubleInput("");
    } else {
        getStringInput("Enter description: ", description, 100);
        invType = INV_NONE;
    }

    if (strlen(description) == 0) { printf("Error: Description cannot be empty.\n"); return; }

    amount = getDoubleInput("Enter amount: ");
    if (amount <= 0) { printf("Error: Amount must be positive.\n"); return; }

    logExpenseToList(user, category, description, amount, invType);

    if (strcmp(category, "investment") == 0) {
        if (invType == INV_STOCKS) {
            manageStock(user, description, amount, interestRate, 1); 
        } else {
            const char* assetName = getInvestmentNodeName(invType);
            manageAsset(user, assetName, amount, interestRate, 1);
        }
    } else {
        updateExpenseCategoryTotal(user, category, amount);
        finalizeUserUpdates(user);
    }
    printf("Transaction logged successfully. New net worth: Rs.%.2f\n", user->netWorth);
}

void handleAddIncome(UserProfile* user) {
    if (user == NULL) return;
    printf("\n--- Add Income (Salary, etc.) ---\n");
    double amount = getDoubleInput("Enter amount to add: ");
    if (amount <= 0) { printf("Error: Amount must be positive.\n"); return; }

    WealthNode* salaryNode = findWealthNode(user->wealthTreeRoot, "salary");
    if (salaryNode == NULL) { printf("Error: 'salary' node not found.\n"); return; }
    
    setWealthNodeValue(user, "salary", salaryNode->value + amount); 
    finalizeUserUpdates(user);
    printf("Income added successfully. New net worth: Rs.%.2f\n", user->netWorth);
}

void handleUpdateInvestment(UserProfile* user) {
    if (user == NULL) return;

    printf("\n--- Update Market Value & Interest Rate ---\n");
    printf("1. Update Specific Stock\n");
    printf("2. Update General Asset (Gold, Real Estate, etc.)\n");
    int choice = getIntInput("Enter choice: ");

    char nodeName[50];
    double value, rate;

    if (choice == 1) {
        getStringInput("Enter Stock Name/Ticker: ", nodeName, 50);
        
        printf("Current Market Value: ");
        value = getDoubleInput("");
        printf("Current Interest Rate (%%): ");
        rate = getDoubleInput("");
        
        manageStock(user, nodeName, value, rate, 0);

    } else if (choice == 2) {
        printf("Asset nodes: gold, real estate, others\n");
        getStringInput("Enter asset name: ", nodeName, 50);
        printf("Current Market Value: ");
        value = getDoubleInput("");
        printf("Current Interest Rate (%%): ");
        rate = getDoubleInput("");

        manageAsset(user, nodeName, value, rate, 0);
    } else {
        printf("Invalid choice.\n");
    }
}

void handleProjectedWealth(UserProfile* user) {
    if (user == NULL) return;
    
    printf("\n--- Projected Net Worth Calculator ---\n");
    printf("This calculation assumes compound interest on assets with set rates.\n");
    int years = getIntInput("Enter number of years to project: ");
    
    if (years < 0) { printf("Years cannot be negative.\n"); return; }

    double projected = calculateProjectedNetWorth(user->wealthTreeRoot, years);
    
    printf("\nCurrent Net Worth:   Rs.%.2f\n", user->netWorth);
    printf("Projected (%d yrs):  Rs.%.2f\n", years, projected);
    printf("Estimated Growth:    Rs.%.2f\n", projected - user->netWorth);
}

double getCostBasis(UserProfile* user, const char* name) {
    double total = 0.0;
    ExpenditureNode* current = user->expenseListHead;
    while (current != NULL) {
        if (strcicmp(current->description, name) == 0) {
            total += current->amount;
        }
        current = current->next;
    }
    return total;
}

void handleViewInvestmentPortfolio(UserProfile* user) {
    if (user == NULL) {
        printf("Error: Invalid user profile.\n");
        return;
    }

    printf("\n==========================================================================\n");
    printf(" %-20s | %-12s | %-12s | %-10s\n", "Asset", "Cost Basis", "Market Value", "Gain/Loss");
    printf("==========================================================================\n");

    double totalCost = 0.0;
    double totalValue = 0.0;

    WealthNode* invRoot = findWealthNode(user->wealthTreeRoot, "Investments");
    if (invRoot) {
        WealthNode* stockCat = findWealthNode(invRoot, "stock");
        if (stockCat) {
            WealthNode* child = stockCat->firstChild;
            if (child == NULL) {
            } else {
                printf(" [STOCKS]\n");
                while (child != NULL) {
                    double cost = getCostBasis(user, child->name);
                    double market = child->value;
                    double diff = market - cost;
                    
                    printf(" %-20s | Rs.%-9.2f | Rs.%-9.2f | Rs.%-8.2f\n", 
                           child->name, cost, market, diff);
                    
                    totalCost += cost;
                    totalValue += market;
                    child = child->nextSibling;
                }
            }
        }

        const char* generics[] = {"gold", "real estate", "others"};
        printf(" [GENERAL]\n");
        for (int i = 0; i < 3; i++) {
            WealthNode* node = findWealthNode(invRoot, generics[i]);
            if (node) {
                double cost = 0.0;
                ExpenditureNode* curr = user->expenseListHead;
                InvestmentType targetType = INV_NONE;
                if (strcmp(generics[i], "gold")==0) targetType = INV_GOLD;
                if (strcmp(generics[i], "real estate")==0) targetType = INV_PROPERTY;
                if (strcmp(generics[i], "others")==0) targetType = INV_OTHERS;

                while(curr) {
                    if (curr->investmentType == targetType) cost += curr->amount;
                    curr = curr->next;
                }

                double market = node->value;
                double diff = market - cost;
                 printf(" %-20s | Rs.%-9.2f | Rs.%-9.2f | Rs.%-8.2f\n", 
                           generics[i], cost, market, diff);
                
                totalCost += cost;
                totalValue += market;
            }
        }
    }

    printf("--------------------------------------------------------------------------\n");
    printf(" %-20s | Rs.%-9.2f | Rs.%-9.2f | Rs.%-8.2f\n", 
           "TOTAL", totalCost, totalValue, totalValue - totalCost);
    printf("==========================================================================\n");
}

void handleRegister() {
    char name[50];
    printf("\n--- Register New User ---\n");
    getStringInput("Enter name: ", name, 50);
    if (strlen(name) == 0) { printf("Error: Name cannot be empty.\n"); return; }
    if (strcicmp(name, "admin") == 0) { printf("Error: 'admin' is a reserved name.\n"); return; }
    if (g_userHeap != NULL) {
        for (int i = 0; i < g_userHeap->size; i++) {
            if (strcicmp(g_userHeap->userArray[i]->name, name) == 0) {
                printf("Error: User already exists.\n"); return;
            }
        }
    }
    registerNewUser(name);
    printf("User '%s' registered successfully!\n", name);
}

UserProfile* handleLogin() {
    if (g_userHeap == NULL || g_userHeap->size == 0) { printf("\nError: No users.\n"); return NULL; }
    char name[50];
    printf("\n--- User Login ---\n");
    getStringInput("Enter name: ", name, 50);
    if (strlen(name) == 0) return NULL;
    
    if (strcicmp(name, "admin") == 0) {
        static UserProfile adminUser; 
        strcpy(adminUser.name, "admin");
        adminUser.netWorth = 0; 
        return &adminUser;
    }
    for (int i = 0; i < g_userHeap->size; i++) {
        if (g_userHeap->userArray[i] != NULL && strcicmp(g_userHeap->userArray[i]->name, name) == 0) {
            printf("Login successful. Welcome, %s!\n", g_userHeap->userArray[i]->name);
            return g_userHeap->userArray[i];
        }
    }
    printf("Error: User not found.\n");
    return NULL;
}

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
                if (topUser) printf("\nTop User: %s (Rs.%.2f)\n", topUser->name, topUser->netWorth);
                else printf("\nNo users.\n");
                break;
            }
            case 2: displayHeap(g_userHeap); break;
            case 3: printf("Logging out admin...\n"); break;
            default: printf("Invalid choice.\n");
        }
    }
}

void loggedInMenu(UserProfile* user) {
    if (user == NULL) return;
    int choice = 0;
    while (choice != 9) { 
        printf("\n--- Welcome, %s (Net Worth: Rs.%.2f) ---\n", user->name, user->netWorth);
        printf("1. Add Transaction\n");
        printf("2. Add Income\n"); 
        printf("3. Update Investment Market Value\n");
        printf("4. View Transaction Log\n");
        printf("5. View Wealth Tree\n");
        printf("6. View Investment Portfolio\n");
        printf("7. View Projected Net Worth (Prediction)\n"); 
        printf("8. Logout\n");
        choice = getIntInput("Enter your choice: ");
        switch (choice) {
            case 1: handleAddTransaction(user); break;
            case 2: handleAddIncome(user); break;
            case 3: handleUpdateInvestment(user); break;
            case 4: printExpenseLog(user->expenseListHead); break;
            case 5: 
                printf("\n--- %s's Wealth Tree ---\n", user->name);
                printWealthTree(user->wealthTreeRoot, 0); 
                break;
            case 6: handleViewInvestmentPortfolio(user); break;
            case 7: handleProjectedWealth(user); break; 
            case 8: printf("Logging out...\n"); return; 
            default: printf("Invalid choice.\n");
        }
    }
}

int main() {
    g_userHeap = createHeap(100);
    if (!g_userHeap) return 1;
    printf("Welcome to the Personal Wealth Management System!\n");
    int choice = 0;
    while (choice != 3) {
        printf("\n--- Main Menu ---\n");
        printf("1. Register New User\n2. Login\n3. Exit\n");
        choice = getIntInput("Enter your choice: ");
        switch (choice) {
            case 1: handleRegister(); break;
            case 2: {
                UserProfile* user = handleLogin();
                if (user) {
                    if (strcicmp(user->name, "admin") == 0) adminMenu();
                    else loggedInMenu(user);
                }
                break;
            }
            case 3: printf("Exiting...\n"); break;
            default: printf("Invalid choice.\n");
        }
    }
    freeHeap(g_userHeap);
    return 0;

}

