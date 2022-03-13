#include "Solomon.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>

// We only want to ensure that we have a console connected on debug builds
#ifndef NDEBUG
bool requestConsole = true;
#else
bool requestConsole = false;
#endif  // !NDEBUG

// Does the current process have a console attached to it?
bool consoleAttached = false;
bool consoleSpawnedByUs = false;

void SolomonWin32Entry(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
                       _In_ int nCmdShow, int* pArgc, char** argv)
{
    // First we grab the number of command line arguments passed to us
    int argc;
    LPWSTR* argstr = CommandLineToArgvW(GetCommandLine(), &argc);
    *pArgc = argc;

    // break early if we haven't been passed a valid pointer to argv
    if (argv == NULL) return;

    // Now fetch the command line arguments sent to us

    // Ensure that a console is always attached in debeg and release builds
    if (requestConsole && !consoleAttached) {
        // Check for a console, if there isn't one attached then we need to create a new one
        if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
            AllocConsole();
            SetConsoleTitleA("Solomon Console");

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                    FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
            consoleAttached = true;
            consoleSpawnedByUs = true;
        } else {
            consoleAttached = true;
        }

        // Fetch the file handle of the console and file output and map them together
        HANDLE hstd = GetStdHandle(STD_OUTPUT_HANDLE);
        int hConsole = _open_osfhandle((long)hstd, _O_TEXT);

        // Open the file handle for the console for writing
        FILE* fp = _fdopen(hConsole, "w");
        freopen_s(&fp, "CONOUT$", "w", stdout);
    }
}

void SolomonWin32Exit()
{
    if (consoleSpawnedByUs) {
        printf("\n");
        system("PAUSE");
    }
}
