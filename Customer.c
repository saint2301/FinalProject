#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Customer.h"


Customer *customerTable[TABLE_SIZE];

int hashfunction(int customerID)
{
    return customerID % TABLE_SIZE;
}

Customer *createCustomer(int id,char Name[],char Phone[])
{
    Customer *newCustomer = (Customer *)malloc(sizeof(Customer));
    if (!newCustomer) {
        fprintf(stderr, "ERROR: Memory allocation failed for customer ID %d.\n", id);
        return NULL;
    }
    newCustomer->CustomerID = id;
    strncpy(newCustomer->Name,  Name,  sizeof(newCustomer->Name)  - 1);
    strncpy(newCustomer->Phone, Phone, sizeof(newCustomer->Phone) - 1);
    newCustomer->Name[sizeof(newCustomer->Name)   - 1] = '\0';
    newCustomer->Phone[sizeof(newCustomer->Phone) - 1] = '\0';
    newCustomer->next = NULL;
    return newCustomer;
}

void insertCustomer(Customer *table[],Customer * newCustomer)
{
    if(!newCustomer)
    {
        return;
    }
    int index = hashfunction(newCustomer -> CustomerID);


    Customer *current = table[index];
    while (current) {
        if (current->CustomerID == newCustomer->CustomerID) {
            fprintf(stderr, "WARNING: Customer ID %d already exists. Skipping insert.\n", newCustomer->CustomerID);
            return;
        }
        current = current->next;
    }
    newCustomer ->next = table[index];
    table[index] = newCustomer;
}

Customer* searchCustomer(Customer * table[],int CustomerID)
{
    int index = hashfunction(CustomerID);
    Customer *current = table[index];

    while(current)
    {
        if(current->CustomerID == CustomerID)
        {
            return current;
        }
        else
        {
            current = current->next;
        }
    }
    return NULL; // not found
}

int CustomerExists(Customer *table[],int CustomerID)
{
    return searchCustomer(table,CustomerID) != NULL; //Returns 1 if the customer exists, 0 otherwise.
}

void DisplayCustomer(Customer *table[],int CustomerID)
{
    Customer *C = searchCustomer(table,CustomerID);
    if(!C)
    {
        printf("Customer ID: %d not found\n",CustomerID);
        return;
    }
     printf("+-----------------------------+\n");
    printf("| Customer ID : %-13d |\n", C->CustomerID);
    printf("| Name        : %-13s |\n", C->Name);
    printf("| Phone       : %-13s |\n", C->Phone);
    printf("+-----------------------------+\n");
}

void DisplayAllCustomers(Customer *table[])
{
    int found = 0;
    printf("\n%-10s %-20s %-15s\n","ID","Name","Phone");
    printf("%-10s %-20s %-15s\n", "----------", "--------------------", "---------------");

    for(int i = 0;i <TABLE_SIZE;i++)
    {
        Customer * current = table[i];
        while(current)
        {
            printf("%-10d %-20s %-15s\n",current->CustomerID,current->Name,current->Phone);
            found = 1;
            current = current->next;
        }
    }
    if (!found) 
    {
        printf("(no customers registered)\n");
    } 
    printf("\n");
}

void loadCustomersFromCSV(Customer *table[], const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "INFO: Could not open '%s'. Starting with empty registry.\n",
                filename);
        return;
    }
 
    char line[128];
    int  lineNumber = 0;
 
    while (fgets(line, sizeof(line), fp)) {
        lineNumber++;
 
        /* Strip trailing newline / carriage-return */
        line[strcspn(line, "\r\n")] = '\0';
 
        /* Skip the header row */
        if (lineNumber == 1) continue;
 
        /* Parse: customerID , name , phone */
        int  id;
        char name[50], phone[20];
 
        if (sscanf(line, "%d,%[^,],%s", &id, name, phone) == 3) {
            Customer *c = createCustomer(id, name, phone);
            if (c) insertCustomer(table, c);
        } else {
            fprintf(stderr,
                    "WARNING: Malformed line %d in '%s': %s\n",
                    lineNumber, filename, line);
        }
    }
 
    fclose(fp);
    printf("Customers loaded from '%s'.\n", filename);
}

void saveCustomersToCSV(Customer *table[], const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "ERROR: Could not open '%s' for writing.\n", filename);
        return;
    }
 
    fprintf(fp, "customerID,name,phone\n");
 
    for (int i = 0; i < TABLE_SIZE; i++) {
        Customer *current = table[i];
        while (current) {
            fprintf(fp, "%d,%s,%s\n",
                    current->CustomerID,
                    current->Name,
                    current->Phone);
            current = current->next;
        }
    }
 
    fclose(fp);
    printf("Customers saved to '%s'.\n", filename);
}

void freeCustomerTable(Customer *table[]) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Customer *current = table[i];
        while (current) {
            Customer *temp = current;
            current = current->next;
            free(temp);
        }
        table[i] = NULL;
    }
}
int main(void) {
    // const char *csvFile = "customers.csv";
    const char *csvFile = "E:\\CSEM2\\Final_project\\FinalProject\\customers.csv";//CSV file location
 
    printf("=== Customer Registry ===\n\n");
 
    loadCustomersFromCSV(customerTable, csvFile);
 
    int choice;
    do {
        printf("\n--- Menu ---\n");
        printf("1. Register new customer\n");
        printf("2. Search customer by ID\n");
        printf("3. Display customer details\n");
        printf("4. Display all customers\n");
        printf("5. Check if customer exists\n");
        printf("6. Save and exit\n");
        printf("Choice: ");
 
        if (scanf("%d", &choice) != 1) 
        {
            saveCustomersToCSV(customerTable, csvFile);
            break;
        }
        getchar();   
 
        if (choice == 1) {

            int  id;
            char name[50], phone[20];
            printf("Enter customer ID  : "); scanf("%d",  &id);   getchar();
            printf("Enter name         : "); fgets(name,  sizeof(name),  stdin);
            printf("Enter phone        : "); fgets(phone, sizeof(phone), stdin);
            name[strcspn(name,   "\n")] = '\0';
            phone[strcspn(phone, "\n")] = '\0';
 
            Customer *c = createCustomer(id, name, phone);
            if (c) {
                insertCustomer(customerTable, c);
                saveCustomersToCSV(customerTable, csvFile);//Auto save
                printf("Customer registered successfully.\n");
            }
 
        } else if (choice == 2) {

            int id;
            printf("Enter customer ID: "); scanf("%d", &id); getchar();
            Customer *c = searchCustomer(customerTable, id);
            if (c) printf("Found: ID=%d  Name=%s  Phone=%s\n",
                          c->CustomerID, c->Name, c->Phone);
            else   printf("Customer ID %d not found.\n", id);
 
        } else if (choice == 3) {

            int id;
            printf("Enter customer ID: "); scanf("%d", &id); getchar();
            DisplayCustomer(customerTable, id);
 
        } else if (choice == 4) {

            DisplayAllCustomers(customerTable);
 
        } else if (choice == 5) {

            int id;
            printf("Enter customer ID: "); scanf("%d", &id); getchar();
            if (CustomerExists(customerTable, id))
                printf("Customer ID %d EXISTS - order can proceed.\n", id);
            else
                printf("Customer ID %d does NOT exist - order rejected.\n", id);
 
        } else if (choice == 6) {

            saveCustomersToCSV(customerTable, csvFile);
 
        } else {
            printf("Invalid choice.\n");
        }
 
    } while (choice != 6);
 
    freeCustomerTable(customerTable);
    printf("Goodbye!\n");
    return 0;
}