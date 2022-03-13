#include <stdio.h>
#include "Solomon.h"

int SolomonMain(int argc, char* argv[])
{
    printf("Hello Solomon\n");
    printf("Listing command line arguments :\n");
    for (int i = 0; i < argc; i++) {
        printf("\t*%i: %s\n", i, argv[i]);
    }
    return 0;
}
