#ifndef CUSTOMER_H
#define CUSTOMER_H

#define TABLE_SIZE 100

typedef struct Customer
{
    int CustomerID;
    char Name[50];
    char Phone[20];
    struct Customer *next;
}Customer;

//Core API
int       hashFunction(int customerID);
Customer *createCustomer(int id, char Name[], char Phone[]);
void      insertCustomer(Customer *table[], Customer *newCustomer);
Customer *searchCustomer(Customer *table[], int customerID);
void      DisplayCustomer(Customer *table[], int customerID);
void      DisplayAllCustomers(Customer *table[]);
int       CustomerExists(Customer *table[], int customerID);
void      freeCustomerTable(Customer *table[]);

//Save/Load

void loadCustomersFromCSV(Customer *table[], const char *fileName);
void saveCustomersToCSV(Customer *table[], const char *fileName);

#endif