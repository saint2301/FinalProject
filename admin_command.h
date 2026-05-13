#ifndef ADMIN_COMMAND_H
#define ADMIN_COMMAND_H

#include "Inventory.h"

#define ADMIN_LOG_FILE "adminlog.csv"

/*
    Admin Undo Stack Node
    Stores enough old product data to reverse a delete or stock update.
*/
typedef struct Action {
    char actionType[20];
    int productID;
    char productName[50];
    float price;
    int stock;
    struct Action *next;
} Action;

/* Stack functions */
Action* createAction(char type[], int productID, char productName[], float price, int stock);
void pushAction(Action **top, Action *newAction);
Action* popAction(Action **top);
void freeActions(Action **top);

/* Undo function */
void undoLastAction(Action **top, Product **inventoryRoot);

/* CSV log function */
void appendAdminLogCSV(const char *filename, char actionType[], int refID, char description[]);

/* Admin commands */
void adminDeleteProduct(Product **inventoryRoot, Action **undoStack, int productID);
void adminUpdateStock(Product *inventoryRoot, Action **undoStack, int productID, int newStock);
void displayAdminLogCSV(const char *filename);

#endif
