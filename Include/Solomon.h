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

// Produce the SolomonMain define which allows the user to make a cross platform entry point
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
