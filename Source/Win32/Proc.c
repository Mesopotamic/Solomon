#include "SolomonWin32.h"

LRESULT CALLBACK SolomonWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_DESTROY:
            // Send the quit message to the operating system
            PostQuitMessage(0);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, CreateSolidBrush(RGB(40, 44, 52)));

            SetTextColor(hdc, RGB(229, 129, 123));
            SetBkColor(hdc, RGB(40, 44, 52));
            TextOut(hdc, 4, 4, "Hello, Solomon!", 16);

            EndPaint(hwnd, &ps);
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
