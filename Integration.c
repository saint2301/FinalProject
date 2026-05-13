#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Integration.h"
#include <Integration.h>

static void clearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* clear input */
    }
}

static int readInt(const char *message) {
    int value;

    printf("%s", message);

    while (scanf("%d", &value) != 1) {
        printf("Invalid input. Enter a number: ");
        clearInputBuffer();
    }

    clearInputBuffer();
    return value;
}

static float readFloat(const char *message) {
    float value;

    printf("%s", message);

    while (scanf("%f", &value) != 1) {
        printf("Invalid input. Enter a number: ");
        clearInputBuffer();
    }

    clearInputBuffer();
    return value;
}

static void readLine(const char *message, char *buffer, int size) {
    printf("%s", message);

    if (fgets(buffer, size, stdin) != NULL) {
        buffer[strcspn(buffer, "\r\n")] = '\0';
    }
}

static void freeInventoryTree(Product *root) {
    if (root == NULL) {
        return;
    }

    freeInventoryTree(root->left);
    freeInventoryTree(root->right);
    free(root);
}

void initializeSystem(IntegrationSystem *system) {
    int i;

    if (system == NULL) {
        return;
    }

    for (i = 0; i < TABLE_SIZE; i++) {
        system->customerTable[i] = NULL;
    }

    system->inventoryRoot = NULL;
    system->orderFront = NULL;
    system->orderRear = NULL;
    system->undoStack = NULL;
}

void loadSystemData(IntegrationSystem *system) {
    if (system == NULL) {
        return;
    }

    loadCustomersFromCSV(system->customerTable, CUSTOMER_CSV);
    loadProductsFromCSV(&system->inventoryRoot, INVENTORY_CSV);
    loadOrdersFromCSV(&system->orderFront, &system->orderRear, ORDERS_CSV);

    printf("System data loaded.\n");
}

void saveSystemData(IntegrationSystem *system) {
    if (system == NULL) {
        return;
    }

    saveCustomersToCSV(system->customerTable, CUSTOMER_CSV);
    saveProductsToCSV(system->inventoryRoot, INVENTORY_CSV);
    saveOrdersToCSV(system->orderFront, ORDERS_CSV);

    printf("System data saved.\n");
}

static void customerMenu(IntegrationSystem *system) {
    int choice;

    do {
        printf("\n========== CUSTOMER MODULE ==========" "\n");
        printf("1. Register new customer\n");
        printf("2. Search customer by ID\n");
        printf("3. Display customer details\n");
        printf("4. Display all customers\n");
        printf("5. Check customer exists\n");
        printf("0. Back\n");

        choice = readInt("Choice: ");

        if (choice == 1) {
            int id = readInt("Enter customer ID: ");
            char name[50];
            char phone[20];
            Customer *customer;

            if (CustomerExists(system->customerTable, id)) {
                printf("Customer ID already exists.\n");
                continue;
            }

            readLine("Enter name: ", name, sizeof(name));
            readLine("Enter phone: ", phone, sizeof(phone));

            customer = createCustomer(id, name, phone);
            insertCustomer(system->customerTable, customer);
            printf("Customer registered successfully.\n");
        } else if (choice == 2) {
            int id = readInt("Enter customer ID: ");
            Customer *customer = searchCustomer(system->customerTable, id);

            if (customer != NULL) {
                printf("Found: %d | %s | %s\n", customer->CustomerID, customer->Name, customer->Phone);
            } else {
                printf("Customer not found.\n");
            }
        } else if (choice == 3) {
            int id = readInt("Enter customer ID: ");
            DisplayCustomer(system->customerTable, id);
        } else if (choice == 4) {
            DisplayAllCustomers(system->customerTable);
        } else if (choice == 5) {
            int id = readInt("Enter customer ID: ");

            if (CustomerExists(system->customerTable, id)) {
                printf("Customer exists.\n");
            } else {
                printf("Customer does not exist.\n");
            }
        } else if (choice != 0) {
            printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

static void inventoryMenu(IntegrationSystem *system) {
    int choice;

    do {
        printf("\n========== INVENTORY MODULE ==========" "\n");
        printf("1. Add product\n");
        printf("2. Search product\n");
        printf("3. Display all products\n");
        printf("0. Back\n");

        choice = readInt("Choice: ");

        if (choice == 1) {
            int id = readInt("Enter product ID: ");
            char name[50];
            float price;
            int stock;

            if (searchProduct(system->inventoryRoot, id) != NULL) {
                printf("Product ID already exists.\n");
                continue;
            }

            readLine("Enter product name: ", name, sizeof(name));
            price = readFloat("Enter price: ");
            stock = readInt("Enter stock: ");

            system->inventoryRoot = insertProduct(system->inventoryRoot, id, name, price, stock);
            printf("Product added successfully.\n");
        } else if (choice == 2) {
            int id = readInt("Enter product ID: ");
            Product *product = searchProduct(system->inventoryRoot, id);

            if (product != NULL) {
                printf("ID: %d | Name: %s | Price: %.2f | Stock: %d\n",
                       product->productID,
                       product->name,
                       product->price,
                       product->stock);
            } else {
                printf("Product not found.\n");
            }
        } else if (choice == 3) {
            printf("\n%-10s %-20s %-10s %-10s\n", "ID", "Name", "Price", "Stock");
            printf("%-10s %-20s %-10s %-10s\n", "----------", "--------------------", "----------", "----------");
            displayProductsInOrder(system->inventoryRoot);
        } else if (choice != 0) {
            printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

static void orderMenu(IntegrationSystem *system) {
    int choice;

    do {
        printf("\n========== ORDER PROCESSING MODULE ==========" "\n");
        printf("1. Add order\n");
        printf("2. Display pending orders\n");
        printf("3. Process next order\n");
        printf("0. Back\n");

        choice = readInt("Choice: ");

        if (choice == 1) {
            int orderID = readInt("Enter order ID: ");
            int customerID = readInt("Enter customer ID: ");
            int productID = readInt("Enter product ID: ");
            int quantity = readInt("Enter quantity: ");
            char shippingType[20];
            Product *product;
            Order *order;

            readLine("Enter shipping type: ", shippingType, sizeof(shippingType));

            if (!CustomerExists(system->customerTable, customerID)) {
                printf("Order rejected: customer does not exist.\n");
                continue;
            }

            product = searchProduct(system->inventoryRoot, productID);

            if (product == NULL) {
                printf("Order rejected: product does not exist.\n");
                continue;
            }

            if (quantity <= 0) {
                printf("Order rejected: quantity must be positive.\n");
                continue;
            }

            order = createOrder(orderID, customerID, productID, quantity, shippingType);
            enqueueOrder(&system->orderFront, &system->orderRear, order);
            printf("Order added to queue.\n");
        } else if (choice == 2) {
            displayPendingOrders(system->orderFront);
        } else if (choice == 3) {
            Order *nextOrder;
            Product *product;
            Order *processed;
            char description[120];

            if (system->orderFront == NULL) {
                printf("No pending orders.\n");
                continue;
            }

            nextOrder = system->orderFront;
            product = searchProduct(system->inventoryRoot, nextOrder->productID);

            if (product == NULL) {
                printf("Cannot process order %d: product not found.\n", nextOrder->orderID);
                continue;
            }

            if (product->stock < nextOrder->quantity) {
                printf("Cannot process order %d: not enough stock.\n", nextOrder->orderID);
                continue;
            }

            product->stock -= nextOrder->quantity;
            processed = dequeueOrder(&system->orderFront, &system->orderRear);
            appendOrderToHistoryCSV(processed, ORDER_HISTORY_CSV);

            snprintf(description, sizeof(description),
                     "Processed order for product %d, quantity %d",
                     processed->productID,
                     processed->quantity);
            appendAdminLogCSV(ADMIN_LOG_FILE, "PROCESS_ORDER", processed->orderID, description);

            printf("Order %d processed successfully.\n", processed->orderID);
            free(processed);
        } else if (choice != 0) {
            printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

static void adminMenu(IntegrationSystem *system) {
    int choice;

    do {
        printf("\n========== ADMIN AND UNDO MODULE ==========" "\n");
        printf("1. Delete product with undo record\n");
        printf("2. Update product stock with undo record\n");
        printf("3. Undo last admin action\n");
        printf("4. Display products\n");
        printf("5. Display admin log\n");
        printf("0. Back\n");

        choice = readInt("Choice: ");

        if (choice == 1) {
            int id = readInt("Enter product ID to delete: ");
            adminDeleteProduct(&system->inventoryRoot, &system->undoStack, id);
        } else if (choice == 2) {
            int id = readInt("Enter product ID to update stock: ");
            int newStock = readInt("Enter new stock: ");

            if (newStock < 0) {
                printf("Stock cannot be negative.\n");
                continue;
            }

            adminUpdateStock(system->inventoryRoot, &system->undoStack, id, newStock);
        } else if (choice == 3) {
            undoLastAction(&system->undoStack, &system->inventoryRoot);
        } else if (choice == 4) {
            printf("\n%-10s %-20s %-10s %-10s\n", "ID", "Name", "Price", "Stock");
            printf("%-10s %-20s %-10s %-10s\n", "----------", "--------------------", "----------", "----------");
            displayProductsInOrder(system->inventoryRoot);
        } else if (choice == 5) {
            displayAdminLogCSV(ADMIN_LOG_FILE);
        } else if (choice != 0) {
            printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

void runIntegrationMenu(IntegrationSystem *system) {
    int choice;

    if (system == NULL) {
        printf("System is invalid.\n");
        return;
    }

    do {
        printf("\n======================================\n");
        printf("      INVENTORY MANAGEMENT SYSTEM\n");
        printf("======================================\n");
        printf("1. Customer Module\n");
        printf("2. Inventory Module\n");
        printf("3. Order Processing Module\n");
        printf("4. Admin and Undo Module\n");
        printf("5. Save All Data\n");
        printf("0. Save and Exit\n");
        printf("======================================\n");

        choice = readInt("Choice: ");

        switch (choice) {
            case 1:
                customerMenu(system);
                break;
            case 2:
                inventoryMenu(system);
                break;
            case 3:
                orderMenu(system);
                break;
            case 4:
                adminMenu(system);
                break;
            case 5:
                saveSystemData(system);
                break;
            case 0:
                saveSystemData(system);
                printf("Goodbye.\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

void freeSystem(IntegrationSystem *system) {
    if (system == NULL) {
        return;
    }

    freeCustomerTable(system->customerTable);
    freeInventoryTree(system->inventoryRoot);
    freeOrderQueue(&system->orderFront, &system->orderRear);
    freeActions(&system->undoStack);

    system->inventoryRoot = NULL;
    system->orderFront = NULL;
    system->orderRear = NULL;
}
