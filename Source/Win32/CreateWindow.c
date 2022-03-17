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
    wc.hInstance = s_hinstance;
    wc.lpszClassName = SOLOMON_CLASS_NAME;
}

SolomonEnum PlatformWindowCreate(void* commonHandle)
{
    // Check that the windows class has been registered?
    // We don't need an error code for this since we can collect it from win32
    if (!s_winClassRegistered) tryRegisterWinClass();

    // Translate the opaque handle to the private visible windows one
    SolomonWindowWin32 handle = *(SolomonWindowWin32*)commonHandle;
    return SolomonEnumSuccess;
}

size_t SolomonWindowSize() { return sizeof(SolomonWindowWin32); }
