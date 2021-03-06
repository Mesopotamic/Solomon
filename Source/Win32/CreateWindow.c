#include "SolomonWin32.h"

/**
 * Register the forward declared objects
 */
bool s_winClassRegistered = false;
HINSTANCE s_hinstance = NULL;

const char SOLOMON_CLASS_NAME[] = "Solomon Window Class";

void tryRegisterWinClass()
{
    // Using the stored instance handle we can register the solomon window class inside the windows operating
    // system
    WNDCLASS wc;
    memset(&wc, 0, sizeof(WNDCLASS));
    wc.lpszClassName = SOLOMON_CLASS_NAME;
    wc.hInstance = s_hinstance;
    wc.lpfnWndProc = SolomonWindowProc;

    RegisterClass(&wc);
}

SolomonEnum PlatformWindowCreate(void* commonHandle)
{
    // Check that the windows class has been registered?
    // We don't need an error code for this since we can collect it from win32
    if (!s_winClassRegistered) tryRegisterWinClass();

    // Translate the opaque handle to the private visible windows one
    SolomonWindowWin32* handle = (SolomonWindowWin32*)commonHandle;

    // Now we can use the window handle and the information sent to us through the handle to create a new
    // window
    DWORD WindowStlye = WS_OVERLAPPEDWINDOW;

    handle->hwnd = CreateWindowEx(0,                   // Optional window styles
                                  SOLOMON_CLASS_NAME,  // Window class name
                                  handle->com.title,   // Window Title
                                  WindowStlye,         // Window Style
                                  10,                  // Initial horizontal position
                                  10,                  // Initial vertical position
                                  handle->com.rect.w,  // Width
                                  handle->com.rect.h,  // Height
                                  NULL,                // Parent window
                                  NULL,                // Menu
                                  s_hinstance,         // Application instance handle
                                  NULL                 // Application extra info
    );

    if (!handle->hwnd) return SolomonEnumOSFail;

    return SolomonEnumSuccess;
}

SolomonEnum PlatformWindowShow(void* commonHandle)
{
    // In order to show the window we need the window handle
    SolomonWindowWin32 window = *(SolomonWindowWin32*)commonHandle;
    HWND windowHandle = ((SolomonWindowWin32*)commonHandle)->hwnd;
    if (!windowHandle) return SolomonEnumSegFail;
    ShowWindow(windowHandle, SW_NORMAL);
    return SolomonEnumSuccess;
}

size_t PlatformWindowSize() { return sizeof(SolomonWindowWin32); }
