#include "wealth.h"

// ================================================================
// PERSON 3: HEAP QUERIES & NET WORTH CALCULATIONS
// ================================================================

/**
 * @brief Returns the user with the highest net worth (root of max-heap).
 * @param heap Pointer to the UserHeap
 * @return Pointer to the top user, or NULL if heap is empty/invalid
 * 
 * Time Complexity: O(1)
 * Error Handling: Checks for NULL heap, NULL array, and empty heap
 */
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


/**
 * @brief Finds the index of a specific user in the heap array.
 * @param heap Pointer to the UserHeap
 * @param user Pointer to the UserProfile to find
 * @return Index of the user (0 to size-1), or -1 if not found
 * 
 * Time Complexity: O(n) where n is heap size
 * Error Handling: Checks for NULL parameters and validates array bounds
 */
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


/**
 * @brief Recursively calculates the total net worth from a wealth tree.
 * @param root Pointer to the root WealthNode
 * @return Total sum of all node values in the tree
 * 
 * Time Complexity: O(n) where n is number of nodes
 * Error Handling: Handles NULL root gracefully, validates node structure
 * 
 * Algorithm: Post-order traversal (children first, then current node)
 * - Process first child and all its siblings
 * - Add current node's value
 */
double calculateNetWorth(WealthNode* root) {
    // Base case: NULL node contributes 0
    if (root == NULL) {
        return 0.0;
    }
    
    // Error handling: Check for negative values (shouldn't happen, but be safe)
    if (root->value < 0) {
        fprintf(stderr, "Warning [calculateNetWorth]: Negative value %.2f in node '%s'\n", 
                root->value, root->name);
    }
    
    double totalWorth = 0.0;
    
    // Add current node's value
    totalWorth += root->value;
    
    // Recursively calculate worth of all children
    // Using first child + next sibling representation
    WealthNode* child = root->firstChild;
    while (child != NULL) {
        totalWorth += calculateNetWorth(child);
        child = child->nextSibling;
    }
    
    return totalWorth;
}


/**
 * @brief Recalculates user's net worth and fixes their heap position.
 * @param user Pointer to the UserProfile to update
 * 
 * Time Complexity: O(log n) for heap operations + O(m) for tree traversal
 *                  where n = heap size, m = number of wealth nodes
 * Error Handling: Validates all pointers and heap state
 * 
 * This function should be called after ANY modification to the wealth tree.
 */
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


/**
 * @brief Frees all memory associated with the heap and its users.
 * @param heap Pointer to the UserHeap to free
 * 
 * Time Complexity: O(n * m) where n = users, m = avg nodes per user
 * Error Handling: Checks for NULL pointers, handles partial cleanup
 * 
 * CRITICAL: This frees the entire user system including:
 * - All UserProfile structs
 * - All WealthNode trees
 * - All ExpenditureNode lists
 * - The heap array itself
 * - The heap struct
 */
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