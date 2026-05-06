#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "admin_command.h"

Action* createAction(char type[], int productID, char productName[], float price, int stock) {
    Action *newAction = (Action *)malloc(sizeof(Action));

    if (newAction == NULL) {
        printf("Error: could not allocate memory for undo action.\n");
        return NULL;
    }

    strncpy(newAction->actionType, type, sizeof(newAction->actionType) - 1);
    newAction->actionType[sizeof(newAction->actionType) - 1] = '\0';

    newAction->productID = productID;

    strncpy(newAction->productName, productName, sizeof(newAction->productName) - 1);
    newAction->productName[sizeof(newAction->productName) - 1] = '\0';

    newAction->price = price;
    newAction->stock = stock;
    newAction->next = NULL;

    return newAction;
}

void pushAction(Action **top, Action *newAction) {
    if (top == NULL || newAction == NULL) {
        return;
    }

    newAction->next = *top;
    *top = newAction;
}

Action* popAction(Action **top) {
    Action *temp;

    if (top == NULL || *top == NULL) {
        return NULL;
    }

    temp = *top;
    *top = (*top)->next;
    temp->next = NULL;

    return temp;
}

void freeActions(Action **top) {
    Action *current;
    Action *next;

    if (top == NULL) {
        return;
    }

    current = *top;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    *top = NULL;
}

void appendAdminLogCSV(const char *filename, char actionType[], int refID, char description[]) {
    FILE *fp = fopen(filename, "a");

    if (fp == NULL) {
        printf("Warning: could not open %s for admin logging.\n", filename);
        return;
    }

    fseek(fp, 0, SEEK_END);
    if (ftell(fp) == 0) {
        fprintf(fp, "actionType,refID,description\n");
    }

    fprintf(fp, "%s,%d,%s\n", actionType, refID, description);
    fclose(fp);
}

void adminDeleteProduct(Product **inventoryRoot, Action **undoStack, int productID) {
    Product *product;
    Action *action;
    char description[120];

    if (inventoryRoot == NULL) {
        printf("Inventory root is invalid.\n");
        return;
    }

    product = searchProduct(*inventoryRoot, productID);

    if (product == NULL) {
        printf("Product ID %d not found. Delete cancelled.\n", productID);
        return;
    }

    action = createAction("DELETE_PRODUCT",
                          product->productID,
                          product->name,
                          product->price,
                          product->stock);

    if (action == NULL) {
        printf("Delete cancelled because undo action could not be created.\n");
        return;
    }

    pushAction(undoStack, action);

    snprintf(description, sizeof(description), "Deleted %s", product->name);
    appendAdminLogCSV(ADMIN_LOG_FILE, "DELETE_PRODUCT", productID, description);

    *inventoryRoot = deleteProduct(*inventoryRoot, productID);
    printf("Product deleted successfully. Undo record saved.\n");
}

void adminUpdateStock(Product *inventoryRoot, Action **undoStack, int productID, int newStock) {
    Product *product;
    Action *action;
    char description[120];
    int oldStock;

    product = searchProduct(inventoryRoot, productID);

    if (product == NULL) {
        printf("Product ID %d not found. Stock update cancelled.\n", productID);
        return;
    }

    oldStock = product->stock;

    action = createAction("UPDATE_STOCK",
                          product->productID,
                          product->name,
                          product->price,
                          product->stock);

    if (action == NULL) {
        printf("Stock update cancelled because undo action could not be created.\n");
        return;
    }

    pushAction(undoStack, action);

    product->stock = newStock;

    snprintf(description, sizeof(description), "Stock changed from %d to %d", oldStock, newStock);
    appendAdminLogCSV(ADMIN_LOG_FILE, "UPDATE_STOCK", productID, description);

    printf("Stock updated successfully. Undo record saved.\n");
}

void undoLastAction(Action **top, Product **inventoryRoot) {
    Action *action;
    Product *product;
    char description[120];

    if (top == NULL || inventoryRoot == NULL) {
        printf("Undo system is invalid.\n");
        return;
    }

    action = popAction(top);

    if (action == NULL) {
        printf("No admin action to undo.\n");
        return;
    }

    if (strcmp(action->actionType, "DELETE_PRODUCT") == 0) {
        product = searchProduct(*inventoryRoot, action->productID);

        if (product == NULL) {
            *inventoryRoot = insertProduct(*inventoryRoot,
                                           action->productID,
                                           action->productName,
                                           action->price,
                                           action->stock);
            printf("Undo complete: deleted product restored.\n");

            snprintf(description, sizeof(description), "Restored deleted product %s", action->productName);
            appendAdminLogCSV(ADMIN_LOG_FILE, "UNDO_DELETE", action->productID, description);
        } else {
            printf("Undo skipped: product ID already exists in inventory.\n");
        }
    } else if (strcmp(action->actionType, "UPDATE_STOCK") == 0) {
        product = searchProduct(*inventoryRoot, action->productID);

        if (product != NULL) {
            int currentStock = product->stock;
            product->stock = action->stock;
            printf("Undo complete: stock restored from %d to %d.\n", currentStock, action->stock);

            snprintf(description, sizeof(description), "Stock restored from %d to %d", currentStock, action->stock);
            appendAdminLogCSV(ADMIN_LOG_FILE, "UNDO_STOCK", action->productID, description);
        } else {
            printf("Undo failed: product no longer exists.\n");
        }
    } else {
        printf("Unknown undo action type: %s\n", action->actionType);
    }

    free(action);
}

void displayAdminLogCSV(const char *filename) {
    FILE *fp = fopen(filename, "r");
    char line[256];

    if (fp == NULL) {
        printf("No admin log found yet.\n");
        return;
    }

    printf("\n========== ADMIN LOG ==========" "\n");

    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\r\n")] = '\0';
        printf("%s\n", line);
    }

    fclose(fp);
}
