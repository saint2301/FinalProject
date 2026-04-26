#ifndef INVENTORY_H
#define INVENTORY_H

typedef struct Product {
	int productID;
	char name[50];
	float price;
	int stock;
	struct Product *left;
	struct Product *right;
} Product;

Product* createProduct(int id, char name[], float price, int stock);
Product* insertProduct(Product root, int id, char name[], float price, int stock);
Product searchProduct(Product root, int id);
Product deleteProduct(Product *root, int id);
void displayProductsInOrder(Product *root);

void loadProductsFromCSV(Product **root, const char *filename);
void saveProductsToCSV(Product *root, const char *filename);
void writeProductsInOrder(Product *root, FILE *fp);

#endif