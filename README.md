# Personal Wealth Management System

A console-based application written in C. This program allows multiple users to register, log in, and manage their finances by tracking both investments and daily expenses. The system dynamically ranks all users by their calculated net worth.

## Key Features

* **Multi-User System:** Users can register and log in to access their own private financial data.
* **Wealth Tracking:** Organizes all assets and expenses into a general tree structure, including categories like "Investments" (stocks, gold, property) and "Expenses" (health, travel).
* **Transaction Logging:** All individual transactions (expenses and investment purchases) are logged to a linear linked list for detailed record-keeping.
* **Dynamic Net Worth Calculation:** Automatically recalculates a user's total net worth after every transaction, correctly summing assets and subtracting expenses.
* **User Priority:** All users are stored in a **Max-Heap**, allowing the system to find the "Top Wealthiest User" in constant $O(1)$ time.
* **Portfolio Reporting:** Users can view a portfolio summary showing the *total cost* of their investments, which is separate from their *current market value*.

## Data Structures Used

This project was built to demonstrate the practical application of core data structures and how they can be integrated.

1.  **Max-Heap (Non-Linear):**
    * **Purpose:** Used as a **Priority Queue** to store all `UserProfile` structs.
    * **Why:** The user's `netWorth` is the priority key. This allows the program to find the wealthiest user in $O(1)$ time (by peeking at the root) and maintain a sorted-by-wealth structure efficiently, with $O(\log n)$ insertions and updates.

2.  **General Tree (Non-Linear):**
    * **Purpose:** Each user has their own tree to **organize wealth categories**. It is implemented using a "first child, next sibling" representation.
    * **Why:** A tree is the most natural way to represent hierarchical data. The root is the user, with main branches like "Investments" and "Expenses," which in turn have their own children ("stock," "gold," "health," etc.). This allows for clean, recursive net worth calculation.

3.  **Singly Linked List (Linear):**
    * **Purpose:** Each user has their own linked list to **log all individual transactions**.
    * **Why:** We needed a dynamic structure to store an unknown number of transactions. A linked list provides efficient $O(1)$ insertion for new expenses and serves as the "raw data" log that feeds the other data structures.

```bash
gcc main.c wealth_functions.c -o wealth_manager
