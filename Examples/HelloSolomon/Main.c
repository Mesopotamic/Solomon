#include <stdio.h>
#include "Solomon.h"

/**
 * @brief Declare a Solomon window handle to be shared throughout the program
 */
SolomonWindow window = NULL;

/**
 * @brief Declare a function to handle key inputs for us
 * @param key The key code
 * @param e event type
 */
void defaultSolomonKeyHandler(SolomonKey key, SolomonKeyEvent e);

/**
 * @brief Solomon Entry point. This is a macro that abstracts main, winmain etc, so that all Solomon apps can
 * have one entry point and users don't have to handle that
 */
int SolomonMain(int argc, char* argv[])
{
    // Show that we do in fact recieve command line args
    printf("Hello Solomon\n");
    printf("Listing command line arguments :\n");
    for (int i = 0; i < argc; i++) {
        printf("\t*%i: %s\n", i, argv[i]);
    }

    // Create a Solomon window and attach our custom key command handler
    // So that we can make solomon exit when the user presses esc
    window = SolomonWindowCreate(0, 0, 720, 360, "Hello Solomon!");
    if (!window) {
        printf("\nWhoops, window creation failed!\n");
        return -1;
    }
    SolomonKeyEventAttachHandler(window, defaultSolomonKeyHandler);

    // Now that the window has been created, we can show it to the user. Then we enter in a windowing loop.
    // Once per frame we have to evaluate all the events that happened, including if the OS requested the
    // window to close. We keep doing this loop until the window closes
    SolomonWindowShow(window);
    while (SolomonWindowShouldContinue(window)) {
        SolomonWindowEvaluateEvents(window);
    }

    // The windowing loop has finished so let's clean up solomon and inform the user that the exit happened
    printf("Solomon ended\n");
    return 0;
}

void defaultSolomonKeyHandler(SolomonKey key, SolomonKeyEvent e)
{
    // Exit when the user presses esc key
    if (key == SolomonKeyESC && e == SolomonKeyEventDown) {
        SolomonWindowScheduleClose(window);
        printf("Solomon Exit requested\n");
    }
}
