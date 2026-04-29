//Name: Nanthapat Nanthapat
//Student ID: 68070503432
/*
    Order Processing Module
    - Manages the workflow of customer orders using a dynamic FIFO Linked List Queue.
    - Handles loading, enqueueing, dequeueing, and saving orders via CSV file I/O.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "orders.h" 

// ==========================================
// 1. CORE QUEUE OPERATIONS
// ==========================================

// Create a new Order node dynamically
Order* createOrder(int orderID, int customerID, int productID, int quantity, const char* shippingType) {
    Order* newOrder = (Order*)malloc(sizeof(Order));
    if (!newOrder) {
        printf("Error: Memory allocation failed!\n");
        return NULL;
    }
    
    newOrder->orderID = orderID;
    newOrder->customerID = customerID;
    newOrder->productID = productID;
    newOrder->quantity = quantity;
    
    strncpy(newOrder->shippingType, shippingType, sizeof(newOrder->shippingType) - 1);
    newOrder->shippingType[sizeof(newOrder->shippingType) - 1] = '\0';
    
    strcpy(newOrder->status, "Pending");
    newOrder->next = NULL;
    
    return newOrder;
}

// Add an order to the REAR of the queue (FIFO logic)
void enqueueOrder(Order **front, Order **rear, Order *newOrder) {
    if (newOrder == NULL) return;

    if (*rear == NULL) {
        *front = *rear = newOrder;
        return;
    }
    
    (*rear)->next = newOrder;
    *rear = newOrder;
}

// Remove and return an order from the FRONT of the queue
Order* dequeueOrder(Order **front, Order **rear) {
    if (*front == NULL) {
        return NULL;
    }

    Order* temp = *front;
    *front = (*front)->next;

    if (*front == NULL) {
        *rear = NULL;
    }
    
    strcpy(temp->status, "Processed");

    return temp; 
}

// Display all pending orders by traversing the linked list
void displayPendingOrders(Order *front) {
    if (front == NULL) {
        printf("No pending orders in the queue.\n");
        return;
    }
    
    printf("\n--- Pending Orders ---\n");
    Order* current = front;
    while (current != NULL) {
        printf("Order ID: %d | Customer ID: %d | Product ID: %d | Qty: %d | Type: %s | Status: %s\n",
               current->orderID, current->customerID, current->productID, 
               current->quantity, current->shippingType, current->status);
        current = current->next;
    }
    printf("----------------------\n");
}

// ==========================================
// 2. FILE I/O OPERATIONS
// ==========================================

// Load pending orders into the queue at startup
void loadOrdersFromCSV(Order **front, Order **rear, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Notice: No existing %s found. Starting with an empty queue.\n", filename);
        return;
    }

    char line[256];
    
    if (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "orderID", 7) != 0) {
            rewind(file); 
        }
    }

    while (fgets(line, sizeof(line), file)) {
        int oID, cID, pID, qty;
        char ship[20], stat[20];
        
        if (sscanf(line, "%d,%d,%d,%d,%19[^,],%19[^\r\n]", &oID, &cID, &pID, &qty, ship, stat) >= 5) {
            Order *newOrder = createOrder(oID, cID, pID, qty, ship);
            if (newOrder) {
                if (strlen(stat) > 0) strcpy(newOrder->status, stat);
                enqueueOrder(front, rear, newOrder);
            }
        }
    }
    fclose(file);
}

// Save the current state of the queue back to the pending CSV
void saveOrdersToCSV(Order *front, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not open %s for writing.\n", filename);
        return;
    }
    
    fprintf(file, "orderID,customerID,productID,quantity,shippingType,status\n");
    
    Order *current = front;
    while (current != NULL) {
        fprintf(file, "%d,%d,%d,%d,%s,%s\n",
                current->orderID, current->customerID, current->productID,
                current->quantity, current->shippingType, current->status);
        current = current->next;
    }
    fclose(file);
}

// Append a successfully processed order to the history CSV
void appendOrderToHistoryCSV(Order *order, const char *filename) {
    if (!order) return;
    
    FILE *file = fopen(filename, "a");
    if (!file) {
        printf("Error: Could not open %s for appending.\n", filename);
        return;
    }
    
    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0) {
        fprintf(file, "orderID,customerID,productID,quantity,shippingType,status\n");
    }
    
    fprintf(file, "%d,%d,%d,%d,%s,%s\n",
            order->orderID, order->customerID, order->productID,
            order->quantity, order->shippingType, order->status);
            
    fclose(file);
}

// ==========================================
// 3. MEMORY MANAGEMENT
// ==========================================

// Call this before program termination to prevent memory leaks
void freeOrderQueue(Order **front, Order **rear) {
    Order *current = *front;
    Order *nextOrder;
    
    while (current != NULL) {
        nextOrder = current->next;
        free(current);
        current = nextOrder;
    }
    
    *front = NULL;
    *rear = NULL;
}