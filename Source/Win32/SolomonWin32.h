/*************************************************************************************************************
 * Solomon win32 header, shared internally for all the win 32 things
 *************************************************************************************************************/
#ifndef __SOLOMON_WIN32_H__
#define __SOLOMON_WIN32_H__ (1)

#include <stdbool.h>
#include "SolomonCommon.h"

typedef struct SolomonWindowWin32 {
    SolomonWindowCommon com;  // Common details
    HWND hwnd;                // Handle to the win32 window
} SolomonWindowWin32;

LRESULT CALLBACK SolomonWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Has solomon registered the window class for solomon windows
extern bool s_winClassRegistered;

// Store the instance handle
extern HINSTANCE s_hinstance;

#endif  // !__SOLOMON_WIN32_H__
