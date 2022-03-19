#include "SolomonWin32.h"

// Handle one frames worth of events in the win32
SolomonEnum PlatformWindowEvaluateEvents(SolomonWindow window)
{
    MSG msg;
    memset(&msg, 0, sizeof(MSG));

    // There are two conditions that cause the messaging loop to exit
    // The first one is that the messaging found was a quit message
    // The second is that there are no messages in the buffer
    while (msg.message != WM_QUIT) {
        // Peak into the message buffer to see if there's a message
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            // We recieved a regular message so we can dispatch and translate it
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            // There was no message in the message buffer, so lets end the messaging loop and return control
            // back to the rest of solomon
            break;
        }
    }

    return SolomonEnumSuccess;
}
