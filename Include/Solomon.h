/*************************************************************************************************************
 * Solomon
 * The basic cross platform windowing system which aims to reduce ugly defaul window bars as much as possible
 *
 * Copyright 2022-
 * License : MIT
 * Authours :
 *		Lawrence G
 *************************************************************************************************************/

// The main header Guard
#ifndef __SOLOMON_EXTERNAL_HEADER_H__
#define __SOLOMON_EXTERNAL_HEADER_H__ (1)

/*************************************************************************************************************
 * Different window systems require different headers, for example you can have linux x11, xcb, wayland etc...
 * But we don't want users of Solomon to have to include those headers, or pass any extra defines to their
 * application. How do we manage this?
 * We also don't want to allow the user write access variables, because changing some int values won't update
 * the window. To solve both of these problems we use an opaque handle.
 *
 * The next thing we do with opaque handles is to define the Solomon rectangle, which represents a portion of
 * the window, either split into an area by absolute coordinates or relative ones. In the future we plan to
 * suply solomon UI elements via these rects. Then UI calls are made with the signiture
 * SolomonUI*name*(SolomonWindow, rect, ...)
 * If the user passes null to the rect, then the window will be treated as the rect.
 *
 * For the event handling, we allow the user to pass a function pointer for the different types of callback
 *************************************************************************************************************/

typedef void* SolomonWindow;
typedef void* SolomonRect;

/*************************************************************************************************************
 * Solomon enums used for return codes, the SolomonEnumSuccess is 0, so you can use the functions how you
 * would strcmp ie if(!SolomonFunction){HandleError}
 *************************************************************************************************************/

typedef enum SolomonEnum {
    SolomonEnumSuccess = 0,   // The solomon function was successful
    SolomonEnumMemAllocFail,  // The solomon function failed due to a malloc
    SolomonEnumOSFail,        // The failure came from the window system
    SolomonEnumSegFail,       // Some kind of attempted Null reference was about to happen
    SolomonEnumNotInit,       // Failed because an initialisation failed somewhere
    SolomonEnumCount,         // The number of Solomon Enums
} SolomonEnum;

/**
 * @brief Translates a solomon enum into it's plain text equivelent so you can look up error codes easier
 * @param e Enum to be translated
 * @returns String containing the plain text version of the enum
 */
const char* SolomonEnumTranslate(SolomonEnum e);

/*************************************************************************************************************
 * This section is dedicated to the Solomon window fucntions, that provide the bare minimum you want from a
 * GUI, if you use just this part, then most likely you'll be using Vulkan to draw directly to the swapchain
 *************************************************************************************************************/

/**
 * @brief Gets the size of the internal Solomon window handle so user can allocate windows on their own if
 * they'd like to
 * @returns Size of SolomonWindowPrivate in bytes
 */
size_t SolomonWindowSize();

/**
 * @brief Allocates a Solomon window on the heap and returns a an opaque pointer to the handle
 * @return SolomonWindow handle
 */
SolomonWindow SolomonWindowAllocate();

/**
 * @brief Allocates a Solomon window, on the heap and then hands the basic details over to the internal handle
 * @param x X position of the top left corner of the window (-1 lets the OS decide)
 * @param y Y position of the top left corner of the window (-1 lets the OS decide)
 * @param w Width of the window in pixels
 * @param h Height of the window in pixels
 * @param title The heading given to the window title
 * @returns Opaque handle to the pointer containing the window data
 */
SolomonWindow SolomonWindowCreate(int x, int y, int w, int h, char* title);

/**
 * @brief Shows a successfully initialised window on screen
 * @param window Solomon window handle
 * @returns Success code
 */
SolomonEnum SolomonWindowShow(SolomonWindow window);

/**
 * @brief Evaluates the events that have happened since the last time this function was called, should be
 * called at least once a frame to check for exiting the window loop
 * @param window SolomonWindow handle for the window to process events for
 * @returns Success code
 */
SolomonEnum SolomonWindowEvaluateEvents(SolomonWindow window);

/**
 * @brief Evaluates if the solomon window passed should remain in the windowing loop
 * @param window the Solomon window handle to evaluate
 * @returns 1 if the windowing loop should continue, 0 if it should exit, can be used as a boolean expression,
 * for example in a while loop
 */
int SolomonWindowShouldContinue(SolomonWindow window);

/*************************************************************************************************************
 * Solomon events and callback sections
 *************************************************************************************************************/

typedef enum SolomonKeyEvent { SolomonKeyEventDown, SolomonKeyEventUp } SolomonKeyEvent;

typedef enum SolomonKey {
    SolomonKeyW,
    SolomonKeyA,
    SolomonKeyS,
    SolomonKeyD,
    SolomonKeyESC,
    SolomonKeySpace,
    SolomonKeyEnter,
    SolomonKeyNoKey,
} SolomonKey;

/**
 * @brief Function signature of a function that get's fired when the user presses a key
 * @param SolomonKey A key code for the event being fired
 * @param SolomonKeyEvent Which type of event is happening to the key
 */
typedef void (*SolomonKeyEventHandler)(SolomonKey, SolomonKeyEvent);

/**
 * @brief Attaches the user defined function pointer to be ran whenever the window recieves an event to do
 * with keys
 * @param window The opaque solomon window handle
 * @param handler Function pointer to a function that matches the signiture of SolomonKeyEventHandler. ie
 * returns void and has SolomonKey and SolomonKeyEvent params
 * @returns Solomon success code
 */
SolomonEnum SolomonKeyEventAttachHandler(SolomonWindow window, SolomonKeyEventHandler handler);

/*************************************************************************************************************
 * SolomonUI section, here is where you'll start using rects
 *************************************************************************************************************/

/*************************************************************************************************************
 * Produce the SolomonMain entry point, so that we can have graphics applications on both windows and linux,
 * we use macros to replace the main function. Unfortunatley it is increadibly ugly, but blame Win32 for
 * having a different entry point for GUI applications
 *
 * Because this is just so god damn ugly, we're gonna keep it at the bottom of the Solomon Header to hide my
 * sins
 *************************************************************************************************************/
#ifdef SolomonMain
#undef SolomonMain
#endif

// On windows the entry point we need the entry point to be WinMain, so we declare SolomonMain, then create a
// win main, which calls the solomon windows entry point, which extracts the command line arguments, stores
// the h instance, and finally calls back into user defined Solomon Main
#ifdef WIN32
#include <Windows.h>
void SolomonWin32Entry(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
                       _In_ int nCmdShow, int* pArgv, char** argv);

void SolomonWin32Exit();

#define SolomonMain                                                                                    \
    SolomonMainINT(int argc, char* argv[]);                                                            \
    CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, \
                     _In_ int nCmdShow)                                                                \
    {                                                                                                  \
        int argc;                                                                                      \
        SolomonWin32Entry(hInstance, hPrevInstance, lpCmdLine, nCmdShow, &argc, NULL);                 \
        char** argv = malloc(sizeof(char*) * argc);                                                    \
        SolomonWin32Entry(hInstance, hPrevInstance, lpCmdLine, nCmdShow, &argc, argv);                 \
        int result = SolomonMainINT(argc, argv);                                                       \
        SolomonWin32Exit();                                                                            \
        return result;                                                                                 \
    }                                                                                                  \
    int SolomonMainINT

#else
#define SolomonMain main
#endif  // SolomonMain Entry Point Define

#endif  // !__SOLOMON_EXTERNAL_HEADER_H__
