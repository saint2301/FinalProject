#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "admin_control.h"

/* =========================
   Product Functions
   ========================= */

Product* createProduct(int id, char name[], float price, int stock) {
    Product *newProduct = (Product*)malloc(sizeof(Product));

    if (newProduct == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    newProduct->id = id;
    strcpy(newProduct->name, name);
    newProduct->price = price;
    newProduct->stock = stock;
    newProduct->left = NULL;
    newProduct->right = NULL;

    return newProduct;
}

Product* insertProduct(Product *root, int id, char name[], float price, int stock) {
    if (root == NULL) {
        return createProduct(id, name, price, stock);
    }

    if (id < root->id) {
        root->left = insertProduct(root->left, id, name, price, stock);
    } 
    else if (id > root->id) {
        root->right = insertProduct(root->right, id, name, price, stock);
    } 
    else {
        printf("Product ID already exists.\n");
    }

    return root;
}

Product* searchProduct(Product *root, int id) {
    if (root == NULL || root->id == id) {
        return root;
    }

    if (id < root->id) {
        return searchProduct(root->left, id);
    }

    return searchProduct(root->right, id);
}

Product* findMin(Product *root) {
    while (root != NULL && root->left != NULL) {
        root = root->left;
    }

    return root;
}

Product* deleteProduct(Product *root, int id) {
    if (root == NULL) {
        return NULL;
    }

    if (id < root->id) {
        root->left = deleteProduct(root->left, id);
    } 
    else if (id > root->id) {
        root->right = deleteProduct(root->right, id);
    } 
    else {
        if (root->left == NULL) {
            Product *temp = root->right;
            free(root);
            return temp;
        } 
        else if (root->right == NULL) {
            Product *temp = root->left;
            free(root);
            return temp;
        } 
        else {
            Product *temp = findMin(root->right);

            root->id = temp->id;
            strcpy(root->name, temp->name);
            root->price = temp->price;
            root->stock = temp->stock;

            root->right = deleteProduct(root->right, temp->id);
        }
    }

    return root;
}

void displayProducts(Product *root) {
    if (root == NULL) {
        return;
    }

    displayProducts(root->left);

    printf("ID: %d | Name: %s | Price: %.2f | Stock: %d\n",
           root->id, root->name, root->price, root->stock);

    displayProducts(root->right);
}

void freeProducts(Product *root) {
    if (root == NULL) {
        return;
    }

    freeProducts(root->left);
    freeProducts(root->right);
    free(root);
}

/* =========================
   Undo Stack Functions
   ========================= */

Action* createAction(char type[], int productID, char productName[], float price, int stock) {
    Action *newAction = (Action*)malloc(sizeof(Action));

    if (newAction == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    strcpy(newAction->actionType, type);
    newAction->productID = productID;
    strcpy(newAction->productName, productName);
    newAction->price = price;
    newAction->stock = stock;
    newAction->next = NULL;

    return newAction;
}

void pushAction(Action **top, Action *newAction) {
    if (newAction == NULL) {
        return;
    }

    newAction->next = *top;
    *top = newAction;
}

Action* popAction(Action **top) {
    if (*top == NULL) {
        return NULL;
    }

    Action *temp = *top;
    *top = (*top)->next;
    temp->next = NULL;

    return temp;
}

void undoLastAction(Action **top, Product **inventoryRoot) {
    Action *lastAction = popAction(top);

    if (lastAction == NULL) {
        printf("No action to undo.\n");
        return;
    }

    if (strcmp(lastAction->actionType, "DELETE_PRODUCT") == 0) {
        *inventoryRoot = insertProduct(
            *inventoryRoot,
            lastAction->productID,
            lastAction->productName,
            lastAction->price,
            lastAction->stock
        );

        printf("Undo complete: Deleted product restored.\n");
    } 
    else if (strcmp(lastAction->actionType, "UPDATE_STOCK") == 0) {
        Product *product = searchProduct(*inventoryRoot, lastAction->productID);

        if (product != NULL) {
            product->stock = lastAction->stock;
            printf("Undo complete: Stock restored.\n");
        } 
        else {
            printf("Undo failed: Product not found.\n");
        }
    } 
    else {
        printf("Unknown action type. Cannot undo.\n");
    }

    free(lastAction);
}

void freeActions(Action *top) {
    Action *temp;

    while (top != NULL) {
        temp = top;
        top = top->next;
        free(temp);
    }
}

/* =========================
   Admin Log CSV Function
   ========================= */

void appendAdminLogCSV(const char *filename, char actionType[], int refID, char description[]) {
    FILE *file = fopen(filename, "a");

    if (file == NULL) {
        printf("Could not open admin log file.\n");
        return;
    }

    fprintf(file, "%s,%d,%s\n", actionType, refID, description);

    fclose(file);
}

/* =========================
   Admin Operations
   ========================= */

void adminDeleteProduct(Product **inventoryRoot, Action **undoStack, int productID) {
    Product *product = searchProduct(*inventoryRoot, productID);

    if (product == NULL) {
        printf("Product not found.\n");
        return;
    }

    pushAction(undoStack, createAction(
        "DELETE_PRODUCT",
        product->id,
        product->name,
        product->price,
        product->stock
    ));

    char description[100];
    sprintf(description, "Deleted %s", product->name);

    appendAdminLogCSV("adminlog.csv", "DELETE_PRODUCT", product->id, description);

    *inventoryRoot = deleteProduct(*inventoryRoot, productID);

    printf("Product deleted successfully.\n");
}

void adminUpdateStock(Product *inventoryRoot, Action **undoStack, int productID, int newStock) {
    Product *product = searchProduct(inventoryRoot, productID);

    if (product == NULL) {
        printf("Product not found.\n");
        return;
    }

    pushAction(undoStack, createAction(
        "UPDATE_STOCK",
        product->id,
        product->name,
        product->price,
        product->stock
    ));

    char description[100];
    sprintf(description, "Stock changed from %d to %d", product->stock, newStock);

    appendAdminLogCSV("adminlog.csv", "UPDATE_STOCK", product->id, description);

    product->stock = newStock;

    printf("Stock updated successfully.\n");
}
