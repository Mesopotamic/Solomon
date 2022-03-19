#include <stdio.h>
#include "Solomon.h"

int SolomonMain(int argc, char* argv[])
{
    // Show that we do in fact recieve command line args
    printf("Hello Solomon\n");
    printf("Listing command line arguments :\n");
    for (int i = 0; i < argc; i++) {
        printf("\t*%i: %s\n", i, argv[i]);
    }

    // Create a Solomon window
    SolomonWindow window = SolomonWindowCreate(0, 0, 720, 360, "Hello Solomon!");
    if (!window) {
        printf("\nWhoops, window creation failed!\n");
        return -1;
    }

    // Now that the window has been created, we can show it to the user. Then we enter in a windowing loop.
    // Once per frame we have to evaluate all the events that happened, including if the OS requested the
    // window to close. We keep doing this loop until the window closes
    SolomonWindowShow(window);
    while (SolomonWindowShouldContinue(window)) {
        SolomonWindowEvaluateEvents(window);
    }
    return 0;
}
