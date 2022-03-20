#include "SolomonWin32.h"

LRESULT CALLBACK SolomonWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_ACTIVATE: {
            MARGINS m;
            m.cxLeftWidth = 0;
            m.cxRightWidth = 0;
            m.cyBottomHeight = 0;
            m.cyTopHeight = 0;

            DwmExtendFrameIntoClientArea(hwnd, &m);
            break;
        }
        case WM_DESTROY:
            // Send the quit message to the operating system
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
