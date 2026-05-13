#ifndef INTEGRATION_H
#define INTEGRATION_H

#include "Customer.h"
#include "Inventory.h"
#include "orders.h"
#include "admin_command.h"

#define CUSTOMER_CSV "customers.csv"
#define INVENTORY_CSV "Inventory.csv"
#define ORDERS_CSV "orders.csv"
#define ORDER_HISTORY_CSV "order_history.csv"

/*
    IntegrationSystem is the main control layer.
    It connects customer hash table, inventory BST, order queue,
    and admin undo stack together.
*/
typedef struct IntegrationSystem {
    Customer *customerTable[TABLE_SIZE];
    Product *inventoryRoot;
    Order *orderFront;
    Order *orderRear;
    Action *undoStack;
} IntegrationSystem;

void initializeSystem(IntegrationSystem *system);
void loadSystemData(IntegrationSystem *system);
void saveSystemData(IntegrationSystem *system);
void runIntegrationMenu(IntegrationSystem *system);
void freeSystem(IntegrationSystem *system);

#endif
