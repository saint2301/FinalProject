//Name: Nanthapat Nanthapaiboon
//Student ID: 68070503432

#ifndef ORDERS_H
#define ORDERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Customer.h"
#include "Inventory.h"

// Define the Order structure as required
typedef struct Order {
    int orderID;
    int customerID;
    int productID;
    int quantity;
    char shippingType[20];
    char status[20];
    struct Order *next;
} Order;

// Main Queue Functions
Order* createOrder(int orderID, int customerID, int productID, int quantity, const char* shippingType);
void enqueueOrder(Order **front, Order **rear, Order *newOrder);
Order* dequeueOrder(Order **front, Order **rear);
void displayPendingOrders(Order *front);
int validateOrder(Customer **customerTable, Product *inventoryRoot, int cid, int pid, int qty);

// File I/O Functions
void loadOrdersFromCSV(Order **front, Order **rear, const char *filename);
void saveOrdersToCSV(Order *front, const char *filename);
void appendOrderToHistoryCSV(Order *order, const char *filename);

// Utility Functions
void freeOrderQueue(Order **front, Order **rear);

#endif