#include <stdio.h>
#include "Integration.h"

int main(void) {
    IntegrationSystem system;

    initIntegrationSystem(&system);

    runIntegrationMenu(&system);

    return 0;
}
