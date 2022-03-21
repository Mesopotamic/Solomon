#include "SolomonWin32.h"

SolomonKey wParamToKey(WPARAM wparam);

// Handle one frames worth of events in the win32
SolomonEnum PlatformWindowEvaluateEvents(SolomonWindow window)
{
    // Grab the window handle in a way we can understand it
    SolomonWindowWin32* handle = window;

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

            // Once the message has been handled by the operating system we can also perform our own things
            // Like launching the windows event handler.
            switch (msg.message) {
                case WM_KEYDOWN:
                    handle->com.keyHandler(wParamToKey(msg.wParam), SolomonKeyEventDown);
                    break;
                case WM_KEYUP:
                    handle->com.keyHandler(wParamToKey(msg.wParam), SolomonKeyEventUp);
                    break;
                default:
                    break;
            }
        } else {
            // There was no message in the message buffer, so lets end the messaging loop and return control
            // back to the rest of solomon
            break;
        }
    }

    // Was the msg a quit, if so, then we need to tell Solomon window handle that it should quit
    handle->com.shouldContinue = msg.message != WM_QUIT;

    return SolomonEnumSuccess;
}

SolomonKey wParamToKey(WPARAM wparam)
{
    switch (wparam) {
        case VK_ESCAPE:
            return SolomonKeyESC;
        default:
            return SolomonKeyNoKey;
    }
}
