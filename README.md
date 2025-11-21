# Personal Wealth Management System

A console-based application written in C. This program allows multiple users to register, log in, and manage their finances by tracking both investments and daily expenses. The system dynamically ranks all users by their calculated net worth.

## Key Features

* **Multi-User & Admin System:** Supports multiple concurrent user profiles with secure login. Includes a special **Admin Mode** (login as "admin") to view system-wide statistics and identify the top-ranked users.
* **Comprehensive Wealth Tracking:** Organizes finances into a hierarchy of **Income** (salary), **Expenses** (health, travel, etc.), and **Investments**.
* **Specific Asset Management:** Users can track **individual stock tickers** (e.g., AAPL, TSLA) separately from general assets like Gold or Real Estate. Each asset supports a custom **Annual Interest Rate**.
* **Profit & Loss Portfolio Analysis:** A detailed reporting feature that compares the **Cost Basis** (from transaction logs) against the **Current Market Value** (from the wealth tree) to display the unrealized Gain/Loss for every asset.
* **Projected Wealth Calculator:** Uses the assigned interest rates to calculate and display a user's **Projected Net Worth** for a future number of years using compound interest logic.
* **Transaction Logging:** Every financial action is recorded in a linear linked list, creating a permanent history log of all expenses and purchases.
* **Dynamic Net Worth & Ranking:** The system automatically recalculates Net Worth `(Assets + Income - Expenses)` after every update and uses a **Max-Heap** to instantly re-rank users by wealth in real-time.

## Data Structures Used

This project was built to demonstrate the practical application of core data structures and how they can be integrated.

1.  **Max-Heap (Non-Linear):**
    * **Purpose:** Used as a **Priority Queue** to store all `UserProfile` structs.
    * **Why:** The user's `netWorth` is the priority key. This allows the program to find the wealthiest user in $O(1)$ time (by peeking at the root) and maintain a sorted-by-wealth structure efficiently, with $O(\log n)$ insertions and updates.

2.  **General Tree (Non-Linear):**
    * **Purpose:** Each user has their own tree to **organize wealth categories**. It is implemented using a "first child, next sibling" representation.
    * **Why:** A tree is used to represent the hierarchical data. The root is the user, with main branches like "Investments" and "Expenses," which in turn have their own children ("stock," "gold," "health," etc.). This allows for clean, recursive net worth calculation.

3.  **Singly Linked List (Linear):**
    * **Purpose:** Each user has their own linked list to **log all individual transactions**.
    * **Why:** We needed a dynamic structure to store an unknown number of transactions. A linked list provides efficient $O(1)$ insertion for new expenses and serves as the "raw data" log that feeds the other data structures.

