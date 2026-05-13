#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Inventory.h"

Product* createProduct(int id, char name[], float price, int stock) {
    Product *newNode = (Product*)malloc(sizeof(Product));
    newNode->productID = id;
    strcpy(newNode->name, name);
    newNode->price = price;
    newNode->stock = stock;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

Product* insertProduct(Product *root, int id, char name[], float price, int stock) {
    if (root == NULL) {
        return createProduct(id, name, price, stock);
    }

    if (id < root->productID) {
        root->left = insertProduct(root->left, id, name, price, stock);
    } else if (id > root->productID) {
        root->right = insertProduct(root->right, id, name, price, stock);
    }

    return root;
}

Product* searchProduct(Product *root, int id) {
    if (root == NULL) {
        return NULL;
    }

    if (id == root->productID) {
        return root;
    }

    if (id < root->productID) {
        return searchProduct(root->left, id);
    } else {
        return searchProduct(root->right, id);
    }
}

Product* findMin(Product *root) {
    while (root != NULL && root->left != NULL) {
        root = root->left;
    }
    return root;
}

Product* deleteProduct(Product *root, int targetID) {
    if (root == NULL) {
        return NULL;
    }

    if (targetID < root->productID) {
        root->left = deleteProduct(root->left, targetID);
    } else if (targetID > root->productID) {
        root->right = deleteProduct(root->right, targetID);
    } else {
        // Case 1: no child
        if (root->left == NULL && root->right == NULL) {
            free(root);
            return NULL;
        }

        // Case 2: one child
        else if (root->left == NULL) {
            Product *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            Product *temp = root->left;
            free(root);
            return temp;
        }

        // Case 3: two children
        else {
            Product *temp = findMin(root->right);

            root->productID = temp->productID;
            strcpy(root->name, temp->name);
            root->price = temp->price;
            root->stock = temp->stock;

            root->right = deleteProduct(root->right, temp->productID);
        }
    }

    return root;
}

#include <stdio.h>

void displayProductsInOrder(Product *root) {
    if (root == NULL) {
        return;
    }

    displayProductsInOrder(root->left);

    printf("%-10d %-20s %-10.2f %-10d\n",
           root->productID,
           root->name,
           root->price,
           root->stock);

    displayProductsInOrder(root->right);
}

void writeProductsInOrder(Product *root, FILE *fp) {
    if (root == NULL) {
        return;
    }

    writeProductsInOrder(root->left, fp);

    fprintf(fp, "%d,%s,%.2f,%d\n",
            root->productID,
            root->name,
            root->price,
            root->stock);

    writeProductsInOrder(root->right, fp);
}

void saveProductsToCSV(Product *root, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: cannot open file %s for writing.\n", filename);
        return;
    }

    fprintf(fp, "productID,name,price,stock\n");
    writeProductsInOrder(root, fp);

    fclose(fp);
    printf("Products saved successfully.\n");
}

void loadProductsFromCSV(Product **root, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("products.csv not found. Starting with empty inventory.\n");
        return;
    }

    char line[256];

    // Skip header
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        int id, stock;
        float price;
        char name[50];

        line[strcspn(line, "\n")] = '\0';

        if (sscanf(line, "%d,%49[^,],%f,%d", &id, name, &price, &stock) == 4) {
            *root = insertProduct(*root, id, name, price, stock);
        }
    }
}