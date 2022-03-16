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
 *************************************************************************************************************/

typedef void* SolomonWindow;

/*************************************************************************************************************
 * Solomon enums used for return codes, the SolomonEnumSuccess is 0, so you can use the functions how you
 * would strcmp ie if(!SolomonFunction){HandleError}
 *************************************************************************************************************/

typedef enum SolomonEnum {
    SolomonEnumSuccess = 0,   // The solomon function was successful
    SolomonEnumMemAllocFail,  // The solomon function failed due to a malloc
    SolomonEnumCount,         // The number of Solomon Enums
} SolomonEnum;

/**
 * @brief Translates a solomon enum into it's plain text equivelent so you can look up error codes easier
 * @param e Enum to be translated
 * @returns String containing the plain text version of the enum
 */
const char* SolomonEnumTranslate(SolomonEnum e);
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
