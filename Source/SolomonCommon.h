/*************************************************************************************************************
 * Solomon internal common things that are shared across platforms
 *************************************************************************************************************/
#ifndef __SOLOMON_INTERNAL_COMMON_H__
#define __SOLOMON_INTERNAL_COMMON_H__ (1)
#include <stdbool.h>
#include "Solomon.h"

typedef struct SolomonWindowCommon {
    int x;
    int y;
    int w;
    int h;
    char* title;
    bool windowInit;
} SolomonWindowCommon;

/**
 * We parse off into the platform specific stuff here, platform specific code has the Platform instead of
 * Solomon prefix. These are called by our trampolines after they have done all the common stuff
 */

/**
 * @brief Hands off the rest of the window creation to the platform dependant part
 * @param commonHandle void pointer to the opaque handle
 * @returns Solomon successcode
 */
SolomonEnum PlatformWindowCreate(void* commonHandle);

SolomonEnum PlatformWindowShow(void* commonHandle);

#endif  // !__SOLOMON_INTERNAL_COMMON_H__
