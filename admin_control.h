#ifndef ADMIN_CONTROL_H
#define ADMIN_CONTROL_H

/* =========================
   Product BST Structure
   ========================= */

typedef struct Product {
    int id;
    char name[50];
    float price;
    int stock;
    struct Product *left;
    struct Product *right;
} Product;

/* =========================
   Undo Action Stack Structure
   ========================= */

typedef struct Action {
    char actionType[20];
    int productID;
    char productName[50];
    float price;
    int stock;
    struct Action *next;
} Action;

/* =========================
   Product Functions
   ========================= */

Product* createProduct(int id, char name[], float price, int stock);

Product* insertProduct(Product *root, int id, char name[], float price, int stock);

Product* searchProduct(Product *root, int id);

Product* findMin(Product *root);

Product* deleteProduct(Product *root, int id);

void displayProducts(Product *root);

void freeProducts(Product *root);

/* =========================
   Undo Stack Functions
   ========================= */

Action* createAction(char type[], int productID, char productName[], float price, int stock);

void pushAction(Action **top, Action *newAction);

Action* popAction(Action **top);

void undoLastAction(Action **top, Product **inventoryRoot);

void freeActions(Action *top);

/* =========================
   Admin Log CSV Function
   ========================= */

void appendAdminLogCSV(const char *filename, char actionType[], int refID, char description[]);

/* =========================
   Admin Operations
   ========================= */

void adminDeleteProduct(Product **inventoryRoot, Action **undoStack, int productID);

void adminUpdateStock(Product *inventoryRoot, Action **undoStack, int productID, int newStock);

#endif
