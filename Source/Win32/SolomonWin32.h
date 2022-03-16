/*************************************************************************************************************
 * Solomon win32 header, shared internally for all the win 32 things
 *************************************************************************************************************/
#include "SolomonCommon.h"

typedef struct SolomonWindowWin32 {
    SolomonWindowCommon com;  // Common details
    HWND hwnd;                // Handle to the win32 window
} SolomonWindowWin32;
