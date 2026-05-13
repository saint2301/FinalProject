#include "Integration.h"

int main() {
    IntegrationSystem system;
    initializeSystem(&system);
    loadSystemData(&system);
    runIntegrationMenu(&system);
    saveSystemData(&system);
    freeSystem(&system);
    return 0;
}