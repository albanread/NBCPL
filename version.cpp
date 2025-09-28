#include "version.h"
#include <stdio.h>

void print_version() {
    printf("NewBCPL Compiler Version %d.%d.%d\n", BCPL_VERSION_MAJOR, BCPL_VERSION_MINOR, BCPL_VERSION_PATCH);
}